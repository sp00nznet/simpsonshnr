# Compile system.cpp for PS3
$ErrorActionPreference = "Continue"

$ToolchainBase = "$env:PS3_SDK_ROOT\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "$env:PS3_SDK_ROOT\[132]-PS3_SDK-340_001\cell"
$LibsDir = "C:\simpsonshnr\game\libs"
$RadcoreDir = "$LibsDir\radcore"
$SrcFile = "$RadcoreDir\src\radthread\system.cpp"
$OutDir = "$RadcoreDir\build\ps3\Debug"
$OutFile = "$OutDir\system.o"

$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"

$Includes = @(
    "-I$LibsDir\pure3d\build\ps3\include_fix",
    "-I$RadcoreDir\inc",
    "-I$RadcoreDir\src\pch",
    "-I$RadcoreDir\src\radthread",
    "-I$LibsDir\..\code",
    "-I$SDKBase\target\ppu\include",
    "-I$SDKBase\target\common\include",
    "-I$ToolchainBase\target\ppu\include"
)

$Defines = @(
    "-DRAD_PS3",
    "-DRAD_CONSOLE",
    "-DRAD_DEBUG",
    "-D_DEBUG",
    "-D__CELLOS_LV2__"
)

$Flags = @(
    "-c",
    "-g",
    "-O0",
    "-fno-exceptions",
    "-fpermissive"
)

Write-Host "Compiling $SrcFile..."
$AllArgs = $Flags + $Includes + $Defines + @("-o", $OutFile, $SrcFile)
& $CXX $AllArgs 2>&1
if ($LASTEXITCODE -eq 0) {
    Write-Host "SUCCESS: $OutFile"
} else {
    Write-Host "FAILED with exit code $LASTEXITCODE"
}
