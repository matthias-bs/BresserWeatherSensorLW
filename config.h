///////////////////////////////////////////////////////////////////////////////
// config.h
// 
// RadioLib / LoRaWAN specific configuration including radio module wiring
//
// based on https://github.com/radiolib-org/radiolib-persistence
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
// 20240412 Created
// 20240413 Added ESP32-S3 PowerFeather
// 20240426 Added define ARDUINO_heltec_wifi_lora_32_V3
// 20240530 Added stateDecode(), updated debug() from RadioLib v6.6.0
//          (examples/LoRaWAN/LoRaWAN_Reference/config.h)
// 20240613 Added LORAWAN_NODE (DFRobot FireBeetle ESP32 wiring variant)
// 20240704 Moved MAX_DOWNLINK_SIZE to BresserWeatherSensorLWCfg.h
// 20240710 Fixed pragma messages fro Firebeetle ESP32 pin config
// 20240922 Bumped to RadioLib v7.0.0
//
// ToDo:
// - 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_H
#define _CONFIG_H

#include <RadioLib.h>
#include "secrets.h"
#include "BresserWeatherSensorLWCfg.h"

// How often to send an uplink - consider legal & FUP constraints - see notes
const uint32_t uplinkIntervalSeconds = 5UL * 60UL;    // minutes x seconds

// JoinEUI - previous versions of LoRaWAN called this AppEUI
// for development purposes you can use all zeros - see wiki for details
#define RADIOLIB_LORAWAN_JOIN_EUI  0x0000000000000000

// The Device EUI & two keys can be generated on the TTN console 
#ifndef RADIOLIB_LORAWAN_DEV_EUI   // Replace with your Device EUI
#define RADIOLIB_LORAWAN_DEV_EUI   0x---------------
#endif
#ifndef RADIOLIB_LORAWAN_APP_KEY   // Replace with your App Key 
#define RADIOLIB_LORAWAN_APP_KEY   0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- 
#endif
#ifndef RADIOLIB_LORAWAN_NWK_KEY   // Put your Nwk Key here
#define RADIOLIB_LORAWAN_NWK_KEY   0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x--, 0x-- 
#endif

// For the curious, the #ifndef blocks allow for automated testing &/or you can
// put your EUI & keys in to your platformio.ini - see wiki for more tips



// Regional choices: EU868, US915, AU915, AS923, IN865, KR920, CN780, CN500
const LoRaWANBand_t Region = EU868;
const uint8_t subBand = 0;  // For US915, change this to 2, otherwise leave on 0


// ============================================================================
// Below is to support the sketch - only make changes if the notes say so ...

// Auto select MCU <-> radio connections
// If you get an error message when compiling, it may be that the 
// pinmap could not be determined - see the notes for more info


// Adafruit
#if defined(ARDUINO_FEATHER_ESP32) || defined(ARDUINO_THINGPULSE_EPULSE_FEATHER)
    #define PIN_LORA_NSS      14
    #define PIN_LORA_RST      27
    #define PIN_LORA_IRQ      32
    #define PIN_LORA_GPIO     33
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("NOT TESTED!!!")
    #pragma message("ARDUINO_FEATHER_ESP32/ARDUINO_THINGPULSE_EPULSE_FEATHER defined; assuming RFM95W FeatherWing will be used")
    #pragma message("Required wiring: A to RST, B to DIO1, D to DIO0, E to CS")
    #define LORA_CHIP SX1276

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
    // Use pinning for Adafruit Feather ESP32S2 with RFM95W "FeatherWing" ADA3232
    #define PIN_LORA_NSS      6
    #define PIN_LORA_RST      9
    #define PIN_LORA_IRQ      5
    #define PIN_LORA_GPIO     11
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("NOT TESTED!!!")
    #pragma message("ARDUINO_ADAFRUIT_FEATHER_ESP32S2 defined; assuming RFM95W FeatherWing will be used")
    #pragma message("Required wiring: A to RST, B to DIO1, D to DIO0, E to CS")
    #define LORA_CHIP SX1276

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32_V2)
    #define PIN_LORA_NSS      14
    #define PIN_LORA_RST      27
    #define PIN_LORA_IRQ      32
    #define PIN_LORA_GPIO     33
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("NOT TESTED!!!")
    #pragma message("ARDUINO_ADAFRUIT_FEATHER_ESP32_V2 defined; assuming RFM95W FeatherWing will be used")
    #pragma message("Required wiring: A to RST, B to DIO1, D to DIO0, E to CS")
    #define LORA_CHIP SX1276

