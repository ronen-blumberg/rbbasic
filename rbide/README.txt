===============================================================================
RBIDE - RB BASIC Integrated Development Environment
Version 1.0 - Ronen Blumberg Basic Edition
===============================================================================

OVERVIEW
--------
RBIDE is a classic Win32-based IDE for RB BASIC (Ronen Blumberg Basic).
It provides a retro-styled QBASIC code editor with authentic 1980s/90s colors,
syntax highlighting, and integrated compilation support.

FEATURES
--------
- Authentic QBASIC color scheme (blue background, yellow text, cyan keywords)
- Scintilla-based code editor with GWBASIC/QBASIC syntax highlighting
- File operations: New, Open, Save, Save As
- Edit operations: Undo, Redo, Cut, Copy, Paste, Select All
- BASIC line numbering: Add, Renumber, Remove (with GOTO/GOSUB updates)
- Integrated compilation: Compile, Run, Compile & Run (F5)
- Output panel showing compilation results
- Status bar with file information
- Classic Win32 interface - fast and lightweight
- Keyboard shortcuts (Ctrl+N/O/S, F5, etc.)

SYSTEM REQUIREMENTS
-------------------
- Windows 7 or later (Windows 10/11 recommended)
- rbbasic.exe compiler (must be in parent directory)
- SciLexer.dll (Scintilla editor library)
- MinGW32 for building (specified in rbc.config)

FOLDER STRUCTURE
----------------
This folder should be placed inside your rbbasic-portable directory:

rbbasic-portable\
  ├── rbbasic.exe          - RB BASIC compiler
  ├── rbc.config           - Configuration (MINGW_PATH=mingw32)
  ├── mingw32\             - MinGW compiler (32-bit)
  │   └── bin\
  │       ├── gcc.exe
  │       ├── g++.exe
  │       └── windres.exe
  │
  └── rbide\               - THIS FOLDER
      ├── rbide.exe        - The IDE (after build)
      ├── SciLexer.dll     - Editor component (must download)
      ├── build.bat        - Build script
      ├── README.txt       - This file
      │
      ├── src\             - Source code
      │   ├── main.cpp
      │   ├── mainwindow.cpp
      │   ├── mainwindow.h
      │   └── resource.h
      │
      ├── res\             - Resources (optional)
      │   ├── resource.rc
      │   └── rbide.ico
      │
      ├── obj\             - Build objects (created during build)
      └── docs\            - Documentation

INSTALLATION & BUILDING
-----------------------
1. Ensure this rbide folder is inside rbbasic-portable\

2. Verify rbc.config exists in parent directory:
   - File: ..\rbc.config
   - Should contain: MINGW_PATH=mingw32 (or your MinGW folder name)

3. Download SciLexer.dll:
   Method A - Official Website:
     - From: https://www.scintilla.org/ScintillaDownload.html
     - Download Windows binary package
     - Extract SciLexer.dll
     
   Method B - From Notepad++:
     - Find: C:\Program Files\Notepad++\SciLexer.dll
     - Copy to rbide\ folder
   
   Place SciLexer.dll in rbide\ folder (same as rbide.exe)

4. Build RBIDE:
   - Run: build.bat
   - The script will:
     * Read rbc.config from parent directory
     * Find MinGW compiler from config
     * Compile and link rbide.exe
   - Output: rbide.exe

5. Run RBIDE:
   - Double-click: rbide.exe
   - Or command line: rbide.exe yourfile.bas

HOW RBC.CONFIG INTEGRATION WORKS
---------------------------------
The build.bat script reads the parent directory's rbc.config file to find
the MinGW compiler location. This ensures RBIDE uses the same compiler
setup as the RB BASIC compiler itself.

Example rbc.config:
  MINGW_PATH=mingw32

The build script will then look for:
  ..\mingw32\bin\g++.exe
  ..\mingw32\bin\windres.exe

This allows the entire rbbasic-portable setup to be self-contained and
portable without requiring environment variables or system-wide installation.

USAGE
-----
File Menu:
  New (Ctrl+N)      - Create new BASIC program
  Open (Ctrl+O)     - Open existing .BAS file
  Save (Ctrl+S)     - Save current file
  Save As           - Save with new name
  Exit              - Close RBIDE

