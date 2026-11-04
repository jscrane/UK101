#pragma once

#include <display.h>

class screen: public Display, public Memory::Device {
public:
	void operator= (uint8_t c) override { _set(_acc, c); }
	operator uint8_t () override { return _mem[_acc]; }

	void checkpoint(Checkpoint &) override;
	void restore(Checkpoint &) override;

	screen(): Memory::Device(sizeof(_mem)), _resolution(0) {}
	void begin();
	const char *changeResolution();

protected:
	void _draw(Memory::address a, uint8_t c);

private:
	inline void _set(Memory::address a, uint8_t c) {
		if (c != _mem[a]) { _draw(a, c); _mem[a] = c; }
	}
	uint8_t _mem[DISPLAY_RAM_SIZE];
	uint8_t _resolution;
	const char *setResolution();
};
