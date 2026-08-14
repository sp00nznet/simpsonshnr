# Compile allmemory.cpp for PS3
$ErrorActionPreference = "Continue"

$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"

$BaseDir = "C:\simpsonshnr\game"
$Includes = @(
    "-I$PS3_SDK_DIR\target\ppu\include",
    "-I$PS3_SDK_DIR\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I$BaseDir\code",
    "-I$BaseDir\libs\radcore\inc",
    "-I$BaseDir\libs\radmath",
    "-I$BaseDir\libs\radsound\inc",
    "-I$BaseDir\libs\radmusic\inc",
    "-I$BaseDir\libs\radmovie\inc",
    "-I$BaseDir\libs\radscript\inc",
    "-I$BaseDir\libs\radcontent\inc",
    "-I$BaseDir\libs\pure3d",
    "-I$BaseDir\libs\pure3d\p3d",
    "-I$BaseDir\libs\pure3d\pddi",
    "-I$BaseDir\libs\sim",
    "-I$BaseDir\libs\sim\simcommon",
    "-I$BaseDir\libs\sim\simcollision",
    "-I$BaseDir\libs\sim\simphysics",
    "-I$BaseDir\libs\choreo\inc",
    "-I$BaseDir\libs\poser\inc",
    "-I$BaseDir\libs\scrooby\inc",
    "-I$BaseDir\libs\scrooby\src"
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

Write-Host "Compiling allmemory.cpp..."
$args = $Flags + $Defines + $Includes + @("-o", "$BaseDir\build\ps3\Debug\allmemory.o", "$BaseDir\code\memory\allmemory.cpp")
& $CXX @args 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "SUCCESS" -ForegroundColor Green
} else {
    Write-Host "FAILED with exit code $LASTEXITCODE" -ForegroundColor Red
}
