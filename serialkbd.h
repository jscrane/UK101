#ifndef __SERIALKBD_H
#define __SERIALKBD_H

class serialkbd: public Memory::Device {
public:
	void operator= (byte);
        operator byte() { return pattern(); }

	void up (unsigned);
	void down (unsigned);
	serialkbd (): Memory::Device(1024), _last(0), _delay(128) {
		for (int i=8; i--; )
			_rows[i] = 0;
          // shift-lock on
          _set(0x00, true);
          _key = 0;
          _millis = 0;
	}
	void delay (int d) { _delay=d; }
	int delay () const { return _delay; }
private:
	byte _rows[8];
	byte _last;
	bool _map (unsigned, unsigned short &);
	void _set (byte, bool);
	void _reset (byte);
	int _delay;
  byte pattern();
  long _millis;
  byte _key;
};
#endif
