# T-Deck Unit Tests

This directory contains unit tests for the T-Deck project using the Unity testing framework.

## Test Structure

- `test_main.cpp` - Main test runner with basic functionality tests
- `test_gps.cpp` - GPS-related functionality tests
- `test_radio.cpp` - Radio/LoRa functionality tests  
- `test_ui.cpp` - User interface and LVGL tests
- `test_system.cpp` - System and hardware abstraction tests

## Running Tests

### Method 1: Using PlatformIO CLI
```bash
# Run all tests
pio test

# Run tests for specific environment
pio test -e T-Deck-test

# Run specific test file
pio test -f test_gps

# Run tests with verbose output
pio test -v
```

### Method 2: Using VS Code
1. Open the PlatformIO panel in VS Code
2. Navigate to "PROJECT TASKS" > "T-Deck-test" > "Advanced" > "Test"
3. Click on "Test" to run all tests

### Method 3: Using PlatformIO IDE
1. Open PlatformIO IDE
2. Select the "T-Deck-test" environment
3. Click on the "Test" button in the toolbar

## Test Categories

### GPS Tests (`test_gps.cpp`)
- Coordinate validation (latitude/longitude ranges)
- Time validation (hours, minutes, seconds)
- Date validation (year, month, day)
- Speed validation (non-negative values)

### Radio Tests (`test_radio.cpp`)
- Frequency validation (433-923 MHz range)
- Bandwidth validation (125/250/500 KHz)
- Power level validation (2-22 dBm)
- Transmission interval validation
- Message length validation

### UI Tests (`test_ui.cpp`)
- Color hex value validation
- Percentage value validation (0-100%)
- Opacity validation (0-255)
- Brightness validation (screen: 1-16, keyboard: 0-255)
- Grid layout validation
- Icon sizing validation

### System Tests (`test_system.cpp`)
- Memory allocation tests
- String operation tests
- Math operation tests
- Array operation tests
- Time function tests

## Adding New Tests

1. Create a new `.cpp` file in the `test/` directory
2. Include the Unity framework: `#include <unity.h>`
3. Write test functions using Unity assertions:
   - `TEST_ASSERT_TRUE(condition)`
   - `TEST_ASSERT_EQUAL(expected, actual)`
   - `TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)`
   - `TEST_ASSERT_NOT_NULL(pointer)`
   - `TEST_ASSERT_EQUAL_STRING(expected, actual)`

4. Add your test functions to the main test runner if needed

## Unity Framework Documentation

For more information about Unity testing framework:
- [Unity Documentation](https://github.com/ThrowTheSwitch/Unity)
- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)

## Troubleshooting

### Common Issues

1. **Tests not found**: Ensure test files are in the `test/` directory and have `.cpp` extension
2. **Compilation errors**: Check that all required headers are included
3. **Unity framework not found**: Ensure PlatformIO can access the Unity framework

### Environment Variables

Make sure you have the following environment variables set if using WiFi tests:
- `PIO_WIFI_SSID` - Your WiFi network name
- `PIO_WIFI_PASSWORD` - Your WiFi password

## Test Results

Tests will output results in the serial monitor showing:
- Number of tests run
- Number of failures
- Detailed failure information if any tests fail
- Overall PASS/FAIL status
