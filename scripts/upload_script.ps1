# T-Deck iPhone-Style UI Upload Script
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "T-Deck iPhone-Style UI Upload Script" -ForegroundColor Yellow
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "Building optimized firmware..." -ForegroundColor Green
$buildResult = Start-Process -FilePath "pio" -ArgumentList "run", "-e", "T-Deck-fast" -Wait -PassThru -NoNewWindow

if ($buildResult.ExitCode -ne 0) {
    Write-Host "Build failed! Check the error messages above." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "Please connect your T-Deck device via USB and put it in download mode:" -ForegroundColor Yellow
Write-Host "1. Hold down the BOOT button" -ForegroundColor White
Write-Host "2. Press and release the RESET button" -ForegroundColor White
Write-Host "3. Release the BOOT button" -ForegroundColor White
Write-Host "4. Press any key to continue..." -ForegroundColor Yellow
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

Write-Host ""
Write-Host "Uploading firmware to T-Deck..." -ForegroundColor Green
$uploadResult = Start-Process -FilePath "pio" -ArgumentList "run", "-e", "T-Deck-fast", "--target", "upload" -Wait -PassThru -NoNewWindow

if ($uploadResult.ExitCode -ne 0) {
    Write-Host "Upload failed! Check your connection and try again." -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "Upload completed successfully!" -ForegroundColor Green
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Your T-Deck now has the iPhone-style UI!" -ForegroundColor Yellow
Read-Host "Press Enter to exit"
