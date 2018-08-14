/*
 * tape.cpp -- UK101 Tape Interface
 */
#include <stdint.h>
#include <memory.h>
#include <sdtape.h>
#include <acia.h>
#include "tape.h"

void tape::operator=(uint8_t b) {
	// FIXME: saving
}

tape::operator uint8_t() {
	if (_acc & 1)			// read data
		return read();

	return more()? rdrf | tdre: 0;
}
