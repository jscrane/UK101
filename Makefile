t ?= esp32

ifeq ($t, esp32)
BOARD := node32s
UPLOAD_SPEED := 921600
TERM_SPEED := 115200

CPPFLAGS = -DDEBUG -DCPU_DEBUG \
	-DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=5 -DTFT_DC=2 \
        -DTFT_RST=-1 -DTFT_WIDTH=240 -DTFT_HEIGHT=320 \
        -DSPI_FREQUENCY=40000000 -DLOAD_GLCD
LIBRARIES = TFT_eSPI FS SPIFFS
endif

ifeq ($t, esp8266)
BOARD := d1_mini
UPLOAD_SPEED := 921600
FLASH_SIZE := 4M1M
BUILD_FCPU := 80000000L

CPPFLAGS = -DDEBUG -DCPU_DEBUG \
	-DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=PIN_D6 -DTFT_DC=PIN_D8 \
        -DTFT_RST=-1 -DTFT_WIDTH=240 -DTFT_HEIGHT=320 \
        -DSPI_FREQUENCY=40000000 -DLOAD_GLCD
LIBRARIES = TFT_eSPI
endif

ifeq ($t, lm4f)
PROCESSOR_FAMILY := lm4f
BOARD := lplm4f120h5qr
CPPFLAGS = -DDEBUGGING -DHARDWARE_H=\"hw/lm4f-utft-sd.h\"
LIBRARIES = UTFT SD SpiRAM
endif

uk101: CPPFLAGS += -DUK101
uk101: SPIFFS_DIR := uk101/programs
uk101: all

ohio: CPPFLAGS += -DOHIO
ohio: SPIFFS_DIR := ohio/programs
ohio: all

include arduino-$t.mk
