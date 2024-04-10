IMAGES := $(wildcard *.rom)
HEADERS := $(IMAGES:.rom=.h)

all: $(HEADERS)

clean:
	rm -f $(HEADERS)

%.h: %.rom
	makerom -b -p $< $* > $@

.PHONY: all clean
