# RB BASIC Portable Edition - Distribution Guide
## Ronen Blumberg BASIC Compiler v2.0

---

## 📦 Creating a Portable Distribution

This guide explains how to create a complete, portable RB BASIC distribution that can run from any location (hard drive, USB stick, network drive) without installation.

### Directory Structure

```
RBBASIC/                         (Main folder - can be renamed or moved anywhere)
├── rbbasic.exe                  (Main compiler executable)
├── rbc.config                   (Configuration file)
├── setup.bat                    (Setup/validation script)
├── README.txt                   (Quick start guide)
│
├── mingw32/                     (MinGW32 GCC compiler - from winlibs.com)
│   ├── bin/
│   │   ├── gcc.exe
│   │   ├── g++.exe
│   │   ├── as.exe
│   │   ├── ld.exe
│   │   └── (other MinGW tools)
│   ├── lib/
│   ├── include/
│   └── libexec/
│
├── lib/                         (Libraries)
│   └── SDL2-mingw32/
│       └── i686-w64-mingw32/
│           ├── include/         (SDL2 headers)
│           ├── lib/             (SDL2 .a files)
│           └── bin/             (SDL2.dll)
│
├── examples/                    (Example BASIC programs)
│   ├── hello.bas
│   ├── game.bas
│   ├── graphics.bas
│   └── ...
│
├── docs/                        (Documentation)
│   ├── RBBASIC_LANGUAGE_MANUAL.html
│   ├── RBBASIC_LANGUAGE_MANUAL.md
│   └── RBBASIC_QUICK_REFERENCE.md
│
└── output/                      (Temporary build files - created automatically)
```

---

## 🔧 Step-by-Step Setup

### Step 1: Get MinGW32 from WinLibs

1. Go to https://winlibs.com
2. Download: **GCC 13.x + MinGW-w64 11.x (MSVCRT runtime)** 
3. Architecture: **i686 (32-bit)**
4. Download the archive (e.g., `winlibs-i686-posix-dwarf-gcc-13.2.0-mingw-w64msvcrt-11.0.1-r5.zip`)
5. Extract to a temporary location
6. Copy/rename the extracted `mingw32` or `mingw64` folder to `RBBASIC\mingw32`

### Step 2: Get SDL2 for MinGW32

1. Go to https://github.com/libsdl-org/SDL/releases
2. Download: **SDL2-devel-x.x.x-mingw.tar.gz** (Development Libraries)
3. Extract the archive
4. Copy the `i686-w64-mingw32` folder to `RBBASIC\lib\SDL2-mingw32\`

### Step 3: Add Your Files

1. Place `rbbasic.exe` in the root `RBBASIC` folder
2. Place `rbc.config` in the root folder
3. Place `setup.bat` in the root folder
4. Create `README.txt` with quick start instructions

### Step 4: Add Documentation

1. Create `docs` folder
2. Copy all manual files:
   - RBBASIC_LANGUAGE_MANUAL.html
   - RBBASIC_LANGUAGE_MANUAL.md
   - RBBASIC_QUICK_REFERENCE.md

### Step 5: Add Examples

1. Create `examples` folder
2. Add example BASIC programs:
   - hello.bas (simple Hello World)
   - input.bas (INPUT demonstration)
   - loops.bas (FOR/WHILE loops)
   - graphics.bas (SDL2 graphics demo)
   - game.bas (simple game)

### Step 6: Create README.txt

```
===============================================
RB BASIC (Ronen Blumberg BASIC) v2.0
Portable Edition
===============================================

QUICK START:

1. Run setup.bat to validate installation
2. Open Command Prompt in this folder
3. Run: rbbasic examples\hello.bas
4. Run: compile.bat
5. Run: program.exe

REQUIREMENTS:

- Windows 7 or later (32-bit or 64-bit)
- No installation required
- No admin rights needed
- Works from USB drive

FIRST TIME SETUP:

1. Double-click setup.bat
2. Follow on-screen instructions
3. Ready to use!

DOCUMENTATION:

See docs folder for complete language reference.

HELP:

Run: rbbasic --help

