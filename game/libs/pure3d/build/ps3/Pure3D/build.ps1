# Pure3D PS3 Build Script

$ErrorActionPreference = "Continue"

# Base paths (absolute)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Pure3DDir = (Resolve-Path "$ScriptDir\..\..\..").Path  # C:\simpsonshnr\game\libs\pure3d
$LibsDir = (Resolve-Path "$Pure3DDir\..").Path          # C:\simpsonshnr\game\libs
$SourceDir = "$Pure3DDir\p3d"
$ObjDir = "$ScriptDir\Debug"
$LibOutDir = "$Pure3DDir\build\lib"

# Paths
$ToolchainBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[134]-PS3_Toolchain_411-Win_340_001\cell"
$SDKBase = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX\[132]-PS3_SDK-340_001\cell"

# Compiler
$CXX = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

Write-Host "Build directory: $ScriptDir"
Write-Host "Source directory: $SourceDir"
Write-Host "Output directory: $ObjDir"
Write-Host ""

# Includes - include_fix directory first to intercept problematic SDK headers
$IncludeFixDir = "$Pure3DDir\build\ps3\include_fix"
$Includes = @(
    "-I$IncludeFixDir",
    "-I$LibsDir",
    "-I$LibsDir\..\code",
    "-I$LibsDir\radcore\inc",
    "-I$LibsDir\radcontent\inc",
    "-I$LibsDir\radmath",
    "-I$LibsDir\ps3sdkstubs",
    "-I$Pure3DDir",
    "-I$Pure3DDir\lib\libpng",
    "-I$Pure3DDir\lib\zlib",
    "-I$Pure3DDir\pddi",
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
    "platform/ps3/plat_filemap",
    "platform/ps3/platform",
    "ambientlight",
    "billboardobject",
    "bmp",
    "camera",
    "cameraloader",
    "chunkfile",
    "context",
    "debugdraw",
    "directionallight",
    "displaylist",
    "drawable",
    "dxtn",
    "entity",
    "error",
    "file",
    "fileftt",
    "font",
    "gameattr",
    "geometry",
    "image",
    "imageconverter",
    "imagefactory",
    "imagefont",
    "inventory",
    "light",
    "lightloader",
    "lightschooser",
    "loaders",
    "locator",
    "loadmanager",
    "lzr",
    "matrixstack",
    "memory",
    "memorysection",
    "memheap",
    "mipmapfilter",
    "pointcamera",
    "pointlight",
    "png",
    "primgroup",
    "rawimage",
    "refcounted",
    "shader",
    "sprite",
    "spotlight",
    "table",
    "targa",
    "textdataparser",
    "textstring",
    "texture",
    "texturefont",
    "unicode",
    "utility",
    "vectorcamera",
    "vertexlist",
    "view",
    "anim/animate",
    "anim/animatedobject",
    "anim/billboardobjectanimation",
    "anim/cameraanimation",
    "anim/channel",
    "anim/compositedrawable",
    "anim/drawablepose",
    "anim/event",
    "anim/eventanimation",
    "anim/expressionoffsets",
    "anim/expression",
    "anim/expressionanimation",
    "anim/lightanimation",
    "anim/multicontroller",
    "anim/polyskin",
    "anim/pose",
    "anim/poseanimation",
    "anim/sequencer",
    "anim/skeleton",
    "anim/shaderanimation",
    "anim/textureanimation",
    "anim/vertexoffsetexpression",
    "anim/visibilityanimation",
    "anim/vertexanimcontroller",
    "anim/vertexanimkey",
    "anim/vertexanimobject",
    "effects/effect",
    "effects/optic",
    "effects/opticlensflare",
    "effects/opticloader",
    "effects/particlepool",
    "effects/particlearray",
    "effects/particleemitter",
    "effects/particlegenerator",
    "effects/particleloader",
    "effects/particlesystem",
    "effects/particletype",
    "effects/particleutility",
    "effects/transitionmanager",
    "scenegraph/nodeanimation",
    "scenegraph/scenegraph"
)

# Create output directories
if (!(Test-Path $ObjDir)) {
    New-Item -ItemType Directory -Path $ObjDir | Out-Null
}

@("anim", "effects", "scenegraph", "platform", "platform/ps3") | ForEach-Object {
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

$LibFile = "$LibOutDir\pure3dp3d.a"

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
