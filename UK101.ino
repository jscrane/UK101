#include <stdarg.h>

#include <SPI.h>
#include <SpiRAM.h>
#include <r65emu.h>
#include <r6502.h>
#include <acia.h>
#include <pia.h>

#include "config.h"
#include "screen.h"
#include "ukkbd.h"
#include "tape.h"
#include "sprom.h"
#include "disk.h"
#include "audio_filer.h"

#if defined(UK101)
#include "roms/uk101/cegmonuk_32.h"
#include "roms/uk101/cegmonuk_16.h"
#include "roms/uk101/monuk02_32.h"
#include "roms/uk101/monuk02_16.h"
#include "roms/uk101/encoder.h"
#include "roms/uk101/toolkit2.h"
#include "roms/uk101/exmon.h"
#include "roms/uk101/basuk01.h"
#include "roms/uk101/basuk02.h"
#include "roms/uk101/basuk03.h"
#if defined(ORIGINAL_BASIC)
#include "roms/uk101/basuk04.h"
#elif defined(PREMIER_BASIC4)
#include "roms/uk101/premier_basic4.h"
#define basuk04 premier_basic4
#endif
#include "roms/uk101/premier_basic5.h"
#include "roms/uk101/premier_basic6.h"

prom tk2(toolkit2, 2048);
prom enc(encoder, 2048);
prom basic1(basuk01, 2048);
prom basic2(basuk02, 2048);
prom basic3(basuk03, 2048);
prom basic4(basuk04, 2048);
prom basic5(premier_basic5, 2048);
prom basic6(premier_basic6, 2048);

static sprom sproms[] = {
	sprom(cegmonuk_32, 2048),
	sprom(monuk02_32, 2048),
	sprom(cegmonuk_16, 2048),
	sprom(monuk02_16, 2048),
};
promswitch monitors(sproms, 4, 0xf800);

#else
#include "roms/ohio/syn600.h"
#include "roms/ohio/ohiomon.h"
#include "roms/ohio/cegmon_c2.h"
#include "roms/ohio/basohio01.h"
#include "roms/ohio/basohio02.h"
#if defined(OSI_BASIC)
#include "roms/ohio/basohio03.h"
#elif defined(OSI_BASIC_BUGFIX)
#include "roms/ohio/basohio03_bugfix.h"
#define basohio03 basohio03_bugfix
#endif
#include "roms/ohio/basohio04.h"

prom basic1(basohio01, 2048);
prom basic2(basohio02, 2048);
prom basic3(basohio03, 2048);
prom basic4(basohio04, 2048);

static sprom sproms[] = {
	sprom(syn600, 2048),
	sprom(ohiomon, 2048),
	sprom(cegmon_c2, 2048),
};
promswitch monitors(sproms, 3, 0xf800);
#endif

static bool halted = false;

ram<> pages[RAM_PAGES];

//socket_filer files(HOSTNAME);
//serial_filer files(Serial);
flash_filer files(PROGRAMS);

// novelty wrapper to send saved files to audio device
audio_filer audio(files);
tape tape(audio);

#if defined(USE_DISK)
flash_file drive_a(1), drive_b(2), drive_c(3), drive_d(4);
disk disk(drive_a, drive_b, drive_c, drive_d);
disk_timer disk_timer;
#endif

ukkbd kbd;
screen screen;
r6502 cpu(memory);

void reset() {
	bool sd = hardware_reset();

	kbd.reset();
	screen.begin();
	files.stop();
	files.start();
#if defined(USE_DISK)
	disk.reset();
#endif

	if (!sd)
		screen.status("No SD Card");
	else if (!files.start())
		screen.status("Failed to open " PROGRAMS);
}

void setup() {
#if defined(DEBUGGING) || defined(CPU_DEBUG)
	Serial.begin(TERMINAL_SPEED);
#endif

	hardware_init(cpu);

	for (unsigned i = 0; i < RAM_PAGES; i++)
		memory.put(pages[i], i * ram<>::page_size);

#if defined(USE_SPIRAM)
	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#endif
#if defined(UK101)
	memory.put(tk2, 0x8000);
	memory.put(enc, 0x8800);
	memory.put(basic5, 0x9000);
	memory.put(basic6, 0x9800);
#endif
	memory.put(basic1, 0xa000);
	memory.put(basic2, 0xa800);
	memory.put(basic3, 0xb000);
	memory.put(basic4, 0xb800);

#if defined(USE_DISK)
	memory.put(disk, 0xc000);
	memory.put(disk_timer, 0xde00);
#endif

	memory.put(screen, 0xd000);
	memory.put(kbd, 0xdf00);

	memory.put(tape, 0xf000);
	monitors.set(0);

	reset();
}

const char *open(const char *filename) {
	if (filename) {
		files.open(filename);
		screen.status(filename);
		return filename;
	}
	screen.status("No file");
	return 0;
}

void loop() {
#if defined(CPU_DEBUG)
	static bool cpu_debug = CPU_DEBUG;
#endif
	static const char *filename;
	static uint8_t fd;
	static const char *device_names[] = { "Tape:", "A:", "B:", "C:", "D:", 0 };

	if (ps2.available()) {
		unsigned scan = ps2.read2();
		byte key = scan & 0xff;
		if (is_down(scan))
			kbd.down(key);
		else
			switch (key) {
			case PS2_F1:
				reset();
				break;
			case PS2_F2:
				filename = open(files.advance());
				break;
			case PS2_F3:
				filename = open(files.rewind());
				break;
			case PS2_F4:
				monitors.next();
				cpu.reset();
				break; 
			case PS2_F5:
				screen.clear();
				screen.status(screen.changeResolution());
				cpu.reset();
				break; 
			case PS2_F6:
				screen.status(files.checkpoint());
				break;
			case PS2_F7:
				if (filename)
					files.restore(filename);
				break; 
			case PS2_F8:
				fd++;
				if (!device_names[fd])
					fd = 0;
				files.select(fd);
				screen.statusf("%s%s", device_names[fd], filename? filename: "No file");
				break;
#if defined(CPU_DEBUG)
			case PS2_F10:
				cpu_debug = !cpu_debug;
				break;
#endif
			default:
				kbd.up(key);
				break;
			}
	} else if (!cpu.halted()) {
#if defined(CPU_DEBUG)
		if (cpu_debug) {
			char buf[256];
			Serial.println(cpu.status(buf, sizeof(buf)));
			cpu.run(1);
		} else
			cpu.run(CPU_INSTRUCTIONS);
#else
		cpu.run(CPU_INSTRUCTIONS);
#endif
	}
}
