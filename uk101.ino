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

void setup() {
  Serial.begin(115200);

#if defined(PS2_KBD)
  PS2.begin(KBD_DATA, KBD_IRQ, PS2Keymap_UK);
#endif
  
  Memory memory;
  memory.put(monitors[currmon], 0xf800);
  prom msbasic(basic, 8192);
  memory.put(msbasic, 0xa000);
  prom tk2(toolkit2, 2048);
  memory.put(tk2, 0x8000);
  prom enc(encoder, 2048);
  memory.put(enc, 0x8800);

  ram pages[RAM_SIZE / 1024];
  for (int i = 0; i < RAM_SIZE; i += 1024)
    memory.put(pages[i / 1024], i);

  tape acia;
  memory.put(acia, 0xf000);
  kbd kbd;
  memory.put(kbd, 0xdf00);
  
  display disp;
  memory.put(disp, 0xd000);
  
  jmp_buf ex;
  r6502 cpu(&memory, &ex, status);

  cpu.reset();
  const char *filename = acia.start();
  const char *status = filename;
  char chkpt[] = { "CHKPT" };
  int cpid = 0, n;
  if (!setjmp(ex)) {
    while (!halted) {
      cpu.run(100);

      if (KBD_DEV.available()) {
        unsigned key = KBD_DEV.read();
        char cpbuf[13];
        File file;
        switch (key) {
          case PS2_F1:
            cpu.reset();
            kbd.reset();
            break;
          case PS2_F2:
            status = filename = acia.advance();
            break;
          case PS2_F3:
            status = filename = acia.rewind();
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
            status = cpbuf;
            break;
          case PS2_F7:
            acia.stop();
            file = SD.open(filename, O_READ);
            cpu.restore(file);
            for (int i = 0; i < RAM_SIZE; i += 1024)
              pages[i / 1024].restore(file);
            disp.restore(file);
            file.close();
            n = sscanf(filename, "%[A-Z].%d", cpbuf, &cpid);
            cpid = (n == 1)? 0: cpid+1;
            status = filename = acia.start();
            break; 
          default:
            kbd.down(key);
            break;
          }
        if (status) {
          disp.status(status);
          status = 0;
        }
      }
    }    
  }
  disp.status(cpu.status());
}

void loop() {
}
