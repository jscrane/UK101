UK101-Energia
=============

[UK101](https://en.wikipedia.org/wiki/Compukit_UK101) Hardware emulator on [Stellarpad](http://www.energia.nu/Guide_StellarisLaunchPad.html) using [Energia](http://energia.nu/).

Software:
---------
- [My port](https://github.com/jscrane/UTFT-Energia) 
of Henning Karlsen's [UTFT library]
(http://henningkarlsen.com/electronics/library.php?id=52),
- [Rei Vilo's port](https://github.com/rei-vilo/SD_TM4C) of 
Adafruit/Sparkfun's [SD library](https://github.com/adafruit/SD),
- [My port](https://github.com/jscrane/SpiRAM) 
of Phil Stewart's [SpiRAM library](http://playground.arduino.cc/Main/SpiRAM)

Hardware:
---------
- A [Stellaris Launchpad](http://www.energia.nu/Guide_StellarisLaunchPad.html),
- A supported TFT screen, such as [this one](http://forum.stellarisiti.com/topic/626-ssd1289-32-320x240-tft-16bit-parallel-interface-touch-libraries/),
- An SD drive,
- A PS/2 keyboard.
- A 23k256 SPI RAM chip (optional)
(see config.h for wiring details)

Firmware:
---------
- F1: full reset (cpu, keyboard, SD card, etc.)
- F2: advance tap
- F3: rewind tape
- F4: toggle monitor rom
- F5: toggle display 16/32 lines
- F6: checkpoint to file (default CHKPOINT.xxx)
- F7: restore from checkpoint (select image with F2)
