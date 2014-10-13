PROCESSOR_FAMILY := lm4f
BOARD := lplm4f120h5qr
SKETCH = uk101.ino
SOURCES = display.cpp Memory.cpp ps2drv.cpp r6502.cpp spiram.cpp tape.cpp ukkbd.cpp

include ~/src/uC-Makefile/energia.mk
