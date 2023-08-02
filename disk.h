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

// kludge to workaround "timing routine hang":
// 5.25 disks with OS65D v3.3 appear to hang. The reason is probably due to the timing routine, described
// here: https://osiweb.org/Peek65/Peek65_V6N10-10_1985.pdf ("OS65D V3.3 BUG!")
class disk_timer: public Memory::Device {
public:
	disk_timer(): Memory::Device(Memory::page_size), _state(0) {}

	virtual void operator= (uint8_t) {}
	virtual operator uint8_t() { return _get(_acc); }

private:
	uint8_t _get(Memory::address a);

	uint8_t _state;
};

#endif
