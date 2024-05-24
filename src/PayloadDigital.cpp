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
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadDigital.h"

#ifdef DISTANCESENSOR_EN
#if defined(ESP32)
/// Ultrasonic distance sensor
static DistanceSensor_A02YYUW distanceSensor(&Serial2);
#else
/// Ultrasonic distance sensor
static DistanceSensor_A02YYUW distanceSensor(&Serial1);
#endif
#endif

void PayloadDigital::begin(void)
{
#ifdef DISTANCESENSOR_EN
    initDistanceSensor();
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
                if ((ch == DISTANCESENSOR_CH) && (encoder.getLength() <= PAYLOAD_SIZE - 2))
                {
                    uint16_t distance_mm = readDistanceSensor();
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

#ifdef DISTANCESENSOR_EN
void PayloadDigital::initDistanceSensor(void)
{
#if defined(ESP32)
    Serial2.begin(9600, SERIAL_8N1, DISTANCESENSOR_RX, DISTANCESENSOR_TX);
    pinMode(DISTANCESENSOR_PWR, OUTPUT);
    digitalWrite(DISTANCESENSOR_PWR, LOW);
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
    Serial1.setRX(DISTANCESENSOR_RX);
    Serial1.setTX(DISTANCESENSOR_TX);
    Serial1.begin(9600, SERIAL_8N1);
    pinMode(DISTANCESENSOR_PWR, OUTPUT_12MA);
    digitalWrite(DISTANCESENSOR_PWR, LOW);
#endif
}

uint16_t PayloadDigital::readDistanceSensor(void)
{
    // Sensor power on
    digitalWrite(DISTANCESENSOR_PWR, HIGH);
    delay(500);

    int retries = 0;
    DistanceSensor_A02YYUW_MEASSUREMENT_STATUS dstStatus;
    do
    {
        dstStatus = distanceSensor.meassure();

        if (dstStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK)
        {
            log_e("Distance Sensor Error: %d", dstStatus);
        }
    } while (
        (dstStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK) &&
        (++retries < DISTANCESENSOR_RETRIES));

    uint16_t distance_mm;
    if (dstStatus == DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK)
    {
        distance_mm = distanceSensor.getDistance();
    }
    else
    {
        distance_mm = 0;
    }

    // Sensor power off
    digitalWrite(DISTANCESENSOR_PWR, LOW);

    return distance_mm;
}
#endif
