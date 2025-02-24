#pragma once

class tape: public Memory::Device {
public:
	tape(filer &filer);

	virtual void operator=(uint8_t b) { acia.write(_acc, b); }
	virtual operator uint8_t() { return acia.read(_acc); }

private:
	filer &_filer;
	ACIA acia;
};
