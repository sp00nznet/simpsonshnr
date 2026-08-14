@echo off
REM PS3 Build Script for The Simpsons: Hit & Run
REM
REM Usage: build.bat [CONFIG]
REM   CONFIG can be: DEBUG, RELEASE, or TUNE (default: DEBUG)
REM

setlocal

REM Set default config
if "%1"=="" (
    set CONFIG=DEBUG
) else (
    set CONFIG=%1
)

echo ============================================
echo PS3 Build - The Simpsons: Hit and Run
echo Configuration: %CONFIG%
echo ============================================

REM Check for PS3 SDK
set PS3SDK=D:\PS3.Full.3.40.SDK.PS3-DUPLEX
set PS3_TOOLCHAIN=%PS3SDK%\[134]-PS3_Toolchain_411-Win_340_001\cell
set PS3_SDK=%PS3SDK%\[132]-PS3_SDK-340_001\cell

if not exist "%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-gcc.exe" (
    echo ERROR: PS3 Toolchain not found at %PS3_TOOLCHAIN%
    exit /b 1
)

if not exist "%PS3_SDK%\target\ppu\include\cell.h" (
    echo ERROR: PS3 SDK not found at %PS3_SDK%
    exit /b 1
)

echo Using Toolchain: %PS3_TOOLCHAIN%
echo Using SDK: %PS3_SDK%
echo.

REM Run make
..\..\libs\radbuild\GlobalTools\make.exe CONFIG=%CONFIG%

if %ERRORLEVEL% NEQ 0 (
    echo Build failed with error %ERRORLEVEL%
    exit /b 1
)

echo Build completed successfully!
endlocal