Edit Menu:
  Undo (Ctrl+Z)     - Undo last change
  Redo (Ctrl+Y)     - Redo last undo
  Cut (Ctrl+X)      - Cut selection
  Copy (Ctrl+C)     - Copy selection
  Paste (Ctrl+V)    - Paste from clipboard
  Select All (Ctrl+A) - Select entire document
  Find (Ctrl+F)     - Find text [Coming soon]
  
  Add Line Number   - Add next sequential BASIC line number (10, 20, 30...)
  Renumber Lines    - Renumber all lines, updates GOTO/GOSUB/THEN
  Remove Line Numbers - Strip all line numbers from code

Run Menu:
  Compile (F7)      - Compile current file to C and executable
  Run (F5)          - Run the compiled program
  Compile & Run     - Compile then run (one step)
  Stop              - Stop running program [Coming soon]

Help Menu:
  Manual            - Open RB BASIC language manual
  About             - About RBIDE

QBASIC COLOR SCHEME
-------------------
The IDE features authentic QBASIC colors from the 1980s/90s:

  Element           Color               RGB Values
  ------------------------------------------------
  Background        Blue                0, 0, 170
  Default Text      Bright Yellow       255, 255, 85
  Keywords          Cyan (Bold)         85, 255, 255
  Strings           Magenta             255, 85, 255
  Comments          Bright Green        85, 255, 85
  Numbers           White               255, 255, 255
  Line Numbers      Gray                170, 170, 170
  Selection         Dark Blue           0, 0, 128

Example how code appears:
  10 REM Hello World    - Line number: Gray, REM: Cyan, comment: Green
  20 PRINT "Hi!"        - PRINT: Cyan, "Hi!": Magenta
  30 END                - END: Cyan

KEYBOARD SHORTCUTS
------------------
File Operations:
  Ctrl+N        - New file
  Ctrl+O        - Open file
  Ctrl+S        - Save file

Edit Operations:
  Ctrl+Z        - Undo
  Ctrl+Y        - Redo
  Ctrl+X        - Cut
  Ctrl+C        - Copy
  Ctrl+V        - Paste
  Ctrl+A        - Select All
  Ctrl+F        - Find [Coming soon]

Run Operations:
  F5            - Compile & Run
  F7            - Compile only

BASIC LINE NUMBERING FEATURES
------------------------------
The IDE includes powerful line numbering tools for classic BASIC programming:

Add Line Number:
  - Automatically adds the next line number (increments by 10)
  - Example: If your last line is 20, adds "30 " for you
  
Renumber Lines:
  - Renumbers all lines in steps of 10 (10, 20, 30, 40...)
  - Intelligently updates GOTO, GOSUB, and THEN references!
  - Example:
    Before:            After renumbering:
    11 GOTO 13        10 GOTO 20
    13 PRINT "Hi"     20 PRINT "Hi"
    
Remove Line Numbers:
  - Strips all line numbers, leaving just the code
  - Useful when transitioning to modern BASIC syntax

COMPILATION PROCESS
-------------------
When you compile (F5 or F7):

1. RBIDE saves your .BAS file
2. Calls: ..\rbbasic.exe yourfile.bas
3. RB BASIC compiler transpiles BASIC to C
4. MinGW compiles C to .exe
5. Output shown in bottom panel
6. If using F5, program automatically runs

The compiler must be in the parent directory (rbbasic-portable\rbbasic.exe)
or accessible via PATH environment variable.

SYNTAX HIGHLIGHTING
-------------------
The IDE recognizes 80+ BASIC keywords including:

Control Flow:
  FOR, NEXT, IF, THEN, ELSE, ELSEIF, WHILE, WEND, DO, LOOP,
  GOTO, GOSUB, RETURN, SELECT, CASE

I/O:
  PRINT, INPUT, LINE INPUT, OPEN, CLOSE, GET, PUT, READ, DATA,
  WRITE, LPRINT

Math Functions:
  ABS, ATN, COS, EXP, FIX, INT, LOG, RND, SGN, SIN, SQR, TAN

String Functions:
  CHR$, LEFT$, RIGHT$, MID$, LEN, INSTR, STR$, VAL, ASC,
  LCASE$, UCASE$, LTRIM$, RTRIM$, SPACE$, STRING$

Graphics (if supported):
  SCREEN, PSET, LINE, CIRCLE, PAINT, DRAW, PALETTE, COLOR,
  CLS, LOCATE

System:
  BEEP, SOUND, PLAY, TIMER, DATE$, TIME$, SHELL, SYSTEM,
  ENVIRON$, COMMAND$, END, STOP, SLEEP

And many more!

