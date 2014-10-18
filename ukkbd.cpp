#include <Energia.h>

#include <memory.h>
#include "ukkbd.h"

// maps scan codes to uk101 rows/cols + left-shift (where applicable)
static const unsigned short scanmap[128] = {
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x00 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x08 
  0xffff, 0xffff, 0x0002, 0xffff, 0x0006, 0x0017, 0x0077, 0xffff,    // 0x10 
  0xffff, 0xffff, 0x0015, 0x0037, 0x0016, 0x0047, 0x0076, 0xffff,    // 0x18 
  0xffff, 0x0026, 0x0027, 0x0036, 0x0046, 0x0074, 0x0075, 0xffff,    // 0x20 
  0xffff, 0x0014, 0x0025, 0x0035, 0x0044, 0x0045, 0x0073, 0xffff,    // 0x28 
  0xffff, 0x0023, 0x0024, 0x0033, 0x0034, 0x0043, 0x0072, 0xffff,    // 0x30 
  0xffff, 0xffff, 0x0022, 0x0032, 0x0042, 0x0071, 0x0067, 0xffff,    // 0x38 
  0xffff, 0x0021, 0x0031, 0x0041, 0x0055, 0x0065, 0x0066, 0xffff,    // 0x40 
  0xffff, 0x0057, 0x0013, 0x0056, 0x0012, 0x0011, 0x0063, 0xffff,    // 0x48 
  0xffff, 0xffff, 0x0271, 0xffff, 0x0231, 0x0263, 0xffff, 0xffff,    // 0x50 
  0x0000, 0x0001, 0x0053, 0x0222, 0xffff, 0x0275, 0xffff, 0xffff,    // 0x58 
  0xffff, 0x0256, 0xffff, 0xffff, 0xffff, 0xffff, 0x0062, 0xffff,    // 0x60 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x68 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0005, 0xffff,    // 0x70 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x78 
};

static const unsigned short shiftmap[128] = {
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x00 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x08 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0277, 0xffff,    // 0x10 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0276, 0xffff,    // 0x18 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0274, 0x0000, 0xffff,    // 0x20 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0273, 0xffff,    // 0x28 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0054, 0xffff,    // 0x30 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0272, 0x0264, 0xffff,    // 0x38 
  0xffff, 0x0221, 0xffff, 0xffff, 0xffff, 0x0266, 0x0267, 0xffff,    // 0x40 
  0xffff, 0x0257, 0x0213, 0xffff, 0x0064, 0xffff, 0x0000, 0xffff,    // 0x48 
  0xffff, 0xffff, 0x0211, 0xffff, 0xffff, 0x0212, 0xffff, 0xffff,    // 0x50 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x58 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x60 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x68 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x70 
  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,    // 0x78 
};

unsigned short ukkbd::_map(byte sc)
{
  // left- or right-shift
  if (sc == 0x12 || sc == 0x59) {
    _shifted = !_shifted;
    return scanmap[sc];
  } 
  if (_shifted) {
    unsigned short sh = shiftmap[sc];
    if (sh == 0x0000)
      return 0xffff;
    if (sh != 0xffff)
      return sh;
  }
  return scanmap[sc];
}

void ukkbd::reset() {
  for (int i=8; i--; )
    _rows[i] = 0;
  // shift-lock on
  _rows[0] = 1;
  _shifted = false;
}

void ukkbd::operator= (byte row) {
  _last = row;
}

byte ukkbd::pattern() {
  int r = 255-_last;
  byte pattern = 0;
  for (int i=8; i--; r>>=1) 
    if (r & 1)
      pattern |= _rows[7-i];

  return pattern ^ 0xff;
}

void ukkbd::_reset (byte k) {
  _rows[(k & 0xf0) >> 4] &= ~(1 << (k & 0x0f));
}

void ukkbd::up (byte scan) {
  unsigned short k = _map(scan);
  if (k != 0xffff) {
    if (!_shifted && k > 0xff) 
      _reset (k / 0xff);
    if (k > 0)
      _reset (k & 0xff);
  }
}

void ukkbd::_set (byte k) {
  _rows[(k & 0xf0) >> 4] |= 1 << (k & 0x0f);
}

void ukkbd::down (byte scan) {
  unsigned short k = _map(scan);
  if (k != 0xffff) {
    if (_shifted) {
      if (k < 0xff)
        _reset(0x02);
    } else if (k > 0xff)
      _set(k / 0xff);
    if (k > 0)
      _set(k & 0xff);
    else  // toggle caps lock
      _rows[0] ^= 1;
  }
}

