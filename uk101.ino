#include <SPI.h>
#include <SpiRAM.h>
#include <UTFT.h>
#include <SD.h>
#include <r65emu.h>

#include <setjmp.h>
#include <stdarg.h>

#include "hardware.h"
#include "config.h"
#include "display.h"
#include "ukkbd.h"
#include "tape.h"
#include "roms/cegmon_jsc.h"
#include "roms/mon02.h"
#include "roms/bambleweeny.h"
#include "roms/synmon.h"
#include "roms/syn600.h"
#include "roms/ohiomon.h"
#if defined(ORIGINAL_BASIC)
#include "basic.h"
#elif defined(OSI_BASIC)
#include "roms/osibasic.h"
#else
#include "roms/nbasic.h"
#endif
#include "roms/encoder.h"
#include "roms/toolkit2.h"
#include "roms/exmon.h"

static prom monitors[] = {
#if defined(UK101)
  prom(cegmon_jsc, 2048),
  prom(monuk02, 2048),
  prom(bambleweeny, 2048),
#else
  prom(syn600, 2048),
  prom(ohiomon, 2048),
#endif
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
tape tape;
ukkbd kbd;
display disp;

void status(const char *fmt, ...) {
  char tmp[256];  
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, args);
  disp.clear();
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

  bool sd = tape.begin(SD_CS, SD_SPI);
  disp.begin();
  if (sd)
    tape.start();
  else
    disp.status("No SD Card");

  // must initialise spiram after SD card (if it shares the same bus)
  sram.begin(SPIRAM_CS, SPIRAM_SPI);

  halted = (setjmp(ex) != 0);
}

void setup() {
  ps2.begin(KBD_DATA, KBD_IRQ);

  for (int i = 0; i < RAM_SIZE; i += 1024)
    memory.put(pages[i / 1024], i);

  memory.put(sram, SPIRAM_BASE);
  memory.put(tk2, 0x8000);
  memory.put(enc, 0x8800);
  memory.put(msbasic, 0xa000);

  memory.put(disp, 0xd000);
  memory.put(kbd, 0xdf00);
  memory.put(tape, 0xf000);
  memory.put(monitors[currmon], 0xf800);

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
          filename = tape.advance();
          disp.status(filename);
        }
        break;
      case PS2_F3:
        if (ps2.isbreak()) {
          filename = tape.rewind();
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
          disp.clear();
          disp.status(disp.changeResolution());
          cpu.reset();
        }
        break; 
      case PS2_F6:
        if (ps2.isbreak()) {
          tape.stop();
          snprintf(cpbuf, sizeof(cpbuf), "%s.%03d", chkpt, cpid++);
          file = SD.open(cpbuf, O_WRITE | O_CREAT | O_TRUNC);
          cpu.checkpoint(file);
          disp.checkpoint(file);
          for (int i = 0; i < RAM_SIZE; i += 1024)
            pages[i / 1024].checkpoint(file);
          sram.checkpoint(file);
          file.close();
          tape.start();
          disp.status(cpbuf);
        }
        break;
      case PS2_F7:
        if (ps2.isbreak() && filename) {
          tape.stop();
          file = SD.open(filename, O_READ);
          cpu.restore(file);
          disp.clear();
          disp.restore(file);
          for (int i = 0; i < RAM_SIZE; i += 1024)
            pages[i / 1024].restore(file);
          sram.restore(file);
          file.close();
          n = sscanf(filename, "%[A-Z0-9].%d", chkpt, &cpid);
          cpid = (n == 1)? 0: cpid+1;
          tape.start();
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
    cpu.run(CPU_INSTRUCTIONS);
}
