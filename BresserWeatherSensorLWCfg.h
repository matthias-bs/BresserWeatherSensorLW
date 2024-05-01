///////////////////////////////////////////////////////////////////////////////
// BresserWeatherSensorLWCfg.h
//
// User specific configuration for BresserWeatherSensorLW.ino
//
// - Enabling or disabling of features
// - Voltage thresholds for power saving
// - Timing configuration
// - Timezone
//
// created: 04/2024
//
//
// MIT License
//
// Copyright (c) 2024 Matthias Prinke
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// History:
//
// 20240407 Created from BresserWeatherSensorTTNCfg.h
// 20240410 Removed obsolete defines
// 20240413 Refactored ADC handling
// 20240423 Added define ARDUINO_heltec_wifi_lora_32_V3
// 20240427 Added voltage divider and ADC input control pin
//          for ARDUINO_heltec_wifi_lora_32_V3
//          Added BLE configuration/status via LoRaWAN
// 20240430 Modified battery voltage measurement
//
// Note:
// Depending on board package file date, either
// ARDUINO_M5STACK_Core2 or ARDUINO_M5STACK_CORE2
// is used - see https://github.com/espressif/arduino-esp32/issues/9423!
// Consequently both variants have to be checked!!!
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_LWCFG_H)
#define _LWCFG_H

//#include <vector>
//#include <string>

// ===========================
// LoRaWAN command interface
// ===========================

// ---------------------------
// -- LoRaWAN network layer --
// ---------------------------

// CMD_GET_DATETIME
// -----------------
#define CMD_GET_DATETIME 0x86

// Downlink (command):
// byte0:   0x00

// Uplink (response):
// byte0: unixtime[31:24]
// byte1: unixtime[23:16]
// byte2: unixtime[15: 8]
// byte3: unixtime[ 7: 0]
// byte4: rtc_source[ 7: 0]


// CMD_SET_DATETIME
// -----------------
// Port: CMD_SET_DATETIME
#define CMD_SET_DATETIME 0x88

// Downlink (command):
// byte0: unixtime[31:24]
// byte1: unixtime[23:16]
// byte2: unixtime[15: 8]
// byte3: unixtime[ 7: 0]

// Uplink: n.a.

// CMD_SET_SLEEP_INTERVAL
// -----------------------
// Note: Set normal sleep interval in seconds
// Port: CMD_SET_SLEEP_INTERVAL
#define CMD_SET_SLEEP_INTERVAL 0xA8

// Downlink (command):
// byte0: sleep_interval[15:8]
// byte1: sleep_interval[ 7:0]

// Response: n.a.

// CMD_SET_SLEEP_INTERVAL_LONG
// ----------------------------
// Note: Set long sleep interval in seconds (energy saving mode)
// Port: CMD_SET_SLEEP_INTERVAL_LONG
#define CMD_SET_SLEEP_INTERVAL_LONG 0xA9

// Downlink (command):
// byte0: sleep_interval_long[15:8]
// byte1: sleep_interval_long[ 7:0]

// Uplink: n.a.

// CMD_GET_LW_CONFIG
// ------------------
// Port: CMD_GET_LW_CONFIG
#define CMD_GET_LW_CONFIG 0xB1

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: sleep_interval[15: 8]
// byte1: sleep_interval[ 7:0]
// byte2: sleep_interval_long[15:8]
// byte3: sleep_interval_long[ 7:0]

// -----------------------
// -- Application layer --
// -----------------------

// CMD_GET_WS_TIMEOUT
// -------------------
// Note: Get weather sensor RX timeout in seconds
// Port: CMD_GET_WS_TIMEOUT
#define CMD_GET_WS_TIMEOUT              0xC0

// Downlink (command)
// byte0: 0x00

// Uplink (response):
// byte0: ws_timeout[ 7: 0]

// CMD_SET_WS_TIMEOUT
// -------------------
// Note: Set weather sensor RX timeout in seconds
// Port: CMD_SET_WS_TIMEOUT
#define CMD_SET_WS_TIMEOUT 0xC1

// Downlink (command):
// byte0: ws_timeout[ 7: 0]

// Uplink: n.a.

// CMD_RESET_RAINGAUGE
// --------------------
// Port: CMD_RESET_RAINGAUGE
#define CMD_RESET_RAINGAUGE 0xC3

