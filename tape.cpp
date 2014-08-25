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

const char *tape::advance() {
  bool rewound = false;
  while (true) {
    file = dir.openNextFile();
    if (file) {
      if (!file.isDirectory())
        break;
    } else if (!rewound) {
      dir.rewindDirectory();
      rewound = true;
    } else      
      return 0;
  }
  return file.name();
}

const char *tape::rewind() {
  dir.rewindDirectory();
  file = dir.openNextFile();
  return file.name();
}

void tape::operator=(byte b) {
  // FIXME?
}

tape::operator byte() {
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
  return rdrf | tdre;
}
