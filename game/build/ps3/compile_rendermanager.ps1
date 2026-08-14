# Compile allrendermanager.cpp for PS3
$ErrorActionPreference = "Continue"

$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
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

Write-Host "Compiling allrendermanager.cpp..."
$srcPath = "..\..\code\render\RenderManager\allrendermanager.cpp"
$outPath = "Debug\allrendermanager.o"
$compileArgs = "$CXXFLAGS $INCLUDES -o $outPath $srcPath"

$process = Start-Process -FilePath $CXX -ArgumentList $compileArgs -NoNewWindow -Wait -PassThru -RedirectStandardError "error.txt"
if ($process.ExitCode -ne 0) {
    Write-Host "FAILED!" -ForegroundColor Red
    Get-Content "error.txt"
    exit 1
}
Write-Host "SUCCESS: $outPath" -ForegroundColor Green
