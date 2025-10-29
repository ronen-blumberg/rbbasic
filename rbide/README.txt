===============================================================================
RBIDE - RB BASIC Integrated Development Environment
Version 1.0
===============================================================================

OVERVIEW
--------
RBIDE is a classic Win32-based IDE for RB BASIC (Ronen Blumberg BASIC).
It provides a simple, fast code editor with syntax highlighting and integrated
compilation support.

FEATURES
--------
- Scintilla-based code editor with syntax highlighting
- File operations: New, Open, Save, Save As
- Edit operations: Undo, Redo, Cut, Copy, Paste, Select All
- Integrated compilation: Compile, Run, Compile & Run
- Output panel showing compilation results
- Status bar with line/column position
- Classic Win32 interface - fast and lightweight

SYSTEM REQUIREMENTS
-------------------
- Windows 7 or later
- rbbasic.exe compiler (must be in parent directory or PATH)
- Scintilla.dll (required for editor functionality)

INSTALLATION
------------
1. This folder (rbide) should be inside rbbasic-portable\
2. Download Scintilla.dll:
   - From: https://www.scintilla.org/ScintillaDownload.html
   - Or: Extract from Notepad++ installation
   - Place Scintilla.dll in rbide\ folder (same as rbide.exe)

3. Build RBIDE:
   - Run: build.bat
   - This creates rbide.exe

4. Run RBIDE:
   - Double-click rbide.exe
   - Or: rbide.exe yourfile.bas

FOLDER STRUCTURE
----------------
rbide\
  ├── rbide.exe           - The IDE executable (after build)
  ├── Scintilla.dll       - Editor component (you must download)
  ├── build.bat           - Build script
  ├── README.txt          - This file
  │
  ├── src\                - Source code
  │   ├── main.cpp
  │   ├── mainwindow.cpp/h
  │   └── resource.h
  │
  ├── res\                - Resources
  │   ├── resource.rc
  │   └── rbide.ico       - Icon (optional)
  │
  ├── obj\                - Build objects (created during build)
  ├── lib\                - Libraries (Scintilla headers)
  └── docs\               - Documentation

USAGE
-----
File Menu:
  New       - Create new BASIC program
  Open      - Open existing .BAS file
  Save      - Save current file
  Save As   - Save with new name
  Exit      - Close RBIDE

Edit Menu:
  Undo      - Undo last change (Ctrl+Z)
  Redo      - Redo last undo (Ctrl+Y)
  Cut       - Cut selection (Ctrl+X)
  Copy      - Copy selection (Ctrl+C)
  Paste     - Paste from clipboard (Ctrl+V)
  Select All - Select entire document (Ctrl+A)
  Find      - Find text (Ctrl+F) [Not yet implemented]

Run Menu:
  Compile       - Compile current file (F7)
  Run           - Run compiled program (F5)
  Compile & Run - Compile then run (Ctrl+F5)
  Stop          - Stop running program (Shift+F5) [Not yet implemented]

Help Menu:
  Manual    - Open RB BASIC language manual
  About     - About RBIDE

KEYBOARD SHORTCUTS
------------------
File Operations:
  Ctrl+N    - New file
  Ctrl+O    - Open file
  Ctrl+S    - Save file
  Alt+F4    - Exit

Edit Operations:
  Ctrl+Z    - Undo
  Ctrl+Y    - Redo
  Ctrl+X    - Cut
  Ctrl+C    - Copy
  Ctrl+V    - Paste
  Ctrl+A    - Select All
  Ctrl+F    - Find

Run Operations:
  F7        - Compile
  F5        - Run
  Ctrl+F5   - Compile & Run
  Shift+F5  - Stop

GETTING SCINTILLA.DLL
---------------------
Scintilla is a free, open-source editing component.

Option 1 - Official Website:
  1. Go to: https://www.scintilla.org/ScintillaDownload.html
  2. Download the Windows binary package
  3. Extract Scintilla.dll
  4. Copy to rbide\ folder

Option 2 - From Notepad++:
  1. If you have Notepad++ installed:
  2. Find Scintilla.dll in the Notepad++ folder
  3. Copy to rbide\ folder

Option 3 - From GitHub:
  https://github.com/notepad-plus-plus/notepad-plus-plus/tree/master/PowerEditor/bin
  Download Scintilla.dll from there

TROUBLESHOOTING
---------------
Problem: "Failed to load Scintilla.dll"
Solution: Make sure Scintilla.dll is in the same folder as rbide.exe

Problem: "rbbasic.exe not found" when compiling
Solution: Ensure rbide\ folder is inside rbbasic-portable\

Problem: Build fails
Solution: 
  - Check that MinGW32 is installed in ..\mingw32\
  - Run setup.bat in parent folder to validate

Problem: No syntax highlighting
Solution: Scintilla lexer may not be loading. Check that you have
          the correct version of Scintilla.dll

BUILDING FROM SOURCE
--------------------
Requirements:
  - MinGW32 GCC (in ..\mingw32\)
  - Scintilla headers (in lib\scintilla\include\)

Build:
  1. Run: build.bat
  2. Creates: rbide.exe
  3. Download: Scintilla.dll (see above)
  4. Test: rbide.exe

FUTURE ENHANCEMENTS
-------------------
Planned features for future versions:
  - Find/Replace dialog
  - Code folding
  - Auto-completion
  - Multiple file tabs
  - Project management
  - Debugging support
  - Integrated help browser
  - Customizable themes
  - Toolbar with icons

TECHNICAL DETAILS
-----------------
Language: C++ (C++17)
Compiler: MinGW32 GCC
GUI: Win32 API (native)
Editor: Scintilla 5.x
Build: Batch scripts

Size: ~200-300 KB (without Scintilla.dll)
      ~1-2 MB (with Scintilla.dll)

CREDITS
-------
RBIDE - Created by Ronen Blumberg
RB BASIC Compiler - Ronen Blumberg
Scintilla Editor - Neil Hodgson (https://www.scintilla.org)

LICENSE
-------
RBIDE is free for personal and educational use.
Scintilla is licensed under the MIT License.

SUPPORT
-------
For bugs, suggestions, or questions, please see the main RB BASIC
project documentation.

===============================================================================
Version: 1.0
Date: October 2025
Copyright (c) 2025 Ronen Blumberg
===============================================================================
