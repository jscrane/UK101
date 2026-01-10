# Toolkit II
This is a machine-code extension to Microsoft BASIC that hooks into the interpreter’s command dispatch and line-handling routines to add advanced program-editing 
features without modifying BASIC itself. It introduces new commands such as LIST variants, AUTO, DELETE, RENUM, FIND, REPL, DUPL, VIEW, TRON, and TROFF, implemented 
by intercepting BASIC's input stream, reusing the interpreter’s internal line pointers and token scanner, and temporarily patching character-input/output vectors as 
needed. It operates directly on BASIC’s in-memory program format, scanning and rewriting tokenised lines rather than source text, which allows fast manipulation of 
programs.

Overall, it functions as a lightweight “program editor inside BASIC,” aimed at power users.

## Commands

### AUTO
Automatically generates and inserts incrementing line numbers while the user types program lines. Lines increment by 10.

If a line exists already, '*' will be printed before it.  CTRL-T terminates input.

Options:
- none: start numbering at line 10
- n: start numbering at line n

Errors:
- OF (overflow) when the generated line-number exceeds BASIC's maximum (63999)

```
AUTO 100
 100 REM FOO
 110 REM BAR
 120 REM BAZ
 130*
```

### LIST
A paging-aware, interruptible, range-capable reimplementation of LIST.

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
Renumbers program lines and updates all line-number references accordingly.

Options:
- n: renumber all lines with new start at line n in increments of 10

Errors:
- OF (overflow) when the generated line-number exceeds BASIC's maximum (63999)
- BF (buffer full) if the new line is too long
- L# (line number) if the number is missing or malformed

```
RENUM 100
*
LIST

 100 REM BAR
 110 REM BAZ
OK
```

### FIND
Interactively searches program text for a pattern delimited by / and marks matching lines.

Options:
- /str: search for ```str``` in the program and halt on find, hit ENTER to continue, CTRL-T aborts
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
Interactively replaces occurrences of one /pattern with /another throughout the program.

Requires confirmation of each replacement with Y. CTRL-T aborts.

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
Enables and disables BASIC execution tracing by turning on the line-trace flag.
When enabled, a bracketed line number is written directly into screen memory.

### MC
Enters the Machine Code Monitor.

```
MC

>8000/24 G
          TKII V1.1

[C] 1981 P.Rihan/PREMIER
OK
```

### VIEW
Continuously displays input from the ACIA (tape/serial) until interrupted by a keypress.

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
