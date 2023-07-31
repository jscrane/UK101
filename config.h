#ifndef __CONFIG_H__
#define __CONFIG_H__

#if !defined(UK101) && !defined(OHIO)
#define UK101
#endif

#define TFT_BG		BLACK
#define TFT_FG		WHITE
//#define TFT_BG	WHITE
//#define TFT_FG	BLACK

#if defined(USE_UTFT)
#define TFT_ORIENT      landscape
#elif defined(USE_ESPI)
#define TFT_ORIENT      reverse_landscape
#elif defined(USE_VGA)
#define TFT_ORIENT      landscape
#endif

// X-offset (in characters)
#if defined(UK101)
#define X_OFF		12
#define CHARS_PER_LINE	64
#define DISPLAY_RAM_SIZE 2048
#elif defined(OHIO)
#define X_OFF		0
#define CHARS_PER_LINE	32
#define DISPLAY_RAM_SIZE 1024
#endif

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

#define RAM_PAGES	(RAM_SIZE / ram::page_size)

// number of CPU instructions to run per loop
#define CPU_INSTRUCTIONS  1000

// flash_filer directory containing programs
#define PROGRAMS	"/"

// for socket_filer
#if defined(UK101)
#define HOSTNAME	"uk101"
#else
#define HOSTNAME	"ohio"
#endif

// for the original (to me) Compukit basic
// if not defined, get the string garbage-collection fix
// from: http://neoncluster.com
#if defined(UK101)
//#define ORIGINAL_BASIC
#elif defined(OHIO)
#define OSI_BASIC
#endif

#endif
