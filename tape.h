#include "acia.h"

class tape: public acia, public Memory::Device {
public:
  void operator= (byte);
  operator byte ();

  const char *advance();
  const char *rewind();

  void start();
  void stop();
  bool begin(int cs, int module);
  
  tape(): Memory::Device(2048), _pos(0), _len(0) {}

private:
  unsigned int _pos, _len;
  byte _buf[128];
};
