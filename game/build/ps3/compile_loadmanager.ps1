# Compile loadingmanager for PS3
$ErrorActionPreference = "Continue"

$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"

$Includes = @(
    "-I$PS3_SDK_DIR\target\ppu\include",
    "-I$PS3_SDK_DIR\target\common\include",
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
    "-I..\..\libs\scrooby\src"
)

$Defines = @(
    "-DRAD_PS3",
    "-DRAD_DEBUG",
    "-D_DEBUG",
    "-DRAD_CONSOLE",
    "-D__CELLOS_LV2__"
)

$Flags = @(
    "-c",
    "-g",
    "-O0",
    "-fno-exceptions",
    "-fpermissive"
)

Write-Host "Compiling allloadmanager.cpp..."
$args = $Flags + $Defines + $Includes + @("-o", "Debug\allloadmanager.o", "..\..\code\loading\allloadmanager.cpp")
& $CXX @args 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "SUCCESS" -ForegroundColor Green
} else {
    Write-Host "FAILED with exit code $LASTEXITCODE" -ForegroundColor Red
}
