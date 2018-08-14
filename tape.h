#ifndef _TAPE_H
#define _TAPE_H

class tape: public acia, public sdtape, public Memory::Device {
public:
	void operator= (uint8_t);
	operator uint8_t();

	tape(): Memory::Device(2048) {}
};
#endif
