# scrooby (UI) PS3 Build Script

$ErrorActionPreference = "Continue"

# Base paths (absolute)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ScroobyDir = (Resolve-Path "$ScriptDir\..\..").Path
$LibsDir = (Resolve-Path "$ScroobyDir\..").Path
$SourceDir = "$ScroobyDir\src"
$ObjDir = "$ScriptDir\Debug"
$LibOutDir = "$ScroobyDir\lib"
$IncludeFixDir = "$LibsDir\pure3d\build\ps3\include_fix"

# Paths
$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"

# Compiler
$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

Write-Host "Building scrooby for PS3"
Write-Host "Source directory: $SourceDir"
Write-Host ""

# Includes
$Includes = @(
    "-I$IncludeFixDir",
    "-I$ScroobyDir\inc",
    "-I$ScroobyDir\src",
    "-I$LibsDir\radcore\inc",
    "-I$LibsDir\radcontent\inc",
    "-I$LibsDir\pure3d",
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
    "-Wno-non-virtual-dtor",
    "-Wno-shadow"
)

# Source files
$SourceFiles = @(
    "FeApp",
    "FeBoundedDrawable",
    "FeChunkHandler",
    "FeDrawable",
    "FeEntity",
    "FeGroup",
    "FeLanguage",
    "FeLayer",
    "FeLoaders",
    "FeMovie",
    "FeOwner",
    "FePage",
    "FeParent",
    "FePolygon",
    "FeProject",
    "FePure3dObject",
    "FeScreen",
    "FeSprite",
    "FeText",
    "FeTextBible",
    "FeTextStyle",
    "tLinearTable",
    "ResourceManager/FeResourceManager",
    "scrooby/App",
    "strings/pcstring",
    "strings/UnicodeString",
    "strings/stricmp",
    "utility/AsyncFileLoader",
    "utility/BufferReader",
    "utility/debugMessages",
    "utility/EnumConversion",
    "utility/Util",
    "xml/XMLParser",
    "xml/XMLTree"
)

# Create output directories
if (!(Test-Path $ObjDir)) {
    New-Item -ItemType Directory -Path $ObjDir | Out-Null
}

@("ResourceManager", "scrooby", "strings", "utility", "xml") | ForEach-Object {
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

$LibFile = "$LibOutDir\scroobyp3d.a"

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
