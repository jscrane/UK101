#ifndef __CONFIG_H__
#define __CONFIG_H__

// TFT display...
// NOTE: edit memorysaver.h to select the correct chip for your display!
// Daniel Rebollo's boosterpack
#define TFT_BACKLIGHT	PD_6
#define TFT_MODEL	SSD1289
// TFT01_2.4: http://www.elecfreaks.com/store/24-tft-lcd-tft0124-p-110.html
// #undef TFT_BACKLIGHT
// #define TFT_MODEL	S6D1121_8
#define TFT_RS		PC_6
#define TFT_WR		PC_5
#define TFT_CS		PC_7
#define TFT_RST		PC_4
#define TFT_BG                VGA_BLACK
#define TFT_FG                VGA_WHITE
//#define TFT_BG          VGA_WHITE
//#define TFT_FG          VGA_BLACK

// must be a multiple of 1024
#define RAM_SIZE  8192

// PS/2 and Serial keyboards
//#define SER_KBD
//#define KBD_DEV  Serial
#define PS2_KBD
#define KBD_DEV  PS2
#define KBD_DATA    PE_4
#define KBD_IRQ     PE_5

// "tape" storage...
#define SD_CS  39
#define SD_SPI  1

// watch the CPU execute (slowly) on the serial port
//#define CPU_DEBUG 1

#endif
