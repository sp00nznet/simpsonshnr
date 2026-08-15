# Recompile ps3platform (allps3main.cpp)
$ErrorActionPreference = "Continue"

$PS3SDK = "$env:PS3_SDK_ROOT"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"

Set-Location "C:\simpsonshnr\game\build\ps3"

$INCLUDES = @(
    "-I$PS3_SDK\target\ppu\include",
    "-I$PS3_SDK\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I..\..\code",
    "-I..\..\libs\radcore\inc",
    "-I..\..\libs\radmath",
    "-I..\..\libs\radsound\inc",
    "-I..\..\libs\radmusic\inc",
    "-I..\..\libs\radmovie\inc",
    "-I..\..\libs\radscript\inc",
    "-I..\..\libs\radcontent\inc",
    "-I..\..\libs\pure3d",
    "-I..\..\libs\pure3d\p3d",
    "-I..\..\libs\pure3d\pddi",
    "-I..\..\libs\sim",
    "-I..\..\libs\sim\simcommon",
    "-I..\..\libs\sim\simcollision",
    "-I..\..\libs\sim\simphysics",
    "-I..\..\libs\choreo\inc",
    "-I..\..\libs\poser\inc",
    "-I..\..\libs\scrooby\inc",
    "-I..\..\libs\scrooby\src",
    "-I..\..\libs\ps3sdkstubs"
) -join " "

$CXXFLAGS = "-c -g -O0 -fno-exceptions -fpermissive -DRAD_PS3 -DRAD_DEBUG -D_DEBUG -DRAD_CONSOLE -D__CELLOS_LV2__"

Write-Host "Recompiling allps3main.cpp..."
$cmdArgs = "$CXXFLAGS $INCLUDES -o Debug\allps3main.o ..\..\code\main\allps3main.cpp"

$process = Start-Process -FilePath $CXX -ArgumentList $cmdArgs -NoNewWindow -Wait -PassThru -RedirectStandardError "Debug\allps3main.err"

if ($process.ExitCode -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green
    Remove-Item "Debug\allps3main.err" -ErrorAction SilentlyContinue
} else {
    Write-Host "Compilation FAILED!" -ForegroundColor Red
    Get-Content "Debug\allps3main.err"
    exit 1
}
