#include <Arduino.h>
#include <memory.h>
#include <serialio.h>
#include <acia.h>
#include <filer.h>

#include "tape.h"

tape::tape(filer &filer): Memory::Device(256), _filer(filer) {

	acia.register_read_data_handler([this]() { return on_read_data(); });
	acia.register_write_data_handler([this](uint8_t b) { on_write_data(b); });
	acia.register_can_rw_handler([this](void) { return can_rw(); });
}
