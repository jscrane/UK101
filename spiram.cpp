#include <SPI.h>
#include <SpiRAM.h>
#include "Memory.h"
#include "spiram.h"
#include "config.h"

extern SPIClass SPIRAM_DEV;

SpiRAM spiRam(SPIRAM_DEV, SPIRAM_CS);

void spiram::begin(byte cs, int module)
{
  SPI_for_SD.setModule(module);
  SPI_for_SD.setClockDivider(SPI_CLOCK_DIV16);
  SPI_for_SD.setDataMode(SPI_MODE0);
  pinMode(PF_3, OUTPUT);
  pinMode(cs, OUTPUT);
}

void spiram::operator=(byte b)
{
  spiRam.write_byte(_acc, b);
}

spiram::operator byte()
{
  return spiRam.read_byte(_acc);
}

void spiram::checkpoint(Stream &s)
{
  // FIXME
}

void spiram::restore(Stream &s)
{
  // FIXME
}

