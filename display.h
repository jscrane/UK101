#ifndef _DISPLAY_H
#define _DISPLAY_H

class display: public Memory::Device {
public:
	virtual void operator= (byte c) { _set(_acc, c); }
	virtual operator byte () { return _mem[_acc]; }

	display();

private:
	void _set(Memory::address a, byte c);

	byte _mem[DISPLAY_SIZE];
};
#endif
