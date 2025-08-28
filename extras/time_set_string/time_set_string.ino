///////////////////////////////////////////////////////////////////////////////
// time_set_string.ino
//
// Test setting time from UTC time and date string, e.g. GPS NMEA messages
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
#include <assert.h>

#define TZINFO "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00"

/*!
 * \brief Set RTC from epoch to local time
 *
 * \param epochTime epoch
*/
void setLocalTimeFromEpoch(time_t epochTime)
{
    timeval epoch = {epochTime, 0};
    const timeval *tv = &epoch;
    timezone utc = {0, 0};
    const timezone *tz = &utc;
    settimeofday(tv, 0);
}

/*!
 * \brief Get epoch from a time and date string
 *
 * To avoid timezone conversion by mktime, the timezone is
 * temporarily set to UTC.
 *
 * \param timeString time and date in the format YYYY-MM-DD hh:mm:ss
 * 
 * \returns epoch
 */
time_t getTimeFromString(String timeString)
{
    // Parse the input string
    int year, month, day, hour, minute, second;
    if (sscanf(timeString.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6)
    {
        struct tm timeinfo = {0};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = second;

        Serial.println("Switching to timezone: UTC");
        setenv("TZ", "UTC+0UTC+0", 1);
        tzset();
        
        Serial.printf("daylight  : %d\n", _daylight);
        Serial.printf("tzname[0] : %s\n", tzname[0]);
        Serial.printf("tzname[1] : %s\n", tzname[1]);

        time_t t = mktime(&timeinfo);

        Serial.println("Switching to timezone: <TZINFO>");
        setenv("TZ", TZINFO, 1);
        tzset();

        return t;
    }
    assert(false);
}

/*!
 * \brief Print UTC time and date
 */
void printUtc(time_t utc)
{
    struct tm *tm_info = gmtime(&utc);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", tm_info);
    Serial.printf("UTC       : %s\n", buffer);
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
    Serial.printf("Local time: %s\n", buffer);
}

void setup()
{
    Serial.begin(115200);

#ifndef ESP8266
    while (!Serial)
        ; // wait for serial port to connect. Needed for native USB
#endif
    setenv("TZ", TZINFO, 1);
    tzset();

    Serial.println("Set time from GPS time and date (UTC)");
    
    Serial.println("Timezone info:");
    // Serial.printf("timezone: %ld\n", timezone);
    Serial.printf("daylight  : %d\n", _daylight);
    Serial.printf("tzname[0] : %s\n", tzname[0]);
    Serial.printf("tzname[1] : %s\n\n", tzname[1]);

    time_t epochTime;

    String timeStr1 = "2025-01-01 00:00:00";
    Serial.println("Set UTC   : " + timeStr1);

    epochTime = getTimeFromString(timeStr1);
    Serial.printf("Epoch time: %lld\n", (long long)epochTime);
    printUtc(epochTime);
    setLocalTimeFromEpoch(epochTime);
    printTime();
    Serial.println();

    String timeStr2 = "2025-06-01 00:00:00";
    Serial.println("Set UTC   : " + timeStr2);
    epochTime = getTimeFromString(timeStr2);
    Serial.printf("Epoch time: %lld\n", (long long)epochTime);
    printUtc(epochTime);
    setLocalTimeFromEpoch(epochTime);
    printTime();
    Serial.println();
}

void loop()
{
    delay(100);
}
