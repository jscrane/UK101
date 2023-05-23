#include <Arduino.h>
#include <memory.h>
#include <hardware.h>
#include <config.h>

#include "disk.h"
#include "diskimage.h"

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

#define RESET_CTRL	0xff
#define LOAD_HEAD	0x7f
#define UNLOAD_HEAD	0x80
#define STEP_HEAD	0x08
#define STEP_IN		0x04
#define HOLE		0x80

#define ACIA_RESET	0x03
#define ACIA_RDRF	0x01

static uint8_t cra = 0;
static uint8_t dra = 0;
static uint8_t crb = 0;
static uint8_t drb = 0;
static uint16_t off = 0;
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
			DBG(printf("DDRA! %02x\r\n", b));
			return;
		}
		DBG(printf("DRA! %02x\r\n", b));
		break;

	case DRB:
		if (!(crb & DR_MASK)) {
			DBG(printf("DDRB! %02x\r\n", b));
			return;
		}
		DBG(printf("DRB! %02x\r\n", b));
		if (!(b & UNLOAD_HEAD))
			dra &= ~HOLE;
		if ((drb & STEP_HEAD) && (!(b & STEP_HEAD))) {
			if (b & STEP_IN)
				track--;
			else
				track++;
			off = track * 9 * 256;
			DBG(printf("off: %04x\r\n", off));
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
		if (!(dra & HOLE))
			dra |= HOLE;
		return b;

	case DRB:
		DBG(printf("DRB? %02x\r\n", drb));
		return drb;

	case ASR:
		return off < sizeof(disk_image);

	case ADR:
		b = pgm_read_byte(disk_image + off);
		DBG(printf("ADR? %04x %02x\r\n", off, b));
		off++;
		return b;

	default:
		DBG(printf("???? %04x\r\n", a));
		break;
	}
	return 0x00;
}

void disk::begin() {
}
