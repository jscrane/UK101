#ifndef __UKKBD_H
#define __UKKBD_H

class ukkbd: public Memory::Device, public Keyboard {
public:
	void operator= (uint8_t);
	operator uint8_t() { return pattern(); }

	void up(uint8_t);
	void down(uint8_t);
	void reset();

	ukkbd(): Memory::Device(1024) {}

private:
	uint8_t _rows[8];
	uint8_t _last;
	uint16_t _map(uint8_t);
	void _set(uint8_t);
	void _reset(uint8_t);
	uint8_t pattern();
	bool _shifted;
};
#endif
