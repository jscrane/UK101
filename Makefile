t ?= esp32
s ?= uk101
p ?= programs/$s

FS_DIR := $p
LIBRARIES = SimpleTimer PS2KeyRaw Adafruit_GFX Adafruit_BusIO Wire
TERMINAL_EXTRA_FLAGS := -C serialout.txt
CPPFLAGS = -DSIMPLE_TIMER_MICROS
#CPPFLAGS += -DDEBUGGING=0xff
TERMINAL_SPEED := 115200

ifeq ($t, rp2040)
BOARD := adafruit_feather_dvi
flash := 8388608_2097152
CPPFLAGS += -DDVI_BIT_DEPTH=1 -DDVI_RESOLUTION=DVI_RES_640x480p60
LIBRARIES += LittleFS PicoDVI
endif

ifeq ($t, esp32)
UploadSpeed := 921600
LIBRARIES += FS

ifeq ($b, lilygo)
BOARD := ttgo-t7-v14-mini32
SERIAL_PORT := /dev/ttyACM0
CPPFLAGS += -DVGA_BIT_DEPTH=1 -DVGA_RESOLUTION=MODE640x480 -DUSE_SD
LIBRARIES += ESP32Lib SD

else ifeq ($b, olimex)
BOARD := esp32-sbc-fabgl
SERIAL_PORT := /dev/ttyUSB0
DebugLevel := verbose
CPPFLAGS += -DVGA_BIT_DEPTH=1 -DVGA_RESOLUTION=MODE640x480 -DVGA_DEFAULT_FONT=Font8x8
LIBRARIES += ESP32Lib SPIFFS

else
BOARD := lolin32
CPPFLAGS += -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=5 -DTFT_DC=2 \
        -DTFT_RST=-1 -DSPI_FREQUENCY=40000000 -DLOAD_GLCD -DHARDWARE_H=\"hw/esp32-example.h\"
LIBRARIES += TFT_eSPI SPIFFS
endif
endif

ifeq ($t, esp8266)
BOARD := d1_mini
baud := 921600
eesz := 4M2M
xtal := 80
CPPFLAGS += -DUSER_SETUP_LOADED -DILI9341_DRIVER -DTFT_CS=PIN_D8 -DTFT_DC=PIN_D1 \
	-DTFT_RST=-1 -DSPI_FREQUENCY=40000000 -DLOAD_GLCD -DHARDWARE_H=\"hw/esp8bit.h\"
LIBRARIES += TFT_eSPI FS LittleFS
endif

ifeq ($s, uk101)
CPPFLAGS += -DUK101
endif

ifeq ($s, ohio)
CPPFLAGS += -DOHIO
endif

include $t.mk
