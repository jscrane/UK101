#include <Arduino.h>
#include <memory.h>
#include <display.h>
#include <hardware.h>

#include "config.h"
#include "screen.h"
#include "roms/uk101/chguk101.h"

void screen::begin()
{
	Display::begin(TFT_BG, TFT_FG, TFT_ORIENT);
	clear();
}

static struct resolution {
	const char *name;
	unsigned cw, ch;
	boolean double_size;
} resolutions[] = {
#if defined(UK101)
	{"40x30", 8, 8, false},
	{"40x15", 8, 8, true},
	{"45x30", 7, 8, false},
	{"45x32", 7, 7, false},
	{"45x16", 7, 7, true},
#else
	{"32x30", 8, 8, false},
	{"32x32", 8, 7, false},
#endif
};

const char *screen::changeResolution()
{
	_resolution++;
	if (_resolution == sizeof(resolutions) / sizeof(struct resolution))
		_resolution = 0;
	return resolutions[_resolution].name;
}

void screen::_draw(Memory::address a, uint8_t c)
{
	struct resolution &r = resolutions[_resolution];
	int x = r.cw * (a % CHARS_PER_LINE + X_OFF);	// hack to view left edge of screen
	if (x < 0 || x >= _dx)
		return;

	unsigned y = (r.double_size? 2*r.ch: r.ch) * (a / CHARS_PER_LINE);
	if (y >= _dy)
		return;

	uint16_t cg = 8*c, cm = 8*_mem[a];
	for (unsigned j = 0; j < r.ch; j++) {
		uint8_t b = pgm_read_byte(&chguk101[cg + j]);
		uint8_t m = pgm_read_byte(&chguk101[cm + j]);
		if (b != m) {
			uint8_t d = (b ^ m);
			for (unsigned i=1, bit=1; i <= r.cw; i++, bit<<=1)
				if (d & bit) {
					int cx = x + r.cw - i;
					unsigned c = (b & bit)? TFT_FG: TFT_BG;
					if (r.double_size) {
						drawPixel(cx, y + 2*j, c);
						drawPixel(cx, y + 2*j + 1, c);
					} else
						drawPixel(cx, y + j, c);
				}
		}
	}
}

void screen::checkpoint(Stream &s)
{
	s.write(_resolution); 
	s.write(_mem, sizeof(_mem));
}

void screen::restore(Stream &s)
{
	int r = _resolution;
	_resolution = s.read();

	if (_resolution == r) {
		char buf[Memory::page_size];
		for (unsigned p = 0; p < pages(); p++) {
			s.readBytes(buf, sizeof(buf));
			for (unsigned i = 0; i < Memory::page_size; i++)
				_set(i + p*Memory::page_size, buf[i]);
		}
	} else {
		clear();
		s.readBytes((char *)_mem, sizeof(_mem));
		for (unsigned i = 0; i < sizeof(_mem); i++)
			_draw(i, _mem[i]);
	}
}
