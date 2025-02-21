#ifndef __CONFIG_H__
#define __CONFIG_H__

#if !defined(UK101) && !defined(OHIO)
#define UK101
#endif

#define TFT_BG		BLACK
#define TFT_FG		WHITE

#if defined(USE_UTFT)
#define TFT_ORIENT      landscape
#elif defined(USE_ESPI)
#define TFT_ORIENT      reverse_landscape
#elif defined(USE_VGA_FABGL) || defined(USE_VGA_BITLUNI)
#define TFT_ORIENT      landscape
#elif defined(USE_DVI)
#define TFT_ORIENT      landscape
#endif

// X-offset (in characters)
#if defined(UK101)
#define CHARS_PER_LINE	64
#define DISPLAY_RAM_SIZE 2048
#elif defined(OHIO)
#define CHARS_PER_LINE	32
#define DISPLAY_RAM_SIZE 1024
#endif
#define DISPLAY_LINES	(DISPLAY_RAM_SIZE / CHARS_PER_LINE)

// default baud rate for tape
#define BAUD_RATE	300

// RAM
#if defined(USE_SPIRAM)
#define SPIRAM_BASE     RAM_SIZE
#if defined(UK101)
#define SPIRAM_EXTENT	(0xa000u - RAM_SIZE) / Memory::page_size
#elif defined(OHIO)
#define SPIRAM_EXTENT	(0xb000u - RAM_SIZE) / Memory::page_size
#endif
#endif

#define RAM_PAGES	(RAM_SIZE / ram<>::page_size)

// flash_filer directory containing programs
#define PROGRAMS	"/"

// for socket_filer
#if defined(UK101)
#define HOSTNAME	"uk101"
#else
#define HOSTNAME	"ohio"
#endif

#if defined(UK101)
#define ORIGINAL_BASIC
//#define PREMIER_BASIC4
#elif defined(OHIO)
//#define OSI_BASIC
#define OSI_BASIC_BUGFIX
#endif

// floppy disk
#define USE_DISK	DISK525
//#define USE_DISK	DISK8

#endif
