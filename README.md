# BresserWeatherSensorLW

[![CI](https://github.com/matthias-bs/BresserWeatherSensorLW/actions/workflows/CI.yml/badge.svg)](https://github.com/matthias-bs/BresserWeatherSensorLW/actions/workflows/CI.yml)
[![GitHub release](https://img.shields.io/github/release/matthias-bs/BresserWeatherSensorLW?maxAge=3600)](https://github.com/matthias-bs/BresserWeatherSensorLW/releases)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](https://github.com/matthias-bs/BresserWeatherSensorLW/blob/main/LICENSE)

Bresser 868 MHz Weather Sensor Radio Receiver based on ESP32/RP2040 and SX1262/SX1276 &mdash; sends data to a [LoRaWAN Network](https://lora-alliance.org/)

This is a remake of [BresserWeatherSensorTTN](https://github.com/matthias-bs/BresserWeatherSensorTTN) based on [RadioLib](https://github.com/jgromes/RadioLib) instead of [MCCI Arduino LoRaWAN Library](https://github.com/mcci-catena/arduino-lorawan) for LoRaWAN communication.

## Important Notes

* RadioLib's LoRaWAN implementation is currently in beta stage.
* Helium Network is not supported (requires LoRaWAN v1.0.x)
* This should not be the first Arduino sketch you are ever trying to flash to your board - try somthing simple first (e.g. `blink.ino`) to get familiar with the tools and workflow.
* If you are new to LoRaWAN
   * Check out [The Things Fundamentals on LoRaWAN](https://www.thethingsnetwork.org/docs/lorawan/)
   * Read the excellent article [RadioLib LoRaWAN on TTN starter script](https://github.com/jgromes/RadioLib/blob/master/examples/LoRaWAN/LoRaWAN_Starter/notes.md)
* You currently need the latest version (HEAD) of [RadioLib](https://github.com/jgromes/RadioLib)
* Try and configure [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver) stand-alone before using it with BresserWeatherSensorLW
* LoRaWAN downlink commands (and responses) are not compatible with [BresserWeatherSensorTTN](https://github.com/matthias-bs/BresserWeatherSensorTTN)

## Features

* Single 868 MHz Radio Transceiver for both Sensor Data Reception and LoRaWAN Connection
* Compatible to LoRaWAN Specification 1.1.0 / RP001 Regional Parameters 1.1 revision A
* Tested with [The Things Network](https://www.thethingsnetwork.org/)
* Supports multiple 868 MHz Sensors (e.g. Weather Sensor and Soil Moisture Sensor or Indoor Thermometer/Hygrometer)
* See [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver) for supported sensors
* Low Power Design (using ESP32 Deep Sleep Mode / RP2040 Sleep State)
* Fast LoRaWAN Joining after Deep Sleep (using ESP32 RTC RAM / RP2040 RAM)
* [ATC MiThermometer](https://github.com/pvvx/ATC_MiThermometer) Bluetooth Low Energy Thermometer/Hygrometer Integration (optional)
* [Theengs Decoder](https://github.com/theengs/decoder) Bluetooth Low Energy Sensors Integration (optional)
* OneWire Temperature Sensor Integration (optional)
* ESP32/RP2040 Analog Digital Converter Integration (optional)
* [A02YYUW / DFRobot SEN0311 Ultrasonic Distance Sensor](https://wiki.dfrobot.com/_A02YYUW_Waterproof_Ultrasonic_Sensor_SKU_SEN0311) (30...4500mm) (optional)
* [Remote Configuration via LoRaWAN Downlink](https://github.com/matthias-bs/BresserWeatherSensorTTN/blob/main/README.md#remote-configuration-via-lorawan-downlink)

## Project Status

* [x] Weather sensor data reception
* [x] BLE sensor data reception
* [x] LoRaWAN join
* [x] LoRaWAN data uplink
* [x] LoRaWAN network time request & RTC setting
* [x] Energy saving / battery deep-discharge protection
* [x] Wakeup time adjustment
* [x] LoRaWAN control downlink
* [x] LoRaWAN status uplink
* [x] Periodic LinkCheck
* [x] Node/network status/debug information
* [x] setDeviceStatus(battLevel)
* [x] Log messages
* [x] Supported boards
* [x] Enforce minimum uplink interval
* [x] RP2040 specific implementation
* [x] Fix battery voltage measurement for HW targets FIREBEETLE_ESP32_COVER_LORA & LORAWAN_NODE
* [x] Separate LoRaWAN network and application code
* [x] Change LoRaWAN control downlink / status uplink messages
* [x] Update Javascript encoders/decoders
* [x] Implement using of BLE sensor addresses configured via downlink
* [x] Update documentation
* [x] Implement Heltec WiFi LoRa 32 V3 battery voltage measurement

## Contents

* [Supported Hardware](#supported-hardware)
  * [Predefined Pinout and Radio Chip Configurations](#predefined-pinout-and-radio-chip-configurations)
* [LoRaWAN Network Service Configuration](#lorawan-network-service-configuration)
* [Software Build Configuration](#software-build-configuration)
  * [Required Configuration](#required-configuration)
  * [Optional Configuration](#optional-configuration)
  * [Enabling Debug Output](#enabling-debug-output)
  * [Test Run](#test-run)
* [LoRaWAN Payload Formatters](#lorawan-payload-formatters)
  * [The Things Network Payload Formatters Setup](#the-things-network-payload-formatters-setup)
* [MQTT Integration](#mqtt-integration)
  * [The Things Network MQTT Integration](#the-things-network-mqtt-integration)
* [Datacake Integration](#datacake-integration)
  * [Datacake / The Things Network Setup](#datacake--the-things-network-setup)
  * [Desktop Dashboard](#desktop-dashboard)
  * [Mobile Dashboard](#mobile-dashboard)
* [Remote Configuration Commands / Status Requests via LoRaWAN](#remote-configuration-commands--status-requests-via-lorawan)
  * [Parameters](#parameters)
  * [Using Raw Data](#using-raw-data)
  * [Using the Javascript Uplink/Downlink Formatters](#using-the-javascript-uplinkdownlink-formatters)
* [Doxygen Generated Source Code Documentation](#doxygen-generated-source-code-documentation)
* [References](#references)
* [Legal](#legal)

## Supported Hardware

  |  Status       | Setup                                                                                                               | Board (/ Revision)   | Define (Prefix: ARDUINO_) | Radio Module | Notes    |
  | ---------     | ------------------------------------------------------------------------------------------------------------------- | -------------------- | ------------------------- | ------------ | -------- |
  |  :hourglass:  | [LILYGO®TTGO-LORA32 V1](https://github.com/Xinyuan-LilyGo/TTGO-LoRa-Series) | TTGO LoRa32-OLED /<br>TTGO LoRa32 V1 (No TFCard) | TTGO_LORA32_V1 | SX1276 (HPD13A) | -   |
  |  :hourglass:  | [LILYGO®TTGO-LORA32 V2](https://github.com/LilyGO/TTGO-LORA32) | TTGO LoRa32-OLED /<br>TTGO LoRa32 V2             | TTGO_LoRa32_V2 | SX1276 (HPD13A) | For LMIC only: Wire DIO1 to GPIO33 |
  |  :white_check_mark:  |  [LILYGO®TTGO-LORA32 V2.1](https://www.lilygo.cc/products/lora3?variant=42272562282677)    | TTGO LoRa32-OLED /<br>TTGO LoRa32 V2.1 (1.6.1) | TTGO_LoRa32_v21new |  SX1276 (HPD13A) | - |
  |  :hourglass:  | [Heltec Wireless Stick](https://heltec.org/project/wireless-stick/)   | Heltec Wireless Stick     | heltec_wireless_stick  |  SX1276  | - |
  |  :hourglass:  | [Heltec WiFi LoRa 32 V2](https://heltec.org/project/wifi-lora-32/)    | Heltec WiFi LoRa 32(V2)   | heltec_wifi_lora_32_V2 |  SX1276  | - |
  |  :white_check_mark:  | [Heltec WiFi LoRa 32 V3](https://heltec.org/project/wifi-lora-32-v3/) | Heltec WiFi LoRa 32(V3)   | heltec_wifi_32_lora_V3 |  SX1262  | - |
  |  :white_check_mark:  | [LoRaWAN_Node](https://github.com/matthias-bs/LoRaWAN_Node)      | FireBeetle-ESP32 | ESP32_DEV -> LORAWAN_NODE     | SX1276 (RFM95W) | -      |
  |  :white_check_mark:  | [DFRobot FireBeetle ESP32 IoT Microcontroller](https://www.dfrobot.com/product-1590.html) with [FireBeetle Cover LoRa Radio 868MHz](https://www.dfrobot.com/product-1831.html) | FireBeetle-ESP32 |  ESP32_DEV & FIREBEETLE_ESP32_COVER_LORA | SX1276 (LoRa1276) | Wiring on the cover: <br>D2 to RESET<br>D3 to DIO0<br>D4 to CS<br>D5 to DIO1 |
  |  :hourglass:  | [Adafruit Feather ESP32S2 with Adafruit LoRa Radio FeatherWing](https://github.com/matthias-bs/BresserWeatherSensorReceiver#adafruit-feather-esp32s2-with-adafruit-lora-radio-featherwing)                                | Adafruit Feather ESP32-S2 | FEATHER_ESP32S2   | SX1276 (RFM95W) | **No Bluetooth available!**<br>Wiring on the Featherwing:<br>E to IRQ<br>D to CS<br>C to RST<br>A to DI01 |
  |  :white_check_mark:  | [Thingpulse ePulse Feather](https://thingpulse.com/product/epulse-feather-low-power-esp32-development-board/) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231)     | Adafruit ESP32 Feather | FEATHER_ESP32   | SX1276 (RFM95W) | Wiring on the Featherwing:<br>E to IRQ<br>D to CS<br>C to RST<br>A to DI01<br><br>**see** [**#55**](https://github.com/matthias-bs/BresserWeatherSensorTTN/issues/55) |
  |  :white_check_mark:  | [M5Stack Core2](https://docs.m5stack.com/en/core/core2) with [M5Stack Module LoRa868](https://docs.m5stack.com/en/module/lora868)   | M5Core2 | M5STACK_CORE2   | SX1276<br>(RA-01H) | Wiring on the LoRa868 Module: <br>DIO1 to GPIO35<br><br>"M5Unified" must be installed <br>`M5.begin()`is called to control power management |
  |  :hourglass:  | [ESP32-S3 PowerFeather](https://powerfeather.dev/) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231)     | ESP32-S3 PowerFeather | ESP32S3_POWERFEATHER | SX1276 (RFM95W) | Wiring on the Featherwing:<br>E to IRQ<br>D to CS<br>C to RST<br>A to DI01<br><br>"PowerFeather-SDK" must be installed<br>`Board.init();` is called to control power management |
  |  :white_check_mark:  | [Adafruit Feather RP2040](https://www.adafruit.com/product/4884) with [Adafruit LoRa Radio FeatherWing](https://www.adafruit.com/product/3231)     | Adafruit Feather RP2040 | ADAFRUIT_FEATHER_RP2040   | SX1276 (RFM95W) | **No Bluetooth available!**<br>**Configuration: Choose an entry with "FS" in section __Flash Size__!**<br>Wiring on the Featherwing:<br>E to IRQ<br>D to CS<br>C to RST<br>A to DI01 |

:hourglass: &mdash; confirmation pending

:white_check_mark: &mdash; confirmed

### Predefined Pinout and Radio Chip Configurations

By selecting a Board and a Board Revision in the Arduino IDE, a define is passed to the preprocessor/compiler. For the boards listed in [Supported Hardware](#supported-hardware), the default configuration is assumed based on this define. If this is not what you need, you have to switch to Manual Configuration.

If you are not using the Arduino IDE, you can use the defines in [Supported Hardware](#supported-hardware) with your specific tool chain to get the same result.

If enabled in the Arduino IDE Preferences ("Verbose Output"), the preprosessor will provide some output regarding the selected configuration, e.g.

```
ARDUINO_ADAFRUIT_FEATHER_ESP32S2 defined; assuming RFM95W FeatherWing will be used
[...]
Radio chip: SX1276
Pin config: RST->0 , IRQ->5 , NSS->6 , GPIO->11
```

## LoRaWAN Network Service Configuration

Create an account and set up a device configuration in your LoRaWAN network provider's web console, e.g. [The Things Network](https://www.thethingsnetwork.org/).

* LoRaWAN v1.1
* Regional Parameters 1.1 Revision A
* Device class A
* Over the air activation (OTAA)

## Software Build Configuration

### Required Configuration

* Install the Arduino ESP32 board package in the Arduino IDE
* Select your ESP32 board
* Install all libraries as listed in the section [package.json](package.json) &mdash; section dependencies &mdash; via the Arduino IDE Library Manager 
* Clone (or download and unpack) the latest ([BresserWeatherSensorLW Release](https://github.com/matthias-bs/BresserWeatherSensorLW/releases))
* Set your LoRaWAN Network Service credentials &mdash; `RADIOLIB_LORAWAN_DEV_EUI`, `RADIOLIB_LORAWAN_NWK_KEY` and `RADIOLIB_LORAWAN_APP_KEY` &mdash in [secrets.h](secrets.h):

```
// The Device EUI & two keys can be generated on the TTN console

// Replace with your Device EUI
#define RADIOLIB_LORAWAN_DEV_EUI   0x---------------

// Replace with your App Key
#define RADIOLIB_LORAWAN_APP_KEY   0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--

// Put your Nwk Key here
#define RADIOLIB_LORAWAN_NWK_KEY   0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--
```

* Load the sketch [BresserWeatherSensorLW.ino](BresserWeatherSensorLW.ino)
* Compile and Upload

### Optional Configuration

In [BresserWeatherSensorLWCfg.h](BresserWeatherSensorLWCfg.h):

* Configure your time zone by editing `TZ_INFO`
* Disable sensor/interface features which you do not want to use
* Adjust battery voltage levels
* Configure the timing parameters if required
* If enabled, configure your ATC MiThermometer's / Theengs Decoder's BLE MAC Address by by editing `KNOWN_BLE_ADDRESSES`
* Configure the ADC's input pins, dividers and oversampling settings as needed
* Disable sensor decoders wich are not needed

### Enabling Debug Output

[Debug Output Configuration in Arduino IDE](Debug_Output.md)

### Test Run

Watch your board's debug output in the serial console and the LoRaWAN communication in your network provider's web console.

## LoRaWAN Payload Formatters

Upload [Uplink Formatter](scripts/uplink_formatter.js) and [Downlink Formatter](scripts/downlink_formatter.js) scripts in your LoRaWAN network service provider's web console to allow decoding / encoding raw data to / from JSON format.

See [The Things Network MQTT Integration and Payload Formatters](https://github.com/matthias-bs/BresserWeatherSensorTTN/blob/main/README.md#the-things-network-mqtt-integration-payload-formatters) and [TS013-1.0.0 Payload Codec API](https://resources.lora-alliance.org/technical-specifications/ts013-1-0-0-payload-codec-api) for more details.

### The Things Network Payload Formatters Setup

#### Uplink Formatter

Decode uplink payload (a sequence of bytes) into JSON format, i.e. data structures which are readable/suitable for further processing.

In The Things Network Console:
1. Go to "Payload formatters" -> "Uplink"
2. Select "Formatter type": "Custom Javascript formatter"
3. "Formatter code": Paste [scripts/uplink_formatter.js](scripts/uplink_formatter.js)
4. Apply "Save changes"

![TTN Uplink Formatter](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/38b66478-688a-4028-974a-c517cddae662)

> [!NOTE]
> The actual payload depends on the options selected in the Arduino sketch (see) [BresserWeatherSensorsLW.cfg](BresserWeatherSensorsLW.cfg)) &mdash; the decoder must be edited accordingly (add or remove data types and JSON identifiers). The configuration dependent part of the decoder can be created with a C++ preprocessor and the Python script [generate_decoder.py](scripts/generate_decoder.py).

#### Downlink Formatter

Encode downlink payload from JSON to a sequence of bytes.

In The Things Network Console:
1. Go to "Payload formatters" -> "Downlink"
2. Select "Formatter type": "Custom Javascript formatter"
3. "Formatter code": Paste [scripts/downlink_formatter.js](scripts/downlink_formatter.js)
4. Apply "Save changes"

## MQTT Integration

### The Things Network MQTT Integration

TTN provides an MQTT broker.
How to receive and decode the payload with an MQTT client -
see https://www.thethingsnetwork.org/forum/t/some-clarity-on-mqtt-topics/44226/2

V3 topic:

`v3/<ttn app id><at symbol>ttn/devices/<ttn device id>/up`

  
v3 message key field jsonpaths:
  
```
<ttn device id> = .end_device_ids.device_id
<ttn app id> = .end_device_ids.application_ids.application_id  // (not including the <at symbol>ttn in the topic)
<payload> = .uplink_message.frm_payload
```  


JSON-Path with Uplink-Decoder (see [scripts/uplink_formatter.js](scripts/uplink_formatter.js))

`.uplink_message.decoded_payload.bytes.<variable>`

## Datacake Integration

### Datacake / The Things Network Setup

YouTube Video: [Get started for free with LoRaWaN on The Things Network and Datacake IoT Platform](https://youtu.be/WGVFgYp3k3s)

### Desktop Dashboard

![Datacake_Dashboard_Desktop](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/2a876ba1-06b9-4ea3-876c-2fad3d559b01)

### Mobile Dashboard
![Datacake_Dashboard_Mobile](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/fbc0948c-bfd8-4d7d-9780-c113d576d3cf)

## Remote Configuration Commands / Status Requests via LoRaWAN

Many software parameters can be defined at compile time, i.e. in [BresserWeatherSensorLWCfg.h](BresserWeatherSensorLWCfg.h). A few [parameters](#parameters) can also be changed and queried at run time via LoRaWAN, either [using raw data](#using-raw-data) or [using Javascript Uplink/Downlink Formatters](#using-the-javascript-uplinkdownlink-formatters).

### Parameters

| Parameter             | Description                                                                 |
| --------------------- | --------------------------------------------------------------------------- |
| <ws_timeout>          | Weather sensor receive timeout in seconds; 0...255                          |
| <sleep_interval>      | Sleep interval (regular) in seconds; 0...65535                              |
| <sleep_interval_long> | Sleep interval (energy saving mode) in seconds; 0...65535                   |
| \<epoch\>             | Unix epoch time, see https://www.epochconverter.com/ ( \<integer\> / "0x....") |
| <reset_flags>         | Raingauge reset flags; 0...15 (1: hourly / 2: daily / 4: weekly / 8: monthly) / "0x0"..."0xF" |
| <rtc_source>          | Real time clock source; 0x00: GPS / 0x01: RTC / 0x02: LORA / 0x03: unsynched / 0x04: set (source unknown) |
| <sensors_incX>        | Bresser sensor IDs include list; e.g. "0xDEADBEEF"; "0x00000000" => empty list => default values          |
| <sensors_excX>        | Bresser sensor IDs include list; e.g. "0xDEADBEEF"; "0x00000000" => empty list => default values          |
| <ble_active>          | BLE active scan; 1 (active scan) / 0 (passive scan)                         |
| <ble_scantime>        | BLE scan time in seconds; 0...255                                           |
| <ble_addrX>           | BLE sensor MAC addresses; e.g. "DE:AD:BE:EF:12:23"                          |

> [!WARNING]
> Confirmed downlinks should not be used! (see [here](https://www.thethingsnetwork.org/forum/t/how-to-purge-a-scheduled-confirmed-downlink/56849/7) for an explanation.)

> [!IMPORTANT]
> To set sensors_inc / sensors_exc to the compile time default configuration, set the first ID in CMD_SET_SENSORS_INC / CMD_SET_SENSORS_EXC to 0x00000000.
> To set the BLE sensor addresses to the compile time default configuration, set the first address in CMD_SET_BLE_ADDR to 0x000000000000.

> [!NOTE]
> **Default values**<br>
> * Sleep interval (long): see `BresserWeatherSensorLWCfg.h`<br>
> * BLE addresses and scan parameters: see `BresserWeatherSensorLWCfg.h`<br>
> * Weather sensor receive timeout: see `BresserWeatherSensorReceiver/src/WeatherSensorCfg.h`<br>
> * Sensor IDs include/exclude list: see `BresserWeatherSensorReceiver/src/WeatherSensorCfg.h` 

### Using Raw Data

| Command                       | Port       | Downlink                                                                  | Uplink         |
| ----------------------------- | ---------- | ------------------------------------------------------------------------- | -------------- |
| CMD_GET_DATETIME              | 0x86 (134) | 0x00                                                                      | epoch[31:24]<br>epoch[23:16]<br>epoch[15:8]<br>epoch[7:0]<br>rtc_source[7:0] |
| CMD_SET_DATETIME              | 0x88 (136) | epoch[31:24]<br>epoch[23:16]<br>epoch[15:8]<br>epoch[7:0]                 | n.a.           |
| CMD_SET_SLEEP_INTERVAL        | 0xA8 (168) | sleep_interval[15:8]<br>sleep_interval[7:0]                               | n.a.           |
| CMD_SET_SLEEP_INTERVAL_LONG   | 0xA9 (169) | sleep_interval_long[15:8]<br>sleep_interval_long[7:0]                     | n.a.           |
| CMD_GET_LW_CONFIG             | 0xB1 (177) | 0x00                                                                      | sleep_interval[15:8]<br>sleep_interval[7:0]<br>sleep_interval_long[15:8]<br>sleep_interval_long[7:0] |
| CMD_GET_WS_TIMEOUT            | 0xC0 (192) | 0x00                                                                      | ws_timeout[7:0] |
| CMD_SET_WS_TIMEOUT            | 0xC1 (193) | ws_timeout[7:0]                                                           | n.a.            |
| CMD_RESET_RAINGAUGE           | 0xC3 (195) | flags[7:0]                                                                | n.a.            |
| CMD_GET_SENSORS_INC           | 0xC4 (196) | 0x00                                                                      | sensors_inc0[31:24]<br>sensors_inc0[23:15]<br>sensors_inc0[16:8]<br>sensors_inc0[7:0]<br>... |
| CMD_SET_SENSORS_INC           | 0xC5 (197) | sensors_inc0[31:24]<br>sensors_inc0[23:15]<br>sensors_inc0[16:8]<br>sensors_inc0[7:0]<br>... | n.a. |
| CMD_GET_SENSORS_EXC           | 0xC6 (198) | 0x00                                                                      | sensors_exc0[31:24]<br>sensors_exc0[23:15]<br>sensors_exc0[16:8]<br>sensors_exc0[7:0]<br>... |
| CMD_SET_SENSORS_EXC           | 0xC7 (199) | sensors_exc0[31:24]<br>sensors_exc0[23:15]<br>sensors_exc0[16:8]<br>sensors_exc0[7:0]<br>... | n.a. |
| CMD_GET_BLE_ADDR              | 0xC8 (200) | 0x00                                                                      | ble_addr0[47:40]<br>ble_addr0[39:32]<br>ble_addr0[31:24]<br>ble_addr0[23:15]<br>ble_addr0[16:8]<br>ble_addr0[7:0]<br>... |
| CMD_SET_BLE_ADDR              | 0xC9 (201) | ble_addr0[47:40]<br>ble_addr0[39:32]<br>ble_addr0[31:24]<br>ble_addr0[23:15]<br>ble_addr0[16:8]<br>ble_addr0[7:0]<br>... | n.a. |
| CMD_GET_BLE_CONFIG            | 0xCA (202) | 0x00                                                                      | ble_active[7:0]<br>ble_scantime[7:0] |
| CMD_SET_BLE_CONFIG            | 0xCB (203) | ble_active[7:0]<br>ble_scantime[7:0]                                      | n.a.            |

#### The Things Network Examples

##### Example 1: Set SLEEP_INTERVAL to 360 seconds
1. Set port for CMD_SET_SLEEP_INTERVAL to 168
2. Convert interval to hex: 300 = 0x012C
3. Set payload to 0x01 0x2C
4. Send downlink via The Things Network Console

![TTN Downlink as Hex](https://github.com/matthias-bs/BresserWeatherSensorLW/assets/83612361/4b616cd9-4f50-4407-8032-44d240abc09b)


##### Example 2: Set Date/Time

1. Set port for CMD_SET_DATETIME to 136
2. Get epoch (e.g. from https://www.epochconverter.com/hex) (Example: 0x63B2BC32); add an offset (estimated) for time until received (Example: + 64 / 0x40 seconds => 0x63B2BC**7**2) 
3. Set payload to 0x63 0xB2 0xBC 0x72
4. Send downlink via The Things Network Console

### Using the Javascript Uplink/Downlink Formatters

| Command                       | Downlink                                                                  | Uplink                       |
| ----------------------------- | ------------------------------------------------------------------------- | ---------------------------- |
| CMD_GET_DATETIME              | {"cmd": "CMD_GET_DATETIME"}                                               | {"epoch": \<epoch\>}         |
| CMD_SET_DATETIME              | {"epoch": \<epoch\>}                                                      | n.a.                         |
| CMD_SET_SLEEP_INTERVAL        | {"sleep_interval": <sleep_interval>"}                                     | n.a.                         |
| CMD_SET_SLEEP_INTERVAL_LONG   | {"sleep_interval_long": <sleep_interval_long>}                            | n.a.                         |
| CMD_GET_LW_CONFIG             | {"cmd": "CMD_GET_LW_CONFIG"}                                              | {"sleep_interval": <sleep_interval>, "sleep_interval_long": <sleep_interval_longC>} |
| CMD_GET_WS_TIMEOUT            | {"cmd": "CMD_GET_WS_TIMEOUT"}                                             | {"ws_timeout": <ws_timeout>} |
| CMD_SET_WS_TIMEOUT            | {"ws_timeout": <ws_timeout>}                                              | n.a.                         |
| CMD_RESET_RAINGAUGE           | {"reset_flags": <reset_flags>}                                            | n.a.                         |
| CMD_GET_SENSORS_INC           | {"cmd": "CMD_GET_SENSORS_INC"}                                            | {"sensors_inc": [<sensors_inc0>, ..., <sensors_incN>]} |
| CMD_SET_SENSORS_INC           | {"sensors_inc": [<sensors_inc0>, ..., <sensors_incN>]}                    | n.a.                         |
| CMD_GET_SENSORS_EXC           | {"cmd": "CMD_GET_SENSORS_EXC"}                                            | {"sensors_exc": [<sensors_exc0>, ..., <sensors_excN>]} |
| CMD_SET_SENSORS_EXC           | {"sensors_exc": [<sensors_exc0>, ..., <sensors_excN>]}                    | n.a.                         |
| CMD_GET_BLE_ADDR              | {"cmd": "CMD_GET_BLE_ADDR"}                                               | {"ble_addr": [<ble_addr0>, ..., <ble_addrN>]} |
| CMD_SET_BLE_ADDR              | {"ble_addr": [<ble_addr0>, ..., <ble_addrN>]}                             | n.a.                         |
| CMD_GET_BLE_CONFIG            | {"cmd": "CMD_GET_BLE_CONFIG"}                                             | {"ble_active": <ble_active>, "ble_scantime": <ble_scantime>} |
| CMD_SET_BLE_CONFIG            | {"ble_active": <ble_active>, "ble_scantime": <ble_scantime>}              | n.a.                         |

#### The Things Network Examples 

##### Example 1: Set SLEEP_INTERVAL to 360 seconds
1. Build payload as JSON string: `{"sleep_interval": 360}` &mdash;
   the correct port is selected automatically
2. Send downlink via The Things Network Console

![TTN Downlink as JSON](https://github.com/matthias-bs/BresserWeatherSensorLW/assets/83612361/2133676f-5d08-4d71-b580-e628c15b1229)


##### Example 2: Set Date/Time
1. Get epoch (e.g. from https://www.epochconverter.com) (Example: 1692729833); add an offset (estimated) for time until received (Example: + 64 seconds => 16927298**97**) 
2. Build payload as JSON string: {"epoch": 1692729897} 
3. Send downlink via The Things Network Console

## Doxygen Generated Source Code Documentation

[https://matthias-bs.github.io/BresserWeatherSensorTTN/index.html](https://matthias-bs.github.io/BresserWeatherSensorLW/)

## References

Based on
* [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver) by Matthias Prinke
* [RadioLib](https://github.com/jgromes/RadioLib) by Jan Gromeš
* [Lora-Serialization](https://github.com/thesolarnomad/lora-serialization) by Joscha Feth
* [ESP32Time](https://github.com/fbiego/ESP32Time) by Felix Biego
* [OneWireNg](https://github.com/pstolarz/OneWireNg) by Piotr Stolarz
* [DallasTemperature / Arduino-Temperature-Control-Library](https://github.com/milesburton/Arduino-Temperature-Control-Library) by Miles Burton
* [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) by h2zero
* [Theengs Decoder](https://github.com/theengs/decoder) by [Theengs Project](https://github.com/theengs)
* [DistanceSensor_A02YYUW](https://github.com/pportelaf/DistanceSensor_A02YYUW) by Pablo Portela
* [Preferences](https://github.com/vshymanskyy/Preferences) by Volodymyr Shymanskyy

## Legal

> This project is in no way affiliated with, authorized, maintained, sponsored or endorsed by Bresser GmbH or any of its affiliates or subsidiaries.
