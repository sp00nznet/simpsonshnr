# Compile one game-code unity .cpp for PS3.
#   .\compile_unity.ps1 ..\..\code\presentation\gui\allgui.cpp
# Output object goes to Debug\<basename>.o, which link_only.ps1 picks up.
param(
    [Parameter(Mandatory = $true)]
    [string]$Source
)

$ErrorActionPreference = "Continue"

if ( -not $env:PS3_SDK_ROOT ) {
    Write-Host "PS3_SDK_ROOT is not set. Point it at the root of your PS3 SDK 3.40" -ForegroundColor Red
    Write-Host "install -- the folder holding [132]-PS3_SDK-340_001 and" -ForegroundColor Red
    Write-Host "[134]-PS3_Toolchain_411-Win_340_001. See the PS3 section of the README." -ForegroundColor Red
    exit 1
}


$PS3SDK = "$env:PS3_SDK_ROOT"
$PS3_TOOLCHAIN = "$PS3SDK\[134]-PS3_Toolchain_411-Win_340_001\cell"
$PS3_SDK = "$PS3SDK\[132]-PS3_SDK-340_001\cell"
$CXX = "$PS3_TOOLCHAIN\host-win32\ppu\bin\ppu-lv2-g++.exe"

Set-Location $PSScriptRoot

$INCLUDES = @(
    "-I$PS3_SDK\target\ppu\include",
    "-I$PS3_SDK\target\common\include",
    "-I$PS3_TOOLCHAIN\target\ppu\include",
    "-I..\..\code",
    "-I..\..\libs\radcore\inc",
    "-I..\..\libs\radmath",
    "-I..\..\libs\radsound\inc",
    "-I..\..\libs\radmusic\inc",
    "-I..\..\libs\radmovie\inc",
    "-I..\..\libs\radscript\inc",
    "-I..\..\libs\radcontent\inc",
    "-I..\..\libs\pure3d",
    "-I..\..\libs\pure3d\p3d",
    "-I..\..\libs\pure3d\pddi",
    "-I..\..\libs\sim",
    "-I..\..\libs\sim\simcommon",
    "-I..\..\libs\sim\simcollision",
    "-I..\..\libs\sim\simphysics",
    "-I..\..\libs\choreo\inc",
    "-I..\..\libs\poser\inc",
    "-I..\..\libs\scrooby\inc",
    "-I..\..\libs\scrooby\src",
    "-I..\..\libs\ps3sdkstubs"
) -join " "

$CXXFLAGS = "-c -g -O0 -fno-exceptions -fpermissive -DRAD_PS3 -DRAD_DEBUG -D_DEBUG -DRAD_CONSOLE -D__CELLOS_LV2__"

if (!(Test-Path $Source)) {
    Write-Host "No such source: $Source" -ForegroundColor Red
    exit 1
}

$outPath = "Debug\" + [System.IO.Path]::GetFileNameWithoutExtension($Source) + ".o"

Write-Host "Compiling $Source -> $outPath"
$process = Start-Process -FilePath $CXX -ArgumentList "$CXXFLAGS $INCLUDES -o $outPath $Source" `
    -NoNewWindow -Wait -PassThru -RedirectStandardError "error.txt"
if ($process.ExitCode -ne 0) {
    Write-Host "FAILED!" -ForegroundColor Red
    Get-Content "error.txt"
    exit 1
}
Write-Host "SUCCESS: $outPath" -ForegroundColor Green
exit 0
