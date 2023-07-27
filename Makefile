t ?= esp32
s ?= uk101

ifeq ($t, esp32)
CPPFLAGS = -DDEBUGGING -DTERMINAL_SPEED=$(TERMINAL_SPEED)
UPLOAD_SPEED := 921600
LIBRARIES = FS SPIFFS

ifeq ($b, lilygo)
BOARD := ttgo-t7-v14-mini32
TERMINAL_SPEED := 115200
SERIAL_PORT := /dev/ttyACM0
CPPFLAGS += -DHARDWARE_H=\"hw/ttgo-t7-v14-mini32.h\"
LIBRARIES += FabGL WiFi

else
BOARD := node32s
CPPFLAGS = -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=5 -DTFT_DC=2 \
        -DTFT_RST=-1 -DSPI_FREQUENCY=40000000 -DLOAD_GLCD \
	-DHARDWARE_H=\"hw/node32s-example.h\"
LIBRARIES += TFT_eSPI
endif
endif

ifeq ($t, esp8266)
BOARD := d1_mini
UPLOAD_SPEED := 921600
EESZ := 4M1M
F_CPU := 80
CPPFLAGS = -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=PIN_D8 -DTFT_DC=PIN_D1 \
	   -DTFT_RST=-1 -DSPI_FREQUENCY=40000000 -DLOAD_GLCD \
	   -DHARDWARE_H=\"hw/esp8bit.h\"
LIBRARIES = TFT_eSPI
endif

ifeq ($t, tivac)
BOARD := EK-LM4F120XL
CPPFLAGS = -DHARDWARE_H=\"hw/stellarpad-example.h\"
LIBRARIES = UTFT SD
endif

ifeq ($s, uk101)
CPPFLAGS += -DUK101
FS_DIR := uk101/programs
endif

ifeq ($s, ohio)
CPPFLAGS += -DOHIO
FS_DIR := ohio/programs
endif

include $t.mk
