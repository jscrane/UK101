/*
 * tape.cc -- UK101 Tape Interface
 */
#include <SD.h>
#include "config.h"
#include "Memory.h"
#include "tape.h"

static File file, dir;

tape::tape(): _pos(0), _len(0)
{
  pinMode(SD_CS, OUTPUT);
  if (!SD.begin(SD_CS, SPI_HALF_SPEED, SD_SPI)) {
    Serial.println("failed to initialise SD card");
    return;
  }    
  dir = SD.open("/");
  advance();
}

void tape::operator= (byte b) {
  // FIXME: saving...
}

const char *tape::advance() {
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
