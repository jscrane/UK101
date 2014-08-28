#include <Energia.h>
#include <SPI.h>
#include <SpiRAM.h>
#include <UTFT.h>
#include <SD.h>

#include <setjmp.h>
#include <stdarg.h>

#include "config.h"
#include "Memory.h"
#include "cpu.h"
#include "r6502.h"
#include "ram.h"
#include "spiram.h"
#include "prom.h"
#include "display.h"
#include "ps2drv.h"
#include "ukkbd.h"
#include "tape.h"
#include "cegmon_jsc.h"
#include "mon02.h"
#include "bambleweeny.h"
#if defined(ORIGINAL_BASIC)
#include "basic.h"
#else
#include "nbasic.h"
#endif
#include "encoder.h"
#include "toolkit2.h"
#include "exmon.h"

static prom monitors[] = {
  prom(cegmon_jsc, 2048),
  prom(monuk02, 2048),
  prom(bambleweeny, 2048),
};
static int currmon = 0;

static bool halted = false;

PS2Driver ps2;

Memory memory;
prom msbasic(basic, 8192);
prom tk2(toolkit2, 2048);
prom enc(encoder, 2048);
ram pages[RAM_SIZE / 1024];
spiram sram(SPIRAM_SIZE);
tape acia;
ukkbd kbd;
display disp;

void status(const char *fmt, ...) {
  char tmp[256];  
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, args);
  disp.error(tmp);
  va_end(args);
}

jmp_buf ex;
r6502 cpu(&memory, &ex, status);

const char *filename;
char chkpt[] = { "CHKPOINT" };
int cpid = 0;

void reset() {
  kbd.reset();  
  cpu.reset();

  bool sd = acia.begin(SD_CS, SD_SPI);
  disp.begin();
  if (sd)
    filename = acia.start();
  else
    disp.status("No SD Card");

  // must initialise spiram after SD card (if it shares the same bus)
  sram.begin(SPIRAM_CS, SPIRAM_SPI);

  halted = (setjmp(ex) != 0);
}

void setup() {
  Serial.begin(115200);

  ps2.begin(KBD_DATA, KBD_IRQ);
  
  memory.put(tk2, 0x8000);
  memory.put(enc, 0x8800);
  memory.put(msbasic, 0xa000);
  memory.put(monitors[currmon], 0xf800);

  for (int i = 0; i < RAM_SIZE; i += 1024)
    memory.put(pages[i / 1024], i);

  memory.put(sram, SPIRAM_BASE);
  memory.put(acia, 0xf000);
  memory.put(kbd, 0xdf00);
  memory.put(disp, 0xd000);

  reset();
}

void loop() {
  if (ps2.available()) {
    unsigned key = ps2.read();
    char cpbuf[13];
    int n;
    File file;
    switch (key) {
      case PS2_F1:
        if (ps2.isbreak())
          reset();
        break;
      case PS2_F2:
        if (ps2.isbreak()) {
          filename = acia.advance();
          disp.status(filename);
        }
        break;
      case PS2_F3:
        if (ps2.isbreak()) {
          filename = acia.rewind();
          disp.status(filename);
        }
        break;
      case PS2_F4:
        if (ps2.isbreak()) {
          currmon++;
          if (currmon == sizeof(monitors) / sizeof(monitors[0]))
            currmon = 0;
          memory.put(monitors[currmon], 0xf800);
          cpu.reset();
        }
        break; 
      case PS2_F5:
        if (ps2.isbreak()) {
          disp.toggleSize();
          cpu.reset();
        }
        break; 
      case PS2_F6:
        if (ps2.isbreak()) {
          acia.stop();
          snprintf(cpbuf, sizeof(cpbuf), "%s.%03d", chkpt, cpid++);
          file = SD.open(cpbuf, O_WRITE | O_CREAT | O_TRUNC);
          cpu.checkpoint(file);
          disp.checkpoint(file);
          for (int i = 0; i < RAM_SIZE; i += 1024)
            pages[i / 1024].checkpoint(file);
          sram.checkpoint(file);
          file.close();
          filename = acia.start();
          disp.status(cpbuf);
        }
        break;
      case PS2_F7:
        if (ps2.isbreak()) {
          acia.stop();
          file = SD.open(filename, O_READ);
          cpu.restore(file);
          disp.restore(file);
          for (int i = 0; i < RAM_SIZE; i += 1024)
            pages[i / 1024].restore(file);
          sram.restore(file);
          file.close();
          n = sscanf(filename, "%[A-Z0-9].%d", chkpt, &cpid);
          cpid = (n == 1)? 0: cpid+1;
          filename = acia.start();
          disp.status(filename);
        }
        break; 
      default:
        if (ps2.isbreak())
          kbd.up(key);
        else
          kbd.down(key);      
        break;
    }
  } else if (!halted)
    cpu.run(1000);
}
