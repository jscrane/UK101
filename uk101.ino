#include <SPI.h>
#include <SpiRAM.h>
#include <UTFT.h>
#include <SD.h>
#include <r65emu.h>

#include <setjmp.h>
#include <stdarg.h>

#include "config.h"
#include "display.h"
#include "ukkbd.h"
#include "tape.h"
#include "sprom.h"

#if defined(UK101)
#include "uk101/cegmon_jsc.h"
#include "uk101/cegmon_101.h"
#include "uk101/mon02.h"
#include "uk101/bambleweeny.h"
#include "uk101/encoder.h"
#include "uk101/toolkit2.h"
#include "uk101/exmon.h"
#if defined(ORIGINAL_BASIC)
#include "uk101/basic.h"
#else
#include "uk101/nbasic.h"
#endif

prom tk2(toolkit2, 2048);
prom enc(encoder, 2048);

static sprom sproms[] = {
  sprom(cegmon_jsc, 2048),
  sprom(monuk02, 2048),
  sprom(cegmon_101, 2048),
  sprom(bambleweeny, 2048),
};
promswitch monitors(sproms, 4, 0xf800);

#else
#include "ohio/synmon.h"
#include "ohio/syn600.h"
#include "ohio/ohiomon.h"
#include "ohio/cegmon_c2.h"
#include "ohio/osibasic.h"

static sprom sproms[] = {
  sprom(syn600, 2048);
  sprom(ohiomon, 2048);
  sprom(cegmon_c2, 2048);
};
promswitch monitors(sproms, 3, 0xf800);
#endif

static bool halted = false;

prom msbasic(basic, 8192);
ram pages[RAM_SIZE / 1024];
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
  bool sd = hardware_init(cpu);

  kbd.reset();  
  disp.begin();
  if (sd)
    tape.start();
  else
    disp.status("No SD Card");

  halted = (setjmp(ex) != 0);
}

void setup() {
  for (unsigned i = 0; i < RAM_SIZE; i += 1024)
    memory.put(pages[i / 1024], i);

  memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#if defined(UK101)
  memory.put(tk2, 0x8000);
  memory.put(enc, 0x8800);
#endif
  memory.put(msbasic, 0xa000);

  memory.put(disp, 0xd000);
  memory.put(kbd, 0xdf00);
  memory.put(tape, 0xf000);
  monitors.set(0);

  reset();
}

void loop() {
  if (ps2.available()) {
    unsigned key = ps2.read();
    char cpbuf[32];
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
          monitors.next();
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
          snprintf(cpbuf, sizeof(cpbuf), PROGRAMS"%s.%03d", chkpt, cpid++);
          file = SD.open(cpbuf, O_WRITE | O_CREAT | O_TRUNC);
          hardware_checkpoint(file);
          file.close();
          tape.start();
          disp.status(cpbuf);
        }
        break;
      case PS2_F7:
        if (ps2.isbreak() && filename) {
          tape.stop();
          snprintf(cpbuf, sizeof(cpbuf), PROGRAMS"%s", filename);
          file = SD.open(cpbuf, O_READ);
          hardware_restore(file);
          file.close();
          n = sscanf(cpbuf + strlen(PROGRAMS), "%[A-Z0-9].%d", chkpt, &cpid);
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
