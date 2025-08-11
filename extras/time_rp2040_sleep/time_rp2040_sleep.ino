///////////////////////////////////////////////////////////////////////////////
// time_rp2040_sleep.ino
//
// Test RTC running while RP2040 is in sleep mode
// (incl. hack to survive restart)
//
// - Initially, the SW RTC is set to epoch time INIT_TIME
// - Before going to sleep, the HW RTC is set to the current time and date
// - After wake-up, the HW RTC provides the current (i.e. updated) time and date
// - To mimic the ESP32's wake-up from deep sleep, a restart is done
// - The restart resets the RTC, therefore the epoch time is temporarily saved
//   in the Watchdog Scratch register, which retains its value durung restart
// - After restart, the SW RTC is re-initialized with the saved RTC value
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
// 20250811 Created
//
///////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#if defined(ARDUINO_ARCH_RP2040)
#include "src/pico_rtc_utils.h"
#include <hardware/rtc.h>
#endif
#include <time.h>

// 2025-01-01 00:00:00 UTC
const time_t INIT_TIME = 1735689600;

void wakeUp(void)
{
    // see pico-sdk/src/rp2_common/hardware_rtc/rtc.c
    rtc_init();

    time_t now;

    // Restore RTC from Watchdog Scratch register after reset
    if (watchdog_hw->scratch[0] == 0) {
        now = INIT_TIME;
    } else {
        now = watchdog_hw->scratch[0];
    }

    datetime_t dt;
    epoch_to_datetime(&now, &dt);

    // Set HW clock (only used in sleep mode)
    rtc_set_datetime(&dt);

    // Set SW clock
    struct timeval tv = {now, 0}; // `t` is seconds, 0 is microseconds
    settimeofday(&tv, nullptr);
}

/*!
 * \brief Enter sleep mode (RP2040 variant)
 *
 * \param seconds sleep duration in seconds
 */
void gotoSleep(uint32_t seconds)
{
    Serial.printf("Sleeping for %lu s\n", seconds);
    Serial.flush();
    time_t t_now = time(nullptr);
    datetime_t dt;
    epoch_to_datetime(&t_now, &dt);
    rtc_set_datetime(&dt);
    sleep_us(64);
    pico_sleep(seconds);

    // Save the current time, because RTC will be reset (SIC!)
    rtc_get_datetime(&dt);
    time_t now = datetime_to_epoch(&dt, NULL);
    watchdog_hw->scratch[0] = now;
    Serial.printf("Now: %llu\n", now);

    rp2040.restart();
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
    delay(3000);

    wakeUp();
    printTime();
    gotoSleep(30);
}

void loop()
{
    delay(100);
}
