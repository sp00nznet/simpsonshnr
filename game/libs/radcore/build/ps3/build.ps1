# radcore PS3 build script.
#
# Compiles every radcore source that the PS3 link needs and archives them into
# radcorep3d.a. Objects are skipped when they are newer than their .cpp -- there
# is no header dependency tracking, so delete Debug\ after touching a shared
# header (see the DEBUGWATCH vtable incident).

$ErrorActionPreference = "Continue"

$ToolchainBase = "$env:PS3_SDK_ROOT\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "$env:PS3_SDK_ROOT\[132]-PS3_SDK-340_001\cell"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RadcoreDir = (Resolve-Path "$ScriptDir\..\..").Path
$LibsDir = (Resolve-Path "$RadcoreDir\..").Path
$SourceDir = "$RadcoreDir\src"
$ObjDir = "$ScriptDir\Debug"
$LibOutDir = "$RadcoreDir\lib"

$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

$Includes = @(
    "-I$LibsDir\pure3d\build\ps3\include_fix",
    "-I$RadcoreDir\inc",
    "-I$RadcoreDir\src\pch",
    "-I$RadcoreDir\src\radfile\common",
    "-I$RadcoreDir\src\radmemory",
    "-I$RadcoreDir\src\radthread",
    "-I$LibsDir\..\code",
    "-I$SDKBase\target\ppu\include",
    "-I$SDKBase\target\common\include",
    "-I$ToolchainBase\target\ppu\include"
)

$Defines = @( "-DRAD_PS3", "-DRAD_CONSOLE", "-DRAD_DEBUG", "-D_DEBUG", "-D__CELLOS_LV2__" )
$Flags = @( "-c", "-g", "-O0", "-fno-exceptions", "-fpermissive" )

# Source paths relative to src\, without the .cpp extension.
$SourceFiles = @(
    "radcontroller/controllerbuffer",
    "radcontroller/ps3controller",
    "radcrashhandler/ps3crashhandler",
    "raddebug/debug",
    "raddebugcommunication/targetconnection",
    "raddebugcommunication/targetsocketchannel",
    "raddebugcommunication/targetx",
    "raddebugconsole/consoleclient",
    "raddebugwatch/watchclient",
    "raddispatch/dispatcher",
    "radfile/common/buffereddrive",
    "radfile/common/cementer",
    "radfile/common/drive",
    "radfile/common/drivethread",
    "radfile/common/file",
    "radfile/common/filecache",
    "radfile/common/filesystem",
    "radfile/common/instancedrive",
    "radfile/common/platformdrives",
    "radfile/common/radfile",
    "radfile/common/remotedrive",
    "radfile/common/requests",
    "radfile/common/signeddrive",
    "radfile/ps3/ps3bddrive",
    "radfile/ps3/ps3savedrive",
    "radkey/radkey",
    "radmemory/align",
    "radmemory/binallocator",
    "radmemory/dlheap",
    "radmemory/externalmemoryheap",
    "radmemory/externalmemoryobject",
    "radmemory/memorymanager",
    "radmemory/memoryobject",
    "radmemory/memorypool",
    "radmemory/memoryspaceps3",
    "radmemory/platalloc",
    "radmemory/staticheap",
    "radmemory/trackingheap",
    "radmemory/twowayallocator",
    "radmemorymonitor/memmonitorclient",
    "radmemorymonitor/memmonitorclienthelp",
    "radobject/object",
    "radobjectbtree/objectavltree",
    "radobjectbtree/objectbtree",
    "radobjectlist/dynamicarray",
    "radobjectlist/objectlist",
    "radobjectlist/weakcallbackwrapper",
    "radobjectlist/weakinterfacewrapper",
    "radplatform/platform",
    "radprofiler/profiler",
    "radprofiler/profiler2",
    "radprofiler/profilesample",
    "radremotecommand/functionlist",
    "radremotecommand/remotecommandserver",
    "radremotecommand/remotecommandtarget",
    "radstacktrace/ps3/stacktrace",
    "radstats/simplestat",
    "radstats/statmanager",
    "radstring/string",
    "radtextdisplay/textdisplay",
    "radthread/mutex",
    "radthread/semaphore",
    "radthread/system",
    "radthread/thread",
    "radtime/stopwatch",
    "radtime/time"
)

Write-Host "Building radcore for PS3"
Write-Host "Source directory: $SourceDir"
Write-Host ""

$CompiledCount = 0
$SkippedCount = 0
$ErrorCount = 0

foreach ($name in $SourceFiles) {
    $src = "$SourceDir\$($name -replace '/', '\').cpp"
    $obj = "$ObjDir\$([System.IO.Path]::GetFileName($name)).o"

    if (!(Test-Path $src)) {
        Write-Host "MISSING SOURCE: $src" -ForegroundColor Red
        $ErrorCount++
        continue
    }

    if ((Test-Path $obj) -and ((Get-Item $obj).LastWriteTime -gt (Get-Item $src).LastWriteTime)) {
        $SkippedCount++
        continue
    }

    $dir = Split-Path -Parent $obj
    if (!(Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }

    Write-Host "Compile: $name"
    $out = & $CXX ($Flags + $Includes + $Defines + @("-o", $obj, $src)) 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR compiling $name" -ForegroundColor Red
        Write-Host $out
        $ErrorCount++
    } else {
        $CompiledCount++
    }
}

Write-Host ""
Write-Host "Compilation: $CompiledCount compiled, $SkippedCount skipped, $ErrorCount errors"

if ($ErrorCount -gt 0) {
    Write-Host "Build failed with errors" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Creating library..."
if (!(Test-Path $LibOutDir)) { New-Item -ItemType Directory -Path $LibOutDir | Out-Null }

$LibFile = "$LibOutDir\radcorep3d.a"
$AllObjs = Get-ChildItem -Path $ObjDir -Filter "*.o" -Recurse | ForEach-Object { $_.FullName }
Write-Host "Adding $($AllObjs.Count) object files to library"

if (Test-Path $LibFile) { Remove-Item $LibFile -Force }  # rebuild from scratch: ar r keeps stale duplicate members
& $AR cr $LibFile $AllObjs 2>&1
if ($LASTEXITCODE -eq 0) {
    & $RANLIB $LibFile 2>&1
    Write-Host "Library created: $LibFile" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Failed to create library" -ForegroundColor Red
    exit 1
}