===============================================
© 2025 Ronen Blumberg
Free for educational and personal use
===============================================
```

### Step 7: Test the Distribution

1. Copy the entire `RBBASIC` folder to a different location (or USB drive)
2. Open Command Prompt in the new location
3. Run `setup.bat` to verify everything works
4. Compile an example: `rbbasic examples\hello.bas`
5. Run `compile.bat`
6. Run `program.exe`

---

## 📏 Size Estimates

| Component | Approximate Size |
|-----------|------------------|
| rbbasic.exe | 500 KB - 1 MB |
| MinGW32 (full) | 200-300 MB |
| SDL2 | 5-10 MB |
| Documentation | < 1 MB |
| Examples | < 1 MB |
| **Total** | **~220-320 MB** |

### Minimal Distribution (Text-Only)

If you want a smaller package without graphics support:
- **Skip SDL2** - Omit the `lib\SDL2-mingw32` folder
- Graphics commands won't work, but text programs will
- Size: ~200-300 MB

---

## 🔄 Update Checklist

When creating new versions:

- [ ] Update version number in rbbasic.exe
- [ ] Update version in rbc.config comments
- [ ] Update README.txt with version number
- [ ] Update documentation if features changed
- [ ] Test on clean Windows install
- [ ] Test on USB drive
- [ ] Update example programs if needed
- [ ] Create release notes

---

## 📦 Distribution Formats

### Format 1: ZIP Archive (Recommended)

```
RBBASIC-v2.0-Portable.zip
├── RBBASIC/ (contains everything)
└── README.txt (extraction instructions)
```

**Advantages:**
- Easy to download
- Universal compatibility
- Users can extract anywhere

### Format 2: 7-Zip Archive (Smaller)

```
RBBASIC-v2.0-Portable.7z
```

**Advantages:**
- Better compression (~30% smaller)
- Good for large downloads

**Disadvantages:**
- Requires 7-Zip to extract

### Format 3: Self-Extracting Archive

Create a self-extracting EXE using:
- WinRAR
- 7-Zip
- IExpress (built into Windows)

**Advantages:**
- No extraction tool needed
- Can include setup.bat auto-run

---

## 🚀 Advanced: Creating an Installer

For a more professional distribution, consider using:

### Option A: Inno Setup (Free)
- Download from https://jrsoftware.org/isinfo.php
- Create install script
- Generates professional Windows installer
- Can create Start Menu shortcuts
- Can add to Programs list

### Option B: NSIS (Free)
- Nullsoft Scriptable Install System
- More control over installation
- Smaller installer size

### Basic Inno Setup Script

```iss
[Setup]
AppName=RB BASIC
AppVersion=2.0
DefaultDirName={pf}\RBBASIC
DefaultGroupName=RB BASIC
OutputBaseFilename=RBBASIC-Setup-v2.0
Compression=lzma2
SolidCompression=yes

[Files]
Source: "RBBASIC\*"; DestDir: "{app}"; Flags: recursesubdirs

[Icons]
Name: "{group}\RB BASIC Compiler"; Filename: "cmd.exe"; Parameters: "/k cd /d ""{app}"""; IconFilename: "{app}\rbbasic.exe"
Name: "{group}\Documentation"; Filename: "{app}\docs\RBBASIC_LANGUAGE_MANUAL.html"
Name: "{group}\Examples"; Filename: "{app}\examples"
```

---

## ✅ Quality Assurance Checklist

Before distributing:

### Functionality Tests
- [ ] Compiles simple text program
- [ ] Compiles program with graphics (if SDL2 included)
- [ ] Compiles program with sound
- [ ] setup.bat runs without errors
- [ ] All example programs compile
- [ ] Generated programs run correctly

### Portability Tests
- [ ] Works from C:\ drive
- [ ] Works from D:\ drive
- [ ] Works from USB drive
- [ ] Works from network drive
- [ ] Works in folder with spaces in name
- [ ] Works in nested folders (C:\Users\Name\Desktop\RBBASIC)

### Compatibility Tests
- [ ] Windows 7 (if available)
- [ ] Windows 10
- [ ] Windows 11
- [ ] 32-bit Windows (if available)
- [ ] 64-bit Windows

### Documentation Tests
- [ ] README.txt is clear and accurate
- [ ] Manual opens correctly
- [ ] All links in manual work
- [ ] Examples match documentation

---

## 🎯 Distribution Platforms

Where to share your portable RB BASIC:

### Online Platforms
- **GitHub Releases** - Best for developers
- **SourceForge** - Good for retro/hobbyist software
- **Itch.io** - Great for game development tools
- **Your website** - Direct download

### Communities
- **Reddit** - r/programming, r/retrogaming, r/BASIC
- **Discord** - Programming and retro computing servers
- **Forums** - Vintage computer forums, programming forums

### YouTube
- Create demo video
- Tutorial series
- Link to download in description

---

## 📄 License Considerations

Make sure to include:

1. **Your License** - For RB BASIC itself
2. **MinGW License** - Include mingw license files
3. **SDL2 License** - Include SDL2 license (zlib license)
4. **Attribution** - Credit any code you used

Example LICENSE.txt:

```
RB BASIC (Ronen Blumberg BASIC) Compiler
© 2025 Ronen Blumberg

Licensed for personal and educational use.

This software includes:
- MinGW-w64 (https://mingw-w64.org) - Various licenses
- SDL2 (https://libsdl.org) - zlib license
```

---

## 🔐 Digital Signing (Optional)

For professional distribution:
1. Get a code signing certificate
2. Sign rbbasic.exe
3. Sign installer (if created)
4. Prevents Windows SmartScreen warnings

---

## 📊 Version Tracking

Keep track of versions:

```
CHANGELOG.txt

Version 2.0 - October 2025
- Initial portable edition
- Added config system
- Bundled MinGW32
- Added SDL2 graphics support

Version 1.0 - September 2025
- Initial release
```

---

## 🎉 You're Ready!

Your portable RB BASIC distribution is ready to share with the world!

**Next Steps:**
1. Test thoroughly
2. Create archive (ZIP or 7z)
3. Write release announcement
4. Share with community
5. Gather feedback
6. Plan next version

---

**Happy Distributing! 🚀**

*For questions or updates, see the project repository*