#elif defined(ARDUINO_ESP32S3_POWERFEATHER)
    #define PIN_LORA_NSS      15
    #define PIN_LORA_RST      45
    #define PIN_LORA_IRQ      16
    #define PIN_LORA_GPIO     18
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("ARDUINO_ESP32S3_POWERFEATHER defined; assuming RFM95W FeatherWing will be used")
    #pragma message("Required wiring: A to RST, B to DIO1, D to DIO0, E to CS")
    #define LORA_CHIP SX1276

#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  // Use pinning for Adafruit Feather RP2040 with RFM95W "FeatherWing" ADA3232
  #define PIN_LORA_NSS      7
  #define PIN_LORA_RST     11
  #define PIN_LORA_IRQ     8
  #define PIN_LORA_GPIO    10
  #pragma message("ARDUINO_ADAFRUIT_FEATHER_RP2040 defined; assuming RFM95W FeatherWing will be used")
  #pragma message("Required wiring: A to RST, B to DIO1, D to DIO0, E to CS")
  #define LORA_CHIP SX1276

// LilyGo 
#elif defined(ARDUINO_TTGO_LORA32_V1) || defined(ARDUINO_TTGO_LoRa32_V1)
  // https://github.com/espressif/arduino-esp32/blob/master/variants/ttgo-lora32-v1/pins_arduino.h
  // http://www.lilygo.cn/prod_view.aspx?TypeId=50003&Id=1130&FId=t3:50003:3
  // https://github.com/Xinyuan-LilyGo/TTGO-LoRa-Series
  // https://github.com/LilyGO/TTGO-LORA32/blob/master/schematic1in6.pdf
  #define PIN_LORA_NSS      LORA_CS
  #define PIN_LORA_RST      LORA_RST
  #define PIN_LORA_IRQ      LORA_IRQ
  #define PIN_LORA_GPIO     33
  #define PIN_LORA_DIO2     RADIOLIB_NC
  #pragma message ("TTGO LoRa32 v1 - no Display")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_TTGO_LORA32_V2) || defined(ARDUINO_TTGO_LoRa32_V2)
  // https://github.com/espressif/arduino-esp32/blob/master/variants/ttgo-lora32-v2/pins_arduino.h
  #define PIN_LORA_NSS      LORA_CS
  #define PIN_LORA_RST      LORA_RST
  #define PIN_LORA_IRQ      LORA_IRQ
  #define PIN_LORA_GPIO     RADIOLIB_NC
  #define PIN_LORA_DIO2     RADIOLIB_NC
  #define LORA_CHIP SX1276
  #pragma message ("TTGO_LoRa32_V2 + Display")

#elif defined(ARDUINO_TTGO_LoRa32_v21new)
  // T3_V1.6.1
  #define PIN_LORA_NSS      LORA_CS
  #define PIN_LORA_RST      LORA_RST
  #define PIN_LORA_IRQ      LORA_IRQ
  #define PIN_LORA_GPIO     LORA_D1
  #define PIN_LORA_DIO2     RADIOLIB_NC
  #pragma message ("Using TTGO LoRa32 v2.1 marked T3_V1.6.1 + Display")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_TBEAM_USE_RADIO_SX1262)
  #pragma error ("ARDUINO_TBEAM_USE_RADIO_SX1262 awaiting pin map")
  #define LORA_CHIP SX1262

