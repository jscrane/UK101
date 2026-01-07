# ROM Disassemblies

## Approach

- Run disassembler on binary file, using e.g., [this online disassembler](https://www.masswerk.at/6502/disassembler.html).
- Find ```.DATA``` sections by searching for unknown opcodes and adding to a symbol file.
- Resolve external entry-points, e.g., in ```CEGMON``` or ```BASIC``` and add them to the symbol file.
- Invent useful names for internal subroutines and entry-points, i.e., ```L89AB```; perhaps ask [ChatGPT](https://chatgpt.com).

## ROMs

### Toolkit II
A collection of utilities commands run from ```BASIC```:

- LIST
- AUTO
- DELETE
- RENUM
- FIND
- REPL
- TRON
- TROFF
- MC
- VIEW
- DUPL

### Encoder
An Assembler / Disassembler.
