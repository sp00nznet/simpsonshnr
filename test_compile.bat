@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" > nul 2>&1
set XDK=D:\XBoX - Microsoft SDK 2003\XDK
set INCLUDE=%XDK%\xbox\include;%XDK%\include;%INCLUDE%
set LIB=%XDK%\xbox\lib;%XDK%\lib;%LIB%
cl.exe /c /I"%XDK%\xbox\include" C:\simpsonshnr\test_xbox.cpp
