@echo off
setlocal

set PS3SDK=D:\PS3.Full.3.40.SDK.PS3-DUPLEX
set PS3_TOOLCHAIN=%PS3SDK%\[134]-PS3_Toolchain_411-Win_340_001\cell
set PS3_SDK=%PS3SDK%\[132]-PS3_SDK-340_001\cell
set CXX=%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-g++.exe
set AR=%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-ar.exe

set INCLUDES=-I%PS3_SDK%\target\ppu\include -I%PS3_SDK%\target\common\include -I%PS3_TOOLCHAIN%\target\ppu\include
set INCLUDES=%INCLUDES% -I../../../ -I../../../pddi -I../../../../radmath

set CXXFLAGS=-c -g -O0 -fno-exceptions -fpermissive -DRAD_PS3 -DRAD_DEBUG -D_DEBUG -DRAD_CONSOLE -D__CELLOS_LV2__

echo Compiling ps3display.cpp...
"%CXX%" %CXXFLAGS% %INCLUDES% -o Debug\ps3display.o ..\..\..\pddi\ps3\ps3display.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo Compiling ps3device.cpp...
"%CXX%" %CXXFLAGS% %INCLUDES% -o Debug\ps3device.o ..\..\..\pddi\ps3\ps3device.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo Compiling ps3context.cpp...
"%CXX%" %CXXFLAGS% %INCLUDES% -o Debug\ps3context.o ..\..\..\pddi\ps3\ps3context.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo Updating pddip3d.a library...
"%AR%" rcs ..\..\lib\pddip3d.a Debug\ps3display.o Debug\ps3device.o Debug\ps3context.o Debug\ps3prim.o Debug\ps3primstream.o Debug\ps3shader.o Debug\basecontext.o Debug\baseshader.o Debug\font.o Debug\pddiobj.o
if %ERRORLEVEL% NEQ 0 goto :error

echo Done!
goto :end

:error
echo Failed with error %ERRORLEVEL%
exit /b 1

:end
endlocal
