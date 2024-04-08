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

#include <vector>
#include <string>

// Downlink messages
// ------------------
//
// CMD_SET_WEATHERSENSOR_TIMEOUT
// (seconds)
// byte0: 0xA0
// byte1: ws_timeout[ 7: 0]
//
// CMD_SET_SLEEP_INTERVAL
// (seconds)
// byte0: 0xA8
// byte1: sleep_interval[15:8]
// byte2: sleep_interval[ 7:0]

// CMD_SET_SLEEP_INTERVAL_LONG
// (seconds)
// byte0: 0xA9
// byte1: sleep_interval_long[15:8]
// byte2: sleep_interval_long[ 7:0]
//
// CMD_RESET_RAINGAUGE
// byte0: 0xB0
// byte1: flags[7:0] (optional)
//
// CMD_GET_CONFIG
// byte0: 0xB1
//
// CMD_GET_DATETIME
// byte0: 0x86
//
// CMD_SET_DATETIME
// byte0: 0x88
// byte1: unixtime[31:24]
// byte2: unixtime[23:16]
// byte3: unixtime[15: 8]
// byte4: unixtime[ 7: 0]
//
// Response uplink messages
// -------------------------
//
// CMD_GET_DATETIME -> FPort=2
// byte0: unixtime[31:24]
// byte1: unixtime[23:16]
// byte2: unixtime[15: 8]
// byte3: unixtime[ 7: 0]
// byte4: rtc_source[ 7: 0]
//
// CMD_GET_CONFIG -> FPort=3
// byte0: ws_timeout[ 7: 0]
// byte1: sleep_interval[15: 8]
// byte2: sleep_interval[ 7:0]
// byte3: sleep_interval_long[15:8]
// byte4: sleep_interval_long[ 7:0]

#define CMD_SET_WEATHERSENSOR_TIMEOUT   0xA0
#define CMD_SET_SLEEP_INTERVAL          0xA8
#define CMD_SET_SLEEP_INTERVAL_LONG     0xA9
#define CMD_RESET_RAINGAUGE             0xB0
#define CMD_GET_CONFIG                  0xB1
#define CMD_GET_DATETIME                0x86
#define CMD_SET_DATETIME                0x88

// Enable debug mode (debug messages via serial port)
// Arduino IDE: Tools->Core Debug Level: "Debug|Verbose"
// For other architectures than ESP32, see logging.h

// Enable logging for https://github.com/vshymanskyy/Preferences (used for RP2040)
// #define NVS_LOG

//--- Select Board ---
#if !defined(ARDUINO_TTGO_LoRa32_V1) && !defined(ARDUINO_TTGO_LoRa32_V2) &&               \
    !defined(ARDUINO_TTGO_LoRa32_v21new) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && \
    !defined(ARDUINO_FEATHER_ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) &&       \
    !defined(ARDUINO_M5STACK_Core2) && !defined(ARDUINO_M5STACK_CORE2)
// Use pinning for LoRaWAN Node
//#define LORAWAN_NODE

// Use pinning for Firebeetle Cover LoRa
#define FIREBEETLE_ESP32_COVER_LORA
#endif

#if defined(ARDUINO_FEATHER_ESP32)
// The Thingpulse ePulse Feather board has the same pinout as
// the Adafruit Feather ESP32 board, but a different VBAT voltage divider!
// See https://github.com/matthias-bs/BresserWeatherSensorTTN/issues/55
// #define ARDUINO_THINGPULSE_EPULSE_FEATHER
#endif

// Enable LORAWAN debug mode - this generates dummy weather data and skips weather sensor reception
// #define LORAWAN_DEBUG

// LoRaWAN session info is stored in RTC RAM on ESP32 and in Preferences (flash) on RP2040
#if defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#define SESSION_IN_PREFERENCES
#endif

// Battery voltage thresholds for energy saving & deep-discharge prevention

// If SLEEP_EN is defined and battery voltage <= BATTERY_WEAK [mV], MCU will sleep for SLEEP_INTERVAL_LONG
// Go to sleep mode immediately after start if battery voltage <= BATTERY_LOW [mV]
#if defined(ARDUINO_ADAFRUIT_FEATHER_RP2040) || defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
// External voltage divider required
#pragma message("External voltage divider required for battery voltage measurement.")
#pragma message("No power-saving & deep-discharge protection implemented yet.")
#elif defined(FIREBEETLE_ESP32_COVER_LORA)
#pragma message("On-board voltage divider must be enabled for battery voltage measurement - see schematic."
#pragma message("No power-saving & deep-discharge protection implemented yet.")
#elif defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORE2)
#pragma message("Energy saving at weak battery not implemented yet.")
#pragma message("Battery deep-discharge prevention not implemented yet.")
#endif