// Downlink (command):
// byte0: flags[7:0] (optional)

// Uplink: n.a.

// Reset Lightning???
// -------------------


// CMD_GET_SENSORS_INC
// --------------------
// Note: Get sensors include list (0...12 IDs)
// Port: CMD_GET_SENSORS_INC
#define CMD_GET_SENSORS_INC 0xC4

// Downlink (command):
// byte0: 0x00

// Uplink (response): 
// byte0: sensors_inc0[31:24]
// byte1: sensors_inc0[23:16]
// byte2: sensors_inc0[15: 8]
// byte3: sensors_inc0[ 7: 0]
// ...

// CMD_SET_SENSORS_INC
// --------------------
// Note: Set sensors include list (0...12 IDs)
// Port: CMD_SET_SENSORS_INC
#define CMD_SET_SENSORS_INC 0xC5

// Downlink (command):
// byte0: sensors_inc0[31:24]
// byte1: sensors_inc0[23:16]
// byte2: sensors_inc0[15: 8]
// byte3: sensors_inc0[ 7: 0]
// ...

// Uplink: n.a.

// CMD_GET_SENSORS_EXC
// --------------------
// Note: Get sensors exclude list (0...12 * 4 bytes)
// Port: CMD_GET_SENSORS_EXC
#define CMD_GET_SENSORS_EXC 0xC6

// Downlink (command):
// byte0: 0x00

// Uplink (response): 
// byte0: sensors_exc0[31:24]
// byte1: sensors_exc0[23:16]
// byte2: sensors_exc0[15: 8]
// byte3: sensors_exc0[ 7: 0]
// ...

// CMD_SET_SENSORS_EXC
// --------------------
// Note: Set sensors exclude list (0...12 * 4 bytes)
// Port: CMD_SET_SENSORS_EXC
#define CMD_SET_SENSORS_EXC 0xC7

// Downlink (command):
// byte0: sensors_exc0[31:24]
// byte1: sensors_exc0[23:16]
// byte2: sensors_exc0[15: 8]
// byte3: sensors_exc0[ 7: 0]
// ...

// Uplink: n.a.

// CMD_GET_BLE_ADDR
// -----------------
// Note: Get BLE sensors MAC addresses (0..8 * 6 bytes)
// Port: CMD_GET_BLE_ADDR
#define CMD_GET_BLE_ADDR 0xC8

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: ble_addr0[47:24]
// byte1: ble_addr0[23:32]
// byte2: ble_addr0[31:24]
// byte3: ble_addr0[23:16]
// byte4: ble_addr0[15: 8]
// byte5: ble_addr0[ 7: 0]
// ...

// CMD_SET_BLE_ADDR
// -----------------
// Note: Set BLE sensors MAC addresses (0..8 * 6 bytes)
// Port: CMD_SET_BLE_ADDR
#define CMD_SET_BLE_ADDR 0xC9

// Downlink (command):
// byte0: ble_addr0[47:24]
// byte1: ble_addr0[23:32]
// byte2: ble_addr0[31:24]
// byte3: ble_addr0[23:16]
// byte4: ble_addr0[15: 8]
// byte5: ble_addr0[ 7: 0]
// ...

// Response: n.a.

// CMD_GET_BLE_CONFIG
// -------------------
// Note: Scan time in seconds
// Port: CMD_GET_BLE_CONFIG
#define CMD_GET_BLE_CONFIG 0xCA

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: 0x01 (active scan) / 0x00 (passive scan)
// byte1: scan_time[7:0]

// CMD_SET_BLE_CONFIG
// -------------------
// Note: Scan time in seconds
// Port: CMD_SET_BLE_CONFIG
#define CMD_SET_BLE_CONFIG 0xCB

// Uplink (command):
// byte0: active_scan - 0x01 (active scan) / 0x00 (passive scan)
// byte1: scan_time[7:0]

// Response: n.a.

// ===========================


// Enable debug mode (debug messages via serial port)
// Arduino IDE: Tools->Core Debug Level: "Debug|Verbose"
// For other architectures than ESP32, see logging.h

// Enable logging for https://github.com/vshymanskyy/Preferences (used for RP2040)
// #define NVS_LOG

