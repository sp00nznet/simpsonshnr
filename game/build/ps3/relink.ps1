# Relink and create EBOOT
$ErrorActionPreference = "Stop"

$PS3SDK = "$env:PS3_SDK_ROOT"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$LD = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"
$env:PATH = "$PS3_SDK\host-win32\sn\bin;$env:PATH"

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
    "$LIBDIR\pure3d\build\lib\zlibp3d.a",
    "$LIBDIR\radmath\lib\radmathp3d.a",
    "$LIBDIR\radcore\lib\radcorep3d.a"
) -join " "

$SYSLIBS = @(
    "$PS3_SDK\target\ppu\lib\PSGL\RSX\debug\libPSGL.a",
    "$PS3_SDK\target\ppu\lib\PSGL\RSX\debug\libPSGLU.a",
    "$PS3_SDK\target\ppu\lib\PSGL\RSX\debug\libPSGLcgc.a",
    "$PS3_SDK\target\ppu\lib\libresc_stub.a",
    "$PS3_SDK\target\ppu\lib\libdbgfont.a",
    "$PS3_SDK\target\ppu\lib\libpngdec_stub.a",
    "$PS3_SDK\target\ppu\lib\libc.a",
    "$PS3_SDK\target\ppu\lib\libm.a",
    "$PS3_SDK\target\ppu\lib\libstdc++.a",
    "$PS3_SDK\target\ppu\lib\libsysmodule_stub.a",
    "$PS3_SDK\target\ppu\lib\libsysutil_stub.a",
    "$PS3_SDK\target\ppu\lib\libio_stub.a",
    "$PS3_SDK\target\ppu\lib\libfs_stub.a",
    "$PS3_SDK\target\ppu\lib\libgcm_cmd.a",
    "$PS3_SDK\target\ppu\lib\libgcm_sys_stub.a",
    "$PS3_SDK\target\ppu\lib\libspurs_stub.a",
    "$PS3_SDK\target\ppu\lib\librtc_stub.a",
    "$PS3_SDK\target\ppu\lib\libcgc.a",
    "$PS3_SDK\target\ppu\lib\libusbd_stub.a"
) -join " "

$OBJS = (Get-ChildItem "Debug\*.o" | ForEach-Object { $_.FullName }) -join " "

Write-Host "Linking srr2p3d.elf..."
$linkArgs = "-L$PS3_SDK\target\ppu\lib -Wl,--noinhibit-exec -Wl,-Map,..\..\cd\ps3\srr2p3d.map -o ..\..\cd\ps3\srr2p3d.elf $OBJS $GAMELIBS $SYSLIBS"
$linkProcess = Start-Process -FilePath $LD -ArgumentList $linkArgs -NoNewWindow -Wait -PassThru
if ($linkProcess.ExitCode -ne 0) {
    Write-Host "Link FAILED!" -ForegroundColor Red
    exit 1
}
Write-Host "Link successful!" -ForegroundColor Green

Write-Host "Creating EBOOT.BIN..."
$makeFself = "$PS3_SDK\host-win32\bin\make_fself.exe"
& $makeFself "..\..\cd\ps3\srr2p3d.elf" "..\..\cd\ps3\SIMP00001\PS3_GAME\USRDIR\EBOOT.BIN"
Write-Host "EBOOT.BIN created!" -ForegroundColor Green
