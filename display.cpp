#include <Energia.h>
#include <UTFT.h>

#include "config.h"
#include "Memory.h"
#include "display.h"
#include "charset.h"

static UTFT d(TFT_MODEL, TFT_RS, TFT_WR, TFT_CS, TFT_RST);
static unsigned cx, cy, dx, dy, oxs;

void display::begin()
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
  oxs = dx;
}

void display::toggleSize()
{
  _double_size = !_double_size;
  d.fillScr(TFT_BG);
}

void display::error(char *s)
{
  d.fillScr(TFT_BG);
  d.setColor(TFT_FG);
  char *lines[5];
  int l = 0;
  for (char *p = s, *q = s; *p; p++)
    if (*p == '\n') {
      *p++ = 0;
      lines[l++] = q;
      q = p;
    }
  unsigned y = (dy - l*cy)/2;
  for (int i = 0; i < l; i++) {
    char *p = lines[i];
    unsigned x = (dx - strlen(p)*cx)/2;
    d.print(p, x, y);
    y += cy;
  }
}

void display::status(const char *s)
{
  d.setColor(TFT_FG);
  unsigned y = dy - cy, n = strlen(s), xs = dx - n*cx;
  for (unsigned x = oxs; x < xs; x += cx)
    d.print(" ", x, y);
  d.print(s, xs, y);
  oxs = xs;
}

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