#elif defined(ARDUINO_TBEAM_USE_RADIO_SX1276)
  #pragma error ("ARDUINO_TBEAM_USE_RADIO_SX1276 awaiting pin map")
  #define LORA_CHIP SX1276

// AZ-Delivery
#elif defined(ARDUINO_D1_MINI32)
  // ESP32-WROOM-32 
  #define PIN_LORA_NSS      27
  #define PIN_LORA_RST      32
  #define PIN_LORA_IRQ      21
  #define PIN_LORA_GPIO     33
  #define PIN_LORA_DIO2     RADIOLIB_NC
  #pragma message("wemos_d1_mini32 - WEMOS D1 MINI ESP32 defined; assuming RFM95W will be used")
  #define LORA_CHIP SX1276

// Heltec
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32)
  #pragma error ("ARDUINO_HELTEC_WIFI_LORA_32 awaiting pin map")

#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
  // https://github.com/espressif/arduino-esp32/tree/master/variants/heltec_wifi_lora_32_V2/pins_ardiono.h
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_HELTEC_WIFI_LORA_32_V2")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3) || defined(ARDUINO_heltec_wifi_lora_32_V3)
  // Use pinning for Heltec WiFi LoRa32 V3
  #define PIN_LORA_NSS   SS
  #define PIN_LORA_RST   RST_LoRa
  #define PIN_LORA_IRQ   DIO0
  #define PIN_LORA_GPIO  BUSY_LoRa
  #pragma message("ARDUINO_HELTEC_WIFI_LORA_32_V3")
  #define LORA_CHIP SX1262

#elif defined(ARDUINO_HELTEC_WIRELESS_STICK)
  // https://github.com/espressif/arduino-esp32/blob/master/variants/heltec_wireless_stick/pins_arduino.h
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_HELTEC_WIRELESS_STICK")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_HELTEC_WIRELESS_STICK_V3)
  https://github.com/espressif/arduino-esp32/tree/master/variants/heltec_wireless_stick_v3
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_HELTEC_WIRELESS_STICK_V3")
  #define LORA_CHIP SX1262

// #elif defined(ARDUINO_heltec_wifi_kit_32_V2)
//  Presumably no LoRa chip
//  #pragma message ("ARDUINO_heltec_wifi_kit_32_V2 awaiting pin map")
//  #define LORA_CHIP USE_SX1276
//  SX1276 radio = new Module(18, 26, 14, 35);

// #elif defined(ARDUINO_heltec_wifi_kit_32_V3)
//  Presumably no LoRa chip
//   #pragma message ("Using Heltec WiFi LoRa32 v3 - Display + USB-C")
//   #define LORA_CHIP USE_SX1262
//   SX1262 radio = new Module(8, 14, 12, 13);

#elif defined(ARDUINO_CUBECELL_BOARD)
  #pragma error ("ARDUINO_CUBECELL_BOARD awaiting pin map")
  #define LORA_CHIP SX1262
  //SX1262 radio = new Module(RADIOLIB_BUILTIN_MODULE);

#elif defined(ARDUINO_CUBECELL_BOARD_V2)
  #pragma error ("ARDUINO_CUBECELL_BOARD_V2 awaiting pin map")


#elif defined(ARDUINO_M5STACK_CORE2) || defined(ARDUINO_M5STACK_Core2)
  // Note: Depending on board package file date, either variant is used - 
  //       see https://github.com/espressif/arduino-esp32/issues/9423!
  #define PIN_LORA_NSS      33
  #define PIN_LORA_RST      26
  #define PIN_LORA_IRQ      36
  #define PIN_LORA_GPIO     RADIOLIB_NC
  //#define PIN_LORA_GPIO     35 // manual connection  - only required for LMIC
  #pragma message("ARDUINO_M5STACK_CORE2 defined; assuming M5Stack Module LoRa868 will be used")
  #define LORA_CHIP SX1276


