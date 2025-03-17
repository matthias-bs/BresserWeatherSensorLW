//////////////////////////////////////////////////////////////////////////////////////////////////
// adc.cpp
//
// Analog/Digital Converter wrapper/convenience functions
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
// History:
// 20240405 Created
// 20240410 Added RP2040 specific implementation
// 20240413 Refactored ADC handling
// 20240414 Added ESP32-S3 PowerFeather
//          Added getSupplyVoltage()
// 20240423 Added define ARDUINO_heltec_wifi_lora_32_V3
// 20240427 Added ADC input control and battery voltage measurement
//          for ARDUINO_heltec_wifi_lora_32_V3
// 20240430 Modified getBatteryVoltage()
// 20240504 Heltec WiFi 32 LoRa V3: Changed ADC input attenuation to get higher accuracy
// 20240607 Changed ARDUINO_HELTEC_WIFI_LORA_32_V3 to uppercase
// 20241203 Fixed getVoltage(): use parameter 'pin' instead of PIN_ADC_IN
// 20250317 Removed ARDUINO_heltec_wifi_lora_32_V3 and ARDUINO_M5STACK_Core2 (now all uppercase)
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "adc.h"
#include "../logging.h"

#if defined(ARDUINO_M5STACK_CORE2)
#include <M5Unified.h>
#elif defined(ARDUINO_ESP32S3_POWERFEATHER)
#include <PowerFeather.h>
using namespace PowerFeather;
#endif

//
// Get voltage
//
uint16_t
getVoltage(uint8_t pin, uint8_t samples, float div)
{
  float voltage_raw = 0;
  for (uint8_t i = 0; i < UBATT_SAMPLES; i++)
  {
#if defined(ESP32)
    voltage_raw += float(analogReadMilliVolts(pin));
#else
    voltage_raw += float(analogRead(pin)) / 4095.0 * 3300;
#endif
  }
  uint16_t voltage = int(voltage_raw / UBATT_SAMPLES / UBATT_DIV);

  log_d("Voltage @GPIO%02d = %dmV", pin, voltage);

  return voltage;
}

uint16_t getBatteryVoltage(void)
{
#if defined(ARDUINO_TTGO_LoRa32_V1) || defined(ARDUINO_TTGO_LoRa32_V2) || defined(ARDUINO_TTGO_LoRa32_v21new) || \
    defined(ARDUINO_FEATHER_ESP32) || defined(LORAWAN_NODE) || defined(FIREBEETLE_ESP32_COVER_LORA) ||           \
    defined(ARDUINO_THINGPULSE_EPULSE_FEATHER)
  // Here come the good guys...
  return getVoltage();

#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3)
     // Enable ADC input switch, measure voltage and disable ADC input switch
    uint16_t voltage;
    pinMode(ADC_CTRL, OUTPUT);
    digitalWrite(ADC_CTRL, LOW);
    analogSetPinAttenuation(PIN_ADC_IN, ADC_0db);
    delay(100);
    voltage = getVoltage();
    pinMode(ADC_CTRL, INPUT);
    return voltage;

#elif defined(ARDUINO_ARCH_RP2040)
  // Not implemented - no default VBAT input circuit (connect external divider to A0)
  return 0;

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  // Not implemented - no default VBAT input circuit (connect external divider to A0)
  return 0;

#elif defined(ARDUINO_M5STACK_CORE2)
  // battery monitoring chip
  uint16_t voltage = M5.Power.getBatteryVoltage();
  log_d("Voltage = %dmV", voltage);
  return voltage;

#elif defined(ARDUINO_ESP32S3_POWERFEATHER)
  // battery monitoring chip
  uint16_t voltage;
  Result res = Board.getBatteryVoltage(voltage);
  if (res == Result::Ok)
  {
    log_d("Voltage = %dmV", voltage);
    return voltage;
  }
  else
  {
    return 0;
  }

#else
  // Unknown implementation - zero indicates battery voltage measurement not available
  return 0;

#endif
}

uint16_t getSupplyVoltage(void)
{
#if defined(ARDUINO_ESP32S3_POWERFEATHER)
  uint16_t voltage;
  Result res = Board.getSupplyVoltage(voltage);
  if (res == Result::Ok)
  {
    log_d("Voltage = %dmV", voltage);
    return voltage;
  }
  else
  {
    return 0;
  }
#elif defined(PIN_SUPPLY_IN)
  return getVoltage(PIN_SUPPLY_IN, SUPPLY_SAMPLES, SUPPLY_DIV);
#else
  return 0;
#endif
}
