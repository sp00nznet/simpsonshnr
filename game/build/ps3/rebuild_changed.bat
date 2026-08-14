@echo off
setlocal

REM PS3 SDK paths
set PS3SDK=D:\PS3.Full.3.40.SDK.PS3-DUPLEX
set PS3_TOOLCHAIN=%PS3SDK%\[134]-PS3_Toolchain_411-Win_340_001\cell
set PS3_SDK=%PS3SDK%\[132]-PS3_SDK-340_001\cell
set CXX=%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-g++.exe
set LD=%PS3_TOOLCHAIN%\host-win32\ppu\bin\ppu-lv2-g++.exe

REM Add SN tools to path for linker
set PATH=%PS3_SDK%\host-win32\sn\bin;%PATH%

REM Include paths - PS3 SDK
set INCLUDES=-I%PS3_SDK%\target\ppu\include -I%PS3_SDK%\target\common\include -I%PS3_TOOLCHAIN%\target\ppu\include

REM Include paths - Game code
set INCLUDES=%INCLUDES% -I..\..\code

REM Include paths - Foundation Tech
set INCLUDES=%INCLUDES% -I..\..\libs\radcore\inc
set INCLUDES=%INCLUDES% -I..\..\libs\radmath
set INCLUDES=%INCLUDES% -I..\..\libs\radsound\inc
set INCLUDES=%INCLUDES% -I..\..\libs\radmusic\inc
set INCLUDES=%INCLUDES% -I..\..\libs\radmovie\inc
set INCLUDES=%INCLUDES% -I..\..\libs\radscript\inc
set INCLUDES=%INCLUDES% -I..\..\libs\radcontent\inc

REM Include paths - Pure3D (needs multiple subdirs)
set INCLUDES=%INCLUDES% -I..\..\libs\pure3d
set INCLUDES=%INCLUDES% -I..\..\libs\pure3d\p3d
set INCLUDES=%INCLUDES% -I..\..\libs\pure3d\pddi

REM Include paths - Simulation
set INCLUDES=%INCLUDES% -I..\..\libs\sim
set INCLUDES=%INCLUDES% -I..\..\libs\sim\simcommon
set INCLUDES=%INCLUDES% -I..\..\libs\sim\simcollision
set INCLUDES=%INCLUDES% -I..\..\libs\sim\simphysics

REM Include paths - Other libs
set INCLUDES=%INCLUDES% -I..\..\libs\choreo\inc
set INCLUDES=%INCLUDES% -I..\..\libs\poser\inc
set INCLUDES=%INCLUDES% -I..\..\libs\scrooby\inc
set INCLUDES=%INCLUDES% -I..\..\libs\scrooby\src
set INCLUDES=%INCLUDES% -I..\..\libs\ps3sdkstubs

REM Compile flags
set CXXFLAGS=-c -g -O0 -fno-exceptions -fpermissive -DRAD_PS3 -DRAD_DEBUG -D_DEBUG -DRAD_CONSOLE -D__CELLOS_LV2__

echo Compiling allps3main.cpp (includes ps3main.cpp, ps3stubs.cpp, ps3platform.cpp, etc.)...
"%CXX%" %CXXFLAGS% %INCLUDES% -o Debug\allps3main.o ..\..\code\main\allps3main.cpp
if %ERRORLEVEL% NEQ 0 goto :error

echo Recompilation successful!

echo.
echo Linking srr2p3d.elf...
set LIBDIR=..\..\libs

REM Game libraries (order matters - dependents before dependencies)
set GAMELIBS=%LIBDIR%\scrooby\lib\scroobyp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\choreo\lib\choreop3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\poser\lib\poserp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\sim\lib\simp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radscript\lib\radscriptp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radmusic\lib\radmusicp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radsound\lib\radsoundp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radmovie\lib\radmoviep3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radcontent\lib\radcontentp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\pure3d\build\lib\pure3dp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\pure3d\build\lib\pddip3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radmath\lib\radmathp3d.a
set GAMELIBS=%GAMELIBS% %LIBDIR%\radcore\lib\radcorep3d.a

REM PS3 system libraries
set SYSLIBS=%PS3_SDK%\target\ppu\lib\PSGL\RSX\debug\libPSGL.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\PSGL\RSX\debug\libPSGLU.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\PSGL\RSX\debug\libPSGLcgc.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libresc_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libdbgfont.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libpngdec_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libc.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libm.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libstdc++.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libsysmodule_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libsysutil_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libio_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libfs_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libgcm_cmd.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libgcm_sys_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libaudio_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libmixer.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libspurs_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\librtc_stub.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libcgc.a
set SYSLIBS=%SYSLIBS% %PS3_SDK%\target\ppu\lib\libusbd_stub.a

REM Custom zlib for PS3 (built from pure3d's bundled zlib source)
set GAMELIBS=%GAMELIBS% %LIBDIR%\pure3d\build\lib\zlibp3d.a

REM Collect all object files
set OBJS=Debug\*.o

REM Link with map file
"%LD%" -L%PS3_SDK%\target\ppu\lib -Wl,-Map,..\..\cd\ps3\srr2p3d.map -o ..\..\cd\ps3\srr2p3d.elf %OBJS% %GAMELIBS% %SYSLIBS%
if %ERRORLEVEL% NEQ 0 goto :linkerror

echo Link successful!
echo Output: ..\..\cd\ps3\srr2p3d.elf
goto :end

:error
echo Compilation failed with error %ERRORLEVEL%
exit /b 1

:linkerror
echo Link failed with error %ERRORLEVEL%
exit /b 1

:end
endlocal
