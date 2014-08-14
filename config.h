#ifndef __CONFIG_H__
#define __CONFIG_H__

// TFT display...
// Daniel Rebollo's boosterpack
// #define TFT_BACKLIGHT	PD_6
// #define TFT_MODEL	SSD1289
// TFT01_2.4: http://www.elecfreaks.com/store/24-tft-lcd-tft0124-p-110.html
#undef TFT_BACKLIGHT
#define TFT_MODEL	S6D1121_8
#define TFT_RS		PC_6
#define TFT_WR		PC_5
#define TFT_CS		PC_7
#define TFT_RST		PC_4
#define TFT_BG                VGA_BLACK
#define TFT_FG                VGA_WHITE
//#define TFT_BG          VGA_WHITE
//#define TFT_FG          VGA_BLACK

// must be a multiples of 1024
#define RAM_SIZE  8192
#define DISPLAY_SIZE 2048

// PS/2 keyboard
#define KBD_DEV    PS2
#define KBD_DATA    PB_0
#define KBD_IRQ     PB_1
//#define KBD_DEV  Serial

// various monitor roms...
//#define MONITOR "cegmon_c2.h"
//#define MONITOR "cegmon_101.h"
#define MONITOR  "cegmon_jsc.h"
//#define MONITOR  "bambleweeny.h"
//#define MONITOR "mon02.h"

// "tape" storage...
#define SD_CS  39
#define SD_SPI  1

// watch the CPU execute (slowly) on the serial port
//#define CPU_DEBUG 1

#endif
