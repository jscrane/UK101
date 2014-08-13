/*
 * tapeuk_tk.cc -- UK101/Tk Tape Interface
 */
#include <fatfs.h>
#include "Memory.h"
#include "tape.h"

static DIR dir;
static FILINFO finfo;
static FIL file;

tape::tape(): _pos(0), _len(0)
{
  FRESULT r = FatFs.begin(39, SPI_CLOCK_DIV128, 1);
  if (FR_OK != r) {
    Serial.print("failed to initialise SD card: ");
    Serial.println(r);
    return;
  }
    
  // open directory on SD card
  r = FatFs.opendir(&dir, "/");
  if (FR_OK != r) {
    Serial.print("failed to open dir: ");
    Serial.println(r);
  } else
    advance();
}

void tape::operator= (byte b) {
  // FIXME: saving...
}

static FRESULT next_file(DIR *dir, FILINFO *finfo) {
  while (true) {
    FRESULT r = FatFs.readdir(dir, finfo);
    if (FR_OK != r)
      return r;

    if (finfo->fname[0] == 0) {
      r = FatFs.opendir(dir, "/");
      if (FR_OK != r)
        return r;
      continue;
    }
    
    if (!(finfo->fattrib & AM_DIR))
      return FR_OK;
  }
}

void tape::advance() {
  Serial.println("reading dir");
  FRESULT r = next_file(&dir, &finfo);
  if (FR_OK == r) {
    Serial.print("next file is [");
    Serial.print(finfo.fname);
    Serial.println("]");
    r = FatFs.open(&file, finfo.fname, FA_OPEN_EXISTING | FA_READ);
    if (FR_OK != r) {
      Serial.print("failed to open file: ");
      Serial.println(r);
    }
  }
}

tape::operator byte () {
  if (_acc & 1)			// read data
    return _buf[_pos++];
  
  if (_pos >= _len) {		// read status
    _pos = 0;
    FRESULT r = FatFs.read(&file, _buf, sizeof(_buf), &_len);
    if (FR_OK != r)
      return 0;
  
    if (_len == 0) {
      FatFs.close(&file);
      return 0;    // eof
    }
  }
  return acia::rdrf || acia::tdre;
}
