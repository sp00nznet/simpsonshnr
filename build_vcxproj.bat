@echo off
setlocal

REM Set XDK environment
set XDK=D:\XBoX - Microsoft SDK 2003\XDK
set XBOXINCLUDE=%XDK%\xbox\include;%XDK%\include
set XBOXLIB=%XDK%\xbox\lib;%XDK%\lib
set PATH=%XDK%\xbox\bin;%PATH%

REM Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Add XDK paths
set INCLUDE=%XBOXINCLUDE%;%INCLUDE%
set LIB=%XBOXLIB%;%LIB%

cd /d C:\simpsonshnr\game\build\xbox

echo Building with MSBuild...
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" SRR2.vcxproj /p:Configuration=Debug /p:Platform=Win32 /v:minimal 2>&1

endlocal
