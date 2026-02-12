# Copilot Instructions for BresserWeatherSensorLW

## Project Overview

BresserWeatherSensorLW is a LoRaWAN-enabled weather sensor receiver for ESP32 and RP2040 microcontrollers. It receives data from Bresser 868 MHz weather sensors via a radio transceiver (SX1262/SX1276/LR1121) and forwards it to a LoRaWAN network such as The Things Network, ChirpStack, or Helium IoT.

**Key Characteristics:**
- Arduino sketch (`.ino` file) with C++ support files
- Dual-purpose radio: FSK mode for sensor reception, LoRaWAN for network communication
- Supports multiple hardware platforms: ESP32, ESP32-S2, ESP32-S3, RP2040
- Implements power-efficient deep sleep with RTC support
- Modular architecture separating network layer from application layer

## Architecture

### Core Components

1. **BresserWeatherSensorLW.ino** - Main sketch file
   - Setup and loop functions
   - LoRaWAN session management
   - Deep sleep handling
   - Main coordinator between components

2. **AppLayer (src/AppLayer.h/cpp)** - Application layer abstraction
   - Sensor data collection and encoding
   - Payload generation in stages
   - Command/status message handling
   - Separation from LoRaWAN network layer

3. **Payload Modules (src/Payload*.h/cpp)** - Specialized payload handlers
   - `PayloadBresser`: Weather sensor data encoding
   - `PayloadBLE`: Bluetooth sensor integration
   - `PayloadAnalog`: ADC input handling
   - `PayloadDigital`: Digital I/O handling
   - `PayloadOneWire`: Temperature sensor support

4. **SystemContext (src/SystemContext.h/cpp)** - System state management
   - Hardware-specific initialization
   - RTC (Real-Time Clock) management
   - Sleep interval handling
   - Battery voltage monitoring
   - Power-saving mode decisions

5. **Configuration Files**
   - `config.h`: RadioLib/LoRaWAN radio configuration and pin mappings
   - `BresserWeatherSensorLWCfg.h`: Feature flags, timing, voltage thresholds
   - `secrets.h`: LoRaWAN credentials (template with dummy values)
   - `WeatherSensorCfg.h`: Sensor receiver configuration (from BresserWeatherSensorReceiver library)

6. **Command Interface (BresserWeatherSensorLWCmd.h/cpp)**
   - LoRaWAN downlink command decoding
   - Status uplink encoding
   - Remote configuration support

### Key Dependencies

- **RadioLib (v7.5.0+)**: LoRaWAN communication
- **BresserWeatherSensorReceiver (v0.38.2+)**: 868 MHz sensor data reception
- **LoRa Serialization**: Efficient payload encoding
- **ArduinoJson (v7.4.2)**: Configuration file parsing
- **NimBLE-Arduino** (optional): BLE sensor support
- **OneWireNg** (optional): 1-Wire temperature sensors

## Development Environment

### Build System

- **Platform**: Arduino IDE, Arduino CLI, or PlatformIO
- **Supported Boards**:
  - ESP32 variants (Feather, FireBeetle, Heltec, TTGO, etc.)
  - ESP32-S2/S3 (PowerFeather, XIAO)
  - RP2040 (Adafruit Feather)
- **Board Manager URLs**:
  - ESP32: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
  - RP2040: `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`

### CI/CD

The project uses GitHub Actions workflows:
- **CI.yml**: Builds for multiple board variants
- **arduino-lint.yml**: Arduino library/sketch linting
- **doxygen.yml**: API documentation generation
- **spell-check.yml**: Documentation spell checking

Build is triggered on push/PR to main branch or via workflow dispatch.

## Coding Standards and Conventions

### File Organization

```
BresserWeatherSensorLW/
├── BresserWeatherSensorLW.ino    # Main Arduino sketch
├── config.h                       # Radio/LoRaWAN configuration
├── secrets.h                      # LoRaWAN credentials (template)
├── BresserWeatherSensorLWCfg.h   # User configuration
├── BresserWeatherSensorLWCmd.h/cpp # Command interface
├── src/                           # Application layer
│   ├── AppLayer.h/cpp            # Main application layer
│   ├── Payload*.h/cpp            # Payload encoders
│   ├── SystemContext.h/cpp       # System state management
│   ├── LoadSecrets.h/cpp         # Credential loading
│   ├── LoadNodeCfg.h/cpp         # Configuration loading
│   ├── adc/                      # ADC utilities
│   ├── BleSensors/               # BLE sensor code
│   ├── DistanceSensors/          # Distance sensor support
│   └── rp2040/                   # RP2040-specific code
├── data/                          # LittleFS data files
│   ├── secrets.json              # Optional: credentials
│   └── node_config.json          # Optional: deployment config
├── scripts/                       # Support scripts
│   └── bresserweathersensorlw-codec/ # Payload formatters
└── extras/                        # Examples and tests
```

