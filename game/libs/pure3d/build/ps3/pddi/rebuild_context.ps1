# Rebuild ps3context.cpp

$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-ar.exe"

Write-Host "Compiling ps3context.cpp..."

$compileArgs = @(
    "-c", "-O2", "-fno-exceptions", "-fpermissive",
    "-DRAD_PS3", "-DRAD_RELEASE", "-DRAD_CONSOLE", "-D__CELLOS_LV2__",
    "-I$PS3_SDK_DIR\target\ppu\include",
    "-I$PS3_SDK_DIR\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I../../../",
    "-I../../../pddi",
    "-I../../../../radmath",
    "-I../../../../radcore/inc",
    "-I../../../../code",
    "-o", "Debug/ps3context.o",
    "../../../pddi/ps3/ps3context.cpp"
)

& $CXX @compileArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host "Compilation successful!" -ForegroundColor Green

    Write-Host "Updating library archive..."
    & $AR rcs "../../lib/pddip3d.a" Debug/ps3context.o

    if ($LASTEXITCODE -eq 0) {
        Write-Host "Library updated!" -ForegroundColor Green
    } else {
        Write-Host "Archive update failed!" -ForegroundColor Red
    }
} else {
    Write-Host "Compilation failed!" -ForegroundColor Red
}
