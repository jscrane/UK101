#ifndef _SCREEN_H
#define _SCREEN_H

class screen: public Display, public Memory::Device {
public:
	virtual void operator= (uint8_t c) { _set(_acc, c); }
	virtual operator uint8_t () { return _mem[_acc]; }

	virtual void checkpoint(Stream &s);
	virtual void restore(Stream &s);

	screen(): Memory::Device(sizeof(_mem)), _resolution(0) {}
	void begin();
	const char *changeResolution();

protected:
	void _draw(Memory::address a, uint8_t c);

private:
	inline void _set(Memory::address a, uint8_t c) {
		if (c != _mem[a]) { _draw(a, c); _mem[a] = c; }
	}
	uint8_t _mem[DISPLAY_RAM_SIZE];
	int _resolution;
	const char *setResolution();
};
#endif
