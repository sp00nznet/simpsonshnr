@echo off
setlocal

set PS3SDK=D:\PS3.Full.3.40.SDK.PS3-DUPLEX
set PS3_TOOLCHAIN=%PS3SDK%\[134]-PS3_Toolchain_411-Win_340_001\cell
set PS3_SDK=%PS3SDK%\[132]-PS3_SDK-340_001\cell
set CXX=%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-g++.exe
set AR=%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-ar.exe

set INCLUDES=-I%PS3_SDK%\target\ppu\include -I%PS3_SDK%\target\common\include -I%PS3_TOOLCHAIN%\target\ppu\include
set INCLUDES=%INCLUDES% -I..\..\libs\radcore\inc -I..\..\libs\radcore\src\pch
set INCLUDES=%INCLUDES% -I..\..\code -I..\..\code\memory

set CXXFLAGS=-c -g -O0 -fno-exceptions -fpermissive -DRAD_PS3 -DRAD_DEBUG -D_DEBUG -DRAD_CONSOLE -D__CELLOS_LV2__

echo Compiling memorymanager.cpp...
"%CXX%" %CXXFLAGS% %INCLUDES% -o ..\..\libs\radcore\lib\memorymanager.o ..\..\libs\radcore\src\radmemory\memorymanager.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo Updating radcorep3d.a...
"%AR%" rcs ..\..\libs\radcore\lib\radcorep3d.a ..\..\libs\radcore\lib\memorymanager.o
if %ERRORLEVEL% NEQ 0 goto :error

echo Done!
goto :end

:error
echo Failed with error %ERRORLEVEL%
exit /b 1

:end
endlocal
