#ifndef __CONFIG_H__
#define __CONFIG_H__

#define UK101
//#define SUPERBOARD

#define TFT_BG		VGA_BLACK
#define TFT_FG		VGA_WHITE
//#define TFT_BG	VGA_WHITE
//#define TFT_FG	VGA_BLACK

// X-offset (in characters)
#if defined(UK101)
#define X_OFF		 12
#define CHARS_PER_LINE   64
#define DISPLAY_RAM_SIZE 2048
#else
#define X_OFF          0
#define CHARS_PER_LINE 32
#define DISPLAY_RAM_SIZE 1024
#endif

// RAM provided by uC (must be a multiple of 1024)
#define RAM_SIZE	0x2000

// SPI-RAM
#define SPIRAM_BASE     0x2000

// number of CPU instructions to run per loop
#define CPU_INSTRUCTIONS  1000

// for the original (to me) Compukit basic
// if not defined, get the string garbage-collection fix
// from: http://neoncluster.com
#if defined(UK101)
//#define ORIGINAL_BASIC
#else
#define OSI_BASIC
#endif

#endif
