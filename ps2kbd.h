#ifndef __PS2KBD_H
#define __PS2KBD_H

class ps2kbd: public Memory::Device {
public:
  void operator= (byte);
  operator byte() { return pattern(); }

  void up (unsigned);
  void down (unsigned);

  ps2kbd();

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
