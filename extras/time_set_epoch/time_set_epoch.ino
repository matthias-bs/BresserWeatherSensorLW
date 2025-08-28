///////////////////////////////////////////////////////////////////////////////
// time_set_epoch.ino
//
// Test setting time from epoch (Unix time), e.g. from a LoRaWAN network
//
// See
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
// https://www.gnu.org/software/libc/manual/html_node/Calendar-Time.html
//
// created: 08/2025
//
//
// MIT License
//
// Copyright (c) 2025 Matthias Prinke
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
//
// 20250808 Created
//
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <time.h>

#define TZINFO "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

/*!
 * \brief Set RTC from epoch
 *
 * \param epochTime epoch time
 */
void setLocalTimeFromEpoch(time_t epochTime)
{
    timeval epoch = {epochTime, 0};
    const timeval *tv = &epoch;
    timezone utc = {0, 0};
    const timezone *tz = &utc;
    settimeofday(tv, tz);
}

/*!
 * \brief Print local time and date
 */
void printTime(void)
{
    time_t now_t = time(nullptr);
    if (now_t == -1)
    {
        Serial.println("Failed to obtain time");
    }

    struct tm *tm_info = localtime(&now_t);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", tm_info);
    Serial.printf("Local time: %s\n\n", buffer);
}

void setup()
{
    Serial.begin(115200);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif

    // Set timezone
    setenv("TZ", TZINFO, 1);
    tzset();

    Serial.println("Set time from epoch (Unix time)");

    Serial.println("Time set:   2025-01-01T00:00:00 UTC");

    time_t epochTime = 1735689600; // Example epoch time (2025-01-01 00:00:00 UTC)
    setLocalTimeFromEpoch(epochTime);
    printTime();

    Serial.println("Time set:   2025-06-01T00:00:00 UTC");
    epochTime = 1748736000; // Example epoch time (2025-06-01 00:00:00 UTC)

    setLocalTimeFromEpoch(epochTime);
    printTime();
}

void loop()
{
    delay(100);
}
