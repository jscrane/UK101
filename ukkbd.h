#ifndef __UKKBD_H
#define __UKKBD_H

class ukkbd: public Memory::Device, public Keyboard {
public:
	void operator= (byte);
	operator byte() { return pattern(); }

	void up(byte);
	void down(byte);
	void reset();

	ukkbd(): Memory::Device(1024) {}

private:
	byte _rows[8];
	byte _last;
	unsigned short _map(byte);
	void _set(byte);
	void _reset(byte);
	byte pattern();
	bool _shifted;
};
#endif
