@echo off
setlocal EnableDelayedExpansion
REM ============================================================================
REM rbidefb build script
REM RB BASIC FreeBASIC IDE (uses libvt)
REM ============================================================================

echo.
echo ============================================================================
echo Building rbidefb - RB BASIC IDE (FreeBASIC edition)
echo ============================================================================
echo.

REM rbbasic-portable root (parent of this folder)
set "RBBASIC_ROOT=%~dp0.."
pushd "%RBBASIC_ROOT%" >nul 2>&1
set "RBBASIC_ROOT=%CD%"
popd >nul 2>&1

echo RB BASIC Root: %RBBASIC_ROOT%
echo.

REM ---------------------------------------------------------------------------
REM Locate fbc32 (32-bit FreeBASIC compiler).
REM ---------------------------------------------------------------------------
set "FBC="

where fbc32 >nul 2>&1
if !errorlevel! equ 0 (
    for /f "delims=" %%P in ('where fbc32') do (
        if not defined FBC set "FBC=%%P"
    )
)

if not defined FBC if exist "C:\fb_programming\FreeBASIC-1.10.1-winlibs-gcc-9.3.0\fbc32.exe" set "FBC=C:\fb_programming\FreeBASIC-1.10.1-winlibs-gcc-9.3.0\fbc32.exe"
if not defined FBC if exist "C:\FreeBASIC\fbc32.exe"                                          set "FBC=C:\FreeBASIC\fbc32.exe"
if not defined FBC if exist "C:\Program Files\FreeBASIC\fbc32.exe"                            set "FBC=C:\Program Files\FreeBASIC\fbc32.exe"
if not defined FBC if exist "C:\Program Files (x86)\FreeBASIC\fbc32.exe"                      set "FBC=C:\Program Files (x86)\FreeBASIC\fbc32.exe"

if not defined FBC (
    echo ERROR: FreeBASIC compiler ^(fbc32.exe^) not found.
    echo Add fbc32.exe to PATH or install FreeBASIC at one of:
    echo   C:\fb_programming\FreeBASIC-1.10.1-winlibs-gcc-9.3.0\
    echo   C:\FreeBASIC\
    echo   C:\Program Files\FreeBASIC\
    pause
    exit /b 1
)

echo [OK] fbc32: !FBC!
echo.

REM ---------------------------------------------------------------------------
REM Sanity-check libvt headers are installed in FB include path
REM ---------------------------------------------------------------------------
for %%I in ("!FBC!") do set "FB_DIR=%%~dpI"
set "VT_BI=!FB_DIR!inc\vt\vt.bi"
if not exist "!VT_BI!" (
    echo WARNING: libvt header not found at !VT_BI!
    echo   The FB install should include vt/ under inc/.
    echo   Continuing anyway in case fbc finds it elsewhere.
    echo.
)

REM ---------------------------------------------------------------------------
REM Sanity-check fbclaude.bas is reachable
REM ---------------------------------------------------------------------------
set "FBCLAUDE=%RBBASIC_ROOT%\libvt-examples\vtclaude-main\fbclaude.bas"
if not exist "!FBCLAUDE!" (
    echo ERROR: fbclaude.bas not found at:
    echo   !FBCLAUDE!
    echo The Claude chat panel will not build without it.
    pause
    exit /b 1
)
echo [OK] fbclaude.bas: !FBCLAUDE!
echo.

REM ---------------------------------------------------------------------------
REM Prepare import libraries (libcurl, libSDL2).
REM We keep these in lib\ inside rbidefb\ so the build is self-contained.
REM ---------------------------------------------------------------------------
set "VTCLAUDE=%RBBASIC_ROOT%\libvt-examples\vtclaude-main"
if not exist lib mkdir lib

