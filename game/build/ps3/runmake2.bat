@echo off
setlocal

cd /d C:\simpsonshnr\game\build\ps3

set PS3SDK=%PS3_SDK_ROOT%
set PS3_TOOLCHAIN=%PS3SDK%\[134]-PS3_Toolchain_411-Win_340_001\cell
set PS3_SDK=%PS3SDK%\[132]-PS3_SDK-340_001\cell
set PATH=%PS3_SDK%\host-win32\sn\bin;%PATH%

echo Running make...
..\..\libs\radbuild\GlobalTools\make.exe CONFIG=DEBUG

echo Make finished with errorlevel %ERRORLEVEL%
endlocal
