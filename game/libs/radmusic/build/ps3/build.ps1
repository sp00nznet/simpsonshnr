# radmusic PS3 Build Script

$ErrorActionPreference = "Continue"

# Base paths (absolute)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RadmusicDir = (Resolve-Path "$ScriptDir\..\..").Path
$LibsDir = (Resolve-Path "$RadmusicDir\..").Path
$SourceDir = "$RadmusicDir\src"
$ObjDir = "$ScriptDir\Debug"
$LibOutDir = "$RadmusicDir\lib"
$IncludeFixDir = "$LibsDir\pure3d\build\ps3\include_fix"

# Paths
$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"

# Compiler
$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

Write-Host "Building radmusic for PS3"
Write-Host "Source directory: $SourceDir"
Write-Host ""

# Includes
$Includes = @(
    "-I$IncludeFixDir",
    "-I$ScriptDir",
    "-I$RadmusicDir\inc",
    "-I$RadmusicDir\src",
    "-I$RadmusicDir\src\pch",
    "-I$LibsDir\radcore\inc",
    "-I$LibsDir\radcontent\inc",
    "-I$LibsDir\radsound\inc",
    "-I$LibsDir\radmath",
    "-I$LibsDir\..\code",
    "-I$SDKBase\target\ppu\include",
    "-I$SDKBase\target\common\include",
    "-I$ToolchainBase\target\ppu\include"
)

# Defines
$Defines = @(
    "-DRAD_PS3",
    "-DPLAT_PS3",
    "-DRAD_CONSOLE",
    "-DRAD_DEBUG",
    "-D_DEBUG",
    "-D__CELLOS_LV2__"
)

# Flags
$Flags = @(
    "-c",
    "-g",
    "-O0",
    "-fno-exceptions",
    "-fpermissive",
    "-Wall",
    "-Wno-unused",
    "-Wno-non-virtual-dtor"
)

# Source files
$SourceFiles = @(
    "memory/memory",
    "ods/ods",
    "ods/ods_codegen",
    "ods/ods_memory",
    "ods/ods_parser",
    "ods/ods_util",
    "radmusic/framework/framework",
    "radmusic/music/music_composition",
    "radmusic/music/music_engine",
    "radmusic/music/music_engine_state_fade",
    "radmusic/music/music_engine_state_steady",
    "radmusic/music/music_engine_state_stitch",
    "radmusic/music/music_engine_state_stopped",
    "radmusic/music/music_performance",
    "radmusic/resource/resource_manager",
    "radmusic/schema/schema_util",
    "radmusic/sequence/sequence_player",
    "radmusic/sequence/sequence_region",
    "radmusic/sequence/sequence_stream_graph"
)

# Create output directories
if (!(Test-Path $ObjDir)) {
    New-Item -ItemType Directory -Path $ObjDir | Out-Null
}

@("memory", "ods", "radmusic", "radmusic/framework", "radmusic/music", "radmusic/resource", "radmusic/schema", "radmusic/sequence") | ForEach-Object {
    $subdir = "$ObjDir\$_"
    if (!(Test-Path $subdir)) {
        New-Item -ItemType Directory -Path $subdir -Force | Out-Null
    }
}

$ErrorCount = 0
$CompiledCount = 0
$SkippedCount = 0

# Compile each file
foreach ($src in $SourceFiles) {
    $srcFile = "$SourceDir\$src.cpp"
    $objFile = "$ObjDir\$src.o"

    # Ensure output directory exists for subdirs
    $objDir2 = Split-Path $objFile
    if ($objDir2 -and !(Test-Path $objDir2)) {
        New-Item -ItemType Directory -Path $objDir2 -Force | Out-Null
    }

    # Check source exists
    if (!(Test-Path $srcFile)) {
        Write-Host "MISSING: $srcFile" -ForegroundColor Yellow
        continue
    }

    # Skip if object file is newer than source
    if (Test-Path $objFile) {
        $srcTime = (Get-Item $srcFile).LastWriteTime
        $objTime = (Get-Item $objFile).LastWriteTime
        if ($objTime -gt $srcTime) {
            $SkippedCount++
            continue
        }
    }

    Write-Host "Compile: $src"

    $args = $Flags + $Defines + $Includes + @("-o", $objFile, $srcFile)
    $result = & $CXX $args 2>&1

    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR compiling $src" -ForegroundColor Red
        Write-Host $result
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

# Create library
Write-Host ""
Write-Host "Creating library..."

if (!(Test-Path $LibOutDir)) {
    New-Item -ItemType Directory -Path $LibOutDir | Out-Null
}

$LibFile = "$LibOutDir\radmusicp3d.a"

# Get all object files
$AllObjs = Get-ChildItem -Path $ObjDir -Filter "*.o" -Recurse | ForEach-Object { $_.FullName }

Write-Host "Adding $($AllObjs.Count) object files to library"

& $AR cr $LibFile $AllObjs 2>&1
if ($LASTEXITCODE -eq 0) {
    & $RANLIB $LibFile 2>&1
    Write-Host "Library created: $LibFile" -ForegroundColor Green
} else {
    Write-Host "Failed to create library" -ForegroundColor Red
    exit 1
}
