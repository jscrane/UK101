#pragma once

class tape: public Memory::Device {
public:
	tape(filer &filer);

	virtual void operator=(uint8_t b) { acia.write(_acc, b); }
	virtual operator uint8_t() { return acia.read(_acc); }

private:
	uint8_t on_read_data() { return _filer.read(); }
	void on_write_data(uint8_t b) { _filer.write(b); }
	uint8_t can_rw() { return _filer.more()? 3: 2; }

	filer &_filer;
	ACIA acia;
};
