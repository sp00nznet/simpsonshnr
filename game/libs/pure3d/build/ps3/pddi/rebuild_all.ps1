# Rebuild all modified PS3 PDDI files

$PS3SDK = "D:\PS3.Full.3.40.SDK.PS3-DUPLEX"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK_DIR = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"
$AR = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-ar.exe"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Pure3DDir = (Resolve-Path "$ScriptDir\..\..\..").Path
$LibsDir = (Resolve-Path "$Pure3DDir\..").Path

$commonArgs = @(
    "-c", "-g", "-O0", "-fno-exceptions", "-fpermissive",
    "-DRAD_PS3", "-DRAD_DEBUG", "-DRAD_CONSOLE", "-D__CELLOS_LV2__",
    "-I$PS3_SDK_DIR\target\ppu\include",
    "-I$PS3_SDK_DIR\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I$Pure3DDir",
    "-I$Pure3DDir\pddi",
    "-I$LibsDir\radmath",
    "-I$LibsDir\radcore\inc",
    "-I$LibsDir\..\code"
)

$files = @(
    @{src = "$Pure3DDir\pddi\ps3\ps3context.cpp"; obj = "$ScriptDir\Debug\ps3context.o"},
    @{src = "$Pure3DDir\pddi\ps3\ps3device.cpp"; obj = "$ScriptDir\Debug\ps3device.o"},
    @{src = "$Pure3DDir\pddi\ps3\ps3texture.cpp"; obj = "$ScriptDir\Debug\ps3texture.o"},
    @{src = "$Pure3DDir\pddi\ps3\ps3display.cpp"; obj = "$ScriptDir\Debug\ps3display.o"},
    @{src = "$Pure3DDir\pddi\ps3\ps3prim.cpp"; obj = "$ScriptDir\Debug\ps3prim.o"},
    @{src = "$Pure3DDir\pddi\ps3\ps3shader.cpp"; obj = "$ScriptDir\Debug\ps3shader.o"},
    @{src = "$Pure3DDir\pddi\ps3\ps3primstream.cpp"; obj = "$ScriptDir\Debug\ps3primstream.o"},
    @{src = "$Pure3DDir\pddi\base\basecontext.cpp"; obj = "$ScriptDir\Debug\basecontext.o"}
)

$allSuccess = $true

foreach ($file in $files) {
    Write-Host "Compiling $($file.src)..."
    $args = $commonArgs + @("-o", $file.obj, $file.src)
    & $CXX @args

    if ($LASTEXITCODE -eq 0) {
        Write-Host "  OK" -ForegroundColor Green
    } else {
        Write-Host "  FAILED" -ForegroundColor Red
        $allSuccess = $false
    }
}

if ($allSuccess) {
    Write-Host "`nUpdating library archive..."
    # Include all object files in Debug folder
    $allObjs = Get-ChildItem -Path "$ScriptDir\Debug" -Filter "*.o" | ForEach-Object { $_.FullName }
    & $AR rcs "$Pure3DDir\build\lib\pddip3d.a" @allObjs

    if ($LASTEXITCODE -eq 0) {
        Write-Host "Library updated successfully!" -ForegroundColor Green
    } else {
        Write-Host "Archive update failed!" -ForegroundColor Red
    }
} else {
    Write-Host "`nSome files failed to compile. Library not updated." -ForegroundColor Red
}
