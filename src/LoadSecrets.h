///////////////////////////////////////////////////////////////////////////////
// LoadSecrets.h
//
// Load LoRaWAN secrets from file 'secrets.json' on LittleFS, if available
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
// 20240723 Created from BresserWeatherSensorLW.ino
// 20240928 Refactoring & modification for LoRaWAN v1.0.4 (requires no nwkKey)
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
 * \brief Load LoRaWAN secrets from file 'secrets.json' on LittleFS, if available
 *
 * Returns all values by reference/pointer
 *
 * Use https://github.com/earlephilhower/arduino-littlefs-upload for uploading
 * the file to Flash.
 *
 * \param requireNwkKey
 * \param joinEUI
 * \param devEUI
 * \param nwkKey
 * \param appKey
 */
void loadSecrets(bool requireNwkKey, uint64_t &joinEUI, uint64_t &devEUI, uint8_t *nwkKey, uint8_t *appKey);