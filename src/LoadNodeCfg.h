///////////////////////////////////////////////////////////////////////////////
// LoadNodeCfg.cpp
//
// Load LoRaWAN node configuration 'node_config.json' from LittleFS, if available
//
// This configuration file is intended for hardware/deployment environment
// specific settings (e.g. battery voltage levels, timezone)
//
// created: 07/2024
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
// 20240725 Created
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "logging.h"

/*!
 * \brief Load LoRaWAN node configuration 'node_config.json' from LittleFS, if available
 *
 * Returns all values by reference. Keeps the original value(s) if file not found, 
 * cannot be parsed or any value is missing.
 *
 * JSON file format:
 * {
 *   "timezone": "CET-1CEST,M3.5.0,M10.5.0/3",
 *   "battery_weak": 3300,
 *   "battery_low": 3400,
 *   "battery_discharge_lim": 3200,
 *   "battery_charge_lim": 4200,
 *   "battery_capacity": 2200
 * }
 */
void loadNodeCfg(
    String &tzinfo,
    uint16_t &batt_weak,
    uint16_t &batt_low,
    uint16_t &batt_discharge_lim,
    uint16_t &batt_charge_lim,
    uint16_t &batt_capacity);