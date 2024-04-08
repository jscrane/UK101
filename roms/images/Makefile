IMAGES := basuk01 basuk02 basuk03 basuk04 monuk02_16 monuk02_32 cegmonuk_16 cegmonuk_32 toolkit2 encoder
HEADERS := $(foreach i,$(IMAGES),$i.h) 

all: $(HEADERS)

clean:
	rm -f $(HEADERS)

%.h: %.rom
	makerom -b -p $< $* > $@
