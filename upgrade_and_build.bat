@echo off
setlocal

REM Set XDK environment
set XDK=D:\XBoX - Microsoft SDK 2003\XDK
set XBOXINCLUDE=%XDK%\xbox\include;%XDK%\include
set XBOXLIB=%XDK%\xbox\lib;%XDK%\lib
set PATH=%XDK%\xbox\bin;%PATH%

REM Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Add XDK paths to standard paths
set INCLUDE=%XBOXINCLUDE%;%INCLUDE%
set LIB=%XBOXLIB%;%LIB%

echo.
echo XDK: %XDK%
echo INCLUDE: %INCLUDE%
echo LIB: %LIB%
echo.

REM Try to upgrade and build
cd /d C:\simpsonshnr\game\build\xbox
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" SRR2.sln /Upgrade

echo.
echo Upgrade complete. Attempting build...
echo.

"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" SRR2.sln /Build "Debug"

endlocal
