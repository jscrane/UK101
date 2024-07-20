#ifndef _TAPE_H
#define _TAPE_H

class tape: public Memory::Device, public ACIA {
public:
	tape(filer &filer): Memory::Device(256), _filer(filer) {}

	virtual void operator=(uint8_t);
	virtual operator uint8_t();

protected:
	bool acia_more() { return _filer.more(); }
	uint8_t read_data() { return _filer.read(); }
	void write_data(uint8_t b) { _filer.write(b); }

private:
	filer &_filer;
};

#endif
