# Compile ps3bddrive.cpp for PS3
$ErrorActionPreference = "Continue"

$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"
$LibsDir = "C:\simpsonshnr\game\libs"
$RadcoreDir = "$LibsDir\radcore"
$SrcFile = "$RadcoreDir\src\radfile\ps3\ps3bddrive.cpp"
$OutDir = "$RadcoreDir\build\ps3\Debug"
$OutFile = "$OutDir\ps3bddrive.o"

$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"

$GameDir = "C:\simpsonshnr\game\code"
$Includes = @(
    "-I$LibsDir\pure3d\build\ps3\include_fix",
    "-I$RadcoreDir\inc",
    "-I$RadcoreDir\src\radfile\common",
    "-I$GameDir",
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
