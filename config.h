#ifndef _CONFIG_H
#define _CONFIG_H

#include <RadioLib.h>
#include "secrets.h"

// How often to send an uplink - consider legal & FUP constraints - see notes
const uint32_t uplinkIntervalSeconds = 5UL * 60UL;    // minutes x seconds

// Maximum downlink payload size (bytes)
#define MAX_DOWNLINK_SIZE 5

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

// See https://github.com/espressif/arduino-esp32/pull/9250
//#define FIREBEETLE_ESP32_COVER_LORA

// Adafruit
#if defined(ARDUINO_FEATHER_ESP32)
    #define PIN_LORA_NSS      14
    #define PIN_LORA_RST      27
    #define PIN_LORA_IRQ      32
    #define PIN_LORA_GPIO     33
    #define PIN_LORA_DIO2     RADIOLIB_NC
    #pragma message("NOT TESTED!!!")
    #pragma message("ARDUINO_FEATHER_ESP32 defined; assuming RFM95W FeatherWing will be used")
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


// Heltec
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32)
  #pragma error ("ARDUINO_HELTEC_WIFI_LORA_32 awaiting pin map")

#elif defined(ARDUINO_heltec_wifi_lora_32_V2)
  // https://github.com/espressif/arduino-esp32/tree/master/variants/heltec_wifi_lora_32_V2/pins_ardiono.h
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_heltec_wifi_lora_32_V2")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_heltec_wifi_32_lora_V3)
  // Use pinning for Heltec WiFi LoRa32 V3
  #define PIN_LORA_NSS   SS
  #define PIN_LORA_RST   RST_LoRa
  #define PIN_LORA_IRQ   DIO0
  #define PIN_LORA_GPIO  BUSY_LoRa
  #pragma message("ARDUINO_heltec_wifi_lora_32_V3")
  #define LORA_CHIP SX1262

#elif defined(ARDUINO_heltec_wireless_stick)
  // https://github.com/espressif/arduino-esp32/blob/master/variants/heltec_wireless_stick/pins_arduino.h
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_heltec_wireless_stick")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_heltec_wireless_stick_v2)
  // https://github.com/espressif/arduino-esp32/blob/master/variants/heltec_wireless_stick/pins_arduino.h
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_heltec_wireless_stick_v2")
  #define LORA_CHIP SX1276

#elif defined(ARDUINO_heltec_wireless_stick_v3)
  https://github.com/espressif/arduino-esp32/tree/master/variants/heltec_wireless_stick_v3
  #define PIN_LORA_NSS      SS
  #define PIN_LORA_RST      RST_LoRa
  #define PIN_LORA_IRQ      DIO0
  #define PIN_LORA_GPIO     DIO1
  #define PIN_LORA_DIO2     DIO2
  #pragma message("ARDUINO_heltec_wireless_stick_v3")
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


#elif defined(FIREBEETLE_ESP32_COVER_LORA)
  // https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478
  // https://wiki.dfrobot.com/FireBeetle_Covers_LoRa_Radio_868MHz_SKU_TEL0125
  #define PIN_LORA_NSS      27 // D4
  #define PIN_LORA_RST      25 // D2
  #define PIN_LORA_IRQ      26 // D3
  #define PIN_LORA_GPIO      9 // D5
  #define PIN_LORA_DIO2     RADIOLIB_NC
  #pragma message("FIREBEETLE_ESP32_COVER_LORA defined; assuming FireBeetle ESP32 with FireBeetle Cover LoRa will be used")
  #pragma message("Required wiring: D2 to RESET, D3 to DIO0, D4 to CS, D5 to DIO1")
  #define LORA_CHIP SX1276


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

#else
  #pragma message ("Unknown board - no automagic pinmap available")

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


// Helper function to display any issues
void debug(bool isFail, const __FlashStringHelper* message, int state, bool Freeze) {
  if (isFail) {
    Serial.print(message);
    Serial.print("(");
    Serial.print(state);
    Serial.println(")");
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