#include <Energia.h>
#include <SPI.h>
#include <UTFT.h>
#include <PS2Keyboard.h>

//#include <Adafruit_GFX.h>
//#include <Adafruit_ST7735.h>
#include <ffconf.h>
#include <fatfs.h>

#include <setjmp.h>
#include <stdarg.h>
#include "Memory.h"
#include "cpu.h"
#include "r6502.h"
#include "ram.h"
#include "prom.h"
#include "display.h"
//#include "display1k.h"
//#include "serialkbd.h"
#include "ps2kbd.h"
#include "tape.h"
#include "cegmon_jsc.h"
//#include "mon02.h"
//#include "cegmon_c2.h"
//#include "bambleweeny.h"
#include "basic.h"

static bool halted = false;

void status(const char *fmt, ...) {
  char tmp[128];  
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, sizeof(tmp), fmt, args);
  Serial.println(tmp);
  va_end(args);
}

void setup() {
  Serial.begin(115200);
  pinMode(PUSH1, INPUT_PULLUP);
  pinMode(PUSH2, INPUT_PULLUP);
  
  Memory memory;
//  prom monitor(bambleweeny, 2048);
  prom monitor(cegmon_jsc, 2048);
//  prom monitor(mon02, 2048);
  memory.put(monitor, 0xf800);
  prom msbasic(basic, 8192);
  memory.put(msbasic, 0xa000);

  ram p0, p1, p2, p3, p4, p5, p6, p7;
  memory.put(p0, 0x0000);
  memory.put(p1, 0x0400);
  memory.put(p2, 0x0800);
  memory.put(p3, 0x0c00);
  memory.put(p4, 0x1000);
  memory.put(p5, 0x1400);
  memory.put(p6, 0x1800);
  memory.put(p7, 0x1c00);

  tape acia;
  memory.put(acia, 0xf000);
//  serialkbd kbd;
  ps2kbd kbd;
  memory.put(kbd, 0xdf00);
  
  display disp;
//  display1k disp;
  memory.put(disp, 0xd000);
  
  jmp_buf ex;
  r6502 cpu(&memory, &ex, status);

  cpu.reset();
  boolean push1 = false, push2 = false;
  if (!setjmp(ex)) {
    while (!halted) {
      cpu.run(100);

      if (!push2 && digitalRead(PUSH2) == 0) {
        Serial.println(cpu.status());
        acia.advance();
        push2 = true;
      } else if (push2 && digitalRead(PUSH2) == 1)
        push2 = false;

      if (!push1 && digitalRead(PUSH1) == 0) {
        cpu.reset();
        push1 = false;
      } else if (push1 && digitalRead(PUSH1) == 1)
        push1 = false;
    }
  }
  Serial.println(cpu.status());
  // FIXME: halted or illegal instruction
}

void loop() {
}
