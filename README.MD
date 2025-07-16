# LoRa-Security-Keypad

> **LoRa-based security system keypad** - T-Deck Plus central arming/disarming interface for distributed security networks

## 🛒 Hardware Variants

The T-Deck Plus comes in several frequency variants:
- **H737-A**: 915MHz with built-in antenna
- **H737-03**: 915MHz with external antenna
- **H736-A**: 868MHz with built-in antenna  
- **H736-03**: 868MHz with external antenna
- **H755-A**: 433MHz with external antenna

*Choose the appropriate frequency variant based on your region's regulations and range requirements.*

## 📋 Technical Specificationsce with advanced UI and communication capabilities.

**Developed by Aaron Escobar**

## 🎯 Project Overview

The **LoRa-Security-Keypad** transforms the LilyGO T-Deck Plus into a sophisticated **security system arming keypad** that controls a distributed LoRa network of field sensors and deterrent devices. This project provides the central control interface for wireless security installations requiring long-range, reliable connectivity.

**Based on T-Deck Plus:** Upgraded hardware with built-in GPS module and 2000mAh battery for portable security control.

## ✨ Key Features

### Hardware Support
- **ESP32-S3FN16R8** - Dual-core LX7 microprocessor, 240MHz
- **Memory** - 16MB Flash, 8MB PSRAM
- **2.8" IPS LCD Display** - ST7789 SPI Interface (320x240, full viewing angle)
- **Physical Keyboard** - ESP32-C3 based with trackball navigation
- **LoRa Communication** - SX1262 transceiver (+22dBm, 433/868/915MHz)
- **Connectivity** - 2.4GHz Wi-Fi & Bluetooth 5.0 (LE)
- **Audio System** - I2S microphone and built-in speaker
- **GPS Module** - Built-in positioning (L76K/UBlox-M10Q)
- **Storage** - MicroSD card slot
- **Battery** - Built-in 2000mAh rechargeable battery
- **Charging** - USB-C with battery monitoring (IO04)

### Software Features
- **Security Control Interface** with CWS branding and LVGL graphics
- **Distributed LoRa Network** - Central control of field sensors and deterrents
- **Arming/Disarming Interface** - Traditional keypad functionality with modern UI
- **Zone Monitoring** - Real-time status of distributed security sensors
- **Performance Optimized** - Ultra-smooth scrolling and responsive touch
- **Comprehensive Testing** - Built-in hardware diagnostics
- **Audio Feedback** - Voice prompts and security alerts
- **Real-time Communication** - LoRa coordination with field devices
- **Power Management** - Battery monitoring for reliable security operation
- **Development Support** - Arduino, PlatformIO, and MicroPython compatible

## 🚀 Quick Start

