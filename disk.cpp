#include <Arduino.h>
#include <memory.h>
#include <hardware.h>
#include <serialio.h>
#include <filer.h>
#include <flash_filer.h>
#include <acia.h>
#include <pia.h>

#include "config.h"
#include "disk.h"

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
// - writing to disk only works on esp8266 (LittleFS with "r+" file mode)
// - 8" disks not tested

// PB commands
#define WRITE_ENABLE	0x01
#define STEP_DIR	0x04
#define STEP_HEAD	0x08
#define FAULT_RESET	0x10
#define SIDE_SELECT	0x20	// 1 = A/B, 0 = C/D
#define LOW_CURRENT	0x40
#define LOAD_HEAD	0x80

// PA indicators (except DRIVE_SELECT)
#define DRIVE1_READY	0x01
#define TRACK0		0x02
#define FAULT		0x04
#define DRIVE2_READY	0x10
#define WRITE_PROT	0x20
#define DRIVE_SELECT	0x40	// 1 = A/C, 0 = B/D
#define INDEX_HOLE	0x80

#define TICK_FREQ	1000	// 1ms

// 5.25" floppy disk
#if USE_DISK == DISK525
#define DISK_TRACKS	40
#define TRACK_SECTORS	9
#define SECTOR_BYTES	256
#define RPM		300
#define T_REV_MS	60000 / RPM	// 200 ms

#elif USE_DISK == DISK8
#define DISK_TRACKS	77
#define TRACK_SECTORS	12
#define SECTOR_BYTES	256
#define RPM		360
#define T_REV_MS	60000 / RPM	// 166 ms
#endif

#if defined(USE_DISK)
static inline bool is_step_head(uint8_t b) { return !(b & STEP_HEAD); }

static inline bool is_step_in(uint8_t b) { return !(b & STEP_DIR); }

static inline bool is_load_head(uint8_t b) { return !(b & LOAD_HEAD); }

static inline bool is_index_hole(uint8_t b) { return !(b & INDEX_HOLE); }

static inline bool is_write_enabled(uint8_t b) { return !(b & WRITE_ENABLE); }

static inline bool is_side_ab(uint8_t b) { return b & SIDE_SELECT; }

static inline bool is_drive_ac(uint8_t b) { return b & DRIVE_SELECT; }

static inline uint32_t start_offset(int track) { return track * TRACK_SECTORS * SECTOR_BYTES; }

void disk::reset() {

	DBG_EMU(printf("disk reset\r\n"));

	static bool first_time = true;
	if (first_time) {
		hardware_interval_timer(TICK_FREQ, [this]() { tick(); });
		first_time = false;
	}

	drive = &driveA;
	track = 0xff;
	pos = 0;

	pia.register_porta_read_handler([this]() { return on_read_pia_porta(); });
	pia.register_portb_write_handler([this](uint8_t b) { on_write_pia_portb(b); });

	acia.register_read_data_handler([this]() { return on_read_acia_data(); });
	acia.register_write_data_handler([this](uint8_t b) { on_write_acia_data(b); });
	acia.register_reset_handler([this](void) { on_acia_reset(); });
	acia.register_framing_handler([this](uint32_t c) { on_acia_framing(c); });
	acia.register_can_rw_handler([this](void) { return on_acia_rw(); });
}

void disk::tick() {
	ticks++;
	if (ticks == T_REV_MS)
		ticks = 0;
}

void disk::operator=(uint8_t b) {
	if (_acc < 0x10)
		pia.write(_acc, b);
	else
		acia.write(_acc, b);
}

void disk::on_write_pia_portb(uint8_t b) {
	DBG_EMU(printf("DRB! %02x\r\n", b));

	uint8_t dra = pia.read_porta(), drb = pia.read_portb();

	bool last = (drive == &driveA || drive == &driveC);
	if (is_drive_ac(dra))
		drive = is_side_ab(b)? &driveA: &driveC;
	else
		drive = is_side_ab(b)? &driveB: &driveD;

	bool curr = (drive == &driveA || drive == &driveC);
	if (last != curr) {
		track = 0;
		seek_start();
	}

	if (!is_step_head(drb) && is_step_head(b)) {
		// track numbers increase inwards
		if (is_step_in(b))
			track++;
		else
			track--;

		DBG_EMU(printf("track: %d\r\n", track));
		seek_start();
	}

	if (is_load_head(b))
		seek_start();
}

disk::operator uint8_t() {
	if (_acc < 0x10)
		return pia.read(_acc);

	return acia.read(_acc - 0x10);
}

uint8_t disk::on_read_pia_porta() {

	uint8_t dra = WRITE_PROT;

	if (drive == &driveA || drive == &driveC)
		dra |= DRIVE_SELECT;

	if (*drive && ticks > 0)
		dra |= INDEX_HOLE;

	if (track > 0)
		dra |= TRACK0;

	if (!driveA && !driveC)
		dra |= DRIVE1_READY;

	if (!driveB && !driveD)
		dra |= DRIVE2_READY;

	DBG_EMU(printf("DRA? %02x\r\n", dra));
	return dra;
}

uint8_t disk::on_read_acia_data() {
	uint8_t b = drive->read();
	DBG_EMU(printf("ADR? %04x %02x\r\n", pos, b));
	pos++;
	return b;
}

void disk::write(uint8_t b) {
	drive->write(b);
	pos++;
}

void disk::on_write_acia_data(uint8_t b) {
	uint8_t dra = pia.read_porta();
	if (is_write_enabled(dra)) {
		// write header
		if (pos == start_offset(track)) {
			write(0x43);
			write(0x57);
			write(16*(track / 10) + (track % 10));
			write(0x58);
		}
		write(b);
	}
	DBG_EMU(printf("ADR! %04x %02x\r\n", pos, b));
}

uint8_t disk::on_acia_rw() {
	uint8_t dra = pia.read_porta();
	if (is_index_hole(dra))
		return 0;
	return drive->more()? 3: 2;
}

void disk::seek_start() {
	pos = start_offset(track);
	drive->seek(pos);
}

uint8_t disk_timer::_get(Memory::address a) {
	if (a == 0) {
		uint8_t s = _state;
		_state ^= 0x80;
		return s;
	}
	return 0;
}
#endif
