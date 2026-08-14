# sim (physics/collision) PS3 Build Script

$ErrorActionPreference = "Continue"

# Base paths (absolute)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$SimDir = (Resolve-Path "$ScriptDir\..\..").Path
$LibsDir = (Resolve-Path "$SimDir\..").Path
$SourceDir = "$SimDir"
$ObjDir = "$ScriptDir\Debug"
$LibOutDir = "$SimDir\lib"
$IncludeFixDir = "$LibsDir\pure3d\build\ps3\include_fix"

# Paths
$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"

# Compiler
$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

Write-Host "Building sim for PS3"
Write-Host "Source directory: $SourceDir"
Write-Host ""

# Includes
$Includes = @(
    "-I$IncludeFixDir",
    "-I$SimDir",
    "-I$LibsDir\poser\inc",
    "-I$LibsDir\pure3d",
    "-I$LibsDir\radmath",
    "-I$LibsDir\radcontent\inc",
    "-I$LibsDir\radcore\inc",
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
    "-D__CELLOS_LV2__",
    "-DPNG_USER_MEM_SUPPORTED",
    "-DLZO_NO_SYS_TYPES_H"
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
    # Collision
    "simcollision/collision",
    "simcollision/collisionanalyser",
    "simcollision/collisionanalyserdata",
    "simcollision/collisionanalyserdataUID",
    "simcollision/collisionanalyserevent",
    "simcollision/collisionanalyserinfo",
    "simcollision/collisionanalysertranslator",
    "simcollision/collisiondetector",
    "simcollision/collisiondisplay",
    "simcollision/collisionmanager",
    "simcollision/collisionobject",
    "simcollision/collisionvolume",
    "simcollision/impulsebasedcollisionsolver",
    "simcollision/subcollisiondetector",
    "simcollision/proximitydetection",
    # Common
    "simcommon/arraymath",
    "simcommon/dline2",
    "simcommon/impulselink",
    "simcommon/kalmann",
    "simcommon/largesymmetricmatrix",
    "simcommon/physicsproperties",
    "simcommon/sbmatrix",
    "simcommon/simconstraint",
    "simcommon/simenvironment",
    "simcommon/simmath",
    "simcommon/simstate",
    "simcommon/simstatearticulated",
    "simcommon/simstateflexible",
    "simcommon/simstatetarget",
    "simcommon/simtarget",
    "simcommon/simulatedobject",
    "simcommon/simutility",
    "simcommon/skeletoninfo",
    "simcommon/symmatrix",
    "simcommon/tlist",
    "simcommon/trackerjointmodifier",
    "simcommon/trajectoryextrapolator",
    # Physics
    "simphysics/physicsobject",
    "simphysics/articulatedphysicsobject",
    "simphysics/physicsjoint",
    "simphysics/virtualcm",
    "simphysics/restingdetector"
)

# Create output directories
if (!(Test-Path $ObjDir)) {
    New-Item -ItemType Directory -Path $ObjDir | Out-Null
}

@("simcollision", "simcommon", "simphysics") | ForEach-Object {
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

$LibFile = "$LibOutDir\simp3d.a"

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
