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
#include "cegmon_101.h"
#include "cegmon_jsc.h"
#include "mon02.h"
#include "bambleweeny.h"
#include "basic.h"
#ifdef notdef
#include "encoder.h"
#include "toolkit2.h"
#endif

static prom monitors[] = {
  prom(cegmon_101, 2048),
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
#ifdef notdef
  prom tk2(toolkit2, 2048);
  memory.put(tk2, 0x8000);
  prom enc(encoder, 2048);
  memory.put(enc, 0x8800);
#endif

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
  if (!setjmp(ex)) {
    while (!halted) {
      cpu.run(100);

      if (KBD_DEV.available()) {
        unsigned key = KBD_DEV.read();
        const char *file = 0;
        switch (key) {
          case PS2_F1:
            cpu.reset();
            break;
          case PS2_F2:
            file = acia.advance();
            break;
          case PS2_F3:
            file = acia.rewind();
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
          default:
            kbd.down(key);
            break;
          }
        if (file)
          disp.status(file);
      }
    }    
  }
  disp.status(cpu.status());
}

void loop() {
}
