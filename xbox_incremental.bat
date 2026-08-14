@echo off
setlocal

REM Set XDK environment
set XDK=D:\XBoX - Microsoft SDK 2003\XDK

REM Set up Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

REM Set Xbox include/lib paths (BEFORE Windows SDK)
set INCLUDE=%XDK%\xbox\include;%XDK%\include
set LIB=%XDK%\xbox\lib;%XDK%\lib
set PATH=%XDK%\xbox\bin;%PATH%

cd /d C:\simpsonshnr\game\build\xbox

echo.
echo === Incremental Xbox Build ===
echo.

REM Compile the changed traffic manager file with all includes
echo Compiling trafficmanager.cpp...
cl.exe /c /DNDEBUG /DRAD_RELEASE /D_XBOX /DRAD_XBOX /DRAD_CONSOLE /DFINAL ^
    /I"..\..\code" ^
    /I"..\..\libs\radcore\inc" ^
    /I"..\..\libs\radmovie\inc" ^
    /I"..\..\libs\radsound\inc" ^
    /I"..\..\libs\radscript\inc" ^
    /I"..\..\libs\radmusic\inc" ^
    /I"..\..\libs\pure3d" ^
    /I"..\..\libs\scrooby\inc" ^
    /I"..\..\libs\scrooby\src" ^
    /I"..\..\libs\radmath" ^
    /I"..\..\libs\sim" ^
    /I"..\..\libs\poser\inc" ^
    /I"..\..\libs\choreo\inc" ^
    /I"..\..\libs\radcontent\inc" ^
    /I"..\..\libs\radcontent" ^
    /O2 /GR /EHsc ^
    "..\..\code\worldsim\traffic\trafficmanager.cpp" ^
    /Fo"Release\trafficmanager.obj"

if errorlevel 1 (
    echo Compilation FAILED
    exit /b 1
)

echo Compilation successful!
echo.

endlocal