TROUBLESHOOTING
---------------
Problem: "Failed to load SciLexer.dll"
Solution: 
  - Download SciLexer.dll from https://www.scintilla.org/
  - Place in same folder as rbide.exe
  - Make sure it's the correct architecture (32-bit for 32-bit rbide.exe)

Problem: "rbc.config not found"
Solution:
  - Ensure rbide folder is inside rbbasic-portable\
  - Check that rbc.config exists in parent directory
  - File should contain: MINGW_PATH=mingw32

Problem: "MinGW32 compiler not found"
Solution:
  - Check rbc.config MINGW_PATH setting
  - Verify MinGW32 is installed in specified folder
  - Expected: ..\mingw32\bin\g++.exe

Problem: Build fails
Solution: 
  - Verify rbc.config is correct
  - Check that MinGW32 is in the right location
  - Ensure all source files are present in src\

Problem: "rbbasic.exe not found" when compiling
Solution: 
  - Ensure rbbasic.exe is in parent directory
  - Or add to PATH environment variable
  - Expected location: ..\rbbasic.exe

Problem: No syntax highlighting
Solution: 
  - Check that SciLexer.dll is present and correct version
  - Try downloading fresh copy from Scintilla website
  - Scintilla 4.x or 5.x recommended

EXAMPLE BASIC PROGRAM
----------------------
Try this in RBIDE:

10 REM Fibonacci Sequence
20 PRINT "Fibonacci Numbers"
30 INPUT "How many"; N
40 A = 0: B = 1
50 FOR I = 1 TO N
60   PRINT A
70   C = A + B
80   A = B: B = C
90 NEXT I
100 END

Watch the colors:
- Line numbers (10-100): Gray
- REM, PRINT, INPUT, FOR, NEXT, END: Cyan (bold)
- "Fibonacci Numbers", "How many": Magenta
- Comment text: Green
- Variables (N, A, B, C, I): Yellow
- Numbers (0, 1): White

ADVANCED FEATURES
-----------------
Command Line Usage:
  rbide.exe                    - Start with blank file
  rbide.exe myprogram.bas      - Open specific file
  rbide.exe "path\file.bas"    - Open file with path

Multiple Instances:
  You can run multiple RBIDE windows simultaneously to work on
  different BASIC programs at the same time.

File Associations:
  To make .BAS files open in RBIDE by default:
  1. Right-click a .BAS file
  2. Choose "Open with" > "Choose another app"
  3. Browse to rbide.exe
  4. Check "Always use this app"

PLANNED ENHANCEMENTS
--------------------
Future versions may include:
  - Find/Replace dialog
  - Syntax error checking
  - Code completion
  - Debugger integration
  - Breakpoints
  - Variable watch window
  - Multiple file tabs
  - Project management
  - Integrated help system
  - Customizable color themes

TECHNICAL DETAILS
-----------------
Language:        C++17
Compiler:        MinGW32 GCC (from rbc.config)
GUI Framework:   Win32 API (native Windows)
Editor:          Scintilla 5.x
Lexer:           SCLEX_FREEBASIC (adapted for GWBASIC/QBASIC)
Build System:    Batch scripts (reads rbc.config)

Executable Size: ~200-400 KB (without SciLexer.dll)
Memory Usage:    ~10-20 MB typical
Startup Time:    < 1 second

LICENSING
---------
RBIDE Source Code:
  Copyright (c) 2025 Ronen Blumberg
  Free for personal and educational use

Scintilla Editor Component:
  Copyright 1998-2024 by Neil Hodgson <neilh@scintilla.org>
  Licensed under MIT License
  https://www.scintilla.org/

RB BASIC Compiler:
  Copyright (c) 2025 Ronen Blumberg
  See main RB BASIC documentation for details

CREDITS
-------
RBIDE - Created by Ronen Blumberg
RB BASIC Language & Compiler - Ronen Blumberg
Scintilla Editor - Neil Hodgson (https://www.scintilla.org)
Inspired by: GWBASIC and QBASIC (Microsoft, 1980s-1990s)

CONTACT & SUPPORT
-----------------
For bugs, suggestions, or questions about RBIDE, please refer to the
main RB BASIC project documentation and support channels.

For Scintilla-specific issues, see: https://www.scintilla.org/

===============================================================================
Version: 1.0
Date: October 2025
RB BASIC - Ronen Blumberg Basic
Copyright (c) 2025 Ronen Blumberg
===============================================================================
