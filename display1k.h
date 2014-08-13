#ifndef _DISPLAY_H
#define _DISPLAY_H

class display1k: public Memory::Device {
public:
	virtual void operator= (byte c) { _set(_acc, c); }
	virtual operator byte () { return _mem[_acc]; }

	display1k();

private:
	void _set(Memory::address a, byte c);

	byte _mem[1024];
};
#endif
