#include "acia.h"

class tape: public acia {
public:
  void operator= (byte);
  operator byte ();

  const char *advance();
  const char *rewind();

  const char *start();
  void stop();
  
  tape();

private:
  unsigned int _pos, _len;
  byte _buf[128];
};
