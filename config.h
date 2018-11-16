#ifndef __CONFIG_H__
#define __CONFIG_H__

#if !defined(UK101) && !defined(OHIO)
#define UK101
#endif

#define TFT_BG		BLACK
#define TFT_FG		WHITE
//#define TFT_BG	WHITE
//#define TFT_FG	BLACK

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
#if !defined(SPIRAM_CS)

// RAM provided by uC (must be a multiple of 1024)
#define RAM_SIZE	0x8000
#else

#define RAM_SIZE	0x3000
#define SPIRAM_BASE     0x3000
#if defined(UK101)
#define SPIRAM_EXTENT	(20 * 1024 / 256)
#elif defined(OHIO)
#define SPIRAM_EXTENT	(28 * 1024 / 256)
#endif
#endif

// number of CPU instructions to run per loop
#define CPU_INSTRUCTIONS  1000

#define PROGRAMS	"/"

// for the original (to me) Compukit basic
// if not defined, get the string garbage-collection fix
// from: http://neoncluster.com
#if defined(UK101)
//#define ORIGINAL_BASIC
#elif defined(OHIO)
#define OSI_BASIC
#endif

#endif
