# ROM Disassemblies

These are symbol and listing files for some ROMs of interest. The listing file is produced by an online disassembler
from a binary image and a symbol file.

This was the general approach to iterating a symbol file:
- Run disassembler on binary file, using e.g., [this online disassembler](https://www.masswerk.at/6502/disassembler.html).
- Find ```.DATA``` sections by searching for unknown opcodes and adding to a symbol file.
- Turn on "decode illegal opcodes" once all ```.DATA``` sections have been identified: some ROMs use them.
- Resolve external entry-points, e.g., in ```CEGMON``` or ```BASIC``` and add them to the symbol file.
- Invent useful names for internal subroutines and entry-points, i.e., ```L89AB```; [ChatGPT](https://chatgpt.com)
is very useful for this.

## ROMS

- [Toolkit II](toolkit2.md)
- [Encoder](encoder.md)
