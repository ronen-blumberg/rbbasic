# RB BASIC Language Manual
## Complete Reference Guide for RB BASIC Compiler v2.0

---

## Table of Contents

1. [Introduction](#introduction)
2. [Getting Started](#getting-started)
3. [Program Structure](#program-structure)
4. [Data Types](#data-types)
5. [Variables and Arrays](#variables-and-arrays)
6. [Operators](#operators)
7. [Input/Output Statements](#inputoutput-statements)
8. [Control Flow Statements](#control-flow-statements)
9. [Subroutines and Functions](#subroutines-and-functions)
10. [String Functions](#string-functions)
11. [Math Functions](#math-functions)
12. [File I/O](#file-io)
13. [Screen Control](#screen-control)
14. [Graphics Commands](#graphics-commands)
15. [Sound Commands](#sound-commands)
16. [System Commands](#system-commands)
17. [Complete Keyword Reference](#complete-keyword-reference)
18. [Example Programs](#example-programs)
19. [Error Messages and Troubleshooting](#error-messages-and-troubleshooting)

---

## Introduction

### What is RB BASIC?

RB BASIC (Ronen Blue BASIC) is a BASIC-to-C transpiler that provides full compatibility with GW-BASIC and QBasic programs. It compiles classic BASIC code into optimized C code, which is then compiled into native Windows executables.

### Features

- ✅ Full GW-BASIC/QBasic compatibility
- ✅ Compiles to native Windows executables
- ✅ SDL2-based graphics support
- ✅ Fast execution (compiled, not interpreted)
- ✅ Modern development workflow
- ✅ Extensive string manipulation
- ✅ File I/O support
- ✅ Sound and graphics capabilities

### Compatibility

RB BASIC is compatible with:
- **GW-BASIC** - Full support
- **QBasic** - Full support for core features
- **QuickBASIC** - Most features supported
- **QB64** - Core compatibility
- **FreeBASIC** - Basic syntax compatibility

---

## Getting Started

### Installation

1. Ensure you have MinGW32 (GCC) installed
2. Extract RB BASIC compiler to `C:\RBBASIC`
3. Add compiler directory to your PATH
4. Verify SDL2 libraries are in `C:\RBBASIC\lib`

### Your First Program

Create a file named `hello.bas`:

```basic
10 PRINT "Hello, World!"
20 END
```

Compile and run:

```batch
rbbasic hello.bas
compile.bat
program.exe
```

### Command Line Usage

```
rbbasic <source.bas> [-o output.exe] [-c]

Options:
  -o <file>   Specify output executable name
  -c          Generate C code only (don't compile)
  -h, --help  Show help message
```

---

## Program Structure

### Line Numbers

RB BASIC programs use line numbers to organize code:

```basic
10 REM This is the first line
20 PRINT "Hello"
30 END
```

**Rules:**
- Line numbers must be integers (1-65535)
- Line numbers should be in ascending order
- Common practice: increment by 10 to allow insertions
- Line numbers are required for GOTO/GOSUB targets

### Multiple Statements Per Line

Use colon (`:`) to separate statements:

```basic
10 A = 5: B = 10: PRINT A + B
```

### Comments

```basic
10 REM This is a comment
20 ' This is also a comment (alternative syntax)
```

### Program Termination

```basic
END
```

Terminates program execution immediately.

---

## Data Types

### Numeric Types

RB BASIC uses double-precision floating-point for all numeric values:

```basic
10 A = 42          ' Integer value
20 B = 3.14159     ' Floating point
30 C = -273.15     ' Negative number
40 D = 1.23E+10    ' Scientific notation
```

### String Type

Strings are denoted by `$` suffix:

```basic
10 NAME$ = "John Smith"
20 GREETING$ = "Hello, " + NAME$
```

**String Limits:**
- Maximum length: 255 characters
- Enclosed in double quotes (`"`)

---

## Variables and Arrays

### Variable Names

**Rules:**
- Must start with a letter (A-Z)
- Can contain letters, numbers, and underscore
- Maximum length: typically 40 characters
- Case-insensitive (A = a)
- String variables end with `$`

**Valid Examples:**
```basic
10 COUNT = 0
20 SCORE = 100
30 PLAYER_NAME$ = "Alice"
40 X1 = 5
```

**Invalid Examples:**
```basic
10 1COUNT = 0      ' Can't start with number
20 MY-VAR = 5      ' Hyphen not allowed
```

### Variable Types

```basic
' Numeric variables (default)
10 A = 42
20 COUNTER = 100

' String variables ($ suffix)
30 NAME$ = "Bob"
40 ADDRESS$ = "123 Main St"
```

### Arrays

#### Declaration with DIM

```basic
' Single dimension array (0-10 = 11 elements)
10 DIM SCORES(10)

' Two-dimensional array
20 DIM GRID(5, 5)

' String array
30 DIM NAMES$(100)

' Multiple arrays in one statement
40 DIM A(10), B(20), C$(15)
```

**Important:**
- Arrays are zero-indexed: `A(10)` has elements 0-10 (11 total)
- Must use DIM before accessing arrays
- Array bounds are fixed at compile time

#### Using Arrays

```basic
10 DIM NUMBERS(5)
20 FOR I = 0 TO 5
30   NUMBERS(I) = I * 10
40 NEXT I
50 PRINT NUMBERS(3)  ' Prints: 30
```

#### Multi-Dimensional Arrays

```basic
10 DIM BOARD(8, 8)  ' Chess board
20 FOR ROW = 0 TO 7
30   FOR COL = 0 TO 7
40     BOARD(ROW, COL) = 0
50   NEXT COL
60 NEXT ROW
```

---

## Operators

### Arithmetic Operators

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `+` | Addition | `5 + 3` | 8 |
| `-` | Subtraction | `10 - 4` | 6 |
| `*` | Multiplication | `6 * 7` | 42 |
| `/` | Division | `15 / 3` | 5 |
| `^` | Exponentiation | `2 ^ 3` | 8 |
| `MOD` | Modulo | `17 MOD 5` | 2 |

### Comparison Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `=` | Equal to | `A = 5` |
| `<>` | Not equal to | `A <> 5` |
| `<` | Less than | `A < 10` |
| `>` | Greater than | `A > 10` |
| `<=` | Less than or equal | `A <= 10` |
| `>=` | Greater than or equal | `A >= 10` |

### Logical Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `AND` | Logical AND | `A > 0 AND A < 10` |
| `OR` | Logical OR | `A = 5 OR A = 10` |
| `NOT` | Logical NOT | `NOT (A = 5)` |

### String Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Concatenation | `"Hello" + " " + "World"` |

### Operator Precedence

From highest to lowest:

1. `^` (Exponentiation)
2. `-` (Unary minus)
3. `*`, `/` (Multiplication, Division)
4. `MOD` (Modulo)
5. `+`, `-` (Addition, Subtraction)
6. `=`, `<>`, `<`, `>`, `<=`, `>=` (Comparison)
7. `NOT` (Logical NOT)
8. `AND` (Logical AND)
9. `OR` (Logical OR)

Use parentheses to override precedence:

```basic
10 RESULT = (5 + 3) * 2  ' Result: 16
20 VALUE = 5 + 3 * 2     ' Result: 11
```

---

## Input/Output Statements

### PRINT Statement

Display text and values:

```basic
10 PRINT "Hello, World!"
20 PRINT 42
30 PRINT "The answer is"; 42
40 PRINT "A="; A; "B="; B
```

**Print Separators:**

| Separator | Effect | Example |
|-----------|--------|---------|
| `;` | No space | `PRINT "A";5` → `A5` |
| `,` | Tab to next zone | `PRINT "A",5` → `A     5` |
| (none) | Newline after | `PRINT "Hello"` |

**Suppress Newline:**

```basic
10 PRINT "Enter name: ";  ' Semicolon prevents newline
20 INPUT NAME$
```

### INPUT Statement

Get user input:

```basic
' Simple input
10 INPUT A
20 INPUT "Enter your name"; NAME$

' Multiple inputs
30 INPUT "Enter X, Y"; X, Y

' Input with prompt
40 INPUT "Age"; AGE
```

**Notes:**
- User must press Enter
- For multiple values, separate with commas
- String input doesn't require quotes (unless string contains comma)

### LINE INPUT Statement

Read entire line as string (including commas and quotes):

```basic
10 LINE INPUT "Enter full address: "; ADDRESS$
20 LINE INPUT SENTENCE$
```

**Use LINE INPUT when:**
- Input may contain commas
- Input may contain quotes
- You want the entire line exactly as typed

---

## Control Flow Statements

### IF...THEN...ELSE

#### Simple IF

```basic
10 IF A > 10 THEN PRINT "Greater than 10"
20 IF B = 5 THEN GOTO 100
```

#### Block IF (Recommended)

```basic
10 IF SCORE > 100 THEN
20   PRINT "High score!"
30   BONUS = 50
40 ELSE
50   PRINT "Try again"
60   BONUS = 0
70 END IF
```

#### IF with ELSEIF

```basic
10 IF GRADE >= 90 THEN
20   LETTER$ = "A"
30 ELSEIF GRADE >= 80 THEN
40   LETTER$ = "B"
50 ELSEIF GRADE >= 70 THEN
60   LETTER$ = "C"
70 ELSE
80   LETTER$ = "F"
90 END IF
```

### FOR...NEXT Loop

```basic
' Count from 1 to 10
10 FOR I = 1 TO 10
20   PRINT I
30 NEXT I

' Count with step
40 FOR I = 0 TO 100 STEP 10
50   PRINT I
60 NEXT I

' Count backwards
70 FOR I = 10 TO 1 STEP -1
80   PRINT I
90 NEXT I
```

**Nested Loops:**

```basic
10 FOR ROW = 1 TO 5
20   FOR COL = 1 TO 5
30     PRINT "*";
40   NEXT COL
50   PRINT  ' Newline
60 NEXT ROW
```

### WHILE...WEND Loop

```basic
10 COUNT = 0
20 WHILE COUNT < 10
30   PRINT COUNT
40   COUNT = COUNT + 1
50 WEND
```

### DO...LOOP

#### DO WHILE (test at start)

```basic
10 COUNT = 0
20 DO WHILE COUNT < 10
30   PRINT COUNT
40   COUNT = COUNT + 1
50 LOOP
```

#### DO UNTIL (test at start)

```basic
10 DO UNTIL ANSWER$ = "YES"
20   INPUT "Continue (YES/NO)"; ANSWER$
30 LOOP
```

#### DO...LOOP WHILE (test at end)

```basic
10 DO
20   INPUT "Enter number (0 to quit)"; NUM
30   PRINT NUM * 2
40 LOOP WHILE NUM <> 0
```

#### DO...LOOP UNTIL (test at end)

```basic
10 DO
20   INPUT "Guess a number"; GUESS
30   IF GUESS = SECRET THEN PRINT "Correct!"
40 LOOP UNTIL GUESS = SECRET
```

### GOTO Statement

Jump to a line number:

```basic
10 PRINT "Start"
20 GOTO 100
30 PRINT "This won't print"
100 PRINT "End"
```

**Warning:** Excessive GOTO can make code hard to read. Use structured loops when possible.

### GOSUB...RETURN

Call a subroutine:

```basic
10 GOSUB 1000  ' Call subroutine
20 PRINT "Back from subroutine"
30 END

1000 REM Subroutine
1010 PRINT "In subroutine"
1020 RETURN
```

### ON...GOTO

Jump based on expression value:

```basic
10 INPUT "Choose (1-3)"; CHOICE
20 ON CHOICE GOTO 100, 200, 300
30 PRINT "Invalid choice"
40 END

100 PRINT "Option 1": END
200 PRINT "Option 2": END
300 PRINT "Option 3": END
```

### ON...GOSUB

Call subroutine based on expression:

```basic
10 INPUT "Menu (1-3)"; CHOICE
20 ON CHOICE GOSUB 1000, 2000, 3000
30 END

1000 PRINT "Menu 1": RETURN
2000 PRINT "Menu 2": RETURN
3000 PRINT "Menu 3": RETURN
```

---

## Subroutines and Functions

### SUB Procedures

Define reusable subroutines:

```basic
10 CALL GREET("Alice", 5)
20 END

SUB GREET(NAME$, TIMES)
  FOR I = 1 TO TIMES
    PRINT "Hello, "; NAME$
  NEXT I
END SUB
```

### FUNCTION Procedures

Define functions that return values:

```basic
10 PRINT "5! = "; FACTORIAL(5)
20 END

FUNCTION FACTORIAL(N)
  IF N <= 1 THEN
    FACTORIAL = 1
  ELSE
    FACTORIAL = N * FACTORIAL(N - 1)
  END IF
END FUNCTION
```

### DEF FN - User-Defined Functions

Simple one-line functions:

```basic
10 DEF FN SQUARE(X) = X * X
20 DEF FN AREA(R) = 3.14159 * R ^ 2
30 DEF FN CELSIUS(F) = (F - 32) * 5 / 9

100 PRINT FN SQUARE(5)      ' Prints: 25
110 PRINT FN AREA(10)       ' Prints: 314.159
120 PRINT FN CELSIUS(98.6)  ' Prints: 37
```

---

## String Functions

### LEN - String Length

Returns the length of a string:

```basic
10 A$ = "HELLO"
20 PRINT LEN(A$)  ' Prints: 5
```

### LEFT$ - Left Substring

Returns leftmost characters:

```basic
10 A$ = "HELLO WORLD"
20 PRINT LEFT$(A$, 5)  ' Prints: HELLO
```

### RIGHT$ - Right Substring

Returns rightmost characters:

```basic
10 A$ = "HELLO WORLD"
20 PRINT RIGHT$(A$, 5)  ' Prints: WORLD
```

### MID$ - Middle Substring

Returns middle portion:

```basic
10 A$ = "HELLO WORLD"
20 PRINT MID$(A$, 7, 5)   ' Prints: WORLD (start at 7, length 5)
30 PRINT MID$(A$, 7)      ' Prints: WORLD (from 7 to end)
```

**Note:** String positions are 1-based (first character is position 1).

### INSTR - Find Substring

Returns position of substring (0 if not found):

```basic
10 A$ = "HELLO WORLD"
20 POS = INSTR(A$, "WORLD")
30 PRINT POS  ' Prints: 7

40 POS = INSTR(A$, "XYZ")
50 PRINT POS  ' Prints: 0 (not found)
```

### CHR$ - Character from ASCII

Returns character for ASCII code:

```basic
10 PRINT CHR$(65)   ' Prints: A
20 PRINT CHR$(72); CHR$(73)  ' Prints: HI
30 PRINT CHR$(34)   ' Prints: " (quote)
```

**Common ASCII codes:**
- 34 = `"` (quote)
- 65-90 = A-Z
- 97-122 = a-z
- 48-57 = 0-9
- 32 = space
- 13 = carriage return
- 10 = line feed

### ASC - ASCII from Character

Returns ASCII code of first character:

```basic
10 PRINT ASC("A")     ' Prints: 65
20 PRINT ASC("HELLO") ' Prints: 72 (H)
```

### STR$ - Number to String

Converts number to string:

```basic
10 A = 42
20 B$ = STR$(A)
30 PRINT "The answer is" + B$
```

### VAL - String to Number

Converts string to number:

```basic
10 A$ = "42"
20 B = VAL(A$)
30 PRINT B * 2  ' Prints: 84
```

### UCASE$ - Convert to Uppercase

```basic
10 A$ = "Hello World"
20 PRINT UCASE$(A$)  ' Prints: HELLO WORLD
```

### LCASE$ - Convert to Lowercase

```basic
10 A$ = "Hello World"
20 PRINT LCASE$(A$)  ' Prints: hello world
```

### SPACE$ - Generate Spaces

```basic
10 PRINT "Hello" + SPACE$(5) + "World"
' Prints: Hello     World
```

### STRING$ - Repeat Character

```basic
10 PRINT STRING$(10, "*")  ' Prints: **********
20 PRINT STRING$(5, 65)    ' Prints: AAAAA (ASCII 65 = A)
```

---

## Math Functions

### Basic Math Functions

| Function | Description | Example |
|----------|-------------|---------|
| `ABS(x)` | Absolute value | `ABS(-5)` → 5 |
| `SGN(x)` | Sign (-1, 0, 1) | `SGN(-5)` → -1 |
| `INT(x)` | Integer part (floor) | `INT(3.7)` → 3 |
| `FIX(x)` | Truncate to integer | `FIX(-3.7)` → -3 |
| `CINT(x)` | Round to nearest integer | `CINT(3.5)` → 4 |

### Trigonometric Functions

| Function | Description | Example |
|----------|-------------|---------|
| `SIN(x)` | Sine (x in radians) | `SIN(1.5708)` → 1 |
| `COS(x)` | Cosine (x in radians) | `COS(0)` → 1 |
| `TAN(x)` | Tangent (x in radians) | `TAN(0.785398)` → 1 |
| `ATN(x)` | Arctangent | `ATN(1)` → 0.785398 |

**Convert degrees to radians:**
```basic
10 DEF FN RAD(D) = D * 3.14159 / 180
20 ANGLE = FN RAD(90)
30 PRINT SIN(ANGLE)  ' Prints: 1
```

### Exponential and Logarithmic

| Function | Description | Example |
|----------|-------------|---------|
| `EXP(x)` | e raised to x | `EXP(1)` → 2.71828 |
| `LOG(x)` | Natural logarithm | `LOG(2.71828)` → 1 |
| `SQR(x)` | Square root | `SQR(16)` → 4 |

### Random Numbers

#### RND - Random Number

```basic
' Random number between 0 and 1
10 PRINT RND

' Random integer from 1 to 6 (dice)
20 DICE = INT(RND * 6) + 1

' Random integer from MIN to MAX
30 NUM = INT(RND * (MAX - MIN + 1)) + MIN
```

#### RANDOMIZE - Seed Random Generator

```basic
' Seed with current time (different each run)
10 RANDOMIZE

' Seed with specific value (reproducible)
20 RANDOMIZE 12345
```

### TIMER - System Timer

Returns seconds since midnight:

```basic
10 START = TIMER
20 FOR I = 1 TO 10000: NEXT I
30 ELAPSED = TIMER - START
40 PRINT "Time:"; ELAPSED; "seconds"
```

---

## File I/O

### OPEN Statement

Open a file for reading or writing:

```basic
' Open for output (writing)
10 OPEN "DATA.TXT" FOR OUTPUT AS #1

' Open for input (reading)
20 OPEN "DATA.TXT" FOR INPUT AS #1

' Open for append (add to end)
30 OPEN "LOG.TXT" FOR APPEND AS #1
```

**File Numbers:**
- Use #1, #2, #3, etc. to identify files
- Can have multiple files open simultaneously

### PRINT# - Write to File

```basic
10 OPEN "OUTPUT.TXT" FOR OUTPUT AS #1
20 PRINT #1, "Hello, File!"
30 PRINT #1, "Line 2"
40 CLOSE #1
```

### INPUT# - Read from File

```basic
10 OPEN "INPUT.TXT" FOR INPUT AS #1
20 INPUT #1, A$
30 INPUT #1, NUM
40 CLOSE #1
```

### LINE INPUT# - Read Line from File

```basic
10 OPEN "DATA.TXT" FOR INPUT AS #1
20 WHILE NOT EOF(1)
30   LINE INPUT #1, TEXT$
40   PRINT TEXT$
50 WEND
60 CLOSE #1
```

### CLOSE Statement

Close open files:

```basic
' Close specific file
10 CLOSE #1

' Close all files
20 CLOSE
```

### EOF - End of File

Check if at end of file:

```basic
10 OPEN "DATA.TXT" FOR INPUT AS #1
20 WHILE NOT EOF(1)
30   LINE INPUT #1, LINE$
40   PRINT LINE$
50 WEND
60 CLOSE #1
```

### File I/O Example

Write and read a data file:

```basic
' Write data
10 OPEN "SCORES.DAT" FOR OUTPUT AS #1
20 FOR I = 1 TO 5
30   INPUT "Name"; NAME$
40   INPUT "Score"; SCORE
50   PRINT #1, NAME$; ","; SCORE
60 NEXT I
70 CLOSE #1

' Read data
100 OPEN "SCORES.DAT" FOR INPUT AS #1
110 WHILE NOT EOF(1)
120   INPUT #1, NAME$, SCORE
130   PRINT NAME$; " scored "; SCORE
140 WEND
150 CLOSE #1
```

---

## Screen Control

### CLS - Clear Screen

```basic
10 CLS  ' Clear entire screen
```

### LOCATE - Position Cursor

```basic
' Move to row 10, column 20
10 LOCATE 10, 20
20 PRINT "Here!"

' Just set row
30 LOCATE 5
40 PRINT "Row 5"
```

**Screen coordinates:**
- Rows: 1-25 (text mode)
- Columns: 1-80 (text mode)
- Top-left is (1, 1)

### COLOR - Set Colors

```basic
' Set foreground color
10 COLOR 14
20 PRINT "Yellow text"

' Set foreground and background
30 COLOR 15, 1
40 PRINT "White on blue"
```

**Color Values (0-15):**

| Value | Color | Value | Color |
|-------|-------|-------|-------|
| 0 | Black | 8 | Dark Gray |
| 1 | Blue | 9 | Light Blue |
| 2 | Green | 10 | Light Green |
| 3 | Cyan | 11 | Light Cyan |
| 4 | Red | 12 | Light Red |
| 5 | Magenta | 13 | Light Magenta |
| 6 | Brown | 14 | Yellow |
| 7 | Light Gray | 15 | White |

### KEY - Function Key Display

```basic
' Turn off function key display at bottom
10 KEY OFF

' Turn on function key display
20 KEY ON
```

---

## Graphics Commands

### SCREEN - Set Graphics Mode

```basic
' Text mode (80x25)
10 SCREEN 0

' Low resolution graphics (320x200, 4 colors)
20 SCREEN 1

' Medium resolution (640x200, 2 colors)
30 SCREEN 2

' High resolution (640x350, 16 colors) - EGA
40 SCREEN 9

' VGA mode (640x480, 16 colors)
50 SCREEN 12
```

### PSET - Plot Pixel

```basic
' Plot pixel at (100, 100) in default color
10 SCREEN 12
20 PSET (100, 100)

' Plot pixel with specific color
30 PSET (150, 150), 14  ' Yellow pixel
```

### LINE - Draw Line

```basic
' Draw line from (0,0) to (100,100)
10 SCREEN 12
20 LINE (0, 0)-(100, 100)

' Draw line in specific color
30 LINE (50, 50)-(150, 150), 12  ' Red line

' Draw rectangle (box)
40 LINE (10, 10)-(100, 50), 14, B

' Draw filled rectangle
50 LINE (120, 10)-(210, 50), 9, BF
```

**LINE Flags:**
- `B` - Draw box (rectangle)
- `BF` - Draw filled box

### CIRCLE - Draw Circle

```basic
' Circle at (160, 100) with radius 50
10 SCREEN 12
20 CIRCLE (160, 100), 50

' Circle with specific color
30 CIRCLE (320, 240), 75, 12  ' Red circle

' Draw ellipse (aspect ratio)
40 CIRCLE (320, 240), 100, 14, , , 0.5

' Draw arc (start and end angles in radians)
50 CIRCLE (320, 240), 80, 10, 0, 3.14159
```

### PAINT - Fill Area

```basic
' Fill area at (160, 100) with color 14
10 SCREEN 12
20 CIRCLE (160, 100), 50, 15
30 PAINT (160, 100), 14, 15  ' Fill with yellow, border white
```

**Parameters:**
- `(x, y)` - Starting point inside area
- `color` - Fill color
- `border` - Border color (stops filling at this color)

### Graphics Example - Simple Drawing

```basic
10 SCREEN 12
20 CLS

' Draw house
30 LINE (100, 300)-(300, 300), 15    ' Bottom
40 LINE (100, 300)-(100, 150), 15    ' Left wall
50 LINE (300, 300)-(300, 150), 15    ' Right wall
60 LINE (100, 150)-(300, 150), 15    ' Top
70 LINE (100, 150)-(200, 50), 15     ' Roof left
80 LINE (200, 50)-(300, 150), 15     ' Roof right

' Draw door
90 LINE (170, 300)-(230, 220), 12, BF

' Draw window
100 LINE (120, 180)-(160, 220), 14, B

110 A$ = INPUT$(1)  ' Wait for keypress
```

---

## Sound Commands

### SOUND - Generate Tone

```basic
' Play frequency 440 Hz for 18.2 clock ticks (~1 second)
10 SOUND 440, 18.2

' Play middle C (261.63 Hz) for half second
20 SOUND 262, 9

' Silence (frequency 32 means pause)
30 SOUND 32, 18.2
```

**Parameters:**
- `frequency` - Hz (37-32767)
- `duration` - Clock ticks (18.2 per second)

### PLAY - Music Macro Language

```basic
' Play simple melody
10 PLAY "CDEFGAB"

' Play with octave and length
20 PLAY "O4 L4 C D E F G A B O5 C"

' Play scale with different lengths
30 PLAY "L8 C L4 D L8 E F L2 G"
```

**PLAY Command Syntax:**

| Command | Description | Example |
|---------|-------------|---------|
| `A-G` | Play note | `C` |
| `#` or `+` | Sharp | `C#` or `C+` |
| `-` | Flat | `D-` |
| `O`n | Set octave (0-6) | `O4` |
| `L`n | Set length (1-64) | `L4` |
| `T`n | Set tempo (32-255) | `T120` |
| `P`n | Pause | `P4` |
| `>` | Increase octave | `>` |
| `<` | Decrease octave | `<` |
| `N`n | Play note by number | `N60` |

**Note Lengths:**
- `L1` - Whole note
- `L2` - Half note
- `L4` - Quarter note
- `L8` - Eighth note
- `L16` - Sixteenth note

### BEEP - Simple Beep

```basic
10 BEEP  ' System beep sound
```

### Sound Example

```basic
10 REM Simple melody
20 PLAY "T120"         ' Set tempo
30 PLAY "O4 L4"        ' Octave 4, quarter notes
40 PLAY "C C G G A A L2 G"     ' Twinkle twinkle
50 PLAY "L4 F F E E D D L2 C"  ' little star
```

---

## System Commands

### DATA, READ, RESTORE

Store and read data in program:

```basic
10 FOR I = 1 TO 3
20   READ NAME$, AGE
30   PRINT NAME$; " is"; AGE; "years old"
40 NEXT I
50 END

100 DATA "Alice", 25
110 DATA "Bob", 30
120 DATA "Carol", 28
```

**RESTORE:** Reset READ pointer:

```basic
10 READ A, B, C
20 RESTORE         ' Reset to beginning
30 READ X, Y, Z
40 PRINT A, X      ' Both print same value

100 DATA 10, 20, 30
```

### RANDOMIZE - Seed Random Generator

```basic
' Use system time (different each run)
10 RANDOMIZE

' Use specific seed (repeatable)
20 RANDOMIZE 12345
```

### SLEEP - Pause Execution

```basic
' Sleep for 2 seconds
10 SLEEP 2

' Sleep for 500 milliseconds (0.5 seconds)
20 SLEEP 0.5
```

### INKEY$ - Check for Keypress

```basic
' Non-blocking key check
10 K$ = INKEY$
20 IF K$ <> "" THEN PRINT "You pressed: "; K$

' Wait for keypress
30 WHILE INKEY$ = "": WEND
```

### INPUT$ - Read N Characters

```basic
' Read exactly 1 character (wait if needed)
10 A$ = INPUT$(1)
20 PRINT "You pressed: "; A$

' Read 5 characters
30 CODE$ = INPUT$(5)
```

### DATE$ and TIME$ - System Date/Time

```basic
' Get current date
10 PRINT "Date: "; DATE$

' Get current time
20 PRINT "Time: "; TIME$
```

**Format:**
- DATE$: `MM-DD-YYYY`
- TIME$: `HH:MM:SS`

---

## Complete Keyword Reference

### Control Flow Keywords

| Keyword | Description |
|---------|-------------|
| `IF...THEN...ELSE...END IF` | Conditional execution |
| `FOR...TO...STEP...NEXT` | Counted loop |
| `WHILE...WEND` | Loop while condition true |
| `DO...LOOP` | Loop with various conditions |
| `GOTO` | Jump to line number |
| `GOSUB...RETURN` | Call subroutine |
| `ON...GOTO` | Computed goto |
| `ON...GOSUB` | Computed gosub |
| `END` | Terminate program |

### Input/Output Keywords

| Keyword | Description |
|---------|-------------|
| `PRINT` | Display output |
| `INPUT` | Get user input |
| `LINE INPUT` | Get line of input |
| `OPEN` | Open file |
| `CLOSE` | Close file |
| `PRINT #` | Write to file |
| `INPUT #` | Read from file |
| `LINE INPUT #` | Read line from file |

### Data Keywords

| Keyword | Description |
|---------|-------------|
| `DIM` | Declare array |
| `DATA` | Define data values |
| `READ` | Read data values |
| `RESTORE` | Reset data pointer |
| `LET` | Assign variable (optional) |

### Subroutine Keywords

| Keyword | Description |
|---------|-------------|
| `SUB...END SUB` | Define subroutine |
| `FUNCTION...END FUNCTION` | Define function |
| `CALL` | Call subroutine |
| `DEF FN` | Define single-line function |

### Screen Control Keywords

| Keyword | Description |
|---------|-------------|
| `CLS` | Clear screen |
| `LOCATE` | Position cursor |
| `COLOR` | Set text colors |
| `KEY` | Control function key display |

### Graphics Keywords

| Keyword | Description |
|---------|-------------|
| `SCREEN` | Set graphics mode |
| `PSET` | Plot pixel |
| `LINE` | Draw line or box |
| `CIRCLE` | Draw circle/ellipse/arc |
| `PAINT` | Fill area |

### Sound Keywords

| Keyword | Description |
|---------|-------------|
| `SOUND` | Generate tone |
| `PLAY` | Play music macro |
| `BEEP` | System beep |

### System Keywords

| Keyword | Description |
|---------|-------------|
| `RANDOMIZE` | Seed random generator |
| `SLEEP` | Pause execution |
| `REM` | Comment |

### String Functions

| Function | Description |
|----------|-------------|
| `LEN(s$)` | String length |
| `LEFT$(s$, n)` | Left substring |
| `RIGHT$(s$, n)` | Right substring |
| `MID$(s$, start, len)` | Middle substring |
| `INSTR(s1$, s2$)` | Find substring |
| `CHR$(n)` | ASCII to character |
| `ASC(s$)` | Character to ASCII |
| `STR$(n)` | Number to string |
| `VAL(s$)` | String to number |
| `UCASE$(s$)` | Convert to uppercase |
| `LCASE$(s$)` | Convert to lowercase |
| `SPACE$(n)` | Generate spaces |
| `STRING$(n, c$)` | Repeat character |

### Math Functions

| Function | Description |
|----------|-------------|
| `ABS(x)` | Absolute value |
| `SGN(x)` | Sign of number |
| `INT(x)` | Integer part |
| `FIX(x)` | Truncate decimal |
| `CINT(x)` | Round to integer |
| `SIN(x)` | Sine |
| `COS(x)` | Cosine |
| `TAN(x)` | Tangent |
| `ATN(x)` | Arctangent |
| `EXP(x)` | Exponential |
| `LOG(x)` | Natural logarithm |
| `SQR(x)` | Square root |
| `RND` | Random number |

### Other Functions

| Function | Description |
|----------|-------------|
| `TIMER` | Seconds since midnight |
| `INKEY$` | Check for keypress |
| `INPUT$(n)` | Read N characters |
| `DATE$` | Current date |
| `TIME$` | Current time |
| `EOF(n)` | End of file check |
| `TAB(n)` | Tab to column |

---

## Example Programs

### Example 1: Number Guessing Game

```basic
10 REM Number Guessing Game
20 CLS
30 RANDOMIZE
40 SECRET = INT(RND * 100) + 1
50 TRIES = 0
60 PRINT "I'm thinking of a number between 1 and 100"
70 PRINT

100 INPUT "Your guess"; GUESS
110 TRIES = TRIES + 1

120 IF GUESS < SECRET THEN PRINT "Too low!": GOTO 100
130 IF GUESS > SECRET THEN PRINT "Too high!": GOTO 100

140 PRINT "Correct! You got it in"; TRIES; "tries!"
150 END
```

### Example 2: Temperature Converter

```basic
10 REM Temperature Converter
20 CLS
30 PRINT "Temperature Converter"
40 PRINT "===================="
50 PRINT
60 PRINT "1. Fahrenheit to Celsius"
70 PRINT "2. Celsius to Fahrenheit"
80 PRINT
90 INPUT "Choice (1 or 2)"; CHOICE

100 IF CHOICE = 1 THEN GOSUB 1000
110 IF CHOICE = 2 THEN GOSUB 2000
120 END

1000 REM Fahrenheit to Celsius
1010 INPUT "Enter Fahrenheit"; F
1020 C = (F - 32) * 5 / 9
1030 PRINT F; "F ="; C; "C"
1040 RETURN

2000 REM Celsius to Fahrenheit
2010 INPUT "Enter Celsius"; C
2020 F = C * 9 / 5 + 32
2030 PRINT C; "C ="; F; "F"
2040 RETURN
```

### Example 3: Simple Database

```basic
10 REM Simple Contact Database
20 DIM NAMES$(100), PHONES$(100)
30 COUNT = 0

100 CLS
110 PRINT "Contact Database"
120 PRINT "================"
130 PRINT "1. Add Contact"
140 PRINT "2. List All"
150 PRINT "3. Search"
160 PRINT "4. Quit"
170 PRINT
180 INPUT "Choice"; CHOICE
190 ON CHOICE GOSUB 1000, 2000, 3000, 9000
200 GOTO 100

1000 REM Add Contact
1010 IF COUNT >= 100 THEN PRINT "Database full!": RETURN
1020 COUNT = COUNT + 1
1030 INPUT "Name"; NAMES$(COUNT)
1040 INPUT "Phone"; PHONES$(COUNT)
1050 PRINT "Added!"
1060 SLEEP 1
1070 RETURN

2000 REM List All
2010 IF COUNT = 0 THEN PRINT "No contacts!": SLEEP 1: RETURN
2020 FOR I = 1 TO COUNT
2030   PRINT NAMES$(I); " - "; PHONES$(I)
2040 NEXT I
2050 PRINT: PRINT "Press any key...";
2060 A$ = INPUT$(1)
2070 RETURN

3000 REM Search
3010 INPUT "Search for"; SEARCH$
3020 SEARCH$ = UCASE$(SEARCH$)
3030 FOUND = 0
3040 FOR I = 1 TO COUNT
3050   IF INSTR(UCASE$(NAMES$(I)), SEARCH$) > 0 THEN
3060     PRINT NAMES$(I); " - "; PHONES$(I)
3070     FOUND = 1
3080   END IF
3090 NEXT I
3100 IF FOUND = 0 THEN PRINT "Not found!"
3110 PRINT: PRINT "Press any key...";
3120 A$ = INPUT$(1)
3130 RETURN

9000 REM Quit
9010 END
```

### Example 4: Graphics Demo

```basic
10 REM Graphics Demo - Bouncing Ball
20 SCREEN 12
30 CLS

40 X = 320: Y = 240       ' Starting position
50 DX = 5: DY = 3         ' Velocity
60 RADIUS = 20

100 REM Main Loop
110 CIRCLE (X, Y), RADIUS, 0    ' Erase old position
120 X = X + DX
130 Y = Y + DY

140 REM Bounce off edges
150 IF X - RADIUS < 0 OR X + RADIUS > 639 THEN DX = -DX
160 IF Y - RADIUS < 0 OR Y + RADIUS > 479 THEN DY = -DY

170 CIRCLE (X, Y), RADIUS, 15   ' Draw new position
180 PAINT (X, Y), 14, 15        ' Fill with yellow

190 FOR DELAY = 1 TO 1000: NEXT DELAY  ' Slow down

200 K$ = INKEY$
210 IF K$ = "" THEN GOTO 110    ' Continue if no key pressed

220 SCREEN 0
230 END
```

### Example 5: File Processing

```basic
10 REM Save and Load High Scores
20 DIM NAMES$(10), SCORES(10)
30 NUMSCORES = 0

100 GOSUB 5000  ' Load existing scores

200 CLS
210 PRINT "High Score Manager"
220 PRINT "=================="
230 PRINT "1. Add Score"
240 PRINT "2. View Scores"
250 PRINT "3. Save and Quit"
260 INPUT "Choice"; CHOICE
270 ON CHOICE GOSUB 1000, 2000, 3000
280 GOTO 200

1000 REM Add Score
1010 IF NUMSCORES >= 10 THEN PRINT "List full!": RETURN
1020 NUMSCORES = NUMSCORES + 1
1030 INPUT "Name"; NAMES$(NUMSCORES)
1040 INPUT "Score"; SCORES(NUMSCORES)
1050 RETURN

2000 REM View Scores
2010 FOR I = 1 TO NUMSCORES
2020   PRINT I; ". "; NAMES$(I); " - "; SCORES(I)
2030 NEXT I
2040 PRINT: INPUT "Press Enter"; DUMMY$
2050 RETURN

3000 REM Save and Quit
3010 GOSUB 6000
3020 END

5000 REM Load Scores
5010 ON ERROR GOTO 5100
5020 OPEN "SCORES.DAT" FOR INPUT AS #1
5030 INPUT #1, NUMSCORES
5040 FOR I = 1 TO NUMSCORES
5050   INPUT #1, NAMES$(I), SCORES(I)
5060 NEXT I
5070 CLOSE #1
5080 RETURN
5100 NUMSCORES = 0  ' File doesn't exist yet
5110 RETURN

6000 REM Save Scores
6010 OPEN "SCORES.DAT" FOR OUTPUT AS #1
6020 PRINT #1, NUMSCORES
6030 FOR I = 1 TO NUMSCORES
6040   PRINT #1, NAMES$(I); ","; SCORES(I)
6050 NEXT I
6060 CLOSE #1
6070 RETURN
```

---

## Error Messages and Troubleshooting

### Common Compile Errors

#### "Lexer error: Unexpected character"
**Cause:** Invalid character in source code  
**Solution:** Check for special characters, ensure proper quotes

#### "Parser error: Expected..."
**Cause:** Syntax error in BASIC code  
**Solution:** Check syntax, ensure all keywords spelled correctly

#### "Undefined variable"
**Cause:** Variable used before declaration (in array context)  
**Solution:** Use DIM to declare arrays before use

#### "Type mismatch"
**Cause:** String assigned to numeric variable or vice versa  
**Solution:** Ensure variable types match (use $ for strings)

### Common Runtime Errors

#### "Subscript out of range"
**Cause:** Array index outside declared bounds  
**Solution:** Check array size in DIM statement, verify loop bounds

#### "Division by zero"
**Cause:** Dividing by zero  
**Solution:** Check divisor before division

#### "File not found"
**Cause:** Trying to open non-existent file  
**Solution:** Verify file path and name, use error handling

#### "Out of string space"
**Cause:** Too many string operations  
**Solution:** Reduce string concatenations, clear unused strings

### Compiler Warnings

#### "Unused variable"
**Cause:** Variable declared but never used  
**Solution:** Remove unused variables or use them

#### "Format warning"
**Cause:** Type mismatch in printf (usually internal)  
**Solution:** Usually safe to ignore, but report if causing issues

### Best Practices

1. **Always use DIM for arrays**
   ```basic
   10 DIM SCORES(100)  ' Good
   20 SCORES(50) = 100 ' Bad (without DIM)
   ```

2. **Close files when done**
   ```basic
   10 OPEN "DATA.TXT" FOR INPUT AS #1
   20 ' ... process file ...
   30 CLOSE #1  ' Always close!
   ```

3. **Use meaningful variable names**
   ```basic
   10 PLAYER_SCORE = 100    ' Good
   20 X = 100               ' Less clear
   ```

4. **Comment your code**
   ```basic
   10 REM Calculate player score bonus
   20 BONUS = SCORE * 0.1
   ```

5. **Initialize variables**
   ```basic
   10 COUNT = 0    ' Good - explicit initialization
   20 TOTAL = 0
   ```

6. **Use structured control flow**
   ```basic
   ' Good - use FOR loop
   10 FOR I = 1 TO 10
   20   PRINT I
   30 NEXT I
   
   ' Avoid - excessive GOTO
   10 I = 1
   20 PRINT I
   30 I = I + 1
   40 IF I <= 10 THEN GOTO 20
   ```

---

## Appendix: Quick Reference Card

### Essential Commands

```basic
' Output
PRINT "text"
PRINT variable

' Input
INPUT variable
INPUT "prompt"; variable
LINE INPUT text$

' Variables
LET variable = expression  ' LET is optional
variable = expression

' Arrays
DIM array(size)
array(index) = value

' Control Flow
IF condition THEN statement
IF condition THEN
  statements
END IF

FOR variable = start TO end STEP increment
  statements
NEXT variable

WHILE condition
  statements
WEND

' Subroutines
GOSUB line_number
RETURN
```

### Common Patterns

```basic
' Read file line by line
OPEN "file.txt" FOR INPUT AS #1
WHILE NOT EOF(1)
  LINE INPUT #1, line$
  ' process line$
WEND
CLOSE #1

' Menu system
DO
  PRINT "1. Option 1"
  PRINT "2. Option 2"
  INPUT choice
  IF choice = 1 THEN GOSUB 1000
  IF choice = 2 THEN GOSUB 2000
LOOP UNTIL choice = 0

' Sort array
FOR i = 0 TO n - 1
  FOR j = i + 1 TO n
    IF array(i) > array(j) THEN
      temp = array(i)
      array(i) = array(j)
      array(j) = temp
    END IF
  NEXT j
NEXT i
```

---

## Index

[A](#a) | [B](#b) | [C](#c) | [D](#d) | [E](#e) | [F](#f) | [G](#g) | [H](#h) | [I](#i) | [K](#k) | [L](#l) | [M](#m) | [N](#n) | [O](#o) | [P](#p) | [R](#r) | [S](#s) | [T](#t) | [U](#u) | [V](#v) | [W](#w)

### A
- ABS function
- AND operator
- Arrays
- ASC function
- ATN function

### B
- BEEP statement
- Binary operators

### C
- CALL statement
- CHR$ function
- CIRCLE statement
- CINT function
- CLOSE statement
- CLS statement
- COLOR statement
- Comments (REM)
- Control flow
- COS function

### D
- DATA statement
- DATE$ function
- DEF FN statement
- DIM statement
- DO...LOOP

### E
- END statement
- EOF function
- EXP function

### F
- File I/O
- FIX function
- FOR...NEXT loop
- FUNCTION...END FUNCTION

### G
- GOSUB statement
- GOTO statement
- Graphics

### I
- IF...THEN...ELSE
- INPUT statement
- INPUT$ function
- INKEY$ function
- INSTR function
- INT function

### K
- KEY statement

### L
- LCASE$ function
- LEFT$ function
- LEN function
- LET statement
- LINE statement (graphics)
- LINE INPUT statement
- LOCATE statement
- LOG function
- Loops

### M
- Math functions
- MID$ function
- MOD operator

### N
- NEXT statement
- NOT operator

### O
- ON...GOSUB statement
- ON...GOTO statement
- OPEN statement
- Operators
- OR operator

### P
- PAINT statement
- PLAY statement
- PRINT statement
- PSET statement

### R
- RANDOMIZE statement
- READ statement
- REM statement
- RESTORE statement
- RETURN statement
- RIGHT$ function
- RND function

### S
- SCREEN statement
- SGN function
- SIN function
- SLEEP statement
- SOUND statement
- SPACE$ function
- SQR function
- STR$ function
- STRING$ function
- String functions
- SUB...END SUB

### T
- TAN function
- TIME$ function
- TIMER function

### U
- UCASE$ function

### V
- VAL function
- Variables

### W
- WEND statement
- WHILE statement

---

## About This Manual

**RB BASIC Language Manual**  
Version 2.0  
© 2025 Ronen Blue  

This manual covers the RB BASIC compiler v2.0 Enhanced Edition.

For updates, bug reports, and additional resources, visit the project repository or contact the developer.

### Acknowledgments

RB BASIC is inspired by:
- GW-BASIC (Microsoft)
- QBasic (Microsoft)
- QuickBASIC (Microsoft)
- FreeBASIC
- QB64

Special thanks to the BASIC programming community for decades of innovation and creativity.

---

**End of Manual**