#elif defined(ARDUINO_DFROBOT_FIREBEETLE_ESP32)
  #if defined(LORAWAN_NODE)
    // Use pinning for LoRaWAN_Node (https://github.com/matthias-bs/LoRaWAN_Node)
    #define PIN_LORA_NSS      14
    #define PIN_LORA_RST      12
    #define PIN_LORA_IRQ       4
    #define PIN_LORA_GPIO     16
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("ARDUINO_DFROBOT_FIREBEETLE_ESP32 & LORAWAN_NODE defined; assuming this is the LoRaWAN_Node board (DFRobot Firebeetle32 + Adafruit RFM95W LoRa Radio)")
  #elif defined(DFROBOT_COVER_LORA)
    // https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478
    // https://wiki.dfrobot.com/FireBeetle_Covers_LoRa_Radio_868MHz_SKU_TEL0125
    #define PIN_LORA_NSS      27 // D4
    #define PIN_LORA_RST      25 // D2
    #define PIN_LORA_IRQ      26 // D3
    #define PIN_LORA_GPIO      9 // D5
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("ARDUINO_DFROBOT_FIREBEETLE_ESP32 & DFROBOT_COVER_LORA defined; assuming this is a FireBeetle ESP32 with FireBeetle Cover LoRa")
    #pragma message("Required wiring: D2 to RESET, D3 to DIO0, D4 to CS, D5 to DIO1")
  #else 
    #pragma message("Either LORAWAN_NODE or DFROBOT_COVER_LORA must be defined")
  #endif

  #define LORA_CHIP SX1276

#else
  #pragma message ("Unknown board - no automagic pinmap available")

  // Using arbitrary settings for CI workflow with FQBN esp32:esp32:esp32
  // LoRaWAN_Node board
  // https://github.com/matthias-bs/LoRaWAN_Node
  #define PIN_LORA_NSS      14
  #define PIN_LORA_RST      12
  #define PIN_LORA_IRQ       4
  #define PIN_LORA_GPIO     16
  #define LORA_CHIP SX1276

  // SX1262  pin order: Module(NSS/CS, DIO1, RESET, BUSY);
  // SX1262 radio = new Module(8, 14, 12, 13);

  // SX1278 pin order: Module(NSS/CS, DIO0, RESET, DIO1);
  // SX1278 radio = new Module(10, 2, 9, 3);

#endif

LORA_CHIP radio = new Module(PIN_LORA_NSS, PIN_LORA_IRQ, PIN_LORA_RST, PIN_LORA_GPIO);

// Copy over the EUI's & keys in to the something that will not compile if incorrectly formatted
uint64_t joinEUI =   RADIOLIB_LORAWAN_JOIN_EUI;
uint64_t devEUI  =   RADIOLIB_LORAWAN_DEV_EUI;
uint8_t appKey[] = { RADIOLIB_LORAWAN_APP_KEY };
uint8_t nwkKey[] = { RADIOLIB_LORAWAN_NWK_KEY };

// Create the LoRaWAN node
LoRaWANNode node(&radio, &Region, subBand);

