# Compile zlib for PS3
$ErrorActionPreference = "Continue"

$PS3SDK = "$env:PS3_SDK_ROOT"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CC = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-gcc.exe"
$AR = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-ar.exe"

Set-Location "C:\simpsonshnr\game\libs\pure3d\build\ps3\zlib"

# Ensure Debug directory exists
if (!(Test-Path "Debug")) {
    New-Item -ItemType Directory -Path "Debug" | Out-Null
}

$ZLIB_SRC = "..\..\..\lib\zlib"

$commonArgs = @(
    "-c", "-g", "-O2",
    "-DRAD_PS3", "-D__CELLOS_LV2__",
    "-I$PS3_SDK_DIR\target\ppu\include",
    "-I$PS3_SDK_DIR\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I$ZLIB_SRC"
)

# Zlib source files (only the ones needed for inflate/decompress)
$zlibFiles = @(
    "adler32.c",
    "crc32.c",
    "inflate.c",
    "infblock.c",
    "infcodes.c",
    "inffast.c",
    "inftrees.c",
    "infutil.c",
    "zutil.c"
)

$allSuccess = $true
$objects = @()

foreach ($file in $zlibFiles) {
    $objName = [System.IO.Path]::GetFileNameWithoutExtension($file) + ".o"
    Write-Host "Compiling $file..."
    $args = $commonArgs + @("-o", "Debug\$objName", "$ZLIB_SRC\$file")
    & $CC @args

    if ($LASTEXITCODE -eq 0) {
        Write-Host "  OK" -ForegroundColor Green
        $objects += "Debug\$objName"
    } else {
        Write-Host "  FAILED" -ForegroundColor Red
        $allSuccess = $false
    }
}

if ($allSuccess) {
    Write-Host ""
    Write-Host "Creating zlib library archive..."
    $arArgs = @("rcs", "..\..\..\build\lib\zlibp3d.a") + $objects
    & $AR @arArgs

    if ($LASTEXITCODE -eq 0) {
        Write-Host "Library created: zlibp3d.a" -ForegroundColor Green
    } else {
        Write-Host "Archive creation failed!" -ForegroundColor Red
    }
} else {
    Write-Host ""
    Write-Host "Some files failed to compile!" -ForegroundColor Red
}
