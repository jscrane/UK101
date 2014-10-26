/*
 * tape.cpp -- UK101 Tape Interface
 */
#include "memory.h"
#include "sdtape.h"
#include "acia.h"
#include "tape.h"

void tape::operator=(byte b) {
	// FIXME: saving
}

tape::operator byte() {
	if (_acc & 1)			// read data
		return read();

	return more()? rdrf | tdre: 0;
}
