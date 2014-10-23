#include <UTFT.h>
#include <memory.h>
#include <utftdisplay.h>
#include <hardware.h>

#include "config.h"
#include "display.h"
#include "uk101/charset.h"

void display::begin()
{
	UTFTDisplay::begin(TFT_BG, TFT_FG);
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

const char *display::changeResolution()
{
	_resolution++;
	if (_resolution == sizeof(resolutions) / sizeof(struct resolution))
		_resolution = 0;
	return resolutions[_resolution].name;
}

void display::_draw(Memory::address a, byte c)
{
	struct resolution &r = resolutions[_resolution];
	int x = r.cw * (a % CHARS_PER_LINE - X_OFF);	// hack to view left edge of screen
	if (x < 0 || x >= _dx)
		return;

	unsigned y = (r.double_size? 2*r.ch: r.ch) * (a / CHARS_PER_LINE);
	if (y >= _dy)
		return;

	for (unsigned i = 0; i < r.ch; i++) {
		byte b = charset[c][i];
		for (unsigned j = 0; j < r.cw; j++) {
			int _cx = x + r.cw - j;
			utft.setColor((b & (1 << j))? TFT_FG: TFT_BG);
			if (r.double_size) {
				utft.drawPixel(_cx, y + 2*i);
				utft.drawPixel(_cx, y + 2*i + 1);
			} else
				utft.drawPixel(_cx, y + i);
		}
	}
}

void display::checkpoint(Stream &s)
{
	s.write(_resolution); 
	s.write(_mem, sizeof(_mem));
}

void display::restore(Stream &s)
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
