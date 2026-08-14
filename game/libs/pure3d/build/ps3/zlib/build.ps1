# zlib PS3 Build Script

$ErrorActionPreference = "Continue"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ZlibDir = (Resolve-Path "$ScriptDir\..\..\..\lib\zlib").Path
$ObjDir = "$ScriptDir\Debug"
$LibOutDir = (Resolve-Path "$ScriptDir\..\..\lib").Path

# Paths
$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"

$CC = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-gcc.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

Write-Host "Building zlib for PS3..."
Write-Host "Source: $ZlibDir"
Write-Host "Output: $ObjDir"

# Includes - need PS3 SDK for standard headers
$Includes = @(
    "-I$ZlibDir",
    "-I$SDKBase\target\ppu\include",
    "-I$SDKBase\target\common\include",
    "-I$ToolchainBase\target\ppu\include"
)

# Defines
$Defines = @(
    "-DRAD_PS3",
    "-D__CELLOS_LV2__"
)

# Flags - C compiler flags
$Flags = @(
    "-c",
    "-g",
    "-O2",
    "-Wall",
    "-Wno-unused"
)

# Source files needed for inflate (decompression)
$SourceFiles = @(
    "adler32",
    "crc32",
    "infblock",
    "infcodes",
    "inffast",
    "inflate",
    "inftrees",
    "infutil",
    "zutil"
)

# Create output directory
if (!(Test-Path $ObjDir)) {
    New-Item -ItemType Directory -Path $ObjDir | Out-Null
}

$ErrorCount = 0
$CompiledCount = 0

foreach ($src in $SourceFiles) {
    $srcFile = "$ZlibDir\$src.c"
    $objFile = "$ObjDir\$src.o"

    if (!(Test-Path $srcFile)) {
        Write-Host "MISSING: $srcFile" -ForegroundColor Yellow
        continue
    }

    # Skip if object file is newer than source
    if (Test-Path $objFile) {
        $srcTime = (Get-Item $srcFile).LastWriteTime
        $objTime = (Get-Item $objFile).LastWriteTime
        if ($objTime -gt $srcTime) {
            Write-Host "Skip: $src (up to date)"
            continue
        }
    }

    Write-Host "Compile: $src"

    $args = $Flags + $Defines + $Includes + @("-o", $objFile, $srcFile)
    $result = & $CC $args 2>&1

    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR compiling $src" -ForegroundColor Red
        Write-Host $result
        $ErrorCount++
    } else {
        $CompiledCount++
    }
}

Write-Host ""
Write-Host "Compilation: $CompiledCount compiled, $ErrorCount errors"

if ($ErrorCount -gt 0) {
    Write-Host "Build failed" -ForegroundColor Red
    exit 1
}

# Create library
Write-Host ""
Write-Host "Creating library..."

$LibFile = "$LibOutDir\zlibp3d.a"

$AllObjs = Get-ChildItem -Path $ObjDir -Filter "*.o" | ForEach-Object { $_.FullName }

Write-Host "Adding $($AllObjs.Count) object files to library"

& $AR cr $LibFile $AllObjs 2>&1
if ($LASTEXITCODE -eq 0) {
    & $RANLIB $LibFile 2>&1
    Write-Host "Library created: $LibFile" -ForegroundColor Green
} else {
    Write-Host "Failed to create library" -ForegroundColor Red
    exit 1
}