//--- Select Board ---
#if !defined(ARDUINO_TTGO_LoRa32_V1) && !defined(ARDUINO_TTGO_LoRa32_V2) &&               \
    !defined(ARDUINO_TTGO_LoRa32_v21new) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && \
    !defined(ARDUINO_FEATHER_ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) &&       \
    !defined(ARDUINO_heltec_wifi_32_lora_V3) && !defined(ARDUINO_heltec_wifi_lora_32_V3) && \
    !defined(ARDUINO_M5STACK_Core2) && !defined(ARDUINO_M5STACK_CORE2) && \
    !defined(ARDUINO_ESP32S3_POWERFEATHER)
// Use pinning for LoRaWAN Node
#define LORAWAN_NODE

// Use pinning for Firebeetle Cover LoRa
//#define FIREBEETLE_ESP32_COVER_LORA
#endif

#if defined(ARDUINO_FEATHER_ESP32)
// The Thingpulse ePulse Feather board has the same pinout as
// the Adafruit Feather ESP32 board, but a different VBAT voltage divider!
// See https://github.com/matthias-bs/BresserWeatherSensorTTN/issues/55
//#define ARDUINO_THINGPULSE_EPULSE_FEATHER
#endif

// Battery voltage thresholds for energy saving & deep-discharge prevention

// If battery voltage <= BATTERY_WEAK [mV], MCU will sleep for SLEEP_INTERVAL_LONG
// Go to sleep mode immediately after start if battery voltage <= BATTERY_LOW [mV]
#define BATTERY_WEAK 3500
#define BATTERY_LOW 3200

// Battery voltage limits in mV (usable range for the device) for battery state calculation
#define BATTERY_DISCHARGE_LIMIT 3200
#define BATTERY_CHARGE_LIMIT 4200

// Minimum sleep interval (in seconds)
#define SLEEP_INTERVAL_MIN 60

// Sleep for SLEEP_INTERVAL seconds after succesful transmission
#define SLEEP_INTERVAL 360

// Long sleep interval, MCU will sleep for SLEEP_INTERVAL_LONG seconds if battery voltage <= BATTERY_WEAK
#define SLEEP_INTERVAL_LONG 900

// RTC to network time sync interval (in minutes)
#define CLOCK_SYNC_INTERVAL 24 * 60

// Timeout for weather sensor data reception (seconds)
#define WEATHERSENSOR_TIMEOUT 180

// If enabled, enter deep sleep mode if receiving weather sensor data was not successful
// #define WEATHERSENSOR_DATA_REQUIRED

// Enable transmission of weather sensor ID
// #define SENSORID_EN

// Enable rain data statistics
#define RAINDATA_EN

// Enable battery / supply voltage uplink
#define ADC_EN

// Enable OneWire temperature measurement
// #define ONEWIRE_EN

// Enable BLE temperature/humidity measurement
// Notes:
// * BLE requires a lot of program memory!
// * ESP32-S2 does not provide BLE!
#if !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ARCH_RP2040)
// #define MITHERMOMETER_EN
#define THEENGSDECODER_EN
#endif

// Enable Bresser Soil Temperature/Moisture Sensor
#define SOILSENSOR_EN

// Enable Bresser Lightning Sensor
#define LIGHTNINGSENSOR_EN

// Enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
#define TZINFO_STR "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

// Enable Ultrasonic Distance Sensor
#if defined(LORAWAN_NODE) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
// #define DISTANCESENSOR_EN
#endif


