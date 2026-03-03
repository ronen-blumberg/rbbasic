# RB BASIC Quick Reference Card

## Essential Syntax

### Output
```basic
PRINT "text"              ' Display text
PRINT variable           ' Display variable
PRINT "A="; A; "B="; B  ' Multiple values
```

### Input
```basic
INPUT variable           ' Get input
INPUT "prompt"; var     ' Input with prompt
LINE INPUT text$        ' Read entire line
```

### Variables
```basic
A = 42                  ' Numeric variable
NAME$ = "John"          ' String variable ($ suffix)
DIM ARRAY(10)          ' Declare array (0-10)
```

## Control Flow

### IF Statement
```basic
IF condition THEN statement
IF condition THEN
  statements
ELSE
  statements
END IF
```

### Loops
```basic
FOR I = 1 TO 10        ' FOR loop
  statements
NEXT I

WHILE condition        ' WHILE loop
  statements
WEND

DO WHILE condition     ' DO WHILE
  statements
LOOP
```

## String Functions

| Function | Description | Example |
|----------|-------------|---------|
| `LEN(s$)` | Length | `LEN("HELLO")` → 5 |
| `LEFT$(s$,n)` | Left n chars | `LEFT$("HELLO",2)` → "HE" |
| `RIGHT$(s$,n)` | Right n chars | `RIGHT$("HELLO",2)` → "LO" |
| `MID$(s$,start,len)` | Middle substring | `MID$("HELLO",2,3)` → "ELL" |
| `CHR$(n)` | ASCII to char | `CHR$(65)` → "A" |
| `ASC(s$)` | Char to ASCII | `ASC("A")` → 65 |
| `UCASE$(s$)` | To uppercase | `UCASE$("hello")` → "HELLO" |
| `LCASE$(s$)` | To lowercase | `LCASE$("HELLO")` → "hello" |

## Math Functions

| Function | Description | Example |
|----------|-------------|---------|
| `ABS(x)` | Absolute value | `ABS(-5)` → 5 |
| `SIN(x)` | Sine (radians) | `SIN(1.5708)` → 1 |
| `COS(x)` | Cosine (radians) | `COS(0)` → 1 |
| `TAN(x)` | Tangent | `TAN(0.785)` → 1 |
| `SQR(x)` | Square root | `SQR(16)` → 4 |
| `INT(x)` | Integer part | `INT(3.7)` → 3 |
| `RND` | Random 0-1 | `INT(RND*6)+1` → 1-6 |

## File I/O

```basic
' Write to file
OPEN "file.txt" FOR OUTPUT AS #1
PRINT #1, "Hello"
CLOSE #1

' Read from file
OPEN "file.txt" FOR INPUT AS #1
WHILE NOT EOF(1)
  LINE INPUT #1, line$
  PRINT line$
WEND
CLOSE #1
```

## Graphics

```basic
SCREEN 12              ' Set graphics mode
CLS                    ' Clear screen
PSET (x, y), color    ' Plot pixel
LINE (x1,y1)-(x2,y2), color  ' Draw line
CIRCLE (x, y), radius, color  ' Draw circle
PAINT (x, y), color   ' Fill area
```

## Sound

```basic
BEEP                  ' System beep
SOUND freq, duration  ' Play tone (Hz, ticks)
PLAY "CDEFGAB"       ' Play notes
```

## Common Patterns

### Menu System
```basic
DO
  PRINT "1. Option 1"
  PRINT "2. Option 2"
  INPUT choice
  IF choice = 1 THEN GOSUB 1000
  IF choice = 2 THEN GOSUB 2000
LOOP UNTIL choice = 0
```

### Read File Line by Line
```basic
OPEN "file.txt" FOR INPUT AS #1
WHILE NOT EOF(1)
  LINE INPUT #1, line$
  ' Process line$
WEND
CLOSE #1
```

### Random Number in Range
```basic
' Random from MIN to MAX
NUM = INT(RND * (MAX - MIN + 1)) + MIN
```

### Simple Animation Loop
```basic
DO
  ' Clear old position
  ' Update position
  ' Draw new position
  K$ = INKEY$
LOOP WHILE K$ = ""
```

## Operators

### Arithmetic
`+` `-` `*` `/` `^` (power) `MOD` (modulo)

### Comparison
`=` `<>` `<` `>` `<=` `>=`

### Logical
`AND` `OR` `NOT`

## Data Types

- **Numeric**: Double-precision floating-point (default)
- **String**: Text with `$` suffix, max 255 chars

## Tips

✓ Use DIM before array access
✓ Close files when done
✓ RANDOMIZE for different random sequences
✓ String positions are 1-based
✓ Arrays are 0-based by default

© 2025 RB BASIC Compiler v2.0
