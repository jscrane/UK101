ENERGIADIR := /usr/local/energia
ENERGIAFAMILY := lm4f
COMPILERFAMILY := arm-none-eabi
ENERGIABOARD := lplm4f120h5qr
SOURCES := r6502.cpp Memory.cpp uk101.ino
LIBRARIES := SPI SpiRAM UTFT

include /usr/local/share/energia-makefile/energia.mk
