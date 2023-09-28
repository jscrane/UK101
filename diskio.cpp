#include <Arduino.h>
#include <memory.h>
#include <hardware.h>
#include <serialio.h>
#include <filer.h>
#include <flash_filer.h>
#include <acia.h>
#include <pia.h>
#include <timed.h>

#include "config.h"
#include "diskio.h"

// See https://github.com/jefftranter/6502/blob/master/asm/OSI/diskboot.s for commented disassembly
// of the CEGMON diskboot routine (option 'D')
// See https://github.com/jefftranter/6502/blob/master/asm/OSI/os65dv33.s for commented disassembly of OS65D
//
// Disk geometry is discussed here: https://osiweb.org/computes_gazette/Compute_n020_p136_OS65D_Disk_Routines_Part1.pdf
//
// A disk interface consisted of a 6820 PIA mapped at 0xC000-0xC003 and a 6850 ACIA mapped at 0xC010-0xC011.
// PIA port A reads disk control lines and port B writes them. Data is read a byte at a time through
// the ACIA's data register.
//
// There is a notch at track 0 on the disk, and detection of this is the only way the disk head
// "knows" where it is. This is flagged by bit #7 low on read of DRA.
// A high->low transition on bit #3 of DRB indicates that the head should step in the direction
// given by bit #2: 0xb -> 0x3 means "step out" (increase track number) and 0xb -> 0x7 "step in".
//
// Current status:
// - writing to disk doesn't work (yet)
// - 8" disks not tested

// PB commands
#define STEP_IN		0x04
#define STEP_HEAD	0x08
#define LOAD_HEAD	0x80

// PA indicators
#define TRACK0		0x02
#define INDEX_HOLE	0x80

#define TICK_FREQ	1000	// 1ms

// 5.25" floppy disk
#if defined(USE_DISK525)
#define DISK_TRACKS	40
#define TRACK_SECTORS	9
#define SECTOR_BYTES	256
#define RPM		300
#define T_REV_MS	60000 / RPM	// 200 ms

#elif defined(USE_DISK8)
#define DISK_TRACKS	77
#define TRACK_SECTORS	12
#define SECTOR_BYTES	256
#define RPM		360
#define T_REV_MS	60000 / RPM	// 166 ms
#endif

#if defined(DEBUGGING)
#define DBG(x) Serial.x
#else
#define DBG(x)
#endif

static diskio *d;

static void IRAM_ATTR timer_handler() { d->tick(); }

void diskio::reset() {
	static bool firsttime = true;

	if (firsttime) {
		d = this;
		timer_create(TICK_FREQ, timer_handler);
		firsttime = false;
	}
	track = -1;
	pos = 0;
}

void IRAM_ATTR diskio::tick() {
	ticks++;
	if (ticks == T_REV_MS) {
		ticks = 0;
		if (track == 0)
			PIA::write_porta_in_bit(TRACK0, false);
	}
	PIA::write_porta_in_bit(INDEX_HOLE, ticks > 0);
}

void diskio::operator=(uint8_t b) {
	if (_acc < 4)
		PIA::write(_acc, b);
	else
		ACIA::write(_acc & 0x03, b);
}

void diskio::write_portb(uint8_t b) {
	DBG(printf("DRB! %02x\r\n", b));

	uint8_t drb = PIA::read_portb();
	if ((drb & STEP_HEAD) && (!(b & STEP_HEAD))) {
		// track numbers increase inwards
		if (!(b & STEP_IN))
			track++;
		else
			track--;

		DBG(printf("track: %d\r\n", track));
		seek_start(track);
	}

	if (!(b & LOAD_HEAD))
		seek_start(track);

	PIA::write_portb(b);
}

diskio::operator uint8_t() {
	if (_acc < 4)
		return PIA::read(_acc);

	return ACIA::read(_acc & 0x03);
}

uint8_t diskio::read_porta() {
	uint8_t b = PIA::read_porta();
	DBG(printf("DRA? %02x\r\n", b));
	return b;
}

uint8_t diskio::read_status() {
	uint8_t dra = PIA::read_porta();
	uint8_t b = (dra & INDEX_HOLE) && ACIA::read_status();
	b |= ACIA::dcd | ACIA::cts;
	DBG(printf("ASR? %02x\r\n", b));
	return b;
}

uint8_t diskio::read_data() {
	uint8_t b = ACIA::read_data();
	DBG(printf("ADR? %04x %02x\r\n", pos, b));
	pos++;
	return b;
}

void diskio::write_control(uint8_t b) {
	// hack to seek to start of track on reset
	if (b == ACIA::reset)
		seek_start(track);
	ACIA::write_control(b);
}

void diskio::seek_start(uint8_t track) {
	pos = track * TRACK_SECTORS * SECTOR_BYTES;
	_f.seek(pos);
}

uint8_t disk_timer::_get(Memory::address a) {
	if (a == 0) {
		uint8_t s = _state;
		_state ^= 0x80;
		return s;
	}
	return 0;
}
