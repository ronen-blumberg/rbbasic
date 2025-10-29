@echo off
REM ============================================================================
REM RB BASIC (Ronen Blumberg BASIC) - Build Compiler
REM This script builds rbbasic.exe from source using local MinGW32
REM ============================================================================

echo.
echo ============================================================================
echo Building RB BASIC Compiler (Ronen Blumberg BASIC) v2.0
echo Portable Edition
echo ============================================================================
echo.

REM Get the directory where this script is located
set RBBASIC_ROOT=%~dp0
set RBBASIC_ROOT=%RBBASIC_ROOT:~0,-1%

echo Project directory: %RBBASIC_ROOT%
echo.

REM Check for MinGW32
if not exist "%RBBASIC_ROOT%\mingw32\bin\g++.exe" (
    echo ============================================
    echo ERROR: MinGW32 compiler not found!
    echo ============================================
    echo.
    echo Expected location: %RBBASIC_ROOT%\mingw32\bin\g++.exe
    echo.
    echo Please add MinGW32 compiler to the mingw32 folder:
    echo   1. Download from https://winlibs.com
    echo   2. Get the i686 ^(32-bit^) version with MSVCRT runtime
    echo   3. Extract to %RBBASIC_ROOT%\mingw32\
    echo.
    echo The structure should be:
    echo   mingw32\
    echo     bin\
    echo       gcc.exe
    echo       g++.exe
    echo       ...
    echo     lib\
    echo     include\
    echo.
    pause
    exit /b 1
)

echo [OK] MinGW32 found: %RBBASIC_ROOT%\mingw32
echo.

REM Check for source files
if not exist "%RBBASIC_ROOT%\src\main.cpp" (
    echo ERROR: Source files not found in src\ folder!
    echo.
    pause
    exit /b 1
)

echo [OK] Source files found in src\ folder
echo.

REM Set compiler paths
set GCC=%RBBASIC_ROOT%\mingw32\bin\g++.exe
set SRC_DIR=%RBBASIC_ROOT%\src

echo Compiling RB BASIC Compiler...
echo Using: %GCC%
echo.

REM Build the compiler
"%GCC%" ^
    -o "%RBBASIC_ROOT%\rbbasic.exe" ^
    "%SRC_DIR%\main.cpp" ^
    "%SRC_DIR%\config.cpp" ^
    "%SRC_DIR%\lexer.cpp" ^
    "%SRC_DIR%\parser.cpp" ^
    "%SRC_DIR%\codegen.cpp" ^
    "%SRC_DIR%\token.cpp" ^
    -std=c++17 ^
    -O2 ^
    -s ^
    -static ^
    -Wall ^
    -Wno-unused-variable ^
    -Wno-unused-but-set-variable

if errorlevel 1 (
    echo.
    echo ============================================
    echo Build FAILED!
    echo ============================================
    echo.
    echo Check the error messages above.
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================
echo Build SUCCESS!
echo ============================================
echo.

REM Show file info
if exist "%RBBASIC_ROOT%\rbbasic.exe" (
    echo Created: rbbasic.exe
    for %%I in ("%RBBASIC_ROOT%\rbbasic.exe") do echo Size: %%~zI bytes
    echo.
) else (
    echo Warning: rbbasic.exe not found after build!
)

echo RB BASIC compiler is ready to use.
echo.
echo Next steps:
echo   1. Run: setup.bat ^(to validate installation^)
echo   2. Create a .BAS program
echo   3. Run: rbbasic yourprogram.bas
echo   4. Run: compile.bat
echo   5. Run: program.exe
echo.

pause
