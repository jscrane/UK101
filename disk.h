#ifndef _DISK_H
#define _DISK_H

class filer;

class disk: public Memory::Device {
public:
	disk(flash_filer &f): Memory::Device(Memory::page_size), _f(f) {}

	virtual void operator= (uint8_t c) { _set(_acc, c); }
	virtual operator uint8_t () { return _get(_acc); }

private:
	void set_index(uint8_t track);
	void seek_start(uint8_t track);

	void _set(Memory::address a, uint8_t c);
	uint8_t _get(Memory::address a);

	flash_filer &_f;
};

#endif