### Prerequisites
- [PlatformIO](https://platformio.org/) installed in VS Code
- [Git](https://git-scm.com/) for version control
- USB-C cable for T-Deck Plus connection
- T-Deck Plus device (available at [LilyGO Store](https://lilygo.cc/products/t-deck-plus-1))

### Environment Setup
1. Set WiFi credentials as environment variables:
   ```bash
   # Windows (PowerShell)
   $env:PIO_WIFI_SSID="YourWiFiName"
   $env:PIO_WIFI_PASSWORD="YourWiFiPassword"
   
   # Linux/Mac
   export PIO_WIFI_SSID="YourWiFiName"
   export PIO_WIFI_PASSWORD="YourWiFiPassword"
   ```

### Building and Flashing
1. Open project in VS Code with PlatformIO
2. Select the target environment:
   - `T-Deck` - Standard performance
   - `T-Deck-fast` - High performance (production)
   - `T-Deck-test` - Testing with Unity framework
3. Build and upload using PlatformIO toolbar

### Application Selection
Switch between applications by editing `platformio.ini`:
```ini
;; Applications - only one is valid
src_dir = applications/MainKeypad        ; Main security keypad interface
; src_dir = applications/Touchpad        ; Touch testing
; src_dir = applications/Microphone      ; Audio testing
; src_dir = applications/LoRaWAN_Starter ; LoRa communication testing
```

## 📁 Project Structure

```
LoRa-Security-Keypad/
├── applications/          # Security control applications
│   ├── MainKeypad/       # Main security keypad interface (active)
│   ├── Touchpad/         # Touch input testing
│   ├── Microphone/       # Audio testing
│   └── ...               # Other specialized security apps
├── boards/               # T-Deck board definitions
├── data/                 # Audio files and security assets
├── docs/                 # Documentation and guides
├── lib/                  # Required libraries
├── schematic/            # Hardware schematics
├── scripts/              # Build and upload utilities
├── shell/                # 3D models and enclosure
├── test/                 # Unit tests
└── platformio.ini        # Build configuration
```

## � Technical Specifications

| Component | Specification |
|-----------|---------------|
| **MCU** | ESP32-S3FN16R8 Dual-core LX7 microprocessor |
| **Flash Memory** | 16MB |
| **PSRAM** | 8MB |
| **Display** | 2.8" ST7789 SPI Interface IPS LCD (320x240, full viewing angle) |
| **Wireless** | 2.4 GHz Wi-Fi & Bluetooth 5.0 (LE) |
| **LoRa** | SX1262 transceiver, +22dBm, 433/868/915MHz |
| **Battery** | Built-in 2000mAh rechargeable (ADC PIN: IO04) |
| **Onboard Features** | GPS, MicroSD Card, Microphone, Speaker |
| **Connectivity** | USB-C charging/programming |
| **Development** | Arduino, PlatformIO-IDE, MicroPython compatible |

## �🔧 Configuration

### Performance Environments

#### T-Deck (Standard)
- Balanced performance and power consumption
- Full debugging enabled
- Suitable for development

#### T-Deck-fast (Production)
- Maximum performance optimizations
- Reduced debugging overhead
- Optimized for deployment

#### T-Deck-test (Testing)
- Unity test framework enabled
- Comprehensive testing capabilities
- Debug-friendly configuration

### Build Flags
The project includes extensive optimizations:
- **CPU**: ESP32-S3FN16R8 Dual-core LX7 at 240MHz maximum frequency
- **Memory**: 16MB Flash, 8MB PSRAM optimization enabled
- **Graphics**: ST7789 IPS display with 16-bit color depth for speed
- **Radio**: SX1262 LoRa (+22dBm) with unused modules excluded for size
- **Power**: 2000mAh battery monitoring and management
- **Debugging**: Configurable levels for development vs production

## 🎮 Usage

### Main Interface
1. **Power On** - Device shows CWS security system splash screen
2. **Navigation** - Use touchscreen or physical controls for system management
3. **Arming/Disarming** - Central security system control interface
4. **Zone Status** - Monitor distributed field sensors and deterrents
5. **Settings** - Configure LoRa network, display, and security features

### Physical Controls
- **Trackball** - Navigate security interface
- **Directional Keys** - Menu navigation and zone selection
- **Click Button** - Arm/disarm and activate functions
- **Keyboard** - Security code input and system commands

### Touch Interface
- **Tap** - Select zones and security functions
- **Swipe** - Scroll through security status lists
- **Long Press** - Access advanced security settings
- **Pinch** - Zoom on zone maps (where applicable)

## 📡 Communication

### WiFi
- Automatic connection on startup
- NTP time synchronization
- Web-based configuration

### LoRa Network
- **Central Command** - Coordinates distributed field sensors (T3-S3, T-Beam)
- **Long-range Communication** - SX1262 transceiver with enhanced range
- **Power Output**: +22dBm maximum transmission power
- **Frequency Options**: 433MHz/868MHz/915MHz (region dependent)
- **External Antenna**: Optional for enhanced coverage area
- **Field Sensor Network** - Real-time communication with security nodes
- **Zone Monitoring** - Status updates from perimeter sensors
- **Deterrent Control** - Remote activation of security devices

### Audio
- **Security Alerts** - Voice prompts and alarm notifications
- **System Status** - Audio feedback for arming/disarming
- **Zone Alerts** - Audio notifications for sensor triggers
- **Microphone Input** - Voice command processing (future feature)

## 🛠️ Development

### Adding New Applications
1. Create folder in `applications/`
2. Add `.ino` file with security control logic
3. Update `platformio.ini` with new path
4. Include required headers and security utilities

### Custom Hardware
- Modify `boards/T-Deck.json` for security hardware changes
- Update pin definitions in utilities.h for sensor interfaces
- Adjust build flags for security-specific optimizations

### Testing
```bash
# Run unit tests
pio test

# Performance testing
scripts/test_performance.bat

# Upload with monitoring
scripts/upload_script.ps1
```

## 📚 Documentation

- **[Performance Guide](docs/PERFORMANCE_GUIDE.md)** - Optimization techniques
- **[Performance Testing](docs/PERFORMANCE_TEST.md)** - Benchmarking procedures
- **[Upload Troubleshooting](docs/UPLOAD_TROUBLESHOOTING.md)** - Common issues and solutions
- **[Enhancements](docs/README_ENHANCEMENTS.md)** - Feature additions and improvements

## 🤝 Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open Pull Request

## 📄 License

This project is based on the LilyGO T-Deck platform. Please refer to original licensing terms.

## 🙏 Acknowledgments

- **[LilyGO®](https://lilygo.cc/)** - For the excellent T-Deck Plus hardware platform
- **[LVGL Team](https://lvgl.io/)** - For the powerful graphics library
- **[PlatformIO](https://platformio.org/)** - For the excellent development environment
- **[ESP32 Community](https://esp32.com/)** - For extensive documentation and support
- **[RadioLib](https://github.com/jgromes/RadioLib)** - For LoRa communication capabilities

## 📞 Support & Resources

- **Product Information**: [T-Deck Plus GitHub](https://github.com/Xinyuan-LilyGO/T-Deck)
- **Technical Support**: [LilyGO GitHub Community](https://github.com/Xinyuan-LilyGo)
- **Hardware Purchase**: [LilyGO Store](https://lilygo.cc/products/t-deck-plus-1)
- **Documentation**: [Project Docs](docs/)

---

**Built with ❤️ for distributed LoRa security applications**
