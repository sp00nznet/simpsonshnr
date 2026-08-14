# Full PS3 Build Script
$ErrorActionPreference = "Stop"

# PS3 SDK paths
$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"
$LD = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"

# Add SN tools to PATH
$env:PATH = "$PS3_SDK\host-win32\sn\bin;$env:PATH"

# Include paths
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

# Compile flags
$CXXFLAGS = "-c -g -O0 -fno-exceptions -fpermissive -DRAD_PS3 -DRAD_DEBUG -D_DEBUG -DRAD_CONSOLE -D__CELLOS_LV2__"

# Source files with their paths (name -> relative path from code dir)
$sources = @{
    "allai" = "ai\allai.cpp"
    "allactor" = "ai\actor\allactor.cpp"
    "allsequencer" = "ai\sequencer\allsequencer.cpp"
    "allaivehicle" = "ai\vehicle\allaivehicle.cpp"
    "allatc" = "atc\allatc.cpp"
    "allcamera" = "camera\allcamera.cpp"
    "allcards" = "cards\allcards.cpp"
    "allcheats" = "cheats\allcheats.cpp"
    "allconsole" = "console\allconsole.cpp"
    "allcontexts" = "contexts\allcontexts.cpp"
    "allgameplay" = "contexts\gameplay\allgameplay.cpp"
    "alldemo" = "contexts\demo\alldemo.cpp"
    "alldata" = "data\alldata.cpp"
    "allmemcard" = "data\memcard\allmemcard.cpp"
    "alldebug" = "debug\alldebug.cpp"
    "allevents" = "events\allevents.cpp"
    "allgameflow" = "gameflow\allgameflow.cpp"
    "allinput" = "input\allinput.cpp"
    "allinteriors" = "interiors\allinteriors.cpp"
    "allloadmanager" = "loading\allloadmanager.cpp"
    "allps3main" = "main\allps3main.cpp"
    "allmemory" = "memory\allmemory.cpp"
    "allmeta" = "meta\allmeta.cpp"
    "allmission" = "mission\allmission.cpp"
    "allsafezone" = "mission\safezone\allsafezone.cpp"
    "allcharactersheet" = "mission\charactersheet\allcharactersheet.cpp"
    "allrewards" = "mission\rewards\allrewards.cpp"
    "allconditions" = "mission\conditions\allconditions.cpp"
    "allobjectives" = "mission\objectives\allobjectives.cpp"
    "allpresentation" = "presentation\allpresentation.cpp"
    "allrespawnmanager" = "mission\respawnmanager\allrespawnmanager.cpp"
    "allfmvplayerps3" = "presentation\fmvplayer\allfmvplayerps3.cpp"
    "allgui" = "presentation\gui\allgui.cpp"
    "allbootup" = "presentation\gui\bootup\allbootup.cpp"
    "allbackend" = "presentation\gui\backend\allbackend.cpp"
    "allfrontend" = "presentation\gui\frontend\allfrontend.cpp"
    "allminigame" = "presentation\gui\minigame\allminigame.cpp"
    "allingame" = "presentation\gui\ingame\allingame.cpp"
    "allhudevents" = "presentation\gui\ingame\hudevents\allhudevents.cpp"
    "allutility" = "presentation\gui\utility\allutility.cpp"
    "allpresevents" = "presentation\presevents\allpresevents.cpp"
    "allculling" = "render\Culling\allculling.cpp"
    "alldsg" = "render\DSG\alldsg.cpp"
    "allbreakables" = "render\breakables\allbreakables.cpp"
    "allskidmarks" = "worldsim\skidmarks\allskidmarks.cpp"
    "allintersect" = "render\IntersectManager\allintersect.cpp"
    "allloaders" = "render\Loaders\allloaders.cpp"
    "allparticles" = "render\Particles\allparticles.cpp"
    "allanimentitydsgmanager" = "render\animentitydsgmanager\allanimentitydsgmanager.cpp"
    "allrenderflow" = "render\RenderFlow\allrenderflow.cpp"
    "allrendermanager" = "render\RenderManager\allrendermanager.cpp"
    "allroads" = "roads\allroads.cpp"
    "allsound" = "sound\allsound.cpp"
    "allsoundavatar" = "sound\avatar\allsoundavatar.cpp"
    "alldialog" = "sound\dialog\alldialog.cpp"
    "allmusic" = "sound\music\allmusic.cpp"
    "allnissound" = "sound\nis\allnissound.cpp"
    "allsoundfx" = "sound\soundfx\allsoundfx.cpp"
    "allsoundrenderer" = "sound\soundrenderer\allsoundrenderer.cpp"
    "allsoundtuning" = "sound\tuning\allsoundtuning.cpp"
    "allsounddebug" = "sound\sounddebug\allsounddebug.cpp"
    "allmovingposn" = "sound\movingpositional\allmovingposn.cpp"
    "allworldsim" = "worldsim\allworldsim.cpp"
    "allcoins" = "worldsim\coins\allcoins.cpp"
    "allcharacter" = "worldsim\character\allcharacter.cpp"
    "allredbrick" = "worldsim\redbrick\allredbrick.cpp"
    "allvehiclecontroller" = "worldsim\redbrick\vehiclecontroller\allvehiclecontroller.cpp"
    "alltraffic" = "worldsim\traffic\alltraffic.cpp"
    "allpedpaths" = "pedpaths\allpedpaths.cpp"
    "allped" = "worldsim\ped\allped.cpp"
    "allspawn" = "worldsim\spawn\allspawn.cpp"
    "allharass" = "worldsim\harass\allharass.cpp"
    "allstateprop" = "stateprop\allstateprop.cpp"
    "allufo" = "mission\ufo\allufo.cpp"
    "allparkedcars" = "worldsim\parkedcars\allparkedcars.cpp"
    "allsupersprintctx" = "contexts\supersprint\allsupersprintctx.cpp"
    "allsupersprint" = "supersprint\allsupersprint.cpp"
    "allfootprint" = "worldsim\character\footprint\allfootprint.cpp"
}

