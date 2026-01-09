# ROM Disassemblies

## Approach

- Run disassembler on binary file, using e.g., [this online disassembler](https://www.masswerk.at/6502/disassembler.html).
- Find ```.DATA``` sections by searching for unknown opcodes and adding to a symbol file.
- Resolve external entry-points, e.g., in ```CEGMON``` or ```BASIC``` and add them to the symbol file.
- Invent useful names for internal subroutines and entry-points, i.e., ```L89AB```; perhaps ask [ChatGPT](https://chatgpt.com).

## Toolkit II
A collection of utilities commands run from ```BASIC```:

- in immediate mode, prints '*'
- CTRL-T is used to abort some commands

### LIST

A paging-aware, interruptible, range-capable reimplementation of LIST that runs alongside BASIC, not inside it.

Options:
- none: list entire program
- n: list from line n to end
- n-m: list from line n to line m
- /n: list n lines from current
- CTRL-T: terminates listing

### AUTO

Automatic line numbering in increments of 10.

Options:
- none: start numbering at line 10
- n: start numbering at line n

### RENUM

Line renumbering.

Options:
- n: renumber all lines with new start at line n
- n,m: renumber all lines from line m, starting at line n
- n,m,i: renumber all lines from line m, starting at line n, in increments of i

### DELETE

DELETE removes one or more BASIC lines from the program.

Options:
- n: delete line n
- n-m: delete lines between n and m

### VIEW

Prints data from serial or tape until a key is pressed.

- FIND
- REPL
- TRON
- TROFF
- MC
- DUPL

### Encoder
An Assembler / Disassembler.
