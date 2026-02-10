///////////////////////////////////////////////////////////////////////////////
// DistanceSensor.cpp
//
// Ultrasonic distance sensor integration (A02YYUW / DFRobot SEN0311)
//
// created: 02/2026
//
//
// MIT License
//
// Copyright (c) 2026 Matthias Prinke
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
// 20260210 Created from PayloadDigital.cpp
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "DistanceSensor.h"

#ifdef A02YYUW_EN

DistanceSensor::DistanceSensor()
{
#if defined(ESP32)
    m_sensor = new DistanceSensor_A02YYUW(&Serial2);
#else
    m_sensor = new DistanceSensor_A02YYUW(&Serial1);
#endif
}

DistanceSensor::~DistanceSensor()
{
    delete m_sensor;
}

void DistanceSensor::begin(void)
{
#if defined(ESP32)
    Serial2.begin(9600, SERIAL_8N1, A02YYUW_RX, A02YYUW_TX);
    pinMode(A02YYUW_PWR, OUTPUT);
    digitalWrite(A02YYUW_PWR, LOW);
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
    Serial1.setRX(A02YYUW_RX);
    Serial1.setTX(A02YYUW_TX);
    Serial1.begin(9600, SERIAL_8N1);
    pinMode(A02YYUW_PWR, OUTPUT_12MA);
    digitalWrite(A02YYUW_PWR, LOW);
#endif
}

uint16_t DistanceSensor::read(void)
{
    // Sensor power on
    digitalWrite(A02YYUW_PWR, HIGH);
    delay(500);

    int retries = 0;
    DistanceSensor_A02YYUW_MEASSUREMENT_STATUS dstStatus;
    do
    {
        dstStatus = m_sensor->meassure();

        if (dstStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK)
        {
            log_e("Distance Sensor Error: %d", dstStatus);
        }
    } while (
        (dstStatus != DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK) &&
        (++retries < A02YYUW_RETRIES));

    uint16_t distance_mm;
    if (dstStatus == DistanceSensor_A02YYUW_MEASSUREMENT_STATUS_OK)
    {
        distance_mm = m_sensor->getDistance();
    }
    else
    {
        distance_mm = 0;
    }

    // Sensor power off
    digitalWrite(A02YYUW_PWR, LOW);

    return distance_mm;
}

#endif // A02YYUW_EN
