#include <Energia.h>
#include <SPI.h>
#include <UTFT.h>
#include <PS2Keyboard.h>
#include <SD.h>

#include <setjmp.h>
#include <stdarg.h>

#include "config.h"
#include "Memory.h"
#include "cpu.h"
#include "r6502.h"
#include "ram.h"
#include "prom.h"
#include "display.h"
#include "kbd.h"
#include "tape.h"
//#include "cegmon_101.h"
#include "cegmon_jsc.h"
#include "mon02.h"
#include "bambleweeny.h"
#include "basic.h"
#include "encoder.h"
#include "toolkit2.h"

static prom monitors[] = {
  prom(cegmon_jsc, 2048),
  prom(monuk02, 2048),
  prom(bambleweeny, 2048),
};
static int currmon = 0;

static bool halted = false;

void status(const char *fmt, ...) {
  char tmp[128];  
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, args);
  Serial.println(tmp);
  va_end(args);
}

#if defined(PS2_KBD)
PS2Keyboard PS2;
#endif

Memory memory;
prom msbasic(basic, 8192);
prom tk2(toolkit2, 2048);
prom enc(encoder, 2048);
ram pages[RAM_SIZE / 1024];
tape acia;
kbd kbd;
display disp;
jmp_buf ex;
r6502 cpu(&memory, &ex, status);

const char *filename;
char chkpt[] = { "CHKPOINT" };
int cpid = 0;

void reset() {
  kbd.reset();  
  cpu.reset();

  if (setjmp(ex)) {
    halted = true;
    disp.status(cpu.status());
  } else
    halted = false;
}

void setup() {
  Serial.begin(115200);

#if defined(PS2_KBD)
  PS2.begin(KBD_DATA, KBD_IRQ, PS2Keymap_UK);
#endif
  
  memory.put(monitors[currmon], 0xf800);
  memory.put(msbasic, 0xa000);
  memory.put(tk2, 0x8000);
  memory.put(enc, 0x8800);

  for (int i = 0; i < RAM_SIZE; i += 1024)
    memory.put(pages[i / 1024], i);

  memory.put(acia, 0xf000);
  memory.put(kbd, 0xdf00);
  memory.put(disp, 0xd000);

  acia.begin();
  disp.begin();

  filename = acia.start();
  reset();  
}

void loop() {
  if (KBD_DEV.available()) {
    unsigned key = KBD_DEV.read();
    char cpbuf[13];
    int n;
    File file;
    switch (key) {
      case PS2_F1:
        reset();
        break;
      case PS2_F2:
        filename = acia.advance();
        disp.status(filename);
        break;
      case PS2_F3:
        filename = acia.rewind();
        disp.status(filename);
        break;
      case PS2_F4:
        currmon++;
        if (currmon == sizeof(monitors) / sizeof(monitors[0]))
          currmon = 0;
        memory.put(monitors[currmon], 0xf800);
        cpu.reset();
        break; 
      case PS2_F5:
        disp.toggleSize();
        cpu.reset();
        break; 
      case PS2_F6:
        acia.stop();
        snprintf(cpbuf, sizeof(cpbuf), "%s.%03d", chkpt, cpid++);
        file = SD.open(cpbuf, O_WRITE | O_CREAT | O_TRUNC);
        cpu.checkpoint(file);
        for (int i = 0; i < RAM_SIZE; i += 1024)
          pages[i / 1024].checkpoint(file);
        disp.checkpoint(file);
        file.close();
        filename = acia.start();
        disp.status(cpbuf);
        break;
      case PS2_F7:
        acia.stop();
        file = SD.open(filename, O_READ);
        cpu.restore(file);
        for (int i = 0; i < RAM_SIZE; i += 1024)
          pages[i / 1024].restore(file);
        disp.restore(file);
        file.close();
        n = sscanf(filename, "%[A-Z].%d", chkpt, &cpid);
        cpid = (n == 1)? 0: cpid+1;
        filename = acia.start();
        disp.status(filename);
        break; 
      default:
        kbd.down(key);
        break;
    }
  } else if (!halted)
    cpu.run(10000);
}
