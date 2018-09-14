#ifndef _TAPE_H
#define _TAPE_H

class tape: public sdtape, public SerialDevice {
public:
	void reset();
	void framing(unsigned data_bits, unsigned stop_bits, parity p);
	void speed(unsigned baud) { _bit_delay = 1000000 / baud; }

	void write(uint8_t);

	uint8_t read() { return sdtape::read(); }
	bool more() { return sdtape::more(); }
private:
	void write_bit(bool bit);

	unsigned _data_bits, _stop_bits, _bit_delay;
	parity _parity;
};
#endif
