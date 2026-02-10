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
// Copyright (c) 2025 Matthias Prinke
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
// 20240504 PowerFeather: added BATTERY_CAPACITY_MAH
//          Moved LoRaWAN command interface to BresserWeatherSensorLWCmd.h
// 20240520 Added definitions for AppLayer payload configuration
// 20240521 Added UBATT_CH/USUPPLY_CH
// 20240524 Added sensor feature flags
//          Moved PAYLOAD_SIZE from BresserWeatherSensorLW.ino
// 20240528 Added encoding of invalid values, modified default payload, fixes
// 20240603 Added definitions for sensor status flags
//          Added appStatusUplinkInterval
// 20240607 Added ARDUINO_DFROBOT_FIREBEETLE_ESP32 variant selection
//          Updated HELTEC_WIFI_LORA_32_V3 definition
//          Modified STATUS_INTERVAL
// 20240608 Added MAX_NUM_868MHZ_SENSORS
// 20240613 Removed workaround for ARDUINO_THINGPULSE_EPULSE_FEATHER
// 20240704 Moved MAX_DOWNLINK_SIZE from config.h, changed to 51
// 20240722 Added LW_STATUS_INTERVAL, 
//          renamed STATUS_INTERVAL to APP_STATUS_INTERVAL
// 20240726 Renamed BATTERY_DISCHARGE_LIMIT/BATTERY_CHARGE_LIMIT
// 20240729 Added PowerFeather specific configuration
// 20240730 Modified PF_SUPPLY_MAINTAIN_VOLTAGE
// 20240804 PowerFeather: Added configuration of max. battery charging current
// 20250209 Added Weather Station 8-in-1
// 20250317 Removed ARDUINO_heltec_wifi_lora_32_V3 and ARDUINO_M5STACK_Core2
//          (now all uppercase)
// 20250318 Renamed PAYLOAD_SIZE to MAX_UPLINK_SIZE
// 20250802 Added BATTERY_RECOVERED
// 20250803 Added support for external RTC chips
// 20250827 Changed BATTERY_LOW to VOLTAGE_CRITICAL
//          Changed BATTERY_WEAK to VOLTAGE_ECO_ENTER/EXIT
//          Removed BATTERY_RECOVERED
// 20250830 Renamed DFROBOT_COVER_LORA to FIREBEETLE_ESP32_COVER_LORA
// 20251017 Added SOC_ECO_ENTER/EXIT
// 20251018 Added SOC_CRITICAL
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_LWCFG_H)
#define _LWCFG_H

#include <stdint.h>

// Enable debug mode (debug messages via serial port)
// Arduino IDE: Tools->Core Debug Level: "Debug|Verbose"
// For other architectures than ESP32, see logging.h

// Enable logging for https://github.com/vshymanskyy/Preferences (used for RP2040)
// #define NVS_LOG

//--- Select Board ---
#if defined(ARDUINO_DFROBOT_FIREBEETLE_ESP32)
// Use pinning for LoRaWAN Node
//#define LORAWAN_NODE

// Use pinning for Firebeetle Cover LoRa
#define FIREBEETLE_ESP32_COVER_LORA
#endif

// PowerFeather specific configuration
struct sPowerFeatherCfg {
  
  uint16_t battery_capacity; /// Battery capacity in mAh
  uint16_t supply_maintain_voltage; /// Supply voltage to maintain in mV
  uint16_t max_charge_current; /// Maximum charging current in mA
  uint8_t soc_eco_enter; /// State of charge (%) to enter eco mode
  uint8_t soc_eco_exit; /// State of charge (%) to exit eco mode
  uint8_t soc_critical; /// State of charge (%) to enter critical mode
  bool temperature_measurement; /// Enable temperature measurement
  bool battery_fuel_gauge; /// Enable battery fuel gauge
};

// M5Stack specific configuration
struct sM5StackCfg {
  uint8_t soc_eco_enter; /// State of charge (%) to enter eco mode
  uint8_t soc_eco_exit; /// State of charge (%) to exit eco mode
  uint8_t soc_critical; /// State of charge (%) to enter critical mode
};

// Uplink message payload size
// The maximum allowed for all data rates is 51 bytes.
const uint8_t MAX_UPLINK_SIZE = 51;

