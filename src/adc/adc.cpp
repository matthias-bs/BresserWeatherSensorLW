//////////////////////////////////////////////////////////////////////////////////////////////////
// adc.h
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
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "adc.h"
#include "../logging.h"

#if defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORE2)
#include <M5Unified.h>
#elif defined(ARDUINO_ESP32S3_POWERFEATHER)
#include <PowerFeather.h>
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
        voltage_raw += float(analogReadMilliVolts(PIN_ADC_IN));
#else
        voltage_raw += float(analogRead(PIN_ADC_IN)) / 4095.0 * 3300;
#endif
    }
    uint16_t voltage = int(voltage_raw / UBATT_SAMPLES / UBATT_DIV);

    log_d("Voltage = %dmV", voltage);

    return voltage;
}


uint16_t getBatteryVoltage(void)
{
    #if defined(ARDUINO_ARCH_RP2040)
    // Not implemented - no default VBAT input circuit (connect external divider to A0)
    return 0;
    #elif defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORE2)
    uint16_t voltage = M5.Power.getBatteryVoltage();
    log_d("Voltage = %dmV", voltage);
    return voltage;
    #elif defined(ADC_EN)
    return getVoltage();
    #else
    return 0;
    #endif
}

