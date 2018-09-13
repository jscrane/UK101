/*
 * tape.cpp -- UK101 Tape Interface
 */
#include <Arduino.h>
#include <stdint.h>
#include <memory.h>
#include <sdtape.h>
#include <acia.h>
#include <hardware.h>
#include <sound_pwm.h>
#include "config.h"
#include "tape.h"

#if defined(PWM_SOUND)
static PWM pwm;
#endif

// FIXME: control register
// initialisation sequence is:
// 0x03: reset
// 0x11: cd16 | ws8n2
void tape::reset() {
#if defined(PWM_SOUND)
	pwm.begin(PWM_SOUND);
#endif
}

static void write_bit(bool bit) {
#if defined(PWM_SOUND)
	pwm.set_freq(bit? 2400: 1200);
#endif
	delayMicroseconds(3333);
}

void tape::write(uint8_t b) {
	pwm.set_duty(PWM_DUTY);
	write_bit(0);
	for (int i = 0; i < 8; i++) {
		write_bit(b & 1);
		b >>= 1;
	}
	write_bit(1);
	write_bit(1);
	pwm.set_duty(0);
}
