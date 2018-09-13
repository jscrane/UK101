#ifndef _TAPE_H
#define _TAPE_H

class tape: public sdtape, public SerialDevice {
public:
	void reset();
	void write(uint8_t);

	uint8_t read() { return sdtape::read(); }
	bool more() { return sdtape::more(); }
};
#endif