REM Generate libcurl.dll.a from libcurl.def if not already present
set "DLLTOOL=!FB_DIR!bin\win32\dlltool.exe"
if not exist "!DLLTOOL!" set "DLLTOOL=!FB_DIR!win32\dlltool.exe"
if not exist "!DLLTOOL!" set "DLLTOOL=!FB_DIR!dlltool.exe"
if not exist "!DLLTOOL!" for %%G in (dlltool.exe) do set "DLLTOOL=%%~$PATH:G"
if not exist lib\libcurl.dll.a (
    if exist "!DLLTOOL!" (
        echo Generating lib\libcurl.dll.a from libcurl.def ...
        "!DLLTOOL!" -d "!VTCLAUDE!\libcurl.def" -l lib\libcurl.dll.a -D libcurl.dll
        if errorlevel 1 (
            echo WARNING: dlltool failed -- chat panel will not link.
        )
    ) else (
        echo WARNING: dlltool not found at !DLLTOOL!.
    )
)

REM Copy libSDL2.dll.a from rbbasic-portable's SDL2 install
set "SDL2_LIB_SRC=%RBBASIC_ROOT%\lib\SDL2-mingw32\i686-w64-mingw32\lib\libSDL2.dll.a"
if not exist lib\libSDL2.dll.a (
    if exist "!SDL2_LIB_SRC!" (
        copy /Y "!SDL2_LIB_SRC!" lib\libSDL2.dll.a >nul
        echo [OK] copied libSDL2.dll.a
    ) else (
        echo WARNING: !SDL2_LIB_SRC! not found.
    )
)

REM ---------------------------------------------------------------------------
REM Clean old build
REM ---------------------------------------------------------------------------
if exist rbidefb.exe del /Q rbidefb.exe 2>nul

echo Compiling rbidefb.exe ...
echo.

"!FBC!" "src\rbidefb.bas" -p lib -x "rbidefb.exe" -s gui

if errorlevel 1 goto :build_failed

REM ---------------------------------------------------------------------------
REM Copy runtime DLLs
REM ---------------------------------------------------------------------------
echo.
echo Copying runtime DLLs...

set "VTCLAUDE=%RBBASIC_ROOT%\libvt-examples\vtclaude-main"

if exist "!VTCLAUDE!\libcurl.dll" (
    if not exist libcurl.dll copy /Y "!VTCLAUDE!\libcurl.dll" libcurl.dll >nul
    echo   [OK] libcurl.dll
) else (
    echo   WARNING: libcurl.dll not found at !VTCLAUDE!\libcurl.dll
)

if exist "!VTCLAUDE!\curl-ca-bundle.crt" (
    if not exist curl-ca-bundle.crt copy /Y "!VTCLAUDE!\curl-ca-bundle.crt" curl-ca-bundle.crt >nul
    echo   [OK] curl-ca-bundle.crt
) else (
    echo   WARNING: curl-ca-bundle.crt not found.
)

set "SDL_SRC="
if exist "%RBBASIC_ROOT%\SDL2.dll"    set "SDL_SRC=%RBBASIC_ROOT%\SDL2.dll"
if not defined SDL_SRC if exist "!VTCLAUDE!\SDL2.dll" set "SDL_SRC=!VTCLAUDE!\SDL2.dll"
if defined SDL_SRC (
    if not exist SDL2.dll copy /Y "!SDL_SRC!" SDL2.dll >nul
    echo   [OK] SDL2.dll
) else (
    echo   WARNING: SDL2.dll not found.
)

echo.
echo ============================================================================
echo Build SUCCESS
echo ============================================================================
echo.
if exist rbidefb.exe (
    for %%I in (rbidefb.exe) do echo Created: rbidefb.exe  ^(%%~zI bytes^)
    echo.
    echo Run: rbidefb.exe
    echo     - F1 help, F2 toggle editor/chat, F5 compile and run,
    echo       F7 compile, F9 run.
    echo.
)
pause
exit /b 0

:build_failed
echo.
echo ============================================================================
echo Build FAILED
echo ============================================================================
echo.
pause
exit /b 1
