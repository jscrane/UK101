#include <Arduino.h>
#include <memory.h>
#include <hardware.h>
#include <config.h>
#include <serialio.h>
#include <filer.h>
#include <flash_filer.h>

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

#define DDRA	0x00
#define DRA	0x00
#define CRA	0x01
#define DDRB	0x02
#define DRB	0x02
#define CRB	0x03
#define DR_MASK	0x04

#define ACR	0x10
#define ASR	0x10
#define ADR	0x11

// PB commands
#define STEP_IN		0x04
#define STEP_HEAD	0x08
#define LOAD_HEAD	0x80

// PA indicators
#define TRACK0		0x02
#define FAULT		0x04
#define INDEX_HOLE	0x80

#define ACIA_RESET	0x03
#define ACIA_RDRF	0x01

// 5.25" floppy disk
#define DISK_TRACKS	40
#define TRACK_SECTORS	9
#define SECTOR_BYTES	256
// 8" disk
// #define DISK_TRACKS	77
// #define TRACK_SECTORS	12
// #define SECTOR_BYTES	256

static uint8_t cra = 0;
static uint8_t dra = 0;
static uint8_t crb = 0;
static uint8_t drb = 0;
static uint32_t pos = 0;
static int track = -1;

#if defined(DEBUGGING)
#define DBG(x) Serial.x
#else
#define DBG(x)
#endif

void disk::_set(Memory::address a, uint8_t b) {
	switch (a & 0xff) {
	case CRA:
		DBG(printf("CRA! %02x\r\n", b));
		cra = b;
		break;

	case CRB:
		DBG(printf("CRB! %02x\r\n", b));
		crb = b;
		break;

	case DRA:
		if (!(cra & DR_MASK)) {
			// data-direction register A
			DBG(printf("DDA! %02x\r\n", b));
			return;
		}
		DBG(printf("DRA! %02x\r\n", b));
		break;

	case DRB:
		if (!(crb & DR_MASK)) {
			// data-direction register B
			DBG(printf("DDB! %02x\r\n", b));
			return;
		}
		DBG(printf("DRB! %02x\r\n", b));
		if ((drb & STEP_HEAD) && (!(b & STEP_HEAD))) {
			// track numbers increase inwards
			if (!(b & STEP_IN))
				track++;
			else
				track--;

			DBG(printf("track: %d\r\n", track));

			pos = track * TRACK_SECTORS * SECTOR_BYTES;
			_f.seek(pos);
		}
		if (!(b & LOAD_HEAD)) {
			if (track == 0)
				dra &= ~(INDEX_HOLE | TRACK0);
			else
				dra &= ~INDEX_HOLE;

			pos = track * TRACK_SECTORS * SECTOR_BYTES;
			_f.seek(pos);
		}
		drb = b;
		break;

	case ACR:
		DBG(printf("ACR! %02x\r\n", b));
		break;

	default:
		DBG(printf("???! %04x %02x\r\n", a, b));
		break;
	};
}

uint8_t disk::_get(Memory::address a) {
	uint8_t b;

	switch (a & 0xff) {
	case DRA:
		b = dra;
		DBG(printf("DRA? %02x\r\n", b));
		if (!(dra & INDEX_HOLE))
			dra |= INDEX_HOLE;
		return b;

	case DRB:
		DBG(printf("DRB? %02x\r\n", drb));
		return drb;

	case ASR:
		b = (dra & INDEX_HOLE) && _f.more();
		DBG(printf("ASR? %02x\r\n", b));
		return b;

	case ADR:
		b = _f.read();
		DBG(printf("ADR? %04x %02x\r\n", pos, b));
		pos++;
		// has disk completed one revolution?
		if ((pos - track * TRACK_SECTORS * SECTOR_BYTES) > TRACK_SECTORS * SECTOR_BYTES) {
			pos = track * TRACK_SECTORS * SECTOR_BYTES;
			_f.seek(pos);

			if (track == 0)
				dra &= ~(INDEX_HOLE | TRACK0);
			else
				dra &= ~INDEX_HOLE;
			DBG(printf("rev: %d\r\n", track));
		}
		return b;

	default:
		DBG(printf("???? %04x\r\n", a));
		break;
	}
	return 0x00;
}

void disk::begin() {
}