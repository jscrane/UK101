#ifndef _AUDIO_FILER_H
#define _AUDIO_FILER_H

// make filers configurable:
// sd_filer
// fs_filer
// serial_filer
// audio_filer
class filer;

class audio_filer: public filer {
public:
	audio_filer(filer &f): _f(f) {}

	void reset();
	void framing(unsigned data_bits, unsigned stop_bits, parity p);
	void speed(unsigned baud) { _bit_delay = 1000000 / baud; }
	void write(uint8_t);

	uint8_t read() { return _f.read(); }
	bool more() { return _f.more(); }

	const char *advance() { return _f.advance(); }
	const char *rewind() { return _f.rewind(); }

	const char *checkpoint() { return _f.checkpoint(); }
	void restore(const char *c) { _f.restore(c); }

	bool start() { return _f.start(); }
	void stop() { _f.stop(); }

private:
	void write_bit(bool bit);

	filer &_f;
	unsigned _data_bits, _stop_bits, _bit_delay;
	parity _parity;
};
#endif
