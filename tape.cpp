/*
 * tape.cc -- UK101 Tape Interface
 */
#include <SD.h>
#include "config.h"
#include "Memory.h"
#include "tape.h"

static File file, dir;

bool tape::begin(int cs, int module)
{
  pinMode(cs, OUTPUT);
  return SD.begin(cs, 3, module);
}

const char *tape::start()
{
  dir = SD.open("/");
  return advance();
}

void tape::stop()
{
  file.close();
}

void tape::operator= (byte b) {
  // FIXME: saving...
}

const char *tape::advance() {
  // FIXME: potential infinite loop here if card removed
  while (true) {
    file = dir.openNextFile();
    if (!file)
      dir.rewindDirectory();
    else if (!file.isDirectory())
      break;
  }
  return file.name();
}

const char *tape::rewind() {
  dir.rewindDirectory();
  file = dir.openNextFile();
  return file.name();
}

tape::operator byte () {
  if (_acc & 1)			// read data
    return _buf[_pos++];
  
  if (_pos >= _len) {		// read status
    _pos = 0;
    _len = file.read(_buf, sizeof(_buf));
  
    if (_len == 0) {
      file.close();
      return 0;    // eof
    }
  }
  return acia::rdrf || acia::tdre;
}
