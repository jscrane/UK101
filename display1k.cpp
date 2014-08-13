#include <Energia.h>
#include <UTFT.h>

#include "Memory.h"
#include "display1k.h"
#include "charset.h"

static UTFT d(S6D1121_8, PC_6, PC_5, PC_7, PC_4);

display1k::display1k() : Memory::Device(sizeof(_mem))
{
  d.InitLCD();
  d.fillScr(VGA_BLACK);
}

void display1k::_set(Memory::address a, byte c)
{
  if (c != _mem[a]) {
    _mem[a] = c;  
    int x = 8 * (a % 64), y = 16 * (a / 64);
    
    // FIXME: hack!
    x -= 13 * 8;
    
    for (int i = 0; i < 8; i++) {
      byte b = charset[c][i];
      for (int j = 0; j < 8; j++) {
        d.setColor((b & (1 << j))? VGA_WHITE: VGA_BLACK);
        d.drawPixel(x + 8 - j, y + 2*i);
        d.drawPixel(x + 8 - j, y + 2*i + 1);
      }
    }      
  }
}

