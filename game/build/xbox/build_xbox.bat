@echo off
setlocal

REM Set up Visual Studio 2022 environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Set XDK paths
set XDK=D:\XBoX - Microsoft SDK 2003\XDK
set INCLUDE=%XDK%\xbox\include;%XDK%\include;%INCLUDE%
set LIB=%XDK%\xbox\lib;%XDK%\lib;%LIB%
set PATH=%XDK%\xbox\bin;%PATH%

echo XDK=%XDK%
echo.
echo Compiler version:
cl.exe 2>&1 | findstr /C:"Version"

echo.
echo Testing include path...
if exist "%XDK%\xbox\include\xtl.h" (
    echo Found xtl.h - XDK includes OK
) else (
    echo ERROR: Cannot find xtl.h
)

echo.
echo Testing lib path...
if exist "%XDK%\xbox\lib\xapilib.lib" (
    echo Found xapilib.lib - XDK libs OK
) else (
    echo ERROR: Cannot find xapilib.lib
)

endlocal