# Create Debug directory if needed
if (!(Test-Path "Debug")) {
    New-Item -ItemType Directory -Path "Debug" | Out-Null
}

$total = $sources.Count
$current = 0
$failed = @()

Write-Host "Compiling $total source files..."
Write-Host ""

foreach ($name in $sources.Keys) {
    $current++
    $srcPath = "..\..\code\$($sources[$name])"
    $objPath = "Debug\$name.o"

    Write-Host "[$current/$total] Compiling $name.cpp..."

    $cmdArgs = "$CXXFLAGS $INCLUDES -o $objPath $srcPath"
    $process = Start-Process -FilePath $CXX -ArgumentList $cmdArgs -NoNewWindow -Wait -PassThru -RedirectStandardError "Debug\$name.err"

    if ($process.ExitCode -ne 0) {
        Write-Host "  FAILED!" -ForegroundColor Red
        $failed += $name
        $errContent = Get-Content "Debug\$name.err" -Raw
        if ($errContent) {
            Write-Host $errContent -ForegroundColor Yellow
        }
    } else {
        Remove-Item "Debug\$name.err" -ErrorAction SilentlyContinue
    }
}

Write-Host ""
if ($failed.Count -gt 0) {
    Write-Host "Compilation finished with $($failed.Count) failures:" -ForegroundColor Red
    $failed | ForEach-Object { Write-Host "  - $_" -ForegroundColor Red }
    exit 1
} else {
    Write-Host "All $total files compiled successfully!" -ForegroundColor Green
}

# Link
Write-Host ""
Write-Host "Linking srr2p3d.elf..."

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
    "$PS3_SDK\target\ppu\lib\libaudio_stub.a",
    "$PS3_SDK\target\ppu\lib\libmixer.a",
    "$PS3_SDK\target\ppu\lib\libspurs_stub.a",
    "$PS3_SDK\target\ppu\lib\librtc_stub.a",
    "$PS3_SDK\target\ppu\lib\libcgc.a",
    "$PS3_SDK\target\ppu\lib\libusbd_stub.a"
) -join " "

$OBJS = (Get-ChildItem "Debug\*.o" | ForEach-Object { $_.FullName }) -join " "

$linkArgs = "-L$PS3_SDK\target\ppu\lib -Wl,-Map,..\..\cd\ps3\srr2p3d.map -o ..\..\cd\ps3\srr2p3d.elf $OBJS $GAMELIBS $SYSLIBS"
$linkProcess = Start-Process -FilePath $LD -ArgumentList $linkArgs -NoNewWindow -Wait -PassThru

if ($linkProcess.ExitCode -ne 0) {
    Write-Host "Link FAILED!" -ForegroundColor Red
    exit 1
}

Write-Host "Link successful!" -ForegroundColor Green
Write-Host "Output: ..\..\cd\ps3\srr2p3d.elf"

# Create EBOOT.BIN
Write-Host ""
Write-Host "Creating EBOOT.BIN..."
$makeFself = "$PS3_SDK\host-win32\bin\make_fself.exe"
& $makeFself "..\..\cd\ps3\srr2p3d.elf" "..\..\cd\ps3\SIMP00001\PS3_GAME\USRDIR\EBOOT.BIN"
Write-Host "EBOOT.BIN created!" -ForegroundColor Green
