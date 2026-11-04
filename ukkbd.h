#pragma once

#include <ps2_raw_kbd.h>

class ukkbd: public Memory::Device, public matrix_keyboard {
public:
	void operator= (uint8_t) override;
	operator uint8_t() override { return pattern(); }

	void checkpoint(Checkpoint &) override {}
	void restore(Checkpoint &) override {}

	void up(uint16_t) override;
	void down(uint16_t) override;
	void reset() override;

	ukkbd(): Memory::Device(1) {}

private:
	uint8_t _rows[8];
	uint8_t _last;
	uint16_t _map(uint8_t);
	void _set(uint8_t);
	void _reset(uint8_t);
	uint8_t pattern();
	bool _shifted;
};