### Code Style

1. **File Headers**
   - Use standardized header with file description
   - Include MIT license notice
   - Maintain detailed history section
   - Add creation date and author

2. **Comments**
   - Doxygen-style comments for functions/classes
   - `/*! ... */` for multi-line documentation
   - `//!` for single-line documentation
   - `//` for inline comments

3. **Naming Conventions**
   - Classes: PascalCase (e.g., `AppLayer`, `PayloadBresser`)
   - Functions: camelCase (e.g., `getPayloadStage1`, `genPayload`)
   - Constants: UPPER_SNAKE_CASE (e.g., `MAX_UPLINK_SIZE`, `PIN_LORA_NSS`)
   - Member variables: camelCase (e.g., `weatherSensor`, `appPrefs`)
   - Defines: UPPER_SNAKE_CASE with descriptive prefixes

4. **Preprocessor Usage**
   - Use `#if defined()` for platform detection
   - Group platform-specific code with clear markers
   - Provide pragma messages for configuration guidance
   - Use include guards: `#if !defined(_FILENAME_H)`

### Platform-Specific Code

Handle platform differences using preprocessor directives:

```cpp
#if defined(ARDUINO_ARCH_ESP32)
    // ESP32-specific code
#elif defined(ARDUINO_ARCH_RP2040)
    // RP2040-specific code
#endif
```

Common board defines:
- `ARDUINO_FEATHER_ESP32`
- `ARDUINO_DFROBOT_FIREBEETLE_ESP32`
- `ARDUINO_HELTEC_WIFI_LORA_32_V3`
- `ARDUINO_TTGO_LORA32_*`
- `ARDUINO_ADAFRUIT_FEATHER_ESP32S2`
- `ARDUINO_ADAFRUIT_FEATHER_RP2040`

### Configuration Pattern

Use a layered configuration approach:

1. **Compile-time Configuration** (header files)
   - Feature enable/disable flags
   - Pin definitions per board
   - Default values

2. **Runtime Configuration** (JSON files on LittleFS)
   - LoRaWAN credentials (`secrets.json`)
   - Deployment-specific settings (`node_config.json`)

3. **Persistent Storage** (Preferences/NVS)
   - User-modifiable settings
   - Calibration values
   - BLE device addresses

Example pattern:
```cpp
#if defined(MY_BOARD)
    #define PIN_LORA_NSS   5
    #define PIN_LORA_RST   14
    // ... more pins
#endif
```

## Key Patterns and Practices

### 1. Application Layer Interface

The `AppLayer` class provides the main interface between LoRaWAN and application logic. It uses multiple inheritance to compose functionality from specialized payload handlers:

```cpp
class AppLayer : public PayloadBresser, PayloadAnalog, PayloadDigital
```

Key methods:
- `begin()`: Initialize sensors and preferences
- `getPayloadStage1()`: First payload encoding pass (sensor data)
- `getPayloadStage2()`: Second payload encoding pass (additional data)
- `decodeDownlink()`: Process incoming LoRaWAN commands
- `getAppStatusUplinkInterval()`: Return status update interval

This composition pattern allows different sensor types to be easily enabled/disabled without changing core logic.

### 2. Payload Encoding

Use `LoraEncoder` from LoRa-Serialization library for efficient encoding:

```cpp
void encodeMyData(LoraEncoder &encoder) {
    encoder.writeTemperature(temperature);
    encoder.writeHumidity(humidity);
    encoder.writeUint16(pressure);
}
```

### 3. Invalid/Unavailable Data

Signal invalid data using defined constants:
- Temperature: `INV_TEMP` (32767)
- Humidity: `INV_UINT8` (255)
- Voltage: `INV_UINT16` (65535)
- etc.

### 4. Power Management

For battery-powered operation:
- Check battery voltage with `getBatteryVoltage()`
- Compare against thresholds: `VOLTAGE_CRITICAL`, `VOLTAGE_ECO_ENTER`
- Adjust sleep time and features based on power state
- Use deep sleep between transmissions

### 5. Time Management

- Use `SystemContext` for time tracking
- Support both internal RTC and external RTC chips
- Handle time synchronization via LoRaWAN
- Implement time-based algorithms (rain gauge, lightning stats)

### 6. Logging

Use Arduino logging macros (ESP32) or custom macros (RP2040):
- `log_d()`: Debug messages
- `log_i()`: Informational messages
- `log_w()`: Warnings
- `log_e()`: Errors

Enable via Arduino IDE: Tools → Core Debug Level

## LoRaWAN Specifics

### Network Configuration

- **LoRaWAN Version**: 1.1.0 (default) or 1.0.4
- **Region**: Configurable (EU868, US915, etc.)
- **Join Method**: OTAA (Over-The-Air Activation)
- **Credentials**: JoinEUI, DevEUI, AppKey, NwkKey (v1.1)

