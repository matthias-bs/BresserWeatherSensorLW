///////////////////////////////////////////////////////////////////////////////
// PayloadOneWire.h
//
// Get 1-Wire temperature sensor values and encode as LoRaWAN payload
//
// created: 05/2024
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
// 20240520 Created
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadOneWire.h"

#ifdef ONEWIRE_EN

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
static OneWire oneWire(PIN_ONEWIRE_BUS); //!< OneWire bus

// Pass our oneWire reference to Dallas Temperature.
static DallasTemperature owTempSensors(&oneWire); //!< Dallas temperature sensors connected to OneWire bus

// Get temperature from Maxim OneWire Sensor
float PayloadOneWire::getOneWireTemperature(uint8_t index)
{
    // Call sensors.requestTemperatures() to issue a global temperature
    // request to all devices on the bus
    owTempSensors.requestTemperatures();

    // Get temperature by index
    float tempC = owTempSensors.getTempCByIndex(index);

    // Check if reading was successful
    if (tempC != DEVICE_DISCONNECTED_C)
    {
        log_d("Temperature = %.2f°C", tempC);
    }
    else
    {
        log_d("Error: Could not read temperature data");
    }

    return tempC;
};

// Encode 1-Wire temperature sensor values for LoRaWAN transmission
void PayloadOneWire::encodeOneWire(uint8_t *appPayloadCfg, LoraEncoder &encoder)
{

    unsigned index = (APP_PAYLOAD_BYTES_ONEWIRE * 8) - 1;
    for (int i = APP_PAYLOAD_BYTES_ONEWIRE - 1; i >= 0; i--)
    {
        for (int bit = 7; bit >= 0; bit--)
        {
            // Check if sensor with given index is enabled
            if ((appPayloadCfg[APP_PAYLOAD_OFFS_ONEWIRE + i] >> bit) & 0x1)
            {
                // Get temperature by index
                float tempC = owTempSensors.getTempCByIndex(index);

                // Check if reading was successful
                if (tempC != DEVICE_DISCONNECTED_C)
                {
                    log_d("Temperature[%d] = %.2f°C", index, tempC);
                }
                else
                {
                    log_d("Error: Could not read temperature[%d] data", index);
                }
                
                encoder.writeTemperature(tempC);
            }
            index--;
        }
    }
}
#endif