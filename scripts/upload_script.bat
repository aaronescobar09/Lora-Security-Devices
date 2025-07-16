@echo off
echo =========================================
echo T-Deck iPhone-Style UI Upload Script
echo =========================================
echo.

echo Building optimized firmware...
pio run -e T-Deck-fast
if %ERRORLEVEL% neq 0 (
    echo Build failed! Check the error messages above.
    pause
    exit /b 1
)

echo.
echo =========================================
echo Build completed successfully!
echo =========================================
echo.

echo Please connect your T-Deck device via USB and put it in download mode:
echo 1. Hold down the BOOT button
echo 2. Press and release the RESET button
echo 3. Release the BOOT button
echo 4. Press any key to continue...
pause > nul

echo.
echo Uploading firmware to T-Deck...
pio run -e T-Deck-fast --target upload
if %ERRORLEVEL% neq 0 (
    echo Upload failed! Check your connection and try again.
    pause
    exit /b 1
)

echo.
echo =========================================
echo Upload completed successfully!
echo =========================================
echo.
echo Your T-Deck now has the iPhone-style UI!
echo Press any key to exit...
pause > nul