// result code to text ...
String stateDecode(const int16_t result) {
  switch (result) {
  case RADIOLIB_ERR_NONE:
    return "ERR_NONE";
  case RADIOLIB_ERR_CHIP_NOT_FOUND:
    return "ERR_CHIP_NOT_FOUND";
  case RADIOLIB_ERR_PACKET_TOO_LONG:
    return "ERR_PACKET_TOO_LONG";
  case RADIOLIB_ERR_RX_TIMEOUT:
    return "ERR_RX_TIMEOUT";
  case RADIOLIB_ERR_CRC_MISMATCH:
    return "ERR_CRC_MISMATCH";
  case RADIOLIB_ERR_INVALID_BANDWIDTH:
    return "ERR_INVALID_BANDWIDTH";
  case RADIOLIB_ERR_INVALID_SPREADING_FACTOR:
    return "ERR_INVALID_SPREADING_FACTOR";
  case RADIOLIB_ERR_INVALID_CODING_RATE:
    return "ERR_INVALID_CODING_RATE";
  case RADIOLIB_ERR_INVALID_FREQUENCY:
    return "ERR_INVALID_FREQUENCY";
  case RADIOLIB_ERR_INVALID_OUTPUT_POWER:
    return "ERR_INVALID_OUTPUT_POWER";
  case RADIOLIB_ERR_NETWORK_NOT_JOINED:
	  return "RADIOLIB_ERR_NETWORK_NOT_JOINED";

  case RADIOLIB_ERR_DOWNLINK_MALFORMED:
    return "RADIOLIB_ERR_DOWNLINK_MALFORMED";
  case RADIOLIB_ERR_INVALID_REVISION:
    return "RADIOLIB_ERR_INVALID_REVISION";
  case RADIOLIB_ERR_INVALID_PORT:
    return "RADIOLIB_ERR_INVALID_PORT";
  case RADIOLIB_ERR_NO_RX_WINDOW:
    return "RADIOLIB_ERR_NO_RX_WINDOW";
  case RADIOLIB_ERR_INVALID_CID:
    return "RADIOLIB_ERR_INVALID_CID";
  case RADIOLIB_ERR_UPLINK_UNAVAILABLE:
    return "RADIOLIB_ERR_UPLINK_UNAVAILABLE";
  case RADIOLIB_ERR_COMMAND_QUEUE_FULL:
    return "RADIOLIB_ERR_COMMAND_QUEUE_FULL";
  case RADIOLIB_ERR_COMMAND_QUEUE_ITEM_NOT_FOUND:
    return "RADIOLIB_ERR_COMMAND_QUEUE_ITEM_NOT_FOUND";
  case RADIOLIB_ERR_JOIN_NONCE_INVALID:
    return "RADIOLIB_ERR_JOIN_NONCE_INVALID";
  case RADIOLIB_ERR_N_FCNT_DOWN_INVALID:
    return "RADIOLIB_ERR_N_FCNT_DOWN_INVALID";
  case RADIOLIB_ERR_A_FCNT_DOWN_INVALID:
    return "RADIOLIB_ERR_A_FCNT_DOWN_INVALID";
  case RADIOLIB_ERR_DWELL_TIME_EXCEEDED:
    return "RADIOLIB_ERR_DWELL_TIME_EXCEEDED";
  case RADIOLIB_ERR_CHECKSUM_MISMATCH:
    return "RADIOLIB_ERR_CHECKSUM_MISMATCH";
  case RADIOLIB_ERR_NO_JOIN_ACCEPT:
    return "RADIOLIB_ERR_NO_JOIN_ACCEPT";
  case RADIOLIB_LORAWAN_SESSION_RESTORED:
    return "RADIOLIB_LORAWAN_SESSION_RESTORED";
  case RADIOLIB_LORAWAN_NEW_SESSION:
    return "RADIOLIB_LORAWAN_NEW_SESSION";
  case RADIOLIB_ERR_NONCES_DISCARDED:
    return "RADIOLIB_ERR_NONCES_DISCARDED";
  case RADIOLIB_ERR_SESSION_DISCARDED:
    return "RADIOLIB_ERR_SESSION_DISCARDED";
  }
  return "See TypeDef.h";
}

// Helper function to display any issues
void debug(bool isFail, const char* message, int state, bool Freeze) {
  if (isFail) {
    log_w("%s - %s (%d)", message, stateDecode(state).c_str(), state);
    while (Freeze);
  }
}

// Helper function to display a byte array
void arrayDump(uint8_t *buffer, uint16_t len) {
  for (uint16_t c = 0; c < len; c++) {
    Serial.printf("%02X", buffer[c]);
  }
  Serial.println();
}

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#pragma message("Radio chip: " STR(LORA_CHIP))
#pragma message("Pin config: NSS->" STR(PIN_LORA_NSS) ", IRQ->" STR(PIN_LORA_IRQ) ", RST->" STR(PIN_LORA_RST) ", GPIO->" STR(PIN_LORA_GPIO) )


#endif
