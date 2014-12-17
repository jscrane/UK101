#include <SPI.h>
#include <SpiRAM.h>
#include <UTFT.h>
#include <SD.h>
#include <r65emu.h>
#include <r6502.h>

#include <setjmp.h>
#include <stdarg.h>

#include "config.h"
#include "display.h"
#include "ukkbd.h"
#include "acia.h"
#include "tape.h"
#include "sprom.h"

#if defined(UK101)
#include "uk101/cegmon_jsc.h"
#include "uk101/cegmon_101.h"
#include "uk101/mon02.h"
#include "uk101/bambleweeny.h"
#include "uk101/encoder.h"
#include "uk101/toolkit2.h"
#include "uk101/exmon.h"
#if defined(ORIGINAL_BASIC)
#include "uk101/basic.h"
#else
#include "uk101/nbasic.h"
#endif

prom tk2(toolkit2, 2048);
prom enc(encoder, 2048);

static sprom sproms[] = {
	sprom(cegmon_jsc, 2048),
	sprom(monuk02, 2048),
	sprom(cegmon_101, 2048),
	sprom(bambleweeny, 2048),
};
promswitch monitors(sproms, 4, 0xf800);

#else
#include "ohio/synmon.h"
#include "ohio/syn600.h"
#include "ohio/ohiomon.h"
#include "ohio/cegmon_c2.h"
#include "ohio/osibasic.h"

static sprom sproms[] = {
	sprom(syn600, 2048);
	sprom(ohiomon, 2048);
	sprom(cegmon_c2, 2048);
};
promswitch monitors(sproms, 3, 0xf800);
#endif

static bool halted = false;

prom msbasic(basic, 8192);
ram pages[RAM_SIZE / 1024];
tape tape;
ukkbd kbd;
display disp;

void status(const char *fmt, ...) {
	char tmp[256];	
	va_list args;
	va_start(args, fmt);
	vsnprintf(tmp, sizeof(tmp), fmt, args);
	disp.clear();
	disp.error(tmp);
	va_end(args);
}

jmp_buf ex;
r6502 cpu(memory, ex, status);

void reset() {
	bool sd = hardware_reset();

	kbd.reset();	
	disp.begin();
	if (sd)
		tape.start(PROGRAMS);
	else
		disp.status("No SD Card");

	halted = (setjmp(ex) != 0);
}

void setup() {
	hardware_init(cpu);
	for (unsigned i = 0; i < RAM_SIZE; i += 1024)
		memory.put(pages[i / 1024], i);

	memory.put(sram, SPIRAM_BASE, SPIRAM_EXTENT);
#if defined(UK101)
	memory.put(tk2, 0x8000);
	memory.put(enc, 0x8800);
#endif
	memory.put(msbasic, 0xa000);

	memory.put(disp, 0xd000);
	memory.put(kbd, 0xdf00);
	memory.put(tape, 0xf000);
	monitors.set(0);

	reset();
}

void loop() {
	static const char *filename;
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
				filename = tape.advance();
				disp.status(filename);
				break;
			case PS2_F3:
				filename = tape.rewind();
				disp.status(filename);
				break;
			case PS2_F4:
				monitors.next();
				cpu.reset();
				break; 
			case PS2_F5:
				disp.clear();
				disp.status(disp.changeResolution());
				cpu.reset();
				break; 
			case PS2_F6:
				disp.status(checkpoint(tape, PROGRAMS));
				break;
			case PS2_F7:
				if (filename)
					restore(tape, PROGRAMS, filename);
				break; 
			default:
				kbd.up(key);
				break;
			}
	} else if (!halted)
		cpu.run(CPU_INSTRUCTIONS);
}
