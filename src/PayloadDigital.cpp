///////////////////////////////////////////////////////////////////////////////
// PayloadDigital.cpp
//
// Read digital input channels and encode as LoRaWAN payload
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
// 20240524 Added payload size check, changed bitmap order
// 20250318 Renamed PAYLOAD_SIZE to MAX_UPLINK_SIZE
// 20260210 Refactored sensor integration for cleaner separation
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadDigital.h"

void PayloadDigital::begin(void)
{
#ifdef DISTANCESENSOR_EN
    m_distanceSensor = new DistanceSensor();
    m_distanceSensor->begin();
#endif
}

void PayloadDigital::encodeDigital(uint8_t *appPayloadCfg, LoraEncoder &encoder)
{
    unsigned ch = (APP_PAYLOAD_BYTES_DIGITAL * 8) - 1;
    for (int i = APP_PAYLOAD_BYTES_DIGITAL - 1; i >= 0; i--)
    {
        for (uint8_t bit = 0; bit <= 7; bit++)
        {
            if ((appPayloadCfg[APP_PAYLOAD_OFFS_DIGITAL + i] >> bit) & 0x1)
            {
#ifdef DISTANCESENSOR_EN
                // Check if channel is enabled
                if ((ch == DISTANCESENSOR_CH) && (encoder.getLength() <= MAX_UPLINK_SIZE - 2))
                {
                    uint16_t distance_mm = m_distanceSensor->read();
                    if (distance_mm > 0)
                    {
                        log_i("ch %02u: Distance:          %4d mm", ch, distance_mm);
                    }
                    else
                    {
                        log_i("ch %02u: Distance:         ---- mm", ch);
                    }
                    encoder.writeUint16(distance_mm);
                }
#endif
            }
            ch--;
        }
    }
}
