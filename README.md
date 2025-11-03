# Chas Advance IoT System

## Table of Contents
- [Quick Start](#quick-start)
- [System Overview](#system-overview)
- [Hardware Specification](#hardware-specification)
- [Project Structure](#project-structure)
- [Installation and Setup](#installation-and-setup)
- [Build and Upload](#build-and-upload)
- [API Documentation](#api-documentation)
- [Unit Testing](#unit-testing)
- [Communication Protocol](#communication-protocol)
- [Temperature Modes](#temperature-modes)
- [Features](#features)
- [Development](#development)
- [Troubleshooting](#troubleshooting)
- [Performance Data](#performance-data)
- [Contributing](#contributing)
- [License](#license)

## Quick Start

1. **Clone repository**:
   ```bash
   git clone https://github.com/ChasAcademy-Team-9/Embedded.git
   cd "Chas Advance"
   ```

2. **Create secrets files** (examples provided):
   ```bash
   # Copy example files and edit with your credentials
   cp "Chas Advance Arduino/include/ARDUINOSECRETS.h.example" "Chas Advance Arduino/include/ARDUINOSECRETS.h"
   cp "Chas Advance ESP32/include/ESPSECRETS.h.example" "Chas Advance ESP32/include/ESPSECRETS.h"
   # Edit the files above with your WiFi credentials (DO NOT commit secrets)
   ```

3. **Build and upload**:
   ```bash
   # Arduino
   cd "Chas Advance Arduino"
   platformio run -e uno_r4_wifi --target upload
   
   # ESP32
   cd "../Chas Advance ESP32"
   platformio run -e esp32-s3-devkitc-1 --target upload
   ```

4. **Run tests**:
   ```bash
   # Arduino tests (on hardware)
   cd "Chas Advance Arduino"
   platformio test -e uno_r4_wifi_test
   
   # ESP32 tests (native)
   cd "../Chas Advance ESP32"
   platformio test -e native
   ```

## System Overview

This project implements a complete IoT system for temperature and humidity measurement with two main components:

- **Arduino Uno R4 WiFi**: Sensor reading, data processing and WiFi communication
- **ESP32-S3**: Data transfer, batch handling and server functionality

The system includes advanced error handling, batch processing, flash storage and comprehensive unit testing.

## System Architecture

```
[DHT Sensor] → [Arduino Uno R4 WiFi] → [WiFi] → [ESP32-S3] → [Server/Cloud]
                      ↓                              ↓
                [Flash Storage]                [Batch Processing]
                [Unit Tests (24)]             [Unit Tests (22)]
```

### Data Flow
1. **Sensor Reading**: Arduino reads temperature and humidity from DHT sensor
2. **Data Validation**: Check against thresholds for different modes (Room/Cooler/Freezer)
3. **Batch Processing**: Median calculation for noise filtering
4. **WiFi Transfer**: JSON data sent to ESP32 via HTTP POST
5. **Reception**: ESP32 receives, processes and forwards data
6. **Error Handling**: Flash storage during connection problems

## Hardware Specification

### Arduino Uno R4 WiFi
- **Processor**: Renesas RA4M1 (48MHz, 32KB RAM, 256KB Flash)
- **Connectivity**: WiFi 802.11 b/g/n (2.4 GHz)
- **Sensors**: DHT11/DHT22 (Temperature & Humidity)
- **Features**: Real-time clock, flash storage, batch processing

### ESP32-S3 (Adafruit Feather)
- **Processor**: Xtensa dual-core 32-bit LX7
- **Connectivity**: WiFi 802.11 b/g/n, Bluetooth 5
- **Features**: Access Point, HTTP server, batch handling
- **Storage**: LittleFS for persistent data

### Sensor Connections

| Component | Arduino Pin | Voltage | Description |
|-----------|-------------|---------|-------------|
| DHT22     | Pin 8       | 3.3V    | Temperature & Humidity |
| Power LED | Pin 13      | 3.3V    | Status indicator |

## Project Structure

```
Chas Advance/
├── README.md                          # This file
├── PULL_REQUEST_TEMPLATE.md           # PR template
├── lib/                               # Shared libraries
│   └── logging/                       # Common logging functionality
├── Chas Advance Arduino/              # Arduino project
│   ├── src/                           # Source code
│   │   ├── main.cpp                   # Main program
│   │   ├── batchHandler.cpp           # Batch processing and median calculation
│   │   ├── wifiHandler.cpp            # WiFi handling and HTTP communication
│   │   ├── mockSensor.cpp             # Mock data for development
│   │   ├── sensorData.cpp             # Sensor validation and thresholds
│   │   ├── thresholds.cpp             # Temperature modes (Room/Cooler/Freezer)
│   │   ├── arduinoLogger.cpp          # Flash logging and data storage
│   │   └── log.cpp                    # Logging functions
│   ├── include/                       # Header files
│   │   ├── ARDUINOSECRETS.h           # WiFi settings (gitignored)
│   │   └── [other headers]
│   ├── test/                          # Unit tests (24 tests)
│   │   ├── test_main.cpp              # Test runner
│   │   ├── test_batch_handler.cpp     # Batch processing tests
│   │   ├── test_sensor_data.cpp       # Sensor validation tests
│   │   ├── test_thresholds.cpp        # Threshold tests
│   │   ├── test_mock_sensor.cpp       # Mock sensor tests
│   │   └── test_flash_functionality.cpp # Flash storage tests
│   ├── README_TESTING.md              # Testing guide
│   ├── TEST_INSTRUCTIONS.md           # Test instructions
│   ├── TESTING_SUMMARY.md             # Test summary
│   └── platformio.ini                 # Project configuration
└── Chas Advance ESP32/                # ESP32 project
    ├── src/                           # Source code
    │   ├── main.cpp                   # Main program
    │   ├── wifiHandler.cpp            # WiFi access point
    │   ├── batchNetworkHandler.cpp    # HTTP server and data handling
    │   ├── batchProcessor.cpp         # Batch processing
    │   ├── jsonParser.cpp             # JSON parsing and serialization
    │   ├── espLogger.cpp              # ESP32 logging system
    │   └── mockJson.cpp               # Mock JSON data
    ├── include/                       # Header files
    │   ├── ESPSECRETS.h               # WiFi settings (gitignored)
    │   └── [other headers]
    ├── test/                          # Unit tests (22 tests)
    │   ├── test_main.cpp              # Test runner
    │   ├── test_json_parser.cpp       # JSON parsing tests
    │   ├── test_sensor_data_handler.cpp # Sensor data tests
    │   ├── test_mock_json.cpp         # Mock JSON tests
    │   ├── test_log_functions.cpp     # Logging tests
    │   └── test_error_handling.cpp    # Error handling tests
    ├── lib/                           # Local libraries
    │   └── ArduinoMock/               # Arduino mock for native testing
    ├── simulate_clients.py            # Python script for simulation
    └── platformio.ini                 # Project configuration
```

## Installation and Setup

### Requirements
- **PlatformIO**: `pip install platformio`
- **VS Code** with PlatformIO Extension (recommended)
- **Arduino Uno R4 WiFi** and **ESP32-S3** hardware

### 1. WiFi Configuration

Create secrets files from the provided examples and edit with your credentials:

#### Arduino (ARDUINOSECRETS.h)
```bash
cp "Chas Advance Arduino/include/ARDUINOSECRETS.h.example" "Chas Advance Arduino/include/ARDUINOSECRETS.h"
```

Edit the file with your ESP32 Access Point credentials:
```cpp
#ifndef ARDUINOSECRETS_H
#define ARDUINOSECRETS_H

// Replace with your ESP32 AP credentials. DO NOT commit this file to git.
const char ssid[] = "YOUR_ESP32_AP_NAME";
const char password[] = "YOUR_ESP32_AP_PASSWORD";
const char *host = "192.168.4.1";          // ESP32 IP (default)
const int port = 80;

#endif
```

#### ESP32 (ESPSECRETS.h)
```bash
cp "Chas Advance ESP32/include/ESPSECRETS.h.example" "Chas Advance ESP32/include/ESPSECRETS.h"
```

Edit the file with your WiFi Access Point settings:
```cpp
#ifndef ESPSECRETS_H
#define ESPSECRETS_H

// Replace with your desired AP credentials. DO NOT commit this file to git.
#define WIFI_SSID "YOUR_ESP32_AP_NAME"
#define WIFI_PASS "YOUR_ESP32_AP_PASSWORD"

#endif
```

**Note**: The secrets files are listed in `.gitignore` and should never be committed to version control.

### 2. Build and Upload

#### Arduino
```bash
cd "Chas Advance Arduino"

# Build for production
platformio run -e uno_r4_wifi

# Upload to Arduino
platformio run -e uno_r4_wifi --target upload

# Monitor serial output
platformio device monitor
```

#### ESP32
```bash
cd "Chas Advance ESP32"

# Build for ESP32-S3
platformio run -e esp32-s3-devkitc-1

# Upload to ESP32
platformio run -e esp32-s3-devkitc-1 --target upload

# Monitor serial output
platformio device monitor
```

## API Documentation

### HTTP Endpoints

The ESP32 runs an HTTP server on port 80 with the following endpoints:

| Method | Endpoint | Description | Content-Type |
|--------|----------|-------------|--------------|
| POST   | `/data`  | Receive single sensor reading | application/json |
| POST   | `/batch` | Receive batch sensor data | application/json |
| GET    | `/status`| Get system status | - |

### Example API Usage

#### Send single sensor reading:
```bash
curl -X POST http://192.168.4.1/data \
  -H "Content-Type: application/json" \
  -d '{
    "ArduinoID": 1,
    "SensorTimeStamp": "2024-11-03T14:30:00Z",
    "Temperature": 22.5,
    "Humidity": 45.0,
    "ErrorType": 0
  }'
```

#### Send batch data:
```bash
curl -X POST http://192.168.4.1/batch \
  -H "Content-Type: application/json" \
  -d '[
    {
      "ArduinoID": 1,
      "SensorTimeStamp": "2024-11-03T14:30:00Z",
      "Temperature": 22.5,
      "Humidity": 45.0,
      "ErrorType": 0
    }
  ]'
```

### HTTP Response Codes

| Code | Description | When |
|------|-------------|------|
| 200  | OK | Data received and processed successfully |
| 400  | Bad Request | Invalid JSON format or missing required fields |
| 422  | Unprocessable Entity | Valid JSON but sensor data contains errors (ErrorType != 0) |
| 500  | Internal Server Error | ESP32 processing error |

## Unit Testing

### Arduino Tests (24 tests)
```bash
cd "Chas Advance Arduino"
platformio test -e uno_r4_wifi_test
```

**Test Categories:**
- **Batch Handler** (6 tests): Median calculation, error handling
- **Sensor Data** (7 tests): Validation against thresholds
- **Thresholds** (3 tests): Temperature modes (Room/Cooler/Freezer)
- **Mock Sensor** (3 tests): Mock data generation
- **Flash Functionality** (5 tests): Flash storage and retransmission

### ESP32 Tests (22 tests - Native)
```bash
cd "Chas Advance ESP32"
platformio test -e native
```

**Test Categories:**
- **JSON Parser** (6 tests): JSON serialization and parsing
- **Sensor Data Handler** (4 tests): Median calculation
- **Mock JSON** (3 tests): Mock data validation
- **Log Functions** (3 tests): Timestamp and logging
- **Error Handling** (6 tests): Error handling scenarios

## Communication Protocol

### WiFi Architecture
1. **ESP32** starts as Access Point (`ESP32 AccessPoint`)
2. **Arduino** connects to ESP32 AP
3. **HTTP POST** used for data transfer (port 80)

### JSON Format

All timestamps use **ISO 8601 format with UTC timezone** (YYYY-MM-DDTHH:MM:SSZ).

#### Single sensor reading
```json
{
  "ArduinoID": 1,
  "SensorTimeStamp": "2024-11-03T14:30:00Z",
  "Temperature": 22.5,
  "Humidity": 45.0,
  "ErrorType": 0
}
```

#### Batch data
```json
[
  {
    "ArduinoID": 1,
    "SensorTimeStamp": "2024-11-03T14:30:00Z",
    "Temperature": 22.5,
    "Humidity": 45.0,
    "ErrorType": 0
  },
  {
    "ArduinoID": 1,
    "SensorTimeStamp": "2024-11-03T14:31:00Z",
    "Temperature": 23.1,
    "Humidity": 46.2,
    "ErrorType": 0
  }
]
```

#### Field Descriptions

| Field | Type | Description | Required |
|-------|------|-------------|----------|
| ArduinoID | integer | Unique identifier for Arduino device | Yes |
| SensorTimeStamp | string | ISO 8601 timestamp in UTC | Yes |
| Temperature | float | Temperature reading in Celsius | Yes |
| Humidity | float | Humidity reading in percentage | Yes |
| ErrorType | integer | Error code (see Error Types below) | Yes |

### Error Types
```cpp
enum ErrorType {
    NO_ERROR = 0,
    SENSOR_FAIL = 1,
    TOO_LOW = 2,
    TOO_HIGH = 3,
    WIFI_ERROR = 4,
    JSON_ERROR = 5
};
```

## Temperature Modes

| Mode        | Temperature (°C) | Humidity (%) |
|-------------|------------------|--------------|
| **ROOM_TEMP** | 18 - 25        | 30 - 70      |
| **COOLER**    | 2 - 6          | 30 - 70      |
| **FREEZER**   | -30 - (-18)    | 30 - 70      |

## Features

### Arduino Features
- **Real-time clock**: NTP synchronization via ESP32
- **Batch processing**: Collects 5 measurements for median calculation
- **Flash storage**: Saves data during WiFi outages
- **Smart reconnection**: Sends saved data upon reconnection
- **Threshold validation**: Automatic error detection

### ESP32 Features
- **Access Point**: Automatic WiFi hotspot
- **HTTP Server**: Receives JSON data on port 80
- **Batch management**: Handling of large data volumes
- **Persistent logging**: LittleFS-based data storage
- **Multi-task processing**: Parallel data processing

## Development

### Code Style
- **C++11 standard** for compatibility
- **Modular design** with separate responsibilities
- **Extensive testing** - 46 unit tests total
- **Documentation** - Commented code and README

### Git Workflow
1. Create feature branch from `main`
2. Implement changes with tests
3. Run all tests: `platformio test`
4. Create Pull Request with template
5. Code review before merge

### CI/CD Ready
- Automated testing with PlatformIO
- Separate test environments
- Mock frameworks for hardware-independent tests

## Troubleshooting

### Common Issues

#### WiFi Connection
```bash
# Check ESP32 AP status
platformio device monitor  # ESP32 terminal

# Check Arduino connection  
platformio device monitor  # Arduino terminal
```

#### Sensor Issues
- **DHT22 errors**: Check 3.3V power supply
- **Timeout errors**: Increase delay between readings
- **Invalid readings**: Check cables and grounding

#### Build Issues
```bash
# Clean build cache
platformio run --target clean

# Reinstall dependencies
platformio lib install --force
```

### Debug Tips
- Use Serial Monitor for real-time logging
- Check error logs for specific error codes
- Test with mock data before hardware tests
- Verify JSON format with online validators

## Performance Data

**Note**: Performance metrics below are measured on specific test environments and may vary based on hardware, USB connection, and system configuration.

### Arduino Uno R4 WiFi
- **RAM usage**: 19.2% (6,288 / 32,768 bytes) - *measured during typical operation*
- **Flash usage**: 25.8% (67,532 / 262,144 bytes) - *production build*
- **Upload time**: ~46 seconds - *varies by USB connection and system*
- **Test execution**: ~40 seconds (24 tests) - *on hardware*

### ESP32-S3
- **Native test speed**: ~10 seconds (22 tests) - *native environment on development PC*
- **HTTP response time**: < 100ms - *local network, small payloads*
- **WiFi connection time**: 2-5 seconds - *depends on environment*
- **Batch processing**: Processing capability varies by payload size and available memory

**Test Environment**: Development setup with USB 2.0 connection, Windows 11, PlatformIO Core 6.x

## License

This project is developed for **Chas Academy Team 9** as part of the SUVx24 course.

## Contributing

We welcome contributions to the Chas Advance IoT System! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

### Quick Start for Contributors

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/your-feature-name`
3. **Setup development environment**:
   ```bash
   pip install platformio
   # Create secrets files from examples
   # Install PlatformIO extension for VS Code (recommended)
   ```
4. **Run all tests** before making changes:
   ```bash
   cd "Chas Advance Arduino"
   platformio test -e uno_r4_wifi_test
   cd "../Chas Advance ESP32"
   platformio test -e native
   ```
5. **Make your changes** with appropriate tests
6. **Verify all tests pass** after changes
7. **Create a Pull Request** with clear description

### Development Guidelines

- **Code Style**: Follow existing C++11 conventions
- **Testing**: Add unit tests for new functionality
- **Documentation**: Update README and inline comments
- **Commit Messages**: Use clear, descriptive commit messages
- **Branch Naming**: Use `feature/`, `bugfix/`, or `hotfix/` prefixes

### Project Structure

- Use existing modular design patterns
- Keep separate responsibilities for Arduino and ESP32 components
- Maintain separation between production and test code
- Follow PlatformIO project conventions

See [PULL_REQUEST_TEMPLATE.md](PULL_REQUEST_TEMPLATE.md) for PR requirements.

## License

This project is developed for **Chas Academy Team 9** as part of the SUVx24 course.

**Copyright © 2024 Chas Academy Team 9**

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Third-party Dependencies

- **PlatformIO**: Apache License 2.0
- **ArduinoJson**: MIT License
- **Unity Test Framework**: MIT License
- **DHT Sensor Library**: MIT License

## Maintainers

- **Team**: Chas Academy Team 9
- **Course**: SUVx24 - Embedded Systems
- **Repository**: [ChasAcademy-Team-9/Embedded](https://github.com/ChasAcademy-Team-9/Embedded)

For questions or support, please create an issue in this repository.
