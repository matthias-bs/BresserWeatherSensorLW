///////////////////////////////////////////////////////////////////////////////
// RTCSet.ino
//
// Utility for setting/testing an external RTC chip connected via I2C bus
// to ESP32 or RP2040.
//
// Uses Adafruit RTClib library.
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
// 20250803 Created
// 20250804 Fixed and tested RP2040 implementation
//
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <time.h>
#include "RTClib.h" // https://github.com/adafruit/RTClib


// Select your RTC chip
RTC_DS3231 rtc;
// RTC_DS1307 rtc;
// RTC_PCF8523 rtc;
// RTC_PCF8563 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Sync the MCU's internal RTC with the external RTC
// NOTE:
// For RP2040, this syncs only the SW RTC, not the HW RTC which retains operation during sleep mode!
void syncRTCWithExtRTC(void)
{
    // Get the current time from the DS3231
    DateTime now = rtc.now();

    // Convert DateTime to time_t
    struct tm timeinfo;
    timeinfo.tm_year = now.year() - 1900;
    timeinfo.tm_mon = now.month() - 1;
    timeinfo.tm_mday = now.day();
    timeinfo.tm_hour = now.hour();
    timeinfo.tm_min = now.minute();
    timeinfo.tm_sec = now.second();

    time_t t = mktime(&timeinfo);

    // Set the internal RTC
    struct timeval tv = {t, 0}; // `t` is seconds, 0 is microseconds
    settimeofday(&tv, nullptr);
}

void setup()
{
    Serial.begin(115200);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif

    if (!rtc.begin())
    {
        Serial.println("Couldn't find RTC");
        Serial.flush();
        while (1)
            delay(10);
    }

    if (rtc.lostPower())
    {
        Serial.println("RTC lost power!");
    }

    Serial.println("Input date and time in format YYYY-MM-DD HH:MM:SS (e.g. 2023-10-01 12:00:00),");
    Serial.println("send <c> to use compile time, or <s> to skip setting the RTC.");

    while (!Serial.available())
    {
        delay(10); // Wait for input
    }

    String input_str = Serial.readStringUntil('\n');
    if (input_str.length() > 0)
    {
        if (input_str == "c" || input_str == "C")
        {
            Serial.println("Using compile time for RTC.");
            rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            Serial.println("RTC set to compile time.");
        }
        if (input_str == "s" || input_str == "S")
        {
            Serial.println("Skipping RTC setting.");
        }
        else
        {
            // Parse the input string
            int year, month, day, hour, minute, second;
            if (sscanf(input_str.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6)
            {
                rtc.adjust(DateTime(year, month, day, hour, minute, second));
                Serial.println("RTC set to user-defined date and time.");
            }
            else
            {
                Serial.println("Invalid date/time format, skipping.");
            }
        }
    }

    // Sync the MCU's internal RTC with the external RTC
    syncRTCWithExtRTC();
}

void loop()
{
    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    time_t now_t = time(nullptr);
    if (now_t == -1)
    {
        Serial.println("Failed to obtain time");
    }

    // Convert time_t to struct tm
    struct tm *tm_info = localtime(&now_t);

    Serial.print("Current time: ");
    Serial.print(asctime(tm_info));

    // Print the RTC time in ISO 8601 format
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", tm_info);
    Serial.print("ISO 8601 format: ");
    Serial.println(buffer);
    Serial.println();

    // Wait for a while before the next loop iteration
    delay(5000);
}
