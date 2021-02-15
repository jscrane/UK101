UK101
=====

[UK101](https://en.wikipedia.org/wiki/Compukit_UK101) Hardware emulator on 
[Stellarpad](http://www.energia.nu/Guide_StellarisLaunchPad.html) using 
[Energia](http://energia.nu/) and ESP8266 / ESP32 with Arduino.

See blog [article](http://programmablehardware.blogspot.ie/2014/08/retrocomputer-resurrection.html).

Software
--------
- [6502 Emulation](https://github.com/jscrane/r65emu) library
- [uC-Makefile](https://github.com/jscrane/uC-Makefile)

Hardware
--------
- See the [6502 Emulation](https://github.com/jscrane/r65emu) library's
  hardware requirements and wiring.
- See _config.h_ for configuration options.

Firmware
--------
Some of the function keys on the PS/2 keyboard control the machine's inner workings:
- F1: full reset (cpu, keyboard, SD card, etc.)
- F2: advance tape
- F3: rewind tape
- F4: toggle monitor rom
- F5: change display resolution: 
  - 40x30, 40x15, 45x30, 45x32, 45x16 (on UK101)
  - 32x30, 32x32 (on Superboard)
- F6: checkpoint to file (default CHKPOINT.xxx)
- F7: restore from checkpoint (select image with F2)

Keyboard
--------
- Some keys on the Numeric Keypad have special mappings:
  - NUM: ^E, 0: ^Q
  - 2: ^F, 4: ^A, 6: ^D, 8: ^S
  - /, *, -, +
