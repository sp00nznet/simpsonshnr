# Link srr2p3d.elf
Set-Location $PSScriptRoot

$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$LD = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"

$env:PATH = "$PS3_SDK_DIR\host-win32\sn\bin;" + $env:PATH

$LIBDIR = "..\..\libs"

$GAMELIBS = @(
    "$LIBDIR\scrooby\lib\scroobyp3d.a",
    "$LIBDIR\choreo\lib\choreop3d.a",
    "$LIBDIR\poser\lib\poserp3d.a",
    "$LIBDIR\sim\lib\simp3d.a",
    "$LIBDIR\radscript\lib\radscriptp3d.a",
    "$LIBDIR\radmusic\lib\radmusicp3d.a",
    "$LIBDIR\radsound\lib\radsoundp3d.a",
    "$LIBDIR\radmovie\lib\radmoviep3d.a",
    "$LIBDIR\radcontent\lib\radcontentp3d.a",
    "$LIBDIR\pure3d\build\lib\pure3dp3d.a",
    "$LIBDIR\pure3d\build\lib\pddip3d.a",
    "$LIBDIR\radmath\lib\radmathp3d.a",
    "$LIBDIR\radcore\lib\radcorep3d.a",
    "$LIBDIR\pure3d\build\lib\zlibp3d.a"
)

$SYSLIBS = @(
    "$PS3_SDK_DIR\target\ppu\lib\PSGL\RSX\debug\libPSGL.a",
    "$PS3_SDK_DIR\target\ppu\lib\PSGL\RSX\debug\libPSGLU.a",
    "$PS3_SDK_DIR\target\ppu\lib\PSGL\RSX\debug\libPSGLcgc.a",
    "$PS3_SDK_DIR\target\ppu\lib\libresc_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libdbgfont.a",
    "$PS3_SDK_DIR\target\ppu\lib\libpngdec_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libc.a",
    "$PS3_SDK_DIR\target\ppu\lib\libm.a",
    "$PS3_SDK_DIR\target\ppu\lib\libstdc++.a",
    "$PS3_SDK_DIR\target\ppu\lib\libsysmodule_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libsysutil_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libio_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libfs_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libgcm_cmd.a",
    "$PS3_SDK_DIR\target\ppu\lib\libgcm_sys_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libaudio_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libmixer.a",
    "$PS3_SDK_DIR\target\ppu\lib\libspurs_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\librtc_stub.a",
    "$PS3_SDK_DIR\target\ppu\lib\libcgc.a",
    "$PS3_SDK_DIR\target\ppu\lib\libusbd_stub.a"
)

$OBJS = Get-ChildItem -Path "Debug" -Filter "*.o" | ForEach-Object { $_.FullName }

Write-Host "Linking srr2p3d.elf..."
Write-Host "Object files: $($OBJS.Count)"

$linkArgs = @("-L$PS3_SDK_DIR\target\ppu\lib", "-Wl,-Map,..\..\cd\ps3\srr2p3d.map", "-o", "..\..\cd\ps3\srr2p3d.elf") + $OBJS + $GAMELIBS + $SYSLIBS
& $LD @linkArgs 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "Link successful!" -ForegroundColor Green
    $elf = Get-Item "..\..\cd\ps3\srr2p3d.elf"
    Write-Host "Output: $($elf.FullName) ($([math]::Round($elf.Length / 1MB, 1)) MB)"
} else {
    Write-Host "Link FAILED with exit code $LASTEXITCODE" -ForegroundColor Red
    exit 1
}
