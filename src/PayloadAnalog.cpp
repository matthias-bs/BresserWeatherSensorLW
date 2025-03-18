///////////////////////////////////////////////////////////////////////////////
// PayloadAnalog.cpp
//
// Read analog input channels and encode as LoRaWAN payload
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
// 20240521 Created
// 20240524 Added payload size check, changed bitmap order
// 20240528 Changesd order of channels, fixed log messages
// 20250318 Renamed PAYLOAD_SIZE to MAX_UPLINK_SIZE
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadAnalog.h"


void PayloadAnalog::begin(void)
{

}

void PayloadAnalog::encodeAnalog(uint8_t *appPayloadCfg, LoraEncoder &encoder)
{
    unsigned ch = 0;
    for (int i = APP_PAYLOAD_BYTES_ANALOG - 1; i >= 0; i--)
    {
        for (uint8_t bit = 0; bit <= 7; bit++)
        {
            // Check if channel is enabled
            if ((appPayloadCfg[APP_PAYLOAD_OFFS_ANALOG + i] >> bit) & 0x1) {
                if ((ch == UBATT_CH) && (encoder.getLength() <= MAX_UPLINK_SIZE - 2))
                {
                    uint16_t uBatt = getBatteryVoltage();
                    log_i("ch %02u: U_batt: %04u mv", ch, uBatt);
                    encoder.writeUint16(uBatt);
                }

                if ((ch == USUPPLY_CH) && (encoder.getLength() <= MAX_UPLINK_SIZE - 2))
                {
                    uint16_t uSupply = getSupplyVoltage();
                    log_i("ch %02u: U_supply: %04u mv", ch, uSupply);
                    encoder.writeUint16(uSupply);
                }
            }
            ch++;
        }
    }
}