// Maximum downlink payload size (bytes)
const uint8_t MAX_DOWNLINK_SIZE = 51;

// Battery voltage thresholds for energy saving & deep-discharge prevention

// MCU voltage <= VOLTAGE_ECO_ENTER [mV] -> MCU will sleep for SLEEP_INTERVAL_LONG (eco mode)
// MCU voltage > VOLTAGE_ECO_EXIT [mV]   -> MCU will sleep for SLEEP_INTERVAL (normal mode)
// MCU voltage <= BATTERY_CRITICAL [mV]  -> MCU enters sleep mode immediately (battery protection)
#define VOLTAGE_ECO_EXIT 3580
#define VOLTAGE_ECO_ENTER 3500
#define VOLTAGE_CRITICAL 3200

// Battery voltage limits in mV (usable range for the device) for battery state calculation
#define BATTERY_DISCHARGE_LIM 3200
#define BATTERY_CHARGE_LIM 4200

// Minimum sleep interval (in seconds)
#define SLEEP_INTERVAL_MIN 60

// Sleep for SLEEP_INTERVAL seconds after successful transmission
#define SLEEP_INTERVAL 360

// Long sleep interval, MCU will sleep for SLEEP_INTERVAL_LONG seconds if battery voltage <= BATTERY_WEAK
#define SLEEP_INTERVAL_LONG 900

// RTC to network time sync interval (in minutes)
#define CLOCK_SYNC_INTERVAL 24 * 60

// LoRaWAN Node status message interval (in frames)
#define LW_STATUS_INTERVAL 60

// Status message uplink interval (in frames)
#define APP_STATUS_INTERVAL 60

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

// Select one of the external RTC chips supported by Adafruit RTClib (optional)
// https://github.com/adafruit/RTClib
//#define EXT_RTC RTC_DS3231
//#define EXT_RTC RTC_DS1307
//#define EXT_RTC RTC_PCF8523
//#define EXT_RTC RTC_PCF8563

// Enable OneWire temperature measurement
#define ONEWIRE_EN

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
// #define DISTANCESENSOR_CH 8
#endif

// Enable DYP-R01CW Laser Distance Sensor(s)
#if defined(LORAWAN_NODE) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
// #define DYP_R01CW_EN
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
#pragma message("On-board voltage divider must be enabled for supply voltage measurement (see schematic).")
// External Li-Ion Battery connected to solar charger
#define PIN_ADC_IN A3
#elif defined(FIREBEETLE_ESP32_COVER_LORA)
#pragma message("On-board voltage divider must be enabled for battery voltage measurement (see schematic).")
#pragma message("No power-saving & deep-discharge protection implemented yet.")
// On-board VB
#define PIN_ADC_IN A0
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3)
// On-board VB
#define PIN_ADC_IN A0
#elif defined(ARDUINO_ESP32S3_POWERFEATHER)
// See https://docs.powerfeather.dev
#define BATTERY_CAPACITY_MAH 2200 // battery capacity in mAh
#define PF_TEMPERATURE_MEASUREMENT false // enable/disable temperature measurement
#define PF_SUPPLY_MAINTAIN_VOLTAGE 0 // ~maximum power point (MPP) voltage if using a solar panel; 0: disabled
#define PF_BATTERY_FUEL_GAUGE true // enable/disable battery fuel gauge
#define PF_MAX_CHARGE_CURRENT_MAH 50 // maximum charging current in mA
#define SOC_ECO_ENTER 20 // State of charge (%) to enter eco mode
#define SOC_ECO_EXIT 25 // State of charge (%) to exit eco mode
#define SOC_CRITICAL 3 // State of charge (%) to enter critical mode
#if BATTERY_CAPACITY_MAH == 0
#pragma message("Battery capacity set to 0 - battery voltage measurement disabled.")
#endif
// unused
#define PIN_ADC_IN -1
#elif defined(ARDUINO_M5STACK_CORE2)
#define SOC_ECO_ENTER 20 // State of charge (%) to enter eco mode
#define SOC_ECO_EXIT 25 // State of charge (%) to exit eco mode
#define SOC_CRITICAL 3 // State of charge (%) to enter critical mode
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

// "Channel" in appPayloadCfg
#define USUPPLY_CH 1 

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

#ifdef DYP_R01CW_EN
// I2C pins for DYP-R01CW Laser Distance Sensor(s)
#if defined(LORAWAN_NODE)
#define DYP_R01CW_SDA 21
#define DYP_R01CW_SCL 22
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
#define DYP_R01CW_SDA 2
#define DYP_R01CW_SCL 3
#endif

// List of I2C addresses for DYP-R01CW sensors (8-bit format, e.g., 0xE8)
// Default sensor address is 0xE8
// Supported addresses: 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, 
//                      0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
//                      0xF8, 0xFA, 0xFC, 0xFE
#define DYP_R01CW_ADDRESSES \
     {                       \
         0xE8                \
     }
#endif

#ifdef ADC_EN
// Voltage divider R1 / (R1 + R2) -> V_meas = V(R1 + R2); V_adc = V(R1)
#if defined(ARDUINO_THINGPULSE_EPULSE_FEATHER)
const float UBATT_DIV = 0.6812;
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3)
#define ADC_CTRL 37
// R17=100k, R14=390k => 100k / (100k + 390 k)
const float UBATT_DIV = 0.2041;
#else
const float UBATT_DIV = 0.5;
#endif
const uint8_t UBATT_SAMPLES = 10;

// "Channel" appPayloadCfg
#define UBATT_CH 0
#endif

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
// BLE scan time in seconds
#define BLE_SCAN_TIME 31
// BLE scan mode (0: passive / 1: active)
#define BLE_SCAN_MODE 1

// BLE battery o.k. threshold in percent
#define BLE_BATT_OK 5

// List of known sensors' BLE addresses
#define KNOWN_BLE_ADDRESSES \
     {                       \
         "a4:c1:38:b8:1f:7f" \
     }


/*
// Empty list - BLE disabled
#define KNOWN_BLE_ADDRESSES \
    {                       \
                            \
    }
*/
#endif

/// Maximum number of 868 MHz sensors - should match the default configuration below
#define MAX_NUM_868MHZ_SENSORS 5

/// AppLayer payload configuration size in bytes
#define APP_PAYLOAD_CFG_SIZE 24

#define APP_STATUS_SIZE 26

// --- Default AppLayer payload configuration ---

// For each sensor/interface type, there is a set of flags.
// If a flag is set, the "channel" is enabled (according to the flags bit position).
// For sensors which use a fixed channel, the flags are used to select
// which signals (features) shall be included in the payload.

// -- Sensor feature flags --

// Weather Sensor
#define PAYLOAD_WS_HUMIDITY     0b00000010
#define PAYLOAD_WS_WIND         0b00000100
#define PAYLOAD_WS_RAINGAUGE    0b00001000
#define PAYLOAD_WS_LIGHT        0b00010000
#define PAYLOAD_WS_UV           0b00100000
#define PAYLOAD_WS_RAIN_H       0b01000000 // Rain post-processing; hourly rainfall
#define PAYLOAD_WS_RAIN_DWM     0b10000000 // Rain post-processing; daily, weekly, monthly
#define PAYLOAD_WS_TGLOBE       0b0000000100000000

// Lightning sensor
#define PAYLOAD_LIGHTNING_RAW   0b00010000 // Sensor raw data
#define PAYLOAD_LIGHTNING_PROC  0b00100000 // Post-processed lightning data

// -- 868 MHz Sensor Types --
// 0 - Weather Station; 1 Ch
// Note: Included in APP_PAYLOAD_CFG_TYPE01

// Flag: Bit 0: Enable battery_ok flags (to be removed)
#define APP_PAYLOAD_CFG_TYPE00 0x00

// 1 - Weather Station; 1 Ch
//   - Professional Wind Gauge (with T and H); 1 Ch
//   - Professional Rain Gauge (with T); 1 Ch
//     Note: Type encoded as 0x9/0xA/0xB in radio message,
//           but changed to 1 in BresserWeatherSensorReceiver!
#define APP_PAYLOAD_CFG_TYPE01 ( \
    1 /* enable sensor */ | \
    PAYLOAD_WS_HUMIDITY | \
    PAYLOAD_WS_WIND | \
    PAYLOAD_WS_RAINGAUGE | \
    /* PAYLOAD_WS_LIGHT | */ \
    PAYLOAD_WS_UV | \
    PAYLOAD_WS_RAIN_H | \
    PAYLOAD_WS_RAIN_DWM \
)

