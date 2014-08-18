#ifndef __KBD_H__
#define __KBD_H__

class kbd: public Memory::Device {
public:
  void operator= (byte);
  operator byte() { return pattern(); }

  void up(unsigned);
  void down(unsigned);
  void reset();

  kbd(): Memory::Device(1024), _last(0), _delay(128) {
    reset();
  }

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
