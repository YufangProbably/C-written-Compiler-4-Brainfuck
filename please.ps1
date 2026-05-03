param(
    [string]$Work
)

function Build-DLL {
    New-Item -Force -ItemType Directory bin | Out-Null
    Remove-Item bin/cc4b.dll -Recurse -Force -ErrorAction SilentlyContinue

    $srcFiles = Get-ChildItem -Recurse src, lib -Filter *.c

    gcc $srcFiles.FullName -o bin/cc4b.dll `
        -I src -I lib -shared -D CC4B_EXPORT -O2
    return $LASTEXITCODE -eq 0
}

function Build-Main {
    New-Item -Force -ItemType Directory bin | Out-Null
    Remove-Item bin/cc4b.exe -Recurse -Force -ErrorAction SilentlyContinue

    $srcFiles = Get-ChildItem -Recurse main, lib -Filter *.c

    gcc $srcFiles.FullName -o bin/cc4b.exe `
        -I main -I src -I lib -L . -l bin/cc4b -O2
    return $LASTEXITCODE -eq 0
}

function Build-Test {
    New-Item -Force -ItemType Directory bin/test | Out-Null
    Remove-Item bin/test/* -Recurse -Force -ErrorAction SilentlyContinue

    $srcFiles = Get-ChildItem -Recurse src, lib -Filter *.c
    $testFiles = Get-ChildItem -Recurse test -Filter *.c

    $allSuccess = $true
    foreach ($testFile in $testFiles) {
        $output = "bin/test/$($testFile.BaseName).exe"
        gcc $testFile.FullName $srcFiles.FullName -o $output `
            -I test -I src -I lib `
            -D CC4B_STATIC `
            -D CC4B_DEBUG_DUMP
        if ($LASTEXITCODE -ne 0) { $allSuccess = $false }
    }
    return $allSuccess
}

function Run-Test {
    $tests = Get-ChildItem -Recurse bin/test -Filter *.exe

    $passedCount = 0
    $failedCount = 0
    foreach ($test in $tests) {
        & $test.FullName
        if ($LASTEXITCODE -eq 0) { $passedCount++ }
        else { $failedCount++ }
    }

    Write-Host "`n" -NoNewline
    if ($failedCount -eq 0) {
        Write-Host "All tests passed" -NoNewline -ForegroundColor Green
    } else {
        Write-Host "$passedCount passed" -NoNewline -ForegroundColor Green
        Write-Host ", " -NoNewline
        Write-Host "$failedCount failed" -NoNewline -ForegroundColor Red
    }
}

switch ($Work) {
    "build-dll" { Build-DLL | Out-Null; break }
    "build-main" { Build-Main | Out-Null; break }
    "build-test" { Build-Test | Out-Null; break }
    "run-test" { Run-Test; break }
}