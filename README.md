# RB BASIC - Ronen Blumberg BASIC

**A portable BASIC-to-C transpiler compiler aiming for QB64/QBasic/GW-BASIC compatibility.**

RB BASIC takes `.bas` source files, transpiles them to C, and compiles them to native Windows executables using a bundled MinGW32 GCC toolchain. No system PATH configuration required — everything runs from a single portable directory.

## Table of Contents

- [Features](#features)
- [Quick Start](#quick-start)
- [Installation](#installation)
- [Usage](#usage)
  - [Command-Line Compiler](#command-line-compiler)
  - [RBIDE (Integrated Development Environment)](#rbide-integrated-development-environment)
- [Project Structure](#project-structure)
- [Compiler Architecture](#compiler-architecture)
- [Language Reference](#language-reference)
  - [Control Flow](#control-flow)
  - [Variables and Data Types](#variables-and-data-types)
  - [Declarations and User Types](#declarations-and-user-types)
  - [Subroutines and Functions](#subroutines-and-functions)
  - [String Functions](#string-functions)
  - [Math Functions](#math-functions)
  - [File I/O](#file-io)
  - [Error Handling](#error-handling)
  - [Console I/O](#console-io)
  - [Graphics (SDL2)](#graphics-sdl2)
  - [Sound](#sound)
  - [System](#system)
  - [QB64 Extensions](#qb64-extensions)
  - [Operators](#operators)
- [Graphics Modes](#graphics-modes)
- [Examples](#examples)
- [Testing](#testing)
- [Building from Source](#building-from-source)
- [Configuration](#configuration)
- [Dependencies](#dependencies)
- [Known Limitations](#known-limitations)
- [License](#license)

## Features

- **Full BASIC Compiler** — not an interpreter; produces native `.exe` files
- **QBasic/GW-BASIC Compatible** — classic BASIC syntax with line numbers (optional) and modern block structures
- **SDL2 Graphics** — SCREEN modes, pixel drawing, flood fill, sprites, turtle graphics (DRAW), palette control
- **SDL2 Sound** — WAV/OGG playback via SDL2_mixer, MML music via PLAY command
- **QB64 Extensions** — `_DISPLAY`, `_LIMIT`, `_LOADIMAGE`, `_SNDOPEN`, `_KEYDOWN`, `_MOUSEX/Y`, `_PRINTSTRING`, `_RGB/_RGB32`
- **User-Defined Types** — `TYPE...END TYPE` with dot-access fields
- **Error Handling** — `ON ERROR GOTO`, `RESUME`, `ERR`/`ERL`
- **File I/O** — Sequential, Binary, and Random access modes
- **Integrated IDE** — Win32 + Scintilla editor with syntax highlighting, auto-complete, error navigation, context help
- **Portable** — Self-contained directory with bundled MinGW32 compiler; no installation or PATH changes needed
- **Automated Test Suite** — 25 tests across console, graphics, and QB64 categories

## Quick Start

```bat
REM 1. Transpile a BASIC program to C
rbbasic.exe hello.bas

REM 2. Compile the generated C to an executable
compile.bat

REM 3. Run it
program.exe
```

Or use the IDE:
```bat
rbide\rbide.exe
REM Open a .bas file → Press F5 to Compile & Run
```

## Installation

1. **Download or clone** this repository to any directory (e.g., `C:\rbbasic-portable`)

2. **MinGW32 Compiler** (required) — must be placed in the `mingw32\` subdirectory:
   - Download the **i686 (32-bit)** version with MSVCRT runtime from [winlibs.com](https://winlibs.com)
   - Extract so that `mingw32\bin\gcc.exe` exists

3. **SDL2 Libraries** (optional, required for graphics/sound):
   - Download [SDL2 development libraries](https://github.com/libsdl-org/SDL/releases) (mingw variant)
   - Extract the `i686-w64-mingw32` folder into `lib\SDL2-mingw32\`
   - For image loading: add SDL2_image development libraries
   - For sound: add SDL2_mixer development libraries

4. **Validate installation**:
   ```bat
   setup.bat
   ```

5. **Build the compiler** (if `rbbasic.exe` is not present):
   ```bat
   build.bat
   ```

## Usage

### Command-Line Compiler

```
rbbasic.exe <source.bas> [-o output.exe] [-c] [-v] [-h]
```

| Flag | Description |
|------|-------------|
| `-o <file>` | Output executable name (default: `program.exe`) |
| `-c` | Generate C code only (skip GCC compilation) |
| `-v` | Verbose output |
| `-h, --help` | Show help |

The compiler performs these steps:
1. **Lexical analysis** — tokenizes the `.bas` source
2. **Parsing** — builds an Abstract Syntax Tree (AST)
3. **Code generation** — emits `output.c` (and `compile.bat` for GCC invocation)
4. **Compilation** — runs GCC to produce `program.exe`

For graphics programs, SDL2 DLLs (`SDL2.dll`, etc.) are automatically copied next to the output executable.

### RBIDE (Integrated Development Environment)

A Win32-native IDE built with the Scintilla text editor component.

**Location**: `rbide\rbide.exe` (requires `SciLexer.dll` in the same directory)

**Features**:
- QBasic-style syntax highlighting (blue background, yellow keywords, green strings)
- **F5** — Compile & Run
- **F3** — Find Next
- **F4** — Navigate to next error
- **F1** — Context help for keyword under cursor
- **Ctrl+G** — Go to line
- **Ctrl+F** — Find & Replace
- Auto-complete (triggers after 3+ characters for BASIC keywords)
- Recent files menu (last 5 files)
- Automatic SDL2 detection — adds graphics/sound linker flags when needed
- Error parsing — click errors in the output panel to jump to the source line

**Building RBIDE**:
```bat
cd rbide
build.bat
```
Requires `SciLexer.dll` from [scintilla.org](https://www.scintilla.org/ScintillaDownload.html) or from a Notepad++ installation.

## Project Structure

```
rbbasic-portable/
├── src/                    # Compiler source code (C++17)
│   ├── main.cpp            # Compiler driver, CLI argument parsing
│   ├── lexer.cpp/h         # Lexical analyzer (tokenizer)
│   ├── parser.cpp/h        # Parser (AST builder)
│   ├── codegen.cpp/h       # C code generator
│   ├── ast.h               # AST node definitions
│   ├── token.cpp/h         # Token types and helpers
│   └── config.cpp/h        # Configuration file reader
├── rbide/                  # IDE source and build
│   ├── src/
│   │   ├── main.cpp        # IDE entry point (Win32 WinMain)
│   │   └── mainwindow.cpp  # Editor window, compilation, all IDE features
│   ├── build.bat           # IDE build script
│   └── lib/scintilla/      # Scintilla editor headers
├── examples/               # Example BASIC programs
│   ├── hello.bas           # Hello World
│   ├── guess.bas           # Number guessing game
│   ├── graphics.bas        # Graphics demo
│   ├── qb64_demo.bas       # QB64 extensions demo
│   ├── QBHikikomori/       # Full game project (console + graphics versions)
│   └── ...                 # More examples for each feature
├── tests/                  # Automated test suite
│   └── console/            # Console test .bas files + .expected output
├── mingw32/                # Bundled MinGW32 GCC toolchain (i686)
├── lib/
│   └── SDL2-mingw32/       # SDL2 development libraries
├── build.bat               # Build the compiler from source
├── setup.bat               # Validate installation, create rbc.config
├── clean.bat               # Remove generated/temporary files
├── rbc.config              # Compiler configuration (INI format)
└── rbbasic.exe             # The compiled BASIC-to-C transpiler
```

## Compiler Architecture

```
  source.bas
      │
      ▼
  ┌────────┐     ┌────────┐     ┌─────────────┐     ┌─────────────┐
  │ Lexer  │────▶│ Parser │────▶│ CodeGenerator│────▶│ GCC (MinGW) │
  │        │     │        │     │              │     │             │
  │ Tokens │     │  AST   │     │  output.c    │     │ program.exe │
  └────────┘     └────────┘     └─────────────┘     └─────────────┘
```

- **Lexer** (`lexer.cpp`) — Scans source text into tokens. Handles line numbers, keywords, string/number literals, compound tokens (e.g., `END IF`, `SELECT CASE`, `ON ERROR`).
- **Parser** (`parser.cpp`) — Recursive-descent parser producing an AST. Supports both numbered-line BASIC and modern block-structured code.
- **AST** (`ast.h`) — Node classes for every statement and expression type.
- **CodeGenerator** (`codegen.cpp`) — Tree-walks the AST, emitting C89-compatible code. Includes runtime helpers for string operations, graphics, file I/O, and error handling. Also generates `compile.bat` with appropriate GCC flags.
- **Compile modes**:
  - **Console** — Links with `shim_winmain.c` for console allocation, uses `-mconsole`
  - **Graphics (SDL2)** — Links with SDL2main, uses `-mwindows -lSDL2main -lSDL2`
  - **Audio-only console** — Console program with SDL2_mixer for sound, uses `-mconsole -lSDL2 -lSDL2_mixer`

## Language Reference

### Control Flow

| Statement | Description |
|-----------|-------------|
| `IF...THEN...ELSE...ELSEIF...END IF` | Block conditional |
| `IF...THEN linenum` | Old-style single-line conditional with GOTO |
| `SELECT CASE expr` / `CASE` / `END SELECT` | Multi-way branch (value, range, `IS` comparisons) |
| `FOR var = start TO end [STEP n]` / `NEXT` | Counted loop |
| `WHILE expr` / `WEND` | Pre-test loop |
| `DO [WHILE\|UNTIL expr]` / `LOOP [WHILE\|UNTIL expr]` | Flexible loop construct |
| `GOTO linenum` | Unconditional jump |
| `GOSUB linenum` / `RETURN` | Subroutine call (up to 1000 deep) |
| `ON expr GOTO line1, line2, ...` | Computed GOTO |
| `ON expr GOSUB line1, line2, ...` | Computed GOSUB |
| `EXIT FOR\|DO\|WHILE\|SUB\|FUNCTION` | Early exit from block |
| `END` | Terminate program |

### Variables and Data Types

- **Numeric variables**: `x`, `counter`, `myVar` — stored as `double`
- **String variables**: `name$`, `text$` — suffix `$` denotes string (up to 4096 characters)
- **Type suffixes**: `%` (integer), `&` (long), `!` (single), `#` (double) — accepted by lexer
- **Arrays**: `DIM arr(100)`, `DIM grid(10, 10)` — 1-based by default, multi-dimensional
- **Constants**: `CONST PI = 3.14159`, `CONST TITLE$ = "My App"`
- **DATA / READ / RESTORE**: Inline data lists read sequentially

### Declarations and User Types

```basic
DIM myArray(100)                    ' Simple array
DIM SHARED globalVar                ' Module-level shared variable
DIM STATIC localPersist             ' Persists across SUB calls
REDIM dynArray(n)                   ' Dynamic resize
REDIM PRESERVE dynArray(n * 2)      ' Resize keeping data
ERASE myArray                       ' Clear array

TYPE PersonType                     ' User-defined record
    FirstName AS STRING
    LastName AS STRING
    Age AS INTEGER
END TYPE

DIM person AS PersonType
person.FirstName = "John"
```

### Subroutines and Functions

```basic
SUB GreetUser (name$)
    PRINT "Hello, "; name$
END SUB

FUNCTION Square (x)
    Square = x * x
END FUNCTION

DEF FNdouble(x) = x * 2            ' Single-line function

CALL GreetUser("World")
PRINT Square(5)
PRINT FNdouble(7)
```

### String Functions

| Function | Description |
|----------|-------------|
| `LEFT$(s$, n)` | First n characters |
| `RIGHT$(s$, n)` | Last n characters |
| `MID$(s$, start, len)` | Substring extraction |
| `MID$(s$, start, len) = r$` | Substring assignment (in-place) |
| `LEN(s$)` | String length |
| `INSTR([start,] s$, find$)` | Find substring position |
| `CHR$(n)` | Character from ASCII code |
| `ASC(s$)` | ASCII code of first character |
| `STR$(n)` | Number to string |
| `VAL(s$)` | String to number |
| `UCASE$(s$)` / `LCASE$(s$)` | Case conversion |
| `LTRIM$(s$)` / `RTRIM$(s$)` / `TRIM$(s$)` | Whitespace trimming |
| `SPACE$(n)` | String of n spaces |
| `STRING$(n, char)` | Repeat character n times |
| `HEX$(n)` / `OCT$(n)` | Numeric base conversion |
| `DATE$` / `TIME$` | Current date/time strings |
| `INKEY$` | Non-blocking keyboard read |

### Math Functions

| Function | Description |
|----------|-------------|
| `SIN(x)`, `COS(x)`, `TAN(x)`, `ATN(x)` | Trigonometry (radians) |
| `SQR(x)` | Square root |
| `ABS(x)` | Absolute value |
| `INT(x)` | Floor to integer |
| `SGN(x)` | Sign (-1, 0, 1) |
| `RND` | Random number 0.0-1.0 |
| `LOG(x)` | Natural logarithm |
| `EXP(x)` | e^x |
| `CINT(x)` / `FIX(x)` | Integer conversion |
| `TIMER` | Seconds since midnight |
| `LBOUND(arr)` / `UBOUND(arr)` | Array bounds |

### File I/O

```basic
' Sequential read
OPEN "data.txt" FOR INPUT AS #1
LINE INPUT #1, text$
INPUT #1, value
CLOSE #1

' Sequential write
OPEN "out.txt" FOR OUTPUT AS #2
PRINT #2, "Hello"
WRITE #2, name$, age, score
CLOSE #2

' Binary access
OPEN "file.dat" FOR BINARY AS #3
PUT #3, 1, value
GET #3, 1, result
SEEK #3, position
CLOSE #3

' Random access
OPEN "db.dat" FOR RANDOM AS #4 LEN = 64
PUT #4, recnum, record
GET #4, recnum, record
CLOSE #4
```

**File functions**: `EOF(n)`, `LOC(n)`, `LOF(n)`, `SEEK(n)` (as function)

### Error Handling

```basic
ON ERROR GOTO 9000           ' Set error handler
' ... code that might error ...
ON ERROR GOTO 0              ' Disable handler

9000 REM Error handler
PRINT "Error"; ERR; "at line"; ERL
RESUME NEXT                  ' Continue after the error
```

### Console I/O

| Statement | Description |
|-----------|-------------|
| `PRINT expr; expr` | Output with formatting (`;` = no space, `,` = tab) |
| `PRINT USING fmt$; values` | Formatted output (`###.##`, `$$`, etc.) |
| `INPUT "prompt"; var` | Read user input |
| `LINE INPUT var$` | Read entire line |
| `INPUT$(n)` | Read n characters |
| `CLS` | Clear screen |
| `LOCATE row, col` | Position cursor |
| `COLOR fg, bg` | Set text colors |

### Graphics (SDL2)

All graphics operations use SDL2 for rendering with a pixel buffer for precise drawing.

```basic
SCREEN 12                         ' 640x480 16-color mode
LINE (0, 0)-(639, 479), 1, BF    ' Filled blue rectangle
CIRCLE (320, 240), 100, 14       ' Yellow circle
PSET (100, 100), 4               ' Red pixel
PAINT (320, 240), 2, 14          ' Flood fill green inside yellow border
PALETTE 1, 255                   ' Remap color 1

' Turtle graphics
DRAW "BM100,100 C14 R50 D50 L50 U50"

' Sprites
DIM sprite(100)
GET (0, 0)-(15, 15), sprite      ' Capture
PUT (100, 100), sprite, PSET     ' Draw
```

| Statement | Description |
|-----------|-------------|
| `SCREEN mode` | Set graphics mode |
| `PSET (x, y), color` | Plot pixel |
| `LINE (x1,y1)-(x2,y2), color [,B\|BF]` | Line, box, or filled box |
| `CIRCLE (x,y), radius, color [,start, end, aspect]` | Circle/arc/ellipse |
| `PAINT (x,y), fill_color, border_color` | Scanline flood fill |
| `DRAW cmdstring$` | GML turtle graphics |
| `GET (x1,y1)-(x2,y2), array` | Capture screen region to array |
| `PUT (x,y), array, action` | Draw sprite (PSET, PRESET, AND, OR, XOR) |
| `PALETTE [index, color]` | Set/reset palette |
| `VIEW (x1,y1)-(x2,y2)` | Set clipping viewport |
| `POINT(x, y)` | Get pixel color |

### Sound

```basic
SOUND 440, 18          ' 440 Hz for ~1 second (18.2 ticks/sec)
BEEP                   ' System beep

' MML music
PLAY "T120 O4 L4 CDEFG2"
' Notes: A-G, # for sharp, - for flat
' O = octave (1-8), L = length (1-64), T = tempo (32-255)
' P = pause
```

### System

| Statement/Function | Description |
|-----------|-------------|
| `SHELL "command"` | Execute system command |
| `SYSTEM` | Exit program |
| `ENVIRON$("VAR")` | Read environment variable |
| `ENVIRON "VAR=value"` | Set environment variable |
| `COMMAND$` | Command-line arguments |
| `SLEEP n` | Sleep n seconds |
| `RANDOMIZE [seed]` | Seed random number generator |
| `PEEK(addr)` / `POKE addr, val` | Read/write emulated 64KB memory |
| `DEF SEG = segment` | Set memory segment |

### QB64 Extensions

RB BASIC supports a subset of QB64 extensions for modern graphics and input:

**Display Control**:
- `_DISPLAY` — Flip back buffer to screen (enables manual rendering mode)
- `_LIMIT fps` — Cap frame rate and process events

**Image Management**:
- `handle = _NEWIMAGE(w, h, 32)` — Create 32-bit image
- `_FREEIMAGE handle` — Release image
- `handle = _LOADIMAGE("file.png")` — Load PNG/JPG (requires SDL2_image)
- `_PUTIMAGE (x, y), handle` — Draw image to screen

**Text Rendering**:
- `_PRINTSTRING (x, y), text$` — Draw text at pixel coordinates (8x8 CP437 bitmap font)

**Color**:
- `_RGB(r, g, b)` — 24-bit color value
- `_RGB32(r, g, b [, a])` — 32-bit color with optional alpha

**Mouse Input**:
- `_MOUSEX`, `_MOUSEY` — Mouse coordinates
- `_MOUSEBUTTON(n)` — Button state (1=left, 2=right, 3=middle)
- `_MOUSEWHEEL` — Wheel scroll ticks

**Keyboard**:
- `_KEYDOWN(keycode)` — Non-blocking key state (SDL keycodes: a-z = 97-122, Enter = 13, Space = 32)

**Sound** (requires SDL2_mixer):
- `handle = _SNDOPEN("file.wav")` — Load WAV/OGG
- `_SNDPLAY handle` — Play sound
- `_SNDSTOP handle` — Stop sound
- `_SNDVOL handle, volume` — Set volume (0.0 to 1.0)

### Operators

| Type | Operators |
|------|-----------|
| Arithmetic | `+` `-` `*` `/` `\` (integer div) `MOD` `^` (power) |
| Comparison | `=` `<>` `<` `>` `<=` `>=` |
| Logical | `AND` `OR` `NOT` |
| String | `+` (concatenation) |

## Graphics Modes

| SCREEN Mode | Resolution | Notes |
|-------------|-----------|-------|
| 0 | Text mode | Console only (default) |
| 1 | 320x200 | CGA compatible |
| 2 | 640x200 | CGA high-res |
| 7 | 320x200 | EGA |
| 9 | 640x350 | EGA high-res |
| 12 | 640x480 | VGA |
| 13 | 320x200 | VGA 256-color (MCGA) |
| 14 | 800x600 | SVGA (RB BASIC extension) |

All graphics modes use SDL2 rendering with a 16-color EGA palette (remappable via `PALETTE`). Values > 63 in PALETTE are treated as packed `&HRRGGBB` colors for full 24-bit support.

## Examples

The `examples/` directory contains demonstration programs for every major feature:

| File | Category | Description |
|------|----------|-------------|
| `hello.bas` | Console | Hello World with user input |
| `guess.bas` | Console | Number guessing game |
| `strings.bas` | Console | String function demonstrations |
| `loops.bas` | Console | FOR, WHILE, DO/LOOP examples |
| `select_case.bas` | Console | SELECT CASE with value, range, IS |
| `exit_loops.bas` | Console | EXIT FOR, EXIT DO, EXIT WHILE |
| `const_swap.bas` | Console | CONST and SWAP statements |
| `redim_test.bas` | Console | REDIM and REDIM PRESERVE |
| `declare_test.bas` | Console | Forward declarations |
| `print_using.bas` | Console | PRINT USING format strings |
| `types_demo.bas` | Console | TYPE...END TYPE user records |
| `error_handling.bas` | Console | ON ERROR GOTO/RESUME/ERR/ERL |
| `write_test.bas` | Console | WRITE# CSV output |
| `binary_file.bas` | Console | BINARY and RANDOM file I/O |
| `mid_assign.bas` | Console | MID$ assignment |
| `system_test.bas` | Console | SHELL, ENVIRON, COMMAND$ |
| `graphics.bas` | Graphics | PSET, LINE, CIRCLE basics |
| `graphtest.bas` | Graphics | Graphics mode testing |
| `graphdemo.bas` | Graphics | Comprehensive drawing demo |
| `draw_demo.bas` | Graphics | DRAW GML turtle graphics |
| `flood_fill.bas` | Graphics | PAINT flood fill |
| `sprites.bas` | Graphics | GET/PUT sprite operations |
| `qb64_demo.bas` | QB64 | _DISPLAY, _LIMIT, _PRINTSTRING, _RGB |
| `qb64_mouse.bas` | QB64 | _MOUSEX/Y, _MOUSEBUTTON |
| `qb64_image.bas` | QB64 | _LOADIMAGE, _PUTIMAGE |
| `qb64_sound.bas` | QB64 | _SNDOPEN, _SNDPLAY, _SNDVOL |
| `homelessness.bas` | Console | Interactive fiction game |
| `QBHikikomori/` | Game | Full hikikomori simulation game (console + graphics versions) |

## Testing

The test suite validates 25 tests across 4 categories:

- **Console tests** (4): `math_vars`, `loops_basic`, `control_flow`, `subs_funcs` — custom tests with `.expected` output files, compared using `fc /w`
- **Example console tests** (11): Reuse example programs that have `.expected` output — `select_case`, `exit_loops`, `const_swap`, `redim_test`, `declare_test`, `mid_assign`, `print_using`, `types_demo`, `error_handling`, `write_test`, `binary_file`
- **Graphics tests** (6): Compile-only (transpile + GCC must succeed) — `graphics`, `graphtest`, `graphdemo`, `draw_demo`, `flood_fill`, `sprites`
- **QB64 tests** (4): Compile-only — `qb64_demo`, `qb64_mouse`, `qb64_sound`, `qb64_image`

Tests are located in `tests/console/` (dedicated tests) and `examples/` (reused examples).

## Building from Source

### Build the Compiler

```bat
build.bat
```

Or manually:
```bat
mingw32\bin\g++.exe -o rbbasic.exe ^
    src\main.cpp src\config.cpp src\lexer.cpp ^
    src\parser.cpp src\codegen.cpp src\token.cpp ^
    -std=c++17 -O2 -s -static -Wall ^
    -Wno-unused-variable -Wno-unused-but-set-variable
```

### Build the IDE

```bat
cd rbide
build.bat
```

The IDE requires:
- Scintilla headers (included in `rbide\lib\scintilla\`)
- `SciLexer.dll` at runtime (download from [scintilla.org](https://www.scintilla.org/ScintillaDownload.html) or copy from Notepad++)

## Configuration

`rbc.config` (INI format, auto-generated by `setup.bat`):

```ini
[Paths]
CompilerRoot=.
MinGW32Path=mingw32
SDL2Path=lib\SDL2-mingw32
OutputPath=output
ExamplesPath=examples

[Compiler]
GCC=mingw32\bin\gcc.exe
GPP=mingw32\bin\g++.exe
Optimization=-O1
Warnings=-Wall -Wno-unused-label -Wno-unknown-pragmas

[SDL2]
IncludePath=lib\SDL2-mingw32\i686-w64-mingw32\include
LibPath=lib\SDL2-mingw32\i686-w64-mingw32\lib
BinPath=lib\SDL2-mingw32\i686-w64-mingw32\bin

[Options]
AutoCopySDL=true        ; Auto-copy SDL2.dll next to output exe
CleanupTemp=false       ; Remove intermediate files after compile
ShowCompileProgress=true
VerboseOutput=false
```

## Dependencies

| Component | Purpose | Required |
|-----------|---------|----------|
| MinGW32 (i686 GCC) | C/C++ compilation backend | Yes |
| SDL2 | Graphics rendering, window management | For graphics programs |
| SDL2_image | PNG/JPG image loading (`_LOADIMAGE`) | For `_LOADIMAGE` |
| SDL2_mixer | WAV/OGG sound playback (`_SNDOPEN`) | For `_SNDOPEN` |
| SciLexer.dll | Scintilla editor component | For RBIDE only |

All dependencies are bundled in the portable directory — no external installations required at runtime.

## Known Limitations

- **Windows only** — targets Win32 with MinGW32 (cross-platform C output could be adapted)
- **32-bit executables** — uses i686 MinGW toolchain
- **`_PRINTSTRING` font** — uses an 8x8 pixel CP437 bitmap font (96 glyphs, ASCII 32-126)
- **String buffer size** — 4096 bytes per string variable (configurable via `RB_STR_SIZE` in generated code)
- **No `&H` hex literals** — use decimal equivalents (e.g., `255` instead of `&HFF`)
- **`_PUTIMAGE` syntax** — supports `_PUTIMAGE (dx, dy), handle` only (no destination range)
- **SUB/FUNCTION with string parameters** — has known codegen issues in some cases
- **GOSUB stack** — limited to 1000 nested calls
- **File handles** — up to 256 concurrent open files
- **Image handles** — up to 256 loaded images
- **Sound handles** — up to 64 loaded sounds

## License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

Copyright (c) 2025 Ronen Blumberg.
