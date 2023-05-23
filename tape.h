#ifndef _TAPE_H
#define _TAPE_H

// split out audio_filer and make filers configurable:
// sd_filer
// fs_filer
// serial_filer
// audio_filer
class filer;

class tape: public serialio {
public:
	tape(filer &f): _f(f) {}

	void reset();
	void framing(unsigned data_bits, unsigned stop_bits, parity p);
	void speed(unsigned baud) { _bit_delay = 1000000 / baud; }
	void write(uint8_t);

	uint8_t read() { return _f.read(); }
	bool more() { return _f.more(); }
private:
	void write_bit(bool bit);

	filer &_f;
	unsigned _data_bits, _stop_bits, _bit_delay;
	parity _parity;
};
#endif
