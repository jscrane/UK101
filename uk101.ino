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
//#include "serialkbd.h"
#include "ps2kbd.h"
#include "tape.h"
#include MONITOR
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

  prom monitor(monitor_rom, 2048);
  memory.put(monitor, 0xf800);
  prom msbasic(basic, 8192);
  memory.put(msbasic, 0xa000);

  ram pages[RAM_SIZE / 1024];
  for (int i = 0; i < RAM_SIZE; i += 1024)
    memory.put(pages[i / 1024], i);

  tape acia;
  memory.put(acia, 0xf000);
//  serialkbd kbd;
  ps2kbd kbd;
  memory.put(kbd, 0xdf00);
  
  display disp;
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
