#include <Energia.h>
#include "ps2drv.h"

static volatile uint8_t last;
static volatile bool isbrk;
static uint8_t DataPin;

// The ISR for the external interrupt
void ps2interrupt(void)
{
  static uint8_t bitcount=0;
  static uint8_t incoming=0;
  static uint32_t prev_ms=0;
  uint32_t now_ms;
  uint8_t n, val;

  val = digitalRead(DataPin);
  now_ms = millis();
  if (now_ms - prev_ms > 250) {
    bitcount = 0;
    incoming = 0;
  }
  prev_ms = now_ms;
  n = bitcount - 1;
  if (n <= 7) {
    incoming |= (val << n);
  }
  bitcount++;
  if (bitcount == 11) {
    if (incoming == 0xf0) {
      last = 0;
      isbrk = true;
    } else {
      last = incoming;
    }
    bitcount = 0;
    incoming = 0;
  }
}

bool PS2Driver::available() {
  return last != 0;
}

bool PS2Driver::isbreak() {
  bool b = isbrk;
  isbrk = false;
  return b;
}

int PS2Driver::read() {
  if (!last)
    return -1;
  uint8_t res = last;
  last = 0;
  return res;
}

void PS2Driver::begin(uint8_t data_pin, uint8_t irq_pin)
{
  DataPin = data_pin;
  pinMode(irq_pin, INPUT_PULLUP);
  pinMode(data_pin, INPUT_PULLUP);
  attachInterrupt(irq_pin, ps2interrupt, FALLING);
}
