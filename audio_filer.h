#pragma once

#include <filer.h>

class audio_filer: public filer {
public:
	audio_filer(filer &f): _f(f) {}

	void reset() override;
	void framing(unsigned data_bits, unsigned stop_bits, parity p);
	void speed(unsigned baud) { _bit_delay = 1000000 / baud; }
	size_t write(uint8_t) override;

	uint8_t read() override { return _f.read(); }
	bool more() override { return _f.more(); }

	const char *advance() override { return _f.advance(); }
	const char *rewind() override { return _f.rewind(); }

	const char *checkpoint() override { return _f.checkpoint(); }
	void restore(const char *c) override { _f.restore(c); }

	bool start() override { return _f.start(); }
	void stop() override { _f.stop(); }

private:
	void write_bit(bool bit);

	filer &_f;
	unsigned _data_bits, _stop_bits, _bit_delay;
	parity _parity;
};
