#include <Arduino.h>
#include <memory.h>
#include <serialio.h>
#include <acia.h>
#include <filer.h>

#include "tape.h"

void tape::operator=(uint8_t b) {
	ACIA::write(_acc, b);
}

tape::operator uint8_t() {
	return ACIA::read(_acc);
}
