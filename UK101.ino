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
#include "roms/uk101/cegmon_jsc.h"
#include "roms/uk101/cegmon_101.h"
#include "roms/uk101/mon02.h"
#include "roms/uk101/bambleweeny.h"
#include "roms/uk101/encoder.h"
#include "roms/uk101/toolkit2.h"
#include "roms/uk101/exmon.h"
#if BASIC == ORIGINAL
#include "roms/uk101/basic.h"
#elif BASIC == BUGFIX
#include "roms/uk101/basic_bugfix.h"
#endif
#include "roms/uk101/premier_basic5.h"
#include "roms/uk101/premier_basic6.h"

prom tk2(toolkit2, 2048);
prom enc(encoder, 2048);
prom b5(basic5, 2048);
prom b6(basic6, 2048);

static sprom sproms[] = {
	sprom(cegmon_101, 2048),
	sprom(cegmon_jsc, 2048),
	sprom(monuk02, 2048),
	sprom(bambleweeny, 2048),
};
promswitch monitors(sproms, 4, 0xf800);

#else
#include "roms/ohio/syn600.h"
#include "roms/ohio/ohiomon.h"
#include "roms/ohio/cegmon_c2.h"
#include "roms/ohio/osibasic.h"

static sprom sproms[] = {
	sprom(syn600, 2048),
	sprom(ohiomon, 2048),
	sprom(cegmon_c2, 2048),
};
promswitch monitors(sproms, 3, 0xf800);
#endif

static bool halted = false;

prom msbasic(basic, 8192);
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
	memory.put(b5, 0x9000);
	memory.put(b6, 0x9800);
#endif
	memory.put(msbasic, 0xa000);

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
				filename = files.advance();
				screen.statusf("%s%s", device_names[fd], filename? filename: "No file");
				break;
			case PS2_F3:
				filename = files.rewind();
				screen.statusf("%s%s", device_names[fd], filename? filename: "No file");
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
