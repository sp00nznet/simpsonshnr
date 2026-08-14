$files = Get-ChildItem -Path 'C:\simpsonshnr\game' -Recurse -Include '*.h','*.hpp','*.cpp'
$count = 0
foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if ($content -and $content -match '\bref\s*<') {
        $newContent = $content -replace '(?<!:)\bref<', '::ref<'
        $newContent = $newContent -replace '(?<!:)\bref\s+<', '::ref<'
        if ($content -ne $newContent) {
            Set-Content -Path $file.FullName -Value $newContent -NoNewline
            Write-Host $file.FullName
            $count++
        }
    }
}
Write-Host "Modified $count files"
