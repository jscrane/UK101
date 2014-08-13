#include <Energia.h>
#include <UTFT.h>

#include "Memory.h"
#include "display.h"
#include "charset.h"

static UTFT d(SSD1289, PC_6, PC_5, PC_7, PC_4);

display::display() : Memory::Device(sizeof(_mem))
{
  pinMode(PD_6, OUTPUT);
  digitalWrite(PD_6, HIGH);
  d.InitLCD();
  d.fillScr(VGA_BLACK);
}

void display::_set(Memory::address a, byte c)
{
  if (c != _mem[a]) {
    _mem[a] = c;  
    int x = 8 * (a % 64), y = 8 * (a / 64);
    
    // FIXME: hack!
    x -= 16 * 8;
    
    for (int i = 0; i < 8; i++) {
      byte b = charset[c][i];
      for (int j = 0; j < 8; j++) {
        d.setColor((b & (1 << j))? VGA_WHITE: VGA_BLACK);
        d.drawPixel(x + 8 - j, y + i);
      }
    }      
  }
}

