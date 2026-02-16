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

#define TICK_FREQ	1000	// 1ms
#endif

ukkbd kbd;
ps2_raw_kbd keyboard(kbd);
screen screen;
Memory memory;
r6502 cpu(memory);
Arduino machine(cpu);

static void file_status() {
	static const char *device_names[MAX_FILES] = { "Tape:", "A:", "B:", "C:", "D:" };
	const char *filename = files.filename();
	screen.statusf("%s%s", device_names[files.device()], filename? filename: "No file");
}

static void function_keys(uint8_t key) {
	const char *filename = files.filename();

	switch (key) {
	case 1:
		machine.reset();
		return;
	case 2:
		files.next_file();
		break;
	case 3:
		files.rewind();
		break;
	case 4:
		monitors.next();
		cpu.reset();
		return;
	case 5:
		screen.clear();
		screen.status(screen.changeResolution());
		cpu.reset();
		return;
	case 6:
		filename = files.checkpoint();
		break;
	case 7:
		if (filename)
			files.restore(filename);
		return;
	case 8:
		files.next_device();
		break;
	case 10:
		machine.debug_cpu();
		return;
	}
	file_status();
}

static void reset(bool sd) {

	keyboard.reset();
	screen.begin();
	files.reset();
#if defined(USE_DISK)
	disk.reset();
#endif

	if (!sd) {
		DBG_EMU("No SD Card");
		screen.status("No SD Card");
	} else if (!files.start()) {
		DBG_EMU("Failed to open " PROGRAMS);
		screen.status("Failed to open " PROGRAMS);
	} else
		file_status();
}

void setup() {
	machine.begin();

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

	machine.interval_timer(TICK_FREQ, []() { disk.tick(); });
#endif

	memory.put(screen, 0xd000);
	memory.put(kbd, 0xdf00);

	memory.put(tape, 0xf000);
	monitors.set(0);

	keyboard.register_fnkey_handler(function_keys);
	machine.register_pollable(keyboard);

	machine.register_reset_handler(reset);
	machine.reset();
}

void loop() {
	machine.run();
}
