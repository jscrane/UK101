/*
 * audio_filer.cpp -- novelty tape output device
 */
#include <Arduino.h>
#include <stdint.h>

#include <machine.h>
#include <memory.h>
#include <serialio.h>
#include <filer.h>
#include <acia.h>

#include "config.h"
#include "audio_filer.h"

void audio_filer::reset() {
	speed(BAUD_RATE);
#if defined(PWM_SOUND)
	noTone(PWM_SOUND);
#endif
}

void audio_filer::framing(unsigned data_bits, unsigned stop_bits, parity parity) {
	_data_bits = data_bits;
	_stop_bits = stop_bits;
	_parity = parity;
}

void audio_filer::write_bit(bool bit) {
#if defined(PWM_SOUND)
	tone(PWM_SOUND, bit? 2400: 1200);
#endif
	delayMicroseconds(_bit_delay);
}

// FIXME: parity
void audio_filer::write(uint8_t b) {
	write_bit(0);
	for (int i = 0; i < _data_bits; i++) {
		write_bit(b & 1);
		b >>= 1;
	}
	write_bit(1);
	if (_stop_bits == 2)
		write_bit(1);
#if defined(PWM_SOUND)
	noTone(PWM_SOUND);
#endif
}
