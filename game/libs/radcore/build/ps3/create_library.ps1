# Create radcorep3d.a library for PS3
$ErrorActionPreference = "Continue"

$ToolchainBase = "$env:PS3_SDK_ROOT\[134]-PS3_Toolchain_411-Win_340_001\cell"
$ObjDir = "C:\simpsonshnr\game\libs\radcore\build\ps3\Debug"
$LibDir = "C:\simpsonshnr\game\libs\radcore\lib"
$LibFile = "$LibDir\radcorep3d.a"

$AR = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ar.exe"
$RANLIB = "$ToolchainBase\host-win32\ppu\bin\ppu-lv2-ranlib.exe"

Write-Host "Creating library: $LibFile"

# Get all object files
$ObjFiles = Get-ChildItem "$ObjDir\*.o" | ForEach-Object { $_.FullName }
Write-Host "Adding $($ObjFiles.Count) object files to library"

# Remove old library if exists
if (Test-Path $LibFile) {
    Remove-Item $LibFile
}

# Create library
& $AR rcs $LibFile $ObjFiles 2>&1

if ($LASTEXITCODE -eq 0) {
    # Index the library
    & $RANLIB $LibFile 2>&1
    Write-Host "SUCCESS: Library created at $LibFile"
} else {
    Write-Host "FAILED with exit code $LASTEXITCODE"
}