// 2 - Thermo-/Hygro-Sensor; 7 Ch
// Ch: 1
#define APP_PAYLOAD_CFG_TYPE02 0x02

// 3 - Pool / Spa Thermometer; 7 Ch
// Ch: 1
#define APP_PAYLOAD_CFG_TYPE03 0x00

// 4 - Soil Moisture Sensor; 7 Ch
// Ch: 1
#define APP_PAYLOAD_CFG_TYPE04 0x02

// 5 - Water Leakage Sensor; 7 Ch
// Ch: 1
#define APP_PAYLOAD_CFG_TYPE05 0x00

// 6 - reserved
#define APP_PAYLOAD_CFG_TYPE06 0x00

// 7 - reserved
#define APP_PAYLOAD_CFG_TYPE07 0x00

// 8 - Air Quality Sensor PM2.5/PM10; 4 Ch
#define APP_PAYLOAD_CFG_TYPE08 0x00

// 9 - Lightning Sensor; 1 Ch
// Ch: 0
#define APP_PAYLOAD_CFG_TYPE09 ( \
    1 /* enable sensor */ | \
    /* PAYLOAD_LIGHTNING_RAW | */ \
    PAYLOAD_LIGHTNING_PROC \
)

// 10 - CO2 Sensor; 4 Ch
#define APP_PAYLOAD_CFG_TYPE10 0x00

// 11 - HCHO/VCO Sensor; 4 Ch
#define APP_PAYLOAD_CFG_TYPE11 0x00

// 12 - reserved
#define APP_PAYLOAD_CFG_TYPE12 0x00

// 13 - reserved
#define APP_PAYLOAD_CFG_TYPE13 0x00

// 14 - reserved
#define APP_PAYLOAD_CFG_TYPE14 0x00

// 15 - reserved
#define APP_PAYLOAD_CFG_TYPE15 0x00

// -- 1-Wire Sensors --
// Index: 0
#define APP_PAYLOAD_CFG_ONEWIRE1 0x00 // onewire[15:8]
#define APP_PAYLOAD_CFG_ONEWIRE0 0x01 // onewire[7:0]

// -- Analog Inputs --
// 0x01: Battery Voltage
// 0x02: Supply Voltage
#define APP_PAYLOAD_CFG_ANALOG1 0x00 // analog[15:8]
#define APP_PAYLOAD_CFG_ANALOG0 0x01 // analog[7:0]

// -- Digital Inputs --
// Assign to any type of "channel",
// e.g. GPIO, SPI, I2C, UART, ...
#define APP_PAYLOAD_CFG_DIGITAL3 0x00 // digital[31:24]
#define APP_PAYLOAD_CFG_DIGITAL2 0x00 // digital[23:16]
#define APP_PAYLOAD_CFG_DIGITAL1 0x00 // digital[15:8]
#define APP_PAYLOAD_CFG_DIGITAL0 0x00 // digital[7:0]

#define APP_PAYLOAD_OFFS_ONEWIRE 16
#define APP_PAYLOAD_BYTES_ONEWIRE 2

#define APP_PAYLOAD_OFFS_ANALOG 18
#define APP_PAYLOAD_BYTES_ANALOG 2

#define APP_PAYLOAD_OFFS_DIGITAL 20
#define APP_PAYLOAD_BYTES_DIGITAL 4

#define APP_PAYLOAD_OFFS_BLE 24
#define APP_PAYLOAD_BYTES_BLE 2

// Encoding of invalid values
// for floating point, see
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/NaN
#define INV_FLOAT 0x3FFFFFFF 
#define INV_UINT32 0xFFFFFFFF
#define INV_UINT16 0xFFFF
#define INV_UINT8 0xFF
#define INV_TEMP 327.67

// Time source & status, see below
//
// bits 0..3 time source
//    0x00 = GPS
//    0x01 = RTC
//    0x02 = LORA
//    0x03 = unsynched
//    0x04 = set (source unknown)
//
// bits 4..7 esp32 sntp time status (not used)
enum class E_TIME_SOURCE : uint8_t
{
  E_GPS = 0x00,
  E_RTC = 0x01,
  E_LORA = 0x02,
  E_UNSYNCHED = 0x04,
  E_SET = 0x08
};

#endif // _LWCFG_H
