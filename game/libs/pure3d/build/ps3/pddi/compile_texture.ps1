# Compile ps3texture.cpp for PS3
$ErrorActionPreference = "Continue"

$PS3SDK = "$env:PS3_SDK_ROOT"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-ar.exe"

Set-Location "C:\simpsonshnr\game\libs\pure3d\build\ps3\pddi"

# Ensure Debug directory exists
if (!(Test-Path "Debug")) {
    New-Item -ItemType Directory -Path "Debug" | Out-Null
}

$commonArgs = @(
    "-c", "-g", "-O0", "-fno-exceptions", "-fpermissive",
    "-DRAD_PS3", "-DRAD_DEBUG", "-D_DEBUG", "-DRAD_CONSOLE", "-D__CELLOS_LV2__",
    "-I$PS3_SDK_DIR\target\ppu\include",
    "-I$PS3_SDK_DIR\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I../../../",
    "-I../../../pddi",
    "-I../../../../radmath",
    "-I../../../../radcore/inc",
    "-I../../../../code"
)

Write-Host "Compiling ps3texture.cpp..."
$args = $commonArgs + @("-o", "Debug/ps3texture.o", "../../../pddi/ps3/ps3texture.cpp")
& $CXX @args

if ($LASTEXITCODE -eq 0) {
    Write-Host "  OK" -ForegroundColor Green

    # Update the library archive
    Write-Host "Updating library archive..."
    & $AR rcs "../../lib/pddip3d.a" "Debug/ps3texture.o"

    if ($LASTEXITCODE -eq 0) {
        Write-Host "Library updated!" -ForegroundColor Green
    } else {
        Write-Host "Archive update failed!" -ForegroundColor Red
    }
} else {
    Write-Host "  FAILED" -ForegroundColor Red
}
