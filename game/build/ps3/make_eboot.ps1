Set-Location $PSScriptRoot

$makeFself = Join-Path "$env:PS3_SDK_ROOT" "[132]-PS3_SDK-340_001\cell\host-win32\bin\make_fself.exe"

$elfPath = "..\..\cd\ps3\srr2p3d.elf"
$ebootPath = "..\..\cd\ps3\SIMP00001\PS3_GAME\USRDIR\EBOOT.BIN"

if (Test-Path -LiteralPath $makeFself) {
    Write-Host "Running make_fself..."
    & $makeFself $elfPath $ebootPath 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "EBOOT.BIN created successfully via make_fself!" -ForegroundColor Green
    } else {
        Write-Host "make_fself returned $LASTEXITCODE, falling back to copy" -ForegroundColor Yellow
        Copy-Item -LiteralPath $elfPath -Destination $ebootPath -Force
    }
} else {
    Write-Host "make_fself not found at: $makeFself" -ForegroundColor Yellow
    Copy-Item -LiteralPath $elfPath -Destination $ebootPath -Force
    Write-Host "Copied ELF as EBOOT.BIN" -ForegroundColor Green
}

$eboot = Get-Item -LiteralPath $ebootPath
Write-Host "Output: $($eboot.FullName)"
Write-Host "Size: $([math]::Round($eboot.Length / 1MB, 1)) MB"
Write-Host "Modified: $($eboot.LastWriteTime)"
