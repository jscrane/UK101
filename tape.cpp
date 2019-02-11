/*
 * tape.cpp -- UK101 Tape Interface
 */
#include <Arduino.h>
#include <stdint.h>
#include <memory.h>
#include <serialio.h>
#include <filer.h>
#include <acia.h>
#include <hardware.h>
#include <sound_pwm.h>
#include "config.h"
#include "tape.h"

extern PWM pwm;

void tape::reset() {
	speed(BAUD_RATE);
#if defined(PWM_DUTY)
	pwm.stop();
#endif
}

void tape::framing(unsigned data_bits, unsigned stop_bits, parity parity) {
	_data_bits = data_bits;
	_stop_bits = stop_bits;
	_parity = parity;
}

void tape::write_bit(bool bit) {
	pwm.set_freq(bit? 2400: 1200);
	delayMicroseconds(_bit_delay);
}

// FIXME: parity
void tape::write(uint8_t b) {
#if defined(PWM_DUTY)
	pwm.set_duty(PWM_DUTY);
#endif
	write_bit(0);
	for (int i = 0; i < _data_bits; i++) {
		write_bit(b & 1);
		b >>= 1;
	}
	write_bit(1);
	if (_stop_bits == 2)
		write_bit(1);
	pwm.set_duty(0);
}
