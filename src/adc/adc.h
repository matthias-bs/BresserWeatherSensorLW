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
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(_ADC_H)
#define _ADC_H

#include <Arduino.h>
#include "../../BresserWeatherSensorLWCfg.h"


/*!
 * \brief Get supply / battery voltage
 * 
 * Returns a voltage measurement with oversampling and divider
 * 
 * \returns Voltage in mV
 */

uint16_t getVoltage(uint8_t pin = PIN_ADC_IN, uint8_t samples = UBATT_SAMPLES, float divider = UBATT_DIV);


/*!
 * \brief Get battery voltage
 * 
 * Returns the battery voltage or zero if not available (board specific)
 * 
 * \returns Voltage in mV or zero if not available
 */
uint16_t getBatteryVoltage(void);

/*!
 * \brief Get supply voltage
 * 
 * Returns the battery voltage or zero if not available (board specific)
 * 
 * \returns Voltage in mV or zero if not available
 */

uint16_t getSupplyVoltage(void);

#endif // _ADC_H