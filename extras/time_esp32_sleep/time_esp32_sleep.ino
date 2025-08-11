///////////////////////////////////////////////////////////////////////////////
// time_esp32_sleep.ino
//
// Test RTC running while ESP32 is in deep sleep mode
//
// - Initially, the RTC is set to epoch time INIT_TIME
// - After initialization (flag rtcSet stored in RTC RAM),
//   the time is read from RTC and printed
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
// 20250809 Created
//
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <time.h>

#define TZINFO "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

RTC_DATA_ATTR bool rtcSet = false;

// 2025-01-01 00:00:00 UTC
const time_t INIT_TIME = 1735689600;

// Sleep time in seconds
const unsigned int SLEEP_TIME = 30;

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

    Serial.println("Test ESP32 RTC with deep sleep mode");

    // Set timezone
    setenv("TZ", TZINFO, 1);
    tzset();

    if (!rtcSet)
    {
        Serial.println("Initializing RTC");

        // Set the internal RTC
        struct timeval tv = {INIT_TIME, 0}; // `t` is seconds, 0 is microseconds
        settimeofday(&tv, nullptr);

        rtcSet = true;
    }
    printTime();
    Serial.println("Sleeping...");
    Serial.flush();
    esp_sleep_enable_timer_wakeup(SLEEP_TIME * 1000UL * 1000UL); // function uses uS
    esp_deep_sleep_start();
}

void loop()
{
    delay(100);
}