ENERGIADIR := /usr/local/energia
ENERGIAFAMILY := lm4f
COMPILERFAMILY := arm-none-eabi
ENERGIABOARD := lplm4f120h5qr
SOURCES := r6502.cpp Memory.cpp uk101.ino
LIBRARIES := SPI Adafruit_GFX Adafruit_ST7735

include /usr/local/share/energia-makefile/energia.mk
