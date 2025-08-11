///////////////////////////////////////////////////////////////////////////////
// time_y2038.ino
//
// Test for the year 2038 problem (no issue)
//
// See
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#unix-time-2038-overflow
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


void setup()
{
    Serial.begin(115200);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif

    Serial.println("Time Y2038 Test");
    Serial.printf("sizeof(time_t) = %zu bytes\n", sizeof(time_t));
    Serial.println("The time should advance past 2038-01-19 03:14:07 UTC");
    String input_str = "2038-01-19 03:13:00";

    // Parse the input string
    int year, month, day, hour, minute, second;
    if (sscanf(input_str.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6)
    {
        struct tm timeinfo;
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = second;

        time_t t = mktime(&timeinfo);

        // Set the internal RTC
        struct timeval tv = {t, 0}; // `t` is seconds, 0 is microseconds
        settimeofday(&tv, nullptr);
        Serial.printf("Set time to %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
        Serial.printf("32-bit time_t value: 0x%llX\n", (long long)t);
    }
}

void loop()
{
    time_t now_t = time(nullptr);
    if (now_t == -1)
    {
        Serial.println("Failed to obtain time");
    }

    // Convert time_t to struct tm
    struct tm *tm_info = localtime(&now_t);

    Serial.printf("32-bit time_t value: 0x%llX\n", (long long)now_t);
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