// ADC for supply/battery voltage measurement
// Defaults:
// ---------
// FireBeetle ESP32:            on-board connection to VB (with R10+R11 assembled)
// TTGO LoRa32:                 on-board connection to VBAT
// Adafruit Feather ESP32:      on-board connection to VBAT
// Adafruit Feather ESP32-S2:   no VBAT input circuit
// Adafruit Feather RP2040:     no VBAT input circuit (connect external divider to A0)
#if defined(ARDUINO_TTGO_LoRa32_V1) || defined(ARDUINO_TTGO_LoRa32_V2) || defined(ARDUINO_TTGO_LoRa32_v21new)
#define PIN_ADC_IN 35
#elif defined(ARDUINO_FEATHER_ESP32)
#define PIN_ADC_IN A13
#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
// External voltage divider required
#pragma message("External voltage divider required for battery voltage measurement.")
#pragma message("No power-saving & deep-discharge protection implemented yet.")
// unused
#define PIN_ADC_IN -1
#elif defined(LORAWAN_NODE)
// External Li-Ion Battery connected to solar charger
#define PIN_ADC_IN A3
#elif defined(FIREBEETLE_ESP32_COVER_LORA)
#pragma message("On-board voltage divider must be enabled for battery voltage measurement (see schematic).")
#pragma message("No power-saving & deep-discharge protection implemented yet.")
// On-board VB
#define PIN_ADC_IN A0
#elif defined(ARDUINO_heltec_wifi_32_lora_V3) || defined(ARDUINO_heltec_wifi_lora_32_V3)
// On-board VB
#define PIN_ADC_IN A0
#elif defined(ESP32S3_POWERFEATHER)
// unused
#define PIN_ADC_IN -1
#elif defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORE2)
// Unused
#define PIN_ADC_IN -1
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#pragma message("External voltage divider required for battery voltage measurement.")
#pragma message("No power-saving & deep-discharge protection implemented yet.")
#define PIN_ADC_IN A0
#else
#pragma message("Unknown battery voltage measurement circuit.")
#pragma message("No power-saving & deep-discharge protection implemented yet.")
// unused
#define PIN_ADC_IN -1
#endif

// Additional ADC pins
#if defined(LORAWAN_NODE)
#define PIN_SUPPLY_IN A0
#endif

#ifdef PIN_SUPPLY_IN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
const float SUPPLY_DIV = 0.5;
const uint8_t SUPPLY_SAMPLES = 10;
#endif

#ifdef PIN_ADC1_IN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
const float ADC1_DIV = 0.5;
const uint8_t ADC1_SAMPLES = 10;
#endif

#ifdef PIN_ADC2_IN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
const float ADC2_DIV = 0.5;
const uint8_t ADC2_SAMPLES = 10;
#endif

#ifdef PIN_ADC3_IN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
const float ADC3_DIV = 0.5;
const uint8_t ADC3_SAMPLES = 10;
#endif

#ifdef ONEWIRE_EN
#if defined(ARDUINO_TTGO_LoRa32_V1)
#define PIN_ONEWIRE_BUS 21
#elif defined(ARDUINO_FEATHER_ESP32) || defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
#define PIN_ONEWIRE_BUS 15
#elif defined(LORAWAN_NODE) || defined(FIREBEETLE_ESP32_COVER_LORA)
#define PIN_ONEWIRE_BUS 5
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#define PIN_ONEWIRE_BUS 6
#else
#define PIN_ONEWIRE_BUS 0
#endif
#endif

#ifdef DISTANCESENSOR_EN
#if defined(LORAWAN_NODE)
#define DISTANCESENSOR_TX 0 // pull-up/open: processed value / low: real-time value
#define DISTANCESENSOR_RX 26
#define DISTANCESENSOR_PWR 25
#define DISTANCESENSOR_RETRIES 5
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#define DISTANCESENSOR_TX 0 // pull-up/open: processed value / low: real-time value
#define DISTANCESENSOR_RX 1
#define DISTANCESENSOR_PWR 7
#define DISTANCESENSOR_RETRIES 8
#endif
#endif

#ifdef ADC_EN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
#if defined(ARDUINO_THINGPULSE_EPULSE_FEATHER)
const float UBATT_DIV = 0.6812;
#elif defined(ARDUINO_heltec_wifi_32_lora_V3) || defined(ARDUINO_heltec_wifi_lora_32_V3)
#define ADC_CTRL 37
// R17=100k, R14=390k => 100k / (100k + 390 k)
const float UBATT_DIV = 0.2041;
#else
const float UBATT_DIV = 0.5;
#endif
const uint8_t UBATT_SAMPLES = 10;
#endif

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
// BLE scan time in seconds
#define BLE_SCAN_TIME 31
// BLE scan mode (0: passive / 1: active)
#define BLE_SCAN_MODE 1

// List of known sensors' BLE addresses
#define KNOWN_BLE_ADDRESSES \
    {                       \
        "a4:c1:38:b8:1f:7f" \
    }
#endif

#endif // _LWCFG_H
