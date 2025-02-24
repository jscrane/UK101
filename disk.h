#pragma once

class disk: public Memory::Device {
public:
	disk(flash_file &a, flash_file &b, flash_file &c, flash_file &d):
		Memory::Device(Memory::page_size),
		driveA(a), driveB(b), driveC(c), driveD(d), drive(&a),
		pos(0), track(0xff), ticks(0) {}

	void reset();

	virtual void operator=(uint8_t);
	virtual operator uint8_t();

private:
	uint8_t on_read_pia_porta();
	void on_write_pia_portb(uint8_t);

	uint8_t on_read_acia_data();
	void on_write_acia_data(uint8_t);
	uint8_t on_acia_rw();

	void seek_start();
	void write(uint8_t);

	flash_file &driveA, &driveB, &driveC, &driveD, *drive;
	uint32_t pos;
	uint8_t track;

	void tick();
	unsigned ticks;

	PIA pia;
	ACIA acia;
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
