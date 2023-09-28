#ifndef _DISKIO_H
#define _DISKIO_H

class diskio: public Memory::Device, public PIA, public ACIA {
public:
	diskio(flash_filer &f): Memory::Device(Memory::page_size), ACIA(f), _f(f), pos(0), track(-1), ticks(0) {}

	void reset();
	void tick();

	virtual void operator=(uint8_t);
	virtual operator uint8_t();

protected:
	virtual void write_portb(uint8_t);
	virtual uint8_t read_porta();
	virtual uint8_t read_status();
	virtual uint8_t read_data();
	virtual void write_control(uint8_t);

private:
	void seek_start();

	flash_filer &_f;
	uint32_t pos;
	int track;

	volatile int ticks;
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
