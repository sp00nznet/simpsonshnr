$src = 'D:\emu\rpcs3-v0.0.26-14568-1852b370_win64\log\tty.log'
$dst = 'C:\simpsonshnr\tty_crash.log'
try {
    $fs = [System.IO.File]::Open($src, 'Open', 'Read', 'ReadWrite')
    $buffer = New-Object byte[] $fs.Length
    $fs.Read($buffer, 0, $fs.Length) | Out-Null
    $fs.Close()
    [System.IO.File]::WriteAllBytes($dst, $buffer)
    Write-Host "Copied $($buffer.Length) bytes to $dst"
} catch {
    Write-Host "Error: $_"
}
