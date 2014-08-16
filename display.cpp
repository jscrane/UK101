#include <Energia.h>
#include <UTFT.h>

#include "config.h"
#include "Memory.h"
#include "display.h"
#include "charset.h"

static UTFT d(TFT_MODEL, TFT_RS, TFT_WR, TFT_CS, TFT_RST);
static unsigned cx, cy, dx, dy;

display::display() : Memory::Device(sizeof(_mem))
{
  extern uint8_t SmallFont[];
  
#if defined(TFT_BACKLIGHT)
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);
#endif
  d.InitLCD();
  d.fillScr(TFT_BG);
  d.setFont(SmallFont);
  dx = d.getDisplayXSize();
  dy = d.getDisplayYSize();
  cx = d.getFontXsize();  
  cy = d.getFontYsize();
}

void display::status(const char *s)
{
  unsigned x = dx - cx*strlen(s), y = dy - cy;
  d.setColor(TFT_FG);
  d.print(s, x, y);
}

// either 16 or 32
#define LINES (DISPLAY_SIZE / 64)
// either 16 or 8
#define CHAR_HT (256 / LINES)

void display::_set(Memory::address a, byte c)
{
  if (c != _mem[a]) {
    _mem[a] = c;  
    int x = 8 * (a % 64), y = CHAR_HT * (a / 64);
    
    // FIXME: hack!
    x -= 13 * 8;
    
    if (x < 0 || x >= dx || y < 0 || y >= dy)
      return;
      
    for (int i = 0; i < 8; i++) {
      byte b = charset[c][i];
      for (int j = 0; j < 8; j++) {
        d.setColor((b & (1 << j))? TFT_FG: TFT_BG);
#if CHAR_HT == 8        
        d.drawPixel(x + 8 - j, y + i);
#else
        d.drawPixel(x + 8 - j, y + 2*i);
        d.drawPixel(x + 8 - j, y + 2*i + 1);
#endif
      }
    }      
  }
}

