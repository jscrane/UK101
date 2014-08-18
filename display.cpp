#include <Energia.h>
#include <UTFT.h>

#include "config.h"
#include "Memory.h"
#include "display.h"
#include "charset.h"

static UTFT d(TFT_MODEL, TFT_RS, TFT_WR, TFT_CS, TFT_RST);
static unsigned cx, cy, dx, dy;

display::display() : Memory::Device(sizeof(_mem)), _double_size(true)
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

void display::toggleSize()
{
  _double_size = !_double_size;
  d.fillScr(TFT_BG);
}

void display::status(const char *s)
{
  unsigned x = dx - cx*12, y = dy - cy, n = strlen(s);
  d.setColor(TFT_FG);
  d.print(s, x, y);
  for (x += cx * n; x < dx; x += cx)
    d.print(" ", x, y);
}

#define CHARS_PER_LINE 64

void display::_set(Memory::address a, byte c)
{
  if (c != _mem[a]) {
    _mem[a] = c;  
    int x = 8 * (a % CHARS_PER_LINE - X_OFF);  // hack to view left edge of screen
    if (x < 0 || x >= dx)
      return;
     
    int y = (_double_size? 16: 8) * (a / CHARS_PER_LINE);    
    if (y < 0 || y >= dy)
      return;

    for (int i = 0; i < 8; i++) {
      byte b = charset[c][i];
      for (int j = 0; j < 8; j++) {
        d.setColor((b & (1 << j))? TFT_FG: TFT_BG);
        if (_double_size) {
          d.drawPixel(x + 8 - j, y + 2*i);
          d.drawPixel(x + 8 - j, y + 2*i + 1);
        } else
          d.drawPixel(x + 8 - j, y + i);
      }
    }      
  }
}

void display::checkpoint(Stream &s)
{
  s.write(_double_size); 
  s.write(_mem, sizeof(_mem));
}

void display::restore(Stream &s)
{
  d.fillScr(TFT_BG);
  _double_size = s.read();
  for (int i = 0; i < sizeof(_mem); i++)
    _set(i, s.read());
}

