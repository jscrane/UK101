#ifndef _DISK_H
#define _DISK_H

class disk: public Memory::Device {
public:
	virtual void operator= (uint8_t c) { _set(_acc, c); }
	virtual operator uint8_t () { return _get(_acc); }

	disk(): Memory::Device(Memory::page_size) {}
	void begin();
private:
	void _set(Memory::address a, uint8_t c);
	uint8_t _get(Memory::address a);
};

#endif
