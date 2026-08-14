@echo off
setlocal

REM Set XDK environment
set XDK=D:\XBoX - Microsoft SDK 2003\XDK
set PATH=%XDK%\xbox\bin;%PATH%

REM Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Add XDK paths BEFORE Windows SDK paths
set INCLUDE=%XDK%\xbox\include;%XDK%\include
set LIB=%XDK%\xbox\lib;%XDK%\lib

cd /d C:\simpsonshnr\game\build\xbox

echo.
echo === Building Simpsons Hit and Run for Xbox ===
echo.
echo Using XDK: %XDK%
echo.

REM Try using devenv with the original vcproj
"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" SRR2.vcproj /Build "Release|Xbox" /Out build_log.txt

type build_log.txt

endlocal