#define BATTERY_WEAK 3500
#define BATTERY_LOW 3200

// Battery voltage limits in mV (usable range for the device) for battery state calculation
#define BATTERY_DISCHARGE_LIMIT 3200
#define BATTERY_CHARGE_LIMIT 4200

// Sleep for SLEEP_INTERVAL seconds after succesful transmission
#define SLEEP_INTERVAL 360

// Long sleep interval, MCU will sleep for SLEEP_INTERVAL_LONG seconds if battery voltage <= BATTERY_WEAK
#define SLEEP_INTERVAL_LONG 900

// RTC to network time sync interval (in minutes)
#define CLOCK_SYNC_INTERVAL 24 * 60

// Force deep sleep after a certain time, even if transmission was not completed
#define FORCE_SLEEP

// Force a new join procedure (instead of re-join) after encountering sleep timeout
#define FORCE_JOIN_AFTER_SLEEP_TIMEOUT

// During initialization (not joined), force deep sleep after SLEEP_TIMEOUT_INITIAL (if enabled)
#define SLEEP_TIMEOUT_INITIAL 1800

// If already joined, force deep sleep after SLEEP_TIMEOUT_JOINED seconds (if enabled)
#define SLEEP_TIMEOUT_JOINED 600

// Additional timeout to be applied after joining if Network Time Request pending
#define SLEEP_TIMEOUT_EXTRA 300

// Timeout for weather sensor data reception (seconds)
#define WEATHERSENSOR_TIMEOUT 180

// If enabled, enter deep sleep mode if receiving weather sensor data was not successful
// #define WEATHERSENSOR_DATA_REQUIRED

// Enable transmission of weather sensor ID
// #define SENSORID_EN

// Enable rain data statistics
#define RAINDATA_EN

#if !defined(ARDUINO_M5STACK_Core2) && !defined(ARDUINO_M5STACK_CORE2)
// Enable battery / supply voltage measurement
// Note: For M5Stack Core2 use 'float batVoltage = M5.Axp.GetBatVoltage();'
#define ADC_EN
#endif

// Enable OneWire temperature measurement
// #define ONEWIRE_EN

// Enable BLE temperature/humidity measurement
// Notes:
// * BLE requires a lot of program memory!
// * ESP32-S2 does not provide BLE!
#if !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ARCH_RP2040)
// #define MITHERMOMETER_EN
//#define THEENGSDECODER_EN
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
#ifdef ADC_EN
#if defined(ARDUINO_TTGO_LoRa32_V1) || defined(ARDUINO_TTGO_LoRa32_V2) || defined(ARDUINO_TTGO_LoRa32_v21new)
#define PIN_ADC_IN 35
#elif defined(ARDUINO_FEATHER_ESP32)
#define PIN_ADC_IN A13
#elif defined(LORAWAN_NODE) || defined(FIREBEETLE_ESP32_COVER_LORA)
#define PIN_ADC_IN A0
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#define PIN_ADC_IN A0
#else
#define PIN_ADC_IN 34
#endif
#endif

// Additional ADC pins (default: FireBeetle ESP32)
// #define PIN_ADC0_IN         A0
// #define PIN_ADC1_IN         A1
// #define PIN_ADC2_IN         A2
#ifdef LORAWAN_NODE
#define PIN_ADC3_IN A3
#endif

#ifdef PIN_ADC0_IN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
const float ADC0_DIV = 0.5;
const uint8_t ADC0_SAMPLES = 10;
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
#else
const float UBATT_DIV = 0.5;
#endif
const uint8_t UBATT_SAMPLES = 10;
#endif

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
// BLE scan time in seconds
#define BLE_SCAN_TIME 31

// List of known sensors' BLE addresses
#define KNOWN_BLE_ADDRESSES \
    {                       \
        "a4:c1:38:b8:1f:7f" \
    }
#endif

#endif // _LWCFG_H
