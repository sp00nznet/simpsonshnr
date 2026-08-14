# Master PS3 Build Script - Build all game libraries

$ErrorActionPreference = "Continue"

# Base paths
$LibsDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"
$IncludeFixDir = "$LibsDir\pure3d\build\ps3\include_fix"

# Compiler
$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

# Common defines and flags
$CommonDefines = @(
    "-DRAD_PS3",
    "-DPLAT_PS3",
    "-DRAD_CONSOLE",
    "-DRAD_DEBUG",
    "-D_DEBUG",
    "-D__CELLOS_LV2__"
)

$CommonFlags = @(
    "-c",
    "-g",
    "-O0",
    "-fno-exceptions",
    "-fpermissive",
    "-Wall",
    "-Wno-unused",
    "-Wno-non-virtual-dtor"
)

$CommonIncludes = @(
    "-I$IncludeFixDir",
    "-I$SDKBase\target\ppu\include",
    "-I$SDKBase\target\common\include",
    "-I$ToolchainBase\target\ppu\include"
)

# Function to build a library
function Build-Library {
    param(
        [string]$Name,
        [string]$SourceDir,
        [string]$OutputLib,
        [string[]]$SourceFiles,
        [string[]]$ExtraIncludes = @(),
        [string[]]$ExtraDefines = @()
    )

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Building $Name" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan

    $ObjDir = "$LibsDir\$Name\build\ps3\Debug"
    $LibOutDir = Split-Path $OutputLib

    # Create directories
    if (!(Test-Path $ObjDir)) {
        New-Item -ItemType Directory -Path $ObjDir -Force | Out-Null
    }
    if (!(Test-Path $LibOutDir)) {
        New-Item -ItemType Directory -Path $LibOutDir -Force | Out-Null
    }

    $Includes = $CommonIncludes + $ExtraIncludes
    $Defines = $CommonDefines + $ExtraDefines

    $ErrorCount = 0
    $CompiledCount = 0

    foreach ($src in $SourceFiles) {
        $srcFile = "$SourceDir\$src.cpp"
        $objName = ($src -replace '/', '\')
        $objFile = "$ObjDir\$objName.o"

        # Ensure output directory exists
        $objSubDir = Split-Path $objFile
        if ($objSubDir -and !(Test-Path $objSubDir)) {
            New-Item -ItemType Directory -Path $objSubDir -Force | Out-Null
        }

        if (!(Test-Path $srcFile)) {
            Write-Host "  MISSING: $srcFile" -ForegroundColor Yellow
            continue
        }

        # Skip if up to date
        if (Test-Path $objFile) {
            $srcTime = (Get-Item $srcFile).LastWriteTime
            $objTime = (Get-Item $objFile).LastWriteTime
            if ($objTime -gt $srcTime) { continue }
        }

        Write-Host "  Compile: $src"

        $args = $CommonFlags + $Defines + $Includes + @("-o", $objFile, $srcFile)
        $result = & $CXX $args 2>&1

        if ($LASTEXITCODE -ne 0) {
            Write-Host "  ERROR: $src" -ForegroundColor Red
            $result | Write-Host
            $ErrorCount++
        } else {
            $CompiledCount++
        }
    }

    if ($ErrorCount -gt 0) {
        Write-Host "  Build failed: $ErrorCount errors" -ForegroundColor Red
        return $false
    }

    # Create library
    $AllObjs = Get-ChildItem -Path $ObjDir -Filter "*.o" -Recurse | ForEach-Object { $_.FullName }
    if ($AllObjs.Count -gt 0) {
        & $AR cr $OutputLib $AllObjs 2>&1 | Out-Null
        & $RANLIB $OutputLib 2>&1 | Out-Null
        Write-Host "  Library created: $OutputLib ($($AllObjs.Count) objects)" -ForegroundColor Green
    }

    return $true
}

# Build poser library
$success = Build-Library -Name "poser" `
    -SourceDir "$LibsDir\poser\src" `
    -OutputLib "$LibsDir\poser\lib\poserp3d.a" `
    -SourceFiles @("joint", "pose", "posedriver", "poseengine", "transform") `
    -ExtraIncludes @(
        "-I$LibsDir\poser\inc",
        "-I$LibsDir\pure3d",
        "-I$LibsDir\radmath",
        "-I$LibsDir\radcore\inc",
        "-I$LibsDir\radcontent\inc",
        "-I$LibsDir\..\code"
    )

# Build scrooby (UI) library
if (Test-Path "$LibsDir\scrooby\build\ps2\files.txt") {
    # Read scrooby source files from PS2 config
    $scroobyFiles = @(
        "App",
        "BoundedDrawable",
        "Drawable",
        "Group",
        "Layer",
        "Movie",
        "OwnerDrawable",
        "Page",
        "Polygon",
        "Project",
        "Pure3dObject",
        "ResourceManager",
        "Screen",
        "Sprite",
        "Text",
        "Utility/Enums",
        "Utility/memory",
        "Utility/rVector"
    )

    $success = Build-Library -Name "scrooby" `
        -SourceDir "$LibsDir\scrooby\src" `
        -OutputLib "$LibsDir\scrooby\lib\scroobyp3d.a" `
        -SourceFiles $scroobyFiles `
        -ExtraIncludes @(
            "-I$LibsDir\scrooby\inc",
            "-I$LibsDir\pure3d",
            "-I$LibsDir\pure3d\pddi",
            "-I$LibsDir\radmath",
            "-I$LibsDir\radcore\inc",
            "-I$LibsDir\radcontent\inc",
            "-I$LibsDir\..\code"
        )
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# List built libraries
Get-ChildItem "$LibsDir\*\lib\*p3d.a" -ErrorAction SilentlyContinue | ForEach-Object {
    Write-Host "  $_" -ForegroundColor Green
}
