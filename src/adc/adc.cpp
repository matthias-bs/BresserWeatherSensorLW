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
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "adc.h"

#ifdef ADC_EN

#ifdef ESP32
// ESP32 ADC with calibration
ESP32AnalogRead adc; //!< ADC object for supply voltage measurement

// ESP32 ADC with calibration
#if defined(PIN_ADC0_IN)
ESP32AnalogRead adc0; //!< ADC object
#endif
#if defined(PIN_ADC1_IN)
ESP32AnalogRead adc1; //!< ADC object
#endif
#if defined(PIN_ADC2_IN)
ESP32AnalogRead adc2; //!< ADC object
#endif
#if defined(PIN_ADC3_IN)
ESP32AnalogRead adc3; //!< ADC object
#endif
#endif
//
// Get supply / battery voltage
//
uint16_t
getVoltage(void)
{
    float voltage_raw = 0;
    for (uint8_t i = 0; i < UBATT_SAMPLES; i++)
    {
#ifdef ESP32
        voltage_raw += float(adc.readMiliVolts());
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
    return getVoltage();
}
//
// Get an additional voltage
//
#if defined(ESP32)
uint16_t
getVoltage(ESP32AnalogRead &adc, uint8_t samples, float divider)
{
    float voltage_raw = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        voltage_raw += float(adc.readMiliVolts());
    }
    uint16_t voltage = int(voltage_raw / samples / divider);

    log_d("Voltage = %dmV", voltage);

    return voltage;
}
#else
uint16_t
getVoltage(pin_size_t pin, uint8_t samples, float divider)
{
    float voltage_raw = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        voltage_raw += float(analogRead(pin)) / 4095.0 * 3.3;
    }
    uint16_t voltage = int(voltage_raw / samples / divider);

    log_d("Voltage = %dmV", voltage);

    return voltage;
}
#endif
#else
uint16_t getBatteryVoltage(void)
{
    return 0;
}
#endif

