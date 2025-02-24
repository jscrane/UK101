#include <Arduino.h>
#include <memory.h>
#include <serialio.h>
#include <acia.h>
#include <filer.h>

#include "tape.h"

tape::tape(filer &filer): Memory::Device(256), _filer(filer) {

	acia.register_read_data_handler([this]() { return _filer.read(); });
	acia.register_write_data_handler([this](uint8_t b) { _filer.write(b); });
	acia.register_can_rw_handler([this](void) { return _filer.more()? 3: 2; });
}
