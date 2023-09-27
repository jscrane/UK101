#ifndef _TAPE_H
#define _TAPE_H

class tape: public Memory::Device, public ACIA {
public:
	tape(filer &filer): Memory::Device(256), ACIA(filer) {}

	virtual void operator=(uint8_t);
	virtual operator uint8_t();
};

#endif
