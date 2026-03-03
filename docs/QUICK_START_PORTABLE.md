# RB BASIC Portable Edition - Quick Start
## Get Your Portable Compiler Running in 10 Minutes!

---

## 🚀 Super Quick Start (TL;DR)

```batch
1. Download MinGW32 from winlibs.com (i686 version)
2. Extract to C:\RBBASIC\mingw32\
3. Copy all portable files to C:\RBBASIC\
4. Run: build_compiler.bat
5. Run: setup.bat
6. Done! Test with: rbbasic test.bas
```

---

## 📋 Detailed Steps

### Step 1: Get MinGW32 (5 minutes)

1. Open browser: https://winlibs.com
2. Look for: **GCC + MinGW-w64** download
3. Choose: **i686 (32-bit)** architecture
4. Download: The MSVCRT runtime version (recommended)
5. File will be named something like: `winlibs-i686-posix-dwarf-gcc-13.2.0-mingw-w64msvcrt-11.0.1-r5.zip`

### Step 2: Extract MinGW32 (2 minutes)

1. Extract the ZIP file
2. Inside you'll find a `mingw32` or `mingw64` folder
3. Rename it to `mingw32` if needed
4. Move/copy it to: `C:\RBBASIC\mingw32\`
5. Verify `C:\RBBASIC\mingw32\bin\gcc.exe` exists

### Step 3: Copy Portable Files (1 minute)

Copy these files from the outputs directory to `C:\RBBASIC\`:

**Required:**
- config.h
- config.cpp
- main_portable.cpp
- rbc.config
- setup.bat
- build_compiler.bat

**Also copy your existing RB BASIC files:**
- lexer.cpp / lexer.h
- parser.cpp / parser.h
- codegen.cpp / codegen.h
- token.cpp / token.h
- ast.h

### Step 4: Build the Compiler (1 minute)

```batch
cd C:\RBBASIC
build_compiler.bat
```

Wait for compilation... Should see "Build SUCCESS!"

Creates: `rbbasic.exe`

### Step 5: Run Setup (1 minute)

```batch
setup.bat
```

This validates your installation and creates default config.

### Step 6: Test It! (30 seconds)

Create `test.bas`:

```basic
10 PRINT "Hello from portable RB BASIC!"
20 PRINT "Ronen Blumberg BASIC v2.0"
30 END
```

Compile it:

```batch
rbbasic test.bas
compile.bat
program.exe
```

Should print:
```
Hello from portable RB BASIC!
Ronen Blumberg BASIC v2.0
```

---

## 🎯 Test Portability

### Test on USB Drive

1. Copy entire `C:\RBBASIC` folder to USB drive (e.g., `E:\RBBASIC`)
2. Open Command Prompt on USB drive
3. Run `setup.bat`
4. Compile test.bas
5. Should work without any PATH setup!

### Test on Laptop

1. Copy `C:\RBBASIC` to your laptop
2. Run `setup.bat`
3. Compile a program
4. Works even without MinGW in system PATH!

---

## 🔧 Troubleshooting

### "GCC not found"

**Problem:** build_compiler.bat can't find GCC
**Solution:** 
- Check `C:\RBBASIC\mingw32\bin\gcc.exe` exists
- If folder is named `mingw64`, rename to `mingw32`
- Re-run `setup.bat` to validate

### "Cannot open file: rbc.config"

**Problem:** Config file missing
**Solution:**
- Copy `rbc.config` to `C:\RBBASIC\`
- Or run `setup.bat` - it creates default config

### Compilation errors

**Problem:** Missing source files
**Solution:**
- Ensure all .cpp and .h files are in `C:\RBBASIC\`
- Check spelling of filenames
- main_portable.cpp (not main.cpp for portable version)

### SDL2 warnings

**Problem:** "SDL2 not found" during setup
**Solution:**
- This is optional for text-only programs
- Graphics won't work without SDL2
- To add SDL2: See PORTABLE_DISTRIBUTION_GUIDE.md

---

## 📦 Optional: Add SDL2 (For Graphics)

If you want graphics support:

### Get SDL2 (5 minutes)

1. Go to: https://github.com/libsdl-org/SDL/releases
2. Download: **SDL2-devel-x.x.x-mingw.tar.gz**
3. Extract it
4. Inside find: `i686-w64-mingw32` folder
5. Copy to: `C:\RBBASIC\lib\SDL2-mingw32\i686-w64-mingw32\`

### Directory structure should be:

```
C:\RBBASIC\lib\SDL2-mingw32\i686-w64-mingw32\
├── include\
│   └── SDL2\
│       └── SDL.h
├── lib\
│   ├── libSDL2.a
│   └── libSDL2main.a
└── bin\
    └── SDL2.dll
```

### Test Graphics

```basic
10 SCREEN 12
20 CLS
30 FOR I = 1 TO 100
40   X = INT(RND * 640)
50   Y = INT(RND * 480)
60   C = INT(RND * 16)
70   PSET (X, Y), C
80 NEXT I
90 A$ = INPUT$(1)
100 END
```

---

## ✅ Success Checklist

After setup, you should have:

- [ ] rbbasic.exe (created by build_compiler.bat)
- [ ] rbc.config (created by setup.bat or manually)
- [ ] mingw32\ folder with gcc.exe inside
- [ ] Can compile test.bas successfully
- [ ] program.exe runs and shows output
- [ ] Works from USB drive (portability test)
- [ ] Works on laptop without PATH setup

---

## 📚 What's Next?

Now that your portable compiler works:

1. **Add Examples**
   - Create `examples\` folder
   - Add demo programs
   - Test each one compiles

2. **Add Documentation**
   - Create `docs\` folder
   - Copy language manual
   - Copy quick reference

3. **Create Distribution**
   - ZIP the entire RBBASIC folder
   - Share with others!
   - They can extract and use immediately

4. **Build IDE**
   - After portable version is solid
   - Simple GUI with WinAPI32
   - Integrate Scintilla editor
   - Use same config system

---

## 🎉 Done!

You now have a fully portable RB BASIC compiler that:
- ✅ Works from anywhere (USB, network drive, etc.)
- ✅ No installation required
- ✅ No PATH configuration needed
- ✅ Bundled compiler and libraries
- ✅ Ready to distribute

**Total setup time: ~10 minutes**

---

## 📞 Help & Support

If you get stuck:
1. Check PORTABLE_EDITION_SUMMARY.md for detailed info
2. Read PORTABLE_DISTRIBUTION_GUIDE.md for full guide
3. Run `setup.bat` to validate installation
4. Check that all files are in correct locations

---

**Happy Portable BASIC Programming! 🚀**

*Ronen Blumberg BASIC - Portable Edition v2.0*