### Uplink Structure

Multiple message types on different ports:
- **Port 1**: Sensor data (default)
- **Port 2**: LoRaWAN node status
- **Port 3**: Application/sensor status
- **Port 10+**: Command responses

### Downlink Commands

Remote configuration via LoRaWAN downlink:
- Set payload configuration
- Configure BLE sensors
- Adjust sensor reception parameters
- Request status information
- Trigger sensor scanning

Implemented in `BresserWeatherSensorLWCmd.h/cpp`

## Testing and Validation

### Pre-Deployment Testing

1. **Standalone Sensor Reception**
   - Test BresserWeatherSensorReceiver first
   - Use examples/BresserWeatherSensorBasic
   - Verify sensor data reception before LoRaWAN

2. **LoRaWAN Join**
   - Verify credentials are correct
   - Check join success in serial output
   - Confirm on network server

3. **Payload Validation**
   - Use payload formatters in `scripts/bresserweathersensorlw-codec/`
   - Test with real sensor data
   - Verify decoded values match transmitted

### Debug Output

Enable via `BresserWeatherSensorLWCfg.h` or Arduino IDE debug level:
- View sensor reception events
- Monitor LoRaWAN join/uplink/downlink
- Track sleep/wake cycles
- Observe battery voltage readings

See `Debug_Output.md` for details.

## Common Tasks

### Adding a New Board

1. Find board define (compile empty sketch with verbose output)
2. Add section in `BresserWeatherSensorLWCfg.h`
3. Define pins and board-specific settings
4. Add section in `config.h` for radio pins
5. If needed, add to `WeatherSensorCfg.h` (sensor receiver pins)
6. Test and document

### Adding a New Sensor Type

1. Create payload encoder in `src/Payload<SensorType>.h/cpp`
2. Add payload configuration options
3. Update `AppLayer` to call encoder
4. Add command support if needed
5. Update payload formatters for decoding
6. Document in README

### Customizing Payload

1. Review `APP_PAYLOAD_CFG_*` defines in `BresserWeatherSensorLWCfg.h`
2. Modify `appPayloadCfgDef` array in `AppLayer.h`
3. Use config helper in `extras/confighelper/` to visualize
4. Adjust payload formatters accordingly

### Loading Configuration from Files

1. Create JSON file in `data/` directory
2. Upload to LittleFS using appropriate tool:
   - ESP32: ESP32 Sketch Data Upload or esp-idf tools
   - RP2040: Pico-W-Go or picotool
3. Enable file loading in code:
   - `loadSecrets()` for credentials
   - `loadNodeCfg()` for deployment config

## Important Notes for AI Code Generation

1. **Preserve Existing Architecture**
   - Don't merge application and network layers
   - Maintain separation between sensor types
   - Keep configuration modular

2. **Platform Compatibility**
   - Always consider ESP32 and RP2040 differences
   - Test platform-specific code paths
   - Use appropriate includes (e.g., `Arduino.h` vs ESP32-specific)

3. **Memory Constraints**
   - Embedded systems have limited RAM
   - Use stack allocation where possible
   - Be mindful of array sizes and buffers
   - `MAX_UPLINK_SIZE` limits payload

4. **Power Efficiency**
   - Minimize wake time
   - Use appropriate sleep modes
   - Consider battery life impact

5. **LoRaWAN Compliance**
   - Respect duty cycle regulations
   - Don't exceed data rate limits
   - Follow Regional Parameters

6. **Backward Compatibility**
   - Maintain API compatibility when possible
   - Document breaking changes clearly
   - Consider migration path for users

7. **Prerequisites for Contributors**
   - Not the first Arduino sketch to try - test with simpler sketches first
   - Test BresserWeatherSensorReceiver standalone before integration
   - Understand LoRaWAN fundamentals (see The Things Network documentation)
   - Use RadioLib v7.5.0+ and arduino-esp32 v3.X.Y
   - Read RadioLib LoRaWAN starter guide

8. **Code Generation Guidelines**
   - Always include MIT license header with copyright notice
   - Maintain detailed history section in file headers
   - Use Doxygen-style comments for public APIs
   - Follow existing naming conventions strictly
   - Add platform-specific guards for new features

## References

- [RadioLib Documentation](https://jgromes.github.io/RadioLib/)
- [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver)
- [The Things Network Docs](https://www.thethingsnetwork.org/docs/)
- [LoRaWAN Specification](https://lora-alliance.org/resource_hub/lorawan-specification-v1-1/)
- [Project README](README.md)
- [API Documentation](https://matthias-bs.github.io/BresserWeatherSensorLW/)

## License

This project is licensed under the MIT License. All generated code should include appropriate MIT license headers and copyright notices.
