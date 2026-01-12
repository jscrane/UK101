# ENCODER
## 6502 Assembler / Disassembler

**Premier Publications (UK) / Ohio Quality Software**  
© 1982 Premier Publications

---

## Overview

ENCODER is an EPROM-based, full-featured 6502 assembler/disassembler supporting full
mnemonics, labels, hexadecimal, decimal, binary, and ASCII input.

ENCODER operates from BASIC workspace. Utility EPROMs such as **Premier Toolkit II**
may be used in conjunction with ENCODER to provide FIND, AUTO, and REPLACE functions.
ENCODER also integrates directly with the **CEGMON** full-screen editor.

This manual provides operational guidance only and is **not** a complete course
in 6502 programming. New users are encouraged to consult the recommended books
listed near the end of this manual.

---

## Initialisation

To enter ENCODER, use one of the following methods.  
`xxxx` refers to the EPROM start address (`8000`, `8800`, `9000`, or `9800`).

- `RESET M xxxxG`
- From BASIC 5: `&GO$xxxx`
- `POKE 11,xx : POKE 12,xx : X=USR(X)`
- `CALL xxxx` (new BASIC only)

After initialisation, the prompt appears:

```
ENCODER A/D ?
```

This requests **Assemble** or **Disassemble**.

---

## Disassemble Mode

Press `D` to enter the disassembler.

Prompt:
```
D$
```

Enter the starting address followed by `RETURN`. Fifteen lines of code will be displayed.

### Controls

1. `LF`, `CTRL-J`, or **Up Arrow** — continue with the next fifteen lines  
2. `Dxxxx` — restart disassembly from address `xxxx`  
3. `RETURN` — enter **CEGMON** monitor command mode  

While in the monitor:
- Press `.U` to re-enter ENCODER
- Press `RESET` (`BREAK` on OHIOs) to reinitialise ENCODER

---

## Assemble Mode

Press `A` after initialisation.

Prompt:
```
M/L ?
```

ENCODER now asks whether to assemble from **Memory** or **Tape Load**.

### Tape Load (`L`)

- Assembles source directly from cassette
- Ensure a clear header tone before pressing `L`
- Syntax errors return control to BASIC
- Up to **1200 baud** supported (hardware dependent)

### Memory (`M`)

- Assembles source code written in BASIC workspace
- Test result: approximately **1000 lines in 72 seconds**

---

## Options

Press **SHIFT-P** to return to the `ENCODER A/D ?` prompt.

> Toolkit II users should note that shared I/O vectors disable SHIFT-P functionality.

At the prompt:

- `RETURN` → return to BASIC
- `A RETURN` → enter monitor command mode
- `D RETURN` → return to ENCODER

### Printer Output

- Source code: use BASIC `SAVE:LIST`
- Assembly/disassembly: set SAVE flag then use `A` or `D`

> Printing greatly reduces assembly speed.

---

## Writing Source Code

The first line of your source must contain:

```
*$xxxx
or
*$xxxx$yyyy
```

Where:
- `xxxx` is the assembly address
- `yyyy` is the final execution address

Example:
```
10 *$1000$A000
```

### Comments

Use `;` immediately after the operand:

```
20 :SCRN=$D000; define screen label
```

Comments appear in listings only and do not affect object code.

### Mnemonics

All mnemonics must be followed by a space:

```
30 LDX #$00
```

Avoid accidentally forming BASIC keywords when combining labels and mnemonics.

---

## Labels

- Up to **4 alphanumeric characters**
- Must be preceded by `:`
- Maximum of **64 labels**

Examples:
```
:LOOP
:LABL=$1234
```

Reset a label for reuse:
```
:LABL=$FFFF
```

Labels may be referenced before definition. ENCODER resolves them when found.

---

## Addressing Modes

The 6502 processor supports the following addressing modes:

- Immediate
- Implied
- Accumulator
- Relative
- Absolute
- Absolute Indexed
- Zero Page
- Zero Page Indexed
- Indirect
- Indexed Indirect
- Indirect Indexed

### Example (LDA)

| Instruction | Addressing Mode | BASIC Equivalent |
|------------|-----------------|------------------|
| `LDA #$20` | Immediate | `A = 32` |
| `LDA $20` | Zero Page | `A = PEEK(32)` |
| `LDA $0220` | Absolute | `A = PEEK(544)` |
| `LDA $20,X` | Zero Page Indexed | `A = PEEK(32+X)` |
| `LDA ($20),Y` | Indirect Indexed | `A = PEEK(PEEK(32)+Y)` |

---

## Directives & Data

Directives are used to tell the assembler where in memory to put the object
code, define labels and set up data stores.

### Start Address

The syntax is *$xxxx* or *$xxxx$yyyy* and is used to tell the assembler where
the object code is to be stored.

### Label Definition

The syntax is `:LABL=$xxxx` or `JSR :LABL`.
All label entries must be preceded by a colon.

### Data Entry

```
#$FF        ; hexadecimal
#255        ; decimal
#%10101010  ; binary
```

### Text

```
"Hello world
```

Labelled text:
```
:TEXT "Sample text
```

### Address Storage

```
#:LABL     ; store low/high byte
#:LABL>   ; store high/low byte
```

Immediate usage:
```
LDA #:LABL
LDA #:LABL>
```

---

## Sample Programs

```asm
10 *$1000
20 :SCRN=$D000
30 LDX #$00
40 LDA #$BB
50 :LOOP STA :SCRN,X
60 INX
70 CPX #$00
80 BNE :LOOP
90 RTS
```

```asm
10 *$1000$0300;             assembly and destination
20 :X1 "THIS IS A SAMPLE;   text to write to screen
30 #$00;                    equivalent of .BYTE
40 :SCRN=$D300;             define screen label
50 LDX #$00;                zero out X register
60 :LOOP LDA :X1,X;         get new character from text
70 STA :SCRN,X;             put to next screen location
80 INX;                     step up one
90 CMP #$00;                test for null (.BYTE CHAR.)
100 BNE :LOOP;              if not keep going
110 RTS;                    finished!
```

```asm
10 *$1000;                  start address
20 :STOR #$A1#$20#$A1;      look up table named :STOR
30 #$20#$A1#$20#$A1#$20
40 #$A1#$20#$A1#$20#$A1
50 LDX #$00;                zero out X register
60 :LOOP LDA :STOR,X;       get next char from STOR
70 STA $D100,X;             put to next screen location
80 INX;                     step up one
90 CPX #$OD;                test for all done
100 BNE :LOOP;              if not keep going
110 RTS;                    finished
```

---

## Error Messages

| Code | Meaning |
|------|--------|
| * | No start address |
| L | Invalid Label |
| M | Wrong addressing mode |
| N | Illegal number |
| O | Out of memory |
| R | Branch out of range |
| S | Syntax error |

---

## Copyright & Notice

ENCODER, its subroutines, and all printed materials are copyright  
**Premier Publications (1982)**.

Copying beyond personal use is strictly prohibited.

ENCODER was written by **Dr. A. Eddleston**.

Premier Publications  
208 Croydon Rd  
Anerley, London SE20 7YX  
Telephone: 01-659-7131
