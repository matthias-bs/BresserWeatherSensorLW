# BresserWeatherSensorLW

[![CI](https://github.com/matthias-bs/BresserWeatherSensorLW/actions/workflows/CI.yml/badge.svg)](https://github.com/matthias-bs/BresserWeatherSensorLW/actions/workflows/CI.yml)
[![GitHub release](https://img.shields.io/github/release/matthias-bs/BresserWeatherSensorLW?maxAge=3600)](https://github.com/matthias-bs/BresserWeatherSensorLW/releases)
[![License: MIT](https://img.shields.io/badge/license-MIT-green)](https://github.com/matthias-bs/BresserWeatherSensorLW/blob/main/LICENSE)

Bresser 868 MHz Weather Sensor Radio Receiver based on ESP32/RP2040 and SX1262/SX1276 &mdash; sends data to a [LoRaWAN Network](https://lora-alliance.org/)

This is a remake of [BresserWeatherSensorTTN](https://github.com/matthias-bs/BresserWeatherSensorTTN) based on [RadioLib](https://github.com/jgromes/RadioLib) instead of [MCCI Arduino LoRaWAN Library](https://github.com/mcci-catena/arduino-lorawan) for LoRaWAN communication.

## Important Notes

* RadioLib's LoRaWAN implementation is currently in beta stage.
* Helium Network is not supported (requires LoRaWAN v1.0.x)
* This should not be the first Arduino sketch ever you are trying to flash to your board - try somthing simple (`blink.ino`) first to get familiar with the tools and workflow.
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

## Status

This project is in early stage of development - stay tuned.

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
* [ ] Change LoRaWAN control downlink / status uplink messages
* [ ] Update Javascript encoders/decoders

 
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


If enabled in the Arduino IDE Preferences ("Verbose Output"), the preprosessor will provide some output regarding the selected configuration, e.g.

```
ARDUINO_ADAFRUIT_FEATHER_ESP32S2 defined; assuming RFM95W FeatherWing will be used
[...]
Radio chip: SX1276
Pin config: RST->0 , IRQ->5 , NSS->6 , GPIO->11
```

**Documentation will be updated soon!**

Meanwhile, refer to [BresserWeatherSensorTTN - README.md](https://github.com/matthias-bs/BresserWeatherSensorTTN/blob/main/README.md)

## Remote Configuration Commands / Status Requests via LoRaWAN

| Test Status | Command                       | Port | Downlink                                                                  | Uplink         |
| ----------- | ----------------------------- | ---- | ------------------------------------------------------------------------- | -------------- |
| [ ] | CMD_GET_DATETIME              | 0x86 (134) | 0x00                                                                      | unixtime[31:24]<br>unixtime[23:16]<br>unixtime[15:8]<br>unixtime[7:0]<br>rtc_source[7:0] |
| [ ] | CMD_SET_DATETIME              | 0x88 (136) | unixtime[31:24]<br>unixtime[23:16]<br>unixtime[15:8] <br> unixtime[7:0]   | n.a.           |
| [ ] | CMD_SET_SLEEP_INTERVAL        | 0xA8 (168) | interval[15:8]<br>interval[7:0]                                           | n.a.           |
| [ ] | CMD_SET_SLEEP_INTERVAL_LONG   | 0xA9 (169) | interval[15:8]<br>interval[7:0]                                           | n.a.           |
| [ ] | CMD_GET_LW_CONFIG             | 0xB1 (177) | 0x00                                                                      | sleep_interval[15:8]<br>sleep_interval[7:0]<br>sleep_interval_long[15:8]<br>sleep_interval_long[7:0] |
| [ ] | CMD_GET_WS_TIMEOUT            | 0xC0 (192) | 0x00                                                                      | ws_timeout[7:0] |
| [ ] | CMD_SET_WS_TIMEOUT            | 0xC1 (193) | ws_timeout[7:0]                                                           | n.a.            |
| [ ] | CMD_RESET_RAINGAUGE           | 0xC3 (195) | flags[7:0]                                                                | n.a.            |
| [ ] | CMD_GET_SENSORS_INC           | 0xC4 (196) | 0x00                                                                      | sensors_inc0[31:24]<br>sensors_inc0[23:15]<br>sensors_inc0[16:8]<br>sensors_inc0[7:0]<br>... |
| [ ] | CMD_SET_SENSORS_INC           | 0xC5 (197) | sensors_inc0[31:24]<br>sensors_inc0[23:15]<br>sensors_inc0[16:8]<br>sensors_inc0[7:0]<br>... | n.a. |
| [ ] | CMD_GET_SENSORS_EXC           | 0xC6 (198) | 0x00                                                                      | sensors_exc0[31:24]<br>sensors_exc0[23:15]<br>sensors_exc0[16:8]<br>sensors_exc0[7:0]<br>... |
| [ ] | CMD_SET_SENSORS_EXC           | 0xC7 (199) | sensors_exc0[31:24]<br>sensors_exc0[23:15]<br>sensors_exc0[16:8]<br>sensors_exc0[7:0]<br>... | n.a. |
| [ ] | CMD_GET_BLE_ADDR              | 0xC8 (200) | 0x00                                                                      | ble_addr0[47:40]<br>ble_addr0[39:32]<br>ble_addr0[31:24]<br>ble_addr0[23:15]<br>ble_addr0[16:8]<br>ble_addr0[7:0]<br>... |
| [ ] | CMD_SET_BLE_ADDR              | 0xC9 (201) | ble_addr0[47:40]<br>ble_addr0[39:32]<br>ble_addr0[31:24]<br>ble_addr0[23:15]<br>ble_addr0[16:8]<br>ble_addr0[7:0]<br>... | n.a. |



:warning: Confirmed downlinks should not be used! (see [here](https://www.thethingsnetwork.org/forum/t/how-to-purge-a-scheduled-confirmed-downlink/56849/7) for an explanation.)

