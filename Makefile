BOARD := node32s
UPLOAD_SPEED := 921600
TERM_SPEED := 115200
CPPFLAGS = -DCPU_DEBUG

uk101: CPPFLAGS += -DUK101
uk101: SPIFFS_DIR := uk101/programs
uk101: all fs

ohio: CPPFLAGS += -DOHIO
ohio: SPIFFS_DIR := ohio/programs
ohio: all fs

include arduino-esp32.mk
