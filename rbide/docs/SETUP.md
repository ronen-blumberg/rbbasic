# RBIDE Setup Guide

## Quick Setup (5 Minutes)

### Step 1: Verify Location
Make sure the `rbide` folder is inside `rbbasic-portable`:
```
rbbasic-portable\
  ├── rbbasic.exe
  ├── mingw32\
  └── rbide\          ← You are here
      ├── build.bat
      ├── src\
      └── ...
```

### Step 2: Get Scintilla.dll

**Download Scintilla.dll from one of these sources:**

#### Option A: Official Scintilla Website
1. Go to: https://www.scintilla.org/ScintillaDownload.html
2. Download: "Windows" binary package (scintilla###.zip)
3. Extract the ZIP file
4. Find `Scintilla.dll` in the extracted folder
5. Copy `Scintilla.dll` to `rbide\` folder

#### Option B: From Notepad++ (If installed)
1. Navigate to: `C:\Program Files\Notepad++\` or `C:\Program Files (x86)\Notepad++\`
2. Copy `Scintilla.dll` from there
3. Paste into `rbide\` folder

#### Option C: GitHub Direct Download
1. Go to: https://github.com/notepad-plus-plus/notepad-plus-plus/tree/master/PowerEditor/bin
2. Click on `Scintilla.dll`
3. Click "Download" button
4. Save to `rbide\` folder

### Step 3: Build RBIDE
```batch
cd rbide
build.bat
```

Wait about 30 seconds. You should see:
```
Build SUCCESS!
Created: rbide.exe
Size: ##### bytes
```

### Step 4: Test RBIDE
```batch
rbide.exe
```

The IDE should open with an empty editor window.

---

## Detailed Setup Instructions

### Prerequisites

**Required:**
- ✅ Windows 7 or later
- ✅ rbbasic-portable folder with rbbasic.exe
- ✅ MinGW32 in `..\mingw32\` folder
- ✅ Scintilla.dll (you must download this)

**Optional:**
- ❌ No Visual Studio needed
- ❌ No .NET Framework needed
- ❌ No Python needed

Everything else is included!

---

### Scintilla.dll - Detailed Instructions

**What is Scintilla?**
- Free, open-source text editing component
- Used by Notepad++, Code::Blocks, and many other editors
- Provides syntax highlighting, code folding, etc.
- MIT licensed (free to use)

**Which version?**
- Any Scintilla 5.x version works
- Scintilla 4.x also works
- Latest version recommended

**File size:**
- Scintilla.dll is typically 1-2 MB

**Where to place it:**
```
rbide\
  ├── rbide.exe
  ├── Scintilla.dll    ← Must be here!
  └── ...
```

---

### Building RBIDE

#### Build Process

1. **Open Command Prompt:**
   ```batch
   cd C:\rbbasic-portable\rbide
   ```

2. **Run build script:**
   ```batch
   build.bat
   ```

3. **What happens:**
   - Checks for MinGW32
   - Compiles main.cpp
   - Compiles mainwindow.cpp
   - Compiles resources (if icon exists)
   - Links everything together
   - Creates rbide.exe

4. **Build time:**
   - First build: ~30 seconds
   - Subsequent builds: ~10-15 seconds

#### Build Output

**Success:**
```
============================================
Build SUCCESS!
============================================

Created: rbide.exe
Size: 245760 bytes

IMPORTANT: You need Scintilla.dll!
...
```

**Failure:**
```
============================================
Build FAILED!
============================================

Check the error messages above.
```

Common build errors:
- `g++: command not found` → MinGW32 not installed
- `No such file or directory` → Wrong folder or missing source files

---

### Testing RBIDE

#### First Run

1. **Make sure you have:**
   - ✅ rbide.exe (just built)
   - ✅ Scintilla.dll (downloaded)
   - ✅ Both in same folder

2. **Launch:**
   ```batch
   rbide.exe
   ```

3. **You should see:**
   - Window titled "RBIDE - Untitled"
   - Menu bar (File, Edit, Run, Help)
   - Large editor area
   - Output panel at bottom
   - Status bar showing "Line: 1  Col: 1"

#### Test Basic Functions

**Test 1: File Menu**
1. Click File → New
   - Editor should clear
   - Title shows "RBIDE - Untitled"

2. Type some BASIC code:
   ```basic
   10 PRINT "Hello from RBIDE!"
   20 END
   ```

3. Click File → Save As
   - Save as: test.bas

**Test 2: Syntax Highlighting**
- Keywords (PRINT, END) should be blue
- Strings ("Hello...") should be red
- Numbers (10, 20) should be orange

**Test 3: Edit Operations**
1. Select some text
2. Press Ctrl+C (copy)
3. Press Ctrl+V (paste)
4. Press Ctrl+Z (undo)

**Test 4: Compilation**
1. Click Run → Compile (or press F7)
2. Output panel should show:
   ```
   Compiling...
   Compilation successful!
   ```
3. Click Run → Run (or press F5)
4. Your program should execute

---

### Troubleshooting

#### Problem: "Failed to load Scintilla.dll"

**Symptoms:**
- Error dialog on startup
- IDE doesn't launch

**Solutions:**
1. **Check file exists:**
   ```batch
   dir Scintilla.dll
   ```
   Should show the file

2. **Check location:**
   Must be in same folder as rbide.exe

3. **Download correct version:**
   - 32-bit Scintilla.dll for 32-bit build
   - Make sure it's not corrupted

4. **Try different source:**
   - If official download doesn't work
   - Try Notepad++ version
   - Or GitHub version

#### Problem: Build fails with "g++ not found"

**Solution:**
1. Check MinGW32 is installed:
   ```batch
   dir ..\mingw32\bin\g++.exe
   ```

2. If not found, reinstall MinGW32:
   - See rbbasic-portable setup instructions
   - Or run `..\setup.bat`

#### Problem: "rbbasic.exe not found" when compiling

**Solution:**
1. Make sure rbide folder is inside rbbasic-portable:
   ```
   rbbasic-portable\
     ├── rbbasic.exe    ← Must be here
     └── rbide\
         └── rbide.exe
   ```

2. Or add rbbasic.exe to PATH (not recommended for portable use)

#### Problem: No syntax highlighting

**Possible causes:**
1. Scintilla.dll not loaded
2. Wrong version of Scintilla.dll
3. Lexer initialization failed

**Solution:**
- Make sure you have the correct Scintilla.dll
- Try downloading from different source
- Check that .bas file is opened (not just typed in new file)

#### Problem: Compilation doesn't work

**Check:**
1. Did you save the file first? (File → Save)
2. Is rbbasic.exe in parent folder?
3. Does manual compilation work?
   ```batch
   ..\rbbasic.exe test.bas
   compile.bat
   program.exe
   ```

---

### Configuration

#### Current Configuration
- Single file editor (like QB64)
- Bottom output panel
- Classic Win32 style
- No tabs yet

#### Default Settings
- Tab width: 4 spaces
- Font: Consolas 10pt
- Line numbers: Yes
- Output panel: Visible

#### Future Configuration
In future versions, these will be customizable via:
- Preferences dialog
- Config file (rbide.ini)
- Registry settings

---

### File Structure After Setup

```
rbbasic-portable\
  ├── rbbasic.exe
  ├── mingw32\
  │   └── bin\
  │       └── g++.exe
  │
  └── rbide\
      ├── rbide.exe        ✅ Built
      ├── Scintilla.dll    ✅ Downloaded
      ├── build.bat
      ├── README.txt
      ├── SETUP.md
      │
      ├── src\
      │   ├── main.cpp
      │   ├── mainwindow.cpp
      │   ├── mainwindow.h
      │   └── resource.h
      │
      ├── res\
      │   ├── resource.rc
      │   └── rbide.ico (optional)
      │
      ├── obj\            (created during build)
      │   ├── main.o
      │   └── mainwindow.o
      │
      └── docs\
```

---

### Next Steps

Once RBIDE is working:

1. **Learn the shortcuts:**
   - F7 = Compile
   - F5 = Run
   - Ctrl+F5 = Compile & Run

2. **Try example programs:**
   ```batch
   rbide ..\examples\hello.bas
   ```

3. **Write your own programs:**
   - File → New
   - Write BASIC code
   - Run → Compile & Run

4. **Read the manual:**
   - Help → Manual

---

### Getting Help

**Documentation:**
- README.txt - Overview and usage
- SETUP.md - This file
- ..\docs\ - RB BASIC language manual

**Common Issues:**
- See Troubleshooting section above
- Check rbbasic-portable documentation

**Reporting Bugs:**
- Note: Which function doesn't work
- Note: Error messages shown
- Note: Steps to reproduce

---

## Summary Checklist

- [ ] rbide folder is inside rbbasic-portable
- [ ] MinGW32 is installed in ..\mingw32\
- [ ] Downloaded Scintilla.dll
- [ ] Placed Scintilla.dll in rbide\ folder
- [ ] Ran build.bat successfully
- [ ] rbide.exe was created
- [ ] Tested launching rbide.exe
- [ ] Tested typing BASIC code
- [ ] Tested File → Save
- [ ] Tested Run → Compile
- [ ] RBIDE is working! 🎉

---

**Version:** 1.0  
**Last Updated:** October 2025  
**Author:** Ronen Blumberg
