# ROM Disassemblies

## Approach

- Run disassembler on binary file, using e.g., [this online disassembler](https://www.masswerk.at/6502/disassembler.html).
- Find ```.DATA``` sections by searching for unknown opcodes and adding to a symbol file.
- Turn on "decode illegal opcodes" once all ```.DATA``` sections have been identified: some ROMs use them.
- Resolve external entry-points, e.g., in ```CEGMON``` or ```BASIC``` and add them to the symbol file.
- Invent useful names for internal subroutines and entry-points, i.e., ```L89AB```; [ChatGPT](https://chatgpt.com)
is very useful for this.

## Toolkit II
A collection of utilities commands run from ```BASIC```:

### AUTO
Program entry with automatic line numbering in increments of 10. CTRL-T terminates input.
If a line exists already, '*' will be printed before it.

Options:
- none: start numbering at line 10
- n: start numbering at line n

```
AUTO 100
 100 REM FOO
 110 REM BAR
 120 REM BAZ
 130*
```

### LIST
A paging-aware, interruptible, range-capable reimplementation of LIST that runs alongside BASIC, not inside it.
CTRL-T: terminates paging.

```
LIST/1

 100 REM FOO
 110 REM BAR*
```

Options:
- none: list entire program
- n: list line n
- n-m: list from line n to line m
- /n: list lines in blocks of n from start, pausing after each block

### DELETE
DELETE removes one or more BASIC lines from the program.

Options:
- none: delete entire program
- n: delete line n
- n-m: delete lines between n and m

```
DELETE 100
*
LIST

 110 REM BAR
 120 REM BAZ
OK
```

### RENUM
Line renumbering.

Options:
- n: renumber all lines with new start at line n in increments of 10

```
RENUM 100
*
LIST

 100 REM BAR
 110 REM BAZ
OK
```

### FIND
Finds a text string in the program. 

Options:
- /str: search for ```str``` in the program and halt on find, hit ENTER to continue, CTRL-T to abort
- none: repeat last search

```
90 REM FOO
120 PRINT"FOO"
LIST

 90 REM FOO
 100 REM BAR
 110 REM BAZ
 120 PRINT"FOO"
OK
FIND /FOO

 90 REM FOO
--------^
 100 REM BAR
 110 REM BAZ
 120 PRINT"FOO"
-----------^*
```

### REPL
Replaces a text string in the program. Requires confirmation of each
replacement with Y. CTRL-T aborts.

Options:
- /from/to: search for ```from```, and interactively replace with ```to```

```
REPL /FOO/BAR

 90 REM FOO
--------^
REPL?
 90 REM BAR
 100 REM BAR
 110 REM BAZ
 120 PRINT"FOO"
-----------^
REPL?
 120 PRINT"BAR"*
LIST

 90 REM BAR
 100 REM BAR
 110 REM BAZ
 120 PRINT"BAR"
OK
```

### TRON and TROFF
Turns on and off tracing.

### MC
Jumps into the Machine Code Monitor.

```
MC

>0800/24 G
          TKII V1.1

[C] 1981 P.Rihan/PREMIER
OK
```

### VIEW
Prints data from tape (or serial) until a key is pressed.

### DUPL
Duplicates an existing BASIC line, inserting it at line 10. If line 10 already
exists, does nothing.

Options:
- n: the line to duplicate

Errors:
- L# (line number) if the number is missing or malformed
- BF (buffer full) if the new line is too long

```
DELETE100
*
DUPL90
LIST

 10 REM BAR
 90 REM BAR
 110 REM BAZ
 120 PRINT"BAR"
OK
```

### Encoder
An Assembler / Disassembler.
