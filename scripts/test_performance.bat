@echo off
echo ========================================
echo T-Deck Performance Test Script
echo ========================================
echo.

echo Building optimized firmware...
pio run -e T-Deck

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ❌ Build failed! Please check the error messages above.
    pause
    exit /b 1
)

echo.
echo ✅ Build successful!
echo.
echo Performance optimizations active:
echo - CPU: 240MHz maximum frequency
echo - PSRAM: 80MHz high-speed mode
echo - Compiler: O3 maximum optimization
echo - LVGL: 16-bit color depth for speed
echo - FreeRTOS: 1000Hz tick rate
echo.

echo Checking firmware size...
pio run -e T-Deck --target size

echo.
echo ========================================
echo Upload Instructions:
echo ========================================
echo.
echo 1. Connect your T-Deck via USB-C
echo 2. Hold BOOT button and press RESET
echo 3. Release RESET, then release BOOT
echo 4. Run: pio run -e T-Deck --target upload
echo.
echo Alternative upload command:
echo pio run -e T-Deck-fast --target upload
echo (Uses additional performance optimizations)
echo.

echo ========================================
echo Performance Test Checklist:
echo ========================================
echo.
echo After uploading, test these scenarios:
echo.
echo ✓ Icon Click Test:
echo   - Navigate to home screen
echo   - Click different app icons rapidly
echo   - Should respond cleanly without sticking
echo.
echo ✓ Menu Navigation Test:
echo   - Switch between different menu pages
echo   - Transitions should be instant and smooth
echo.
echo ✓ Background Processing Test:
echo   - Enable GPS and LoRa radio
echo   - Navigate UI while they're active
echo   - UI should remain fully responsive
echo.
echo ✓ Transparency Slider Test:
echo   - Adjust icon transparency on home screen
echo   - Should update in real-time without lag
echo.

echo.
echo 📊 Expected improvements:
echo   - No more laggy screen responses
echo   - No more stuck icon clicks
echo   - Smooth ~200 FPS UI performance
echo   - Responsive UI during GPS/Radio operations
echo.

pause
