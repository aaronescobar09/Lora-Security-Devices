# T-Deck Upload Troubleshooting Guide

## If Upload Fails:

### 1. **Device Not Found / COM Port Issues**
- Check USB cable connection
- Try a different USB cable (data cable, not charging only)
- Check Device Manager for COM port number
- Run: `pio device list` to see available devices

### 2. **Download Mode Issues**
- Make sure T-Deck is in download mode:
  1. Hold BOOT button
  2. Press and release RESET button
  3. Release BOOT button
- Try the process again if it doesn't work the first time

### 3. **Build Errors**
- Run: `pio run -e T-Deck-fast` first to check compilation
- Check for any missing dependencies
- Use standard environment: `pio run -e T-Deck --target upload`

### 4. **Upload Timeout**
- Restart the T-Deck and try download mode again
- Check that no other programs are using the COM port
- Close Arduino IDE or other serial monitor programs

## Manual Upload Commands:

```bash
# 1. Build firmware
pio run -e T-Deck-fast

# 2. Put device in download mode (BOOT + RESET)

# 3. Upload
pio run -e T-Deck-fast --target upload

# Alternative: Standard build
pio run -e T-Deck --target upload
```

## After Successful Upload:
1. Press RESET button on T-Deck
2. Your iPhone-style UI should appear
3. Enjoy your modernized interface!
