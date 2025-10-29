@echo off
REM ============================================================================
REM RBIDE Build Script
REM RB BASIC Integrated Development Environment
REM ============================================================================

echo.
echo ============================================================================
echo Building RBIDE - RB BASIC IDE
echo ============================================================================
echo.

REM Set paths relative to rbbasic-portable
set RBBASIC_ROOT=%~dp0..
set MINGW=%RBBASIC_ROOT%\mingw32
set GCC=%MINGW%\bin\g++.exe
set WINDRES=%MINGW%\bin\windres.exe

REM Check for MinGW32
if not exist "%GCC%" (
    echo ERROR: MinGW32 not found!
    echo Expected: %GCC%
    echo.
    echo Please ensure MinGW32 is installed in %RBBASIC_ROOT%\mingw32\
    pause
    exit /b 1
)

echo [OK] MinGW32 found
echo.

REM Create obj directory if it doesn't exist
if not exist obj mkdir obj

REM Clean old builds
echo Cleaning old build files...
del /Q obj\*.o 2>nul
del /Q obj\*.res 2>nul
del /Q rbide.exe 2>nul

echo.
echo Compiling source files...
echo.

REM Compile resource file (skip if icon is missing)
if exist res\rbide.ico (
    echo   - Compiling resources...
    "%WINDRES%" res\resource.rc -O coff -o obj\resource.res
    if errorlevel 1 (
        echo     WARNING: Resource compilation failed, continuing without icon
        set RESOURCE_OBJ=
    ) else (
        set RESOURCE_OBJ=obj\resource.res
    )
) else (
    echo   - Skipping resources (no icon file found)
    set RESOURCE_OBJ=
)

REM Compile main.cpp
echo   - Compiling main.cpp...
"%GCC%" -c src\main.cpp -o obj\main.o ^
    -std=c++17 -O2 -Wall
if errorlevel 1 goto :build_failed

REM Compile mainwindow.cpp
echo   - Compiling mainwindow.cpp...
"%GCC%" -c src\mainwindow.cpp -o obj\mainwindow.o ^
    -std=c++17 -O2 -Wall ^
    -Ilib\scintilla\include
if errorlevel 1 goto :build_failed

echo.
echo Linking...

REM Link all together
"%GCC%" obj\main.o obj\mainwindow.o %RESOURCE_OBJ% ^
    -o rbide.exe ^
    -mwindows -static-libgcc -static-libstdc++ ^
    -lcomctl32 -lcomdlg32 -luser32 -lgdi32 -lshell32 ^
    -s

if errorlevel 1 goto :build_failed

echo.
echo ============================================
echo Build SUCCESS!
echo ============================================
echo.

REM Show file info
if exist rbide.exe (
    echo Created: rbide.exe
    for %%I in (rbide.exe) do echo Size: %%~zI bytes
    echo.
    echo IMPORTANT: You need Scintilla.dll!
    echo.
    echo Download Scintilla.dll from:
    echo   https://www.scintilla.org/ScintillaDownload.html
    echo.
    echo Or get it from:
    echo   https://github.com/notepad-plus-plus/notepad-plus-plus/tree/master/PowerEditor/bin
    echo.
    echo Place Scintilla.dll in the same folder as rbide.exe
    echo.
    echo To test:
    echo   1. Download and place Scintilla.dll here
    echo   2. Run: rbide.exe
    echo   3. Open a .BAS file
    echo   4. Try File-^>New, File-^>Open, etc.
    echo.
) else (
    echo WARNING: rbide.exe not found after build!
)

pause
exit /b 0

:build_failed
echo.
echo ============================================
echo Build FAILED!
echo ============================================
echo.
echo Check the error messages above.
echo.
pause
exit /b 1
