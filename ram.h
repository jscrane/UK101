#ifndef _RAM_H
#define _RAM_H

class ram: public Memory::Device {
public:
	virtual void operator= (byte c) { _mem[_acc] = c; }
	virtual operator byte () { return _mem[_acc]; }

	ram (): Memory::Device(sizeof(_mem)) {}

private:
	byte _mem[1024];
};
#endif
