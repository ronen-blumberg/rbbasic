===============================================================================
RB BASIC (Ronen Blumberg BASIC) - Portable Edition v2.0
Full GW-BASIC/QBasic Compatible Compiler
===============================================================================

WELCOME!

This is a portable, self-contained BASIC compiler that works from any location:
  - Hard drive
  - USB flash drive
  - Network drive
  - No installation required
  - No administrator rights needed

===============================================================================
QUICK START
===============================================================================

STEP 1: ADD MINGW32 COMPILER
  1. Download MinGW32 from https://winlibs.com
  2. Get the i686 (32-bit) version with MSVCRT runtime
  3. Extract the mingw32 or mingw64 folder
  4. Rename it to "mingw32" if needed
  5. Place it in this folder so you have:
       rbbasic-portable\
         mingw32\
           bin\
             gcc.exe
             g++.exe
             ...

STEP 2: (OPTIONAL) ADD SDL2 FOR GRAPHICS
  1. Download SDL2 from https://github.com/libsdl-org/SDL/releases
  2. Get SDL2-devel-x.x.x-mingw.tar.gz
  3. Extract and find the i686-w64-mingw32 folder
  4. Place it in lib\SDL2-mingw32\ so you have:
       rbbasic-portable\
         lib\
           SDL2-mingw32\
             i686-w64-mingw32\
               include\
               lib\
               bin\

STEP 3: RUN SETUP
  Double-click: setup.bat
  
  This validates your installation and creates the configuration file.

STEP 4: BUILD THE COMPILER
  Double-click: build.bat
  
  This compiles rbbasic.exe from the source code in the src\ folder.

STEP 5: TEST IT!
  Create a file called hello.bas with this content:
  
    10 PRINT "Hello from RB BASIC!"
    20 PRINT "Ronen Blumberg BASIC v2.0"
    30 END
  
  Then run these commands:
    rbbasic hello.bas
    compile.bat
    program.exe

===============================================================================
FOLDER STRUCTURE
===============================================================================

rbbasic-portable\
  |
  +-- rbbasic.exe          Main compiler (created by build.bat)
  +-- rbc.config           Configuration file
  +-- build.bat            Build the compiler from source
  +-- setup.bat            Validate and configure installation
  +-- clean.bat            Remove temporary files
  +-- README.txt           This file
  |
  +-- mingw32\             MinGW32 GCC compiler (you add this)
  |     +-- bin\
  |     +-- lib\
  |     +-- include\
  |
  +-- lib\                 Libraries
  |     +-- SDL2-mingw32\  SDL2 graphics library (optional, you add this)
  |
  +-- src\                 Compiler source code
  |     +-- main.cpp
  |     +-- config.cpp/h
  |     +-- lexer.cpp/h
  |     +-- parser.cpp/h
  |     +-- codegen.cpp/h
  |     +-- token.cpp/h
  |     +-- ast.h
  |
  +-- examples\            Example BASIC programs (you can add these)
  |
  +-- docs\                Documentation (you can add manuals here)
  |
  +-- output\              Temporary build files (auto-created)

===============================================================================
SYSTEM REQUIREMENTS
===============================================================================

  - Windows 7 or later (32-bit or 64-bit)
  - At least 500 MB free space (for MinGW32)
  - No other dependencies

===============================================================================
FEATURES
===============================================================================

PHASE 1 - Core Features:
  * INPUT, LINE INPUT - User input
  * PRINT - Output with formatting
  * Variables (numeric and string with $)
  * Arrays with DIM
  * String functions: LEFT$, RIGHT$, MID$, LEN, INSTR, CHR$, ASC, etc.
  * Math functions: SIN, COS, TAN, SQR, ABS, INT, RND, etc.
  * FOR...NEXT loops with STEP
  * IF...THEN...ELSE statements

PHASE 2 - Control Flow:
  * WHILE...WEND loops
  * DO...LOOP (WHILE/UNTIL variants)
  * GOSUB...RETURN
  * Block IF...THEN...ELSE...END IF
  * ON...GOTO and ON...GOSUB

PHASE 3 - Advanced:
  * DATA, READ, RESTORE
  * SUB...END SUB procedures
  * FUNCTION...END FUNCTION
  * File I/O (OPEN, CLOSE, INPUT#, PRINT#, LINE INPUT#)
  * DEF FN user-defined functions

GRAPHICS (with SDL2):
  * SCREEN - Set graphics mode
  * PSET - Plot pixels
  * LINE - Draw lines and boxes
  * CIRCLE - Draw circles and arcs
  * PAINT - Fill areas
  * CLS - Clear screen
  * COLOR - Set colors
  * LOCATE - Position cursor

SOUND:
  * SOUND - Generate tones
  * PLAY - Music macro language
  * BEEP - System beep

===============================================================================
GETTING HELP
===============================================================================

COMMAND LINE:
  rbbasic --help         Show compiler help
  rbbasic file.bas       Compile a BASIC program
  rbbasic file.bas -v    Verbose output
  rbbasic file.bas -o myprogram.exe    Custom output name

DOCUMENTATION:
  See the docs\ folder for complete language manuals and guides.

EXAMPLES:
  Check the examples\ folder for sample BASIC programs.

===============================================================================
PORTABILITY
===============================================================================

This entire folder can be:
  * Copied to any location
  * Moved to a USB drive
  * Shared with others
  * Run without installation

No system PATH configuration is needed!
Everything is self-contained.

===============================================================================
TROUBLESHOOTING
===============================================================================

PROBLEM: "GCC not found" error
SOLUTION: Make sure mingw32\bin\gcc.exe exists
          Run setup.bat to validate

PROBLEM: Graphics don't work
SOLUTION: Add SDL2 libraries to lib\SDL2-mingw32\
          SDL2 is optional for text-only programs

PROBLEM: Build fails
SOLUTION: Check that all source files are in src\ folder
          Make sure MinGW32 is properly installed
          Run setup.bat to check configuration

PROBLEM: Program won't run on USB drive
SOLUTION: This should work fine! Make sure you:
          1. Copied the entire folder
          2. MinGW32 is in the mingw32 subfolder
          3. Run setup.bat on the USB drive

===============================================================================
COMPATIBILITY
===============================================================================

RB BASIC is compatible with:
  * GW-BASIC - Full compatibility
  * QBasic - Full compatibility
  * QuickBASIC - Most features
  * QB64 - Core features

Classic BASIC programs should compile and run with minimal or no changes.

===============================================================================
LICENSE & COPYRIGHT
===============================================================================

RB BASIC (Ronen Blumberg BASIC) Compiler
Copyright (c) 2025 Ronen Blumberg

Free for personal and educational use.

This distribution includes:
  * MinGW-w64 GCC compiler (you add this)
    License: Various open source licenses
    Website: https://mingw-w64.org

  * SDL2 library (optional, you add this)
    License: zlib license
    Website: https://libsdl.org

===============================================================================
VERSION HISTORY
===============================================================================

Version 2.0 - October 2025
  * Portable edition with self-contained distribution
  * Configuration system (rbc.config)
  * Full GW-BASIC/QBasic compatibility
  * SDL2 graphics support
  * Sound support
  * Complete Phase 1-3 features

===============================================================================
CONTACT & UPDATES
===============================================================================

For updates, bug reports, and more information, check the project repository
or website (add your links here when available).

===============================================================================

Thank you for using RB BASIC!

Happy BASIC Programming!

Ronen Blumberg
October 2025

===============================================================================
