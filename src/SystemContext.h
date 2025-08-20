///////////////////////////////////////////////////////////////////////////////
// SystemContext.h
//
// System context for BresserWeatherSensorLW
//
// - Hardware (MCU or board) specific initialization
// - Real time clock (RTC) initialization
// - Sleep interval handling (energy saving/battery discharge protection)
// - Sleep mode/wake-up handling
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
//
// History:
//
// 20250806 Created from BresserWeatherSensorLW.ino
// 20250811 Replaced ESP32Time by POSIX functions
// 20250820 Added getBattlevel()
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include <time.h>
#include <Preferences.h>
#include "../BresserWeatherSensorLWCfg.h"
#include "LoadNodeCfg.h"
#include "adc/adc.h"
#include "logging.h"
#if defined(ARDUINO_ESP32S3_POWERFEATHER)
#include <PowerFeather.h>
using namespace PowerFeather;
#endif
#if defined(ARDUINO_M5STACK_CORE2)
#include <M5Unified.h>
#endif
#if defined(ARDUINO_ARCH_RP2040)
#include "rp2040/pico_rtc_utils.h"
#include <hardware/rtc.h>
#endif

#if defined(EXT_RTC)
// Adafruit RTClib - https://github.com/adafruit/RTClib
#include <RTClib.h>
#endif

class SystemContext
{
public:
    uint16_t battery_weak = BATTERY_WEAK;
    uint16_t battery_low = BATTERY_LOW;
    uint16_t battery_discharge_lim = BATTERY_DISCHARGE_LIM;
    uint16_t battery_charge_lim = BATTERY_CHARGE_LIM;
    uint16_t batteryVoltage = 0;  // Battery voltage in mV
    uint16_t supplyVoltage = 0;   // Supply voltage in mV
    uint16_t mcuVoltage = 0;      // MCU supply voltage in mV (depending on the circuit)
    uint16_t sleep_interval;      //!< preferences: sleep interval
    uint16_t sleep_interval_long; //!< preferences: sleep interval long
    uint8_t lw_stat_interval;     //!< preferences: LoRaWAN node status uplink interval

    SystemContext() {
    };

    /**
     * \brief Initialize the system context
     *
     * Initialize the system context
     * - MCU / board specific initialization
     * - Load node configuration from JSON file
     * - Load preferences from flash memory
     * - Initialize RTC and time zone
     *
     */
    void begin(void);

    /**
     * \brief Check if this is the first boot of the system after power-on/HW reset
     *
     * \return true     First boot after power-on/HW reset
     * \return false    Not the first boot
     */
    bool isFirstBoot(void);

    /**
     * \brief Reset the failed join count
     *
     * The failed join count is reset to 0 after a successful join.
     */
    void resetFailedJoinCount(void);

    /**
     * \brief Sleep after a failed join attempt.
     *
     * The sleep duration after a failed join is determined by the
     * failed join count.
     */
    void sleepAfterFailedJoin(void);

    /**
     * \brief Set RTC to epoch
     *
     * Set RTC to epoch and store source and RTC sync timestamp
     *
     * \param epoch     time in seconds since epoch
     * \param source    time source
     */
    void setTime(time_t epoch, E_TIME_SOURCE source);

    /**
     * Print date and time (i.e. local time)
     */
    void printDateTime(void)
    {
        struct tm timeinfo;
        char tbuf[25];

        time_t tnow = time(nullptr);
        localtime_r(&tnow, &timeinfo);
        strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
        log_i("%s", tbuf);
    };

    /**
     * \brief Save preferences to flash memory
     */
    void savePreferences(void);

    /**
     * \brief Get the voltages from ADC / Power Management Chip
     *
     * The MCU voltage is evaluated to determine the state of the power supply.
     */
    void getVoltages(void)
    {
        batteryVoltage = getBatteryVoltage();
        supplyVoltage = getSupplyVoltage();

        if (batteryVoltage != 0)
        {
            mcuVoltage = batteryVoltage; // Default: MCU voltage is the same as battery voltage
        }
        else if (supplyVoltage != 0)
        {
            mcuVoltage = supplyVoltage; // Supply voltage is available, use it as MCU voltage
        }
        else
        {
            mcuVoltage = 0; // No battery or supply voltage available, cannot determine MCU
        }
    };

    /**
     * \brief Sleep if battery voltage is low to prevent deep-discharging
     *
     * Checks if the MCU voltage has reached the shut-off threshold and
     * enters sleep mode for battery deep-discharge protection.
     *
     */
    void sleepIfSupplyLow(void)
    {
        if (mcuVoltage > 0 && mcuVoltage <= battery_low)
        {
            log_i("Battery low!");
            gotoSleep(sleepDuration());
        }
    };

    /**
     * \brief Get the battery fill level
     * 
     * Get the battery fill level for LoRaWAN device status uplink.
     * The LoRaWAN network server may periodically request this information.
     * 
     * 0 = external power source
     * 1 = lowest (empty battery)
     * 254 = highest (full battery)
     * 255 = unable to measure
     */
    uint8_t getBattlevel(void)
    {
        uint16_t voltage = batteryVoltage;
        uint16_t limit_low = battery_discharge_lim;
        uint16_t limit_high = battery_charge_lim;
        uint8_t battLevel;

        if (voltage == 0)
        {
            // Unable to measure battery voltage
            battLevel = 255;
        }
        else if (voltage > limit_high)
        {
            // External power source
            battLevel = 0;
        }
        else
        {
            battLevel = static_cast<uint8_t>(
                static_cast<float>(voltage - limit_low) / static_cast<float>(limit_high - limit_low) * 255);
            battLevel = (battLevel == 0) ? 1 : battLevel;
            battLevel = (battLevel == 255) ? 254 : battLevel;
        }
        return battLevel;
    };

    /**
     * \brief Determine the sleep interval from the MCU voltage level
     *
     * If the MCU voltage is available and below the battery_weak threshold,
     * the long sleep interval is used to retain operation as long as possible.
     *
     * Otherwise, the normal sleep interval is used.
     */
    uint32_t sleepInterval(void)
    {
        if (mcuVoltage > 0 && mcuVoltage <= battery_weak)
        {
            return sleep_interval_long;
        }
        else
        {
            return sleep_interval;
        }
    };

    /**
     * \brief Check if long sleep is active
     *
     * Check if the sleep interval is set to the long sleep interval.
     * This flag is sent in a LoRaWAN uplink message.
     *
     * \return true if long sleep is active
     * \return false if long sleep is not active
     */
    bool longSleepActive(void)
    {
        return (sleepInterval() == sleep_interval_long);
    };

#if defined(EXT_RTC)
    /**
     * \brief Get the Time from external RTC
     */
    void getTimeFromExtRTC(void);
#endif

    /**
     * \brief Check if the RTC is synchronized to a time source
     *
     * \return true     if the RTC is synchronized to a time source
     * \return false    if the RTC is not synchronized
     */
    bool isRtcSynched(void);

    E_TIME_SOURCE getRtcTimeSource(void);

    /**
     * \brief Check if the RTC needs to be synchronized to a time source
     *
     * Checks if the RTC is synchronized to a time source
     *
     * If the RTC is not synchronized or if the last clock sync is older than
     * CLOCK_SYNC_INTERVAL, it returns true.
     *
     * \return true     if the RTC needs to be synchronized
     * \return false    if the RTC is synchronized and the last clock sync is within CLOCK_SYNC_INTERVAL
     */
    bool rtcNeedsSync(void);

    /**
     * \brief Compute sleep duration in seconds
     *
     * Minimum duration: SLEEP_INTERVAL_MIN
     * If battery voltage is available and <= BATTERY_WEAK:
     *   sleep_interval_long
     * else
     *   sleep_interval
     *
     * Additionally, the sleep interval is reduced from the
     * default value to achieve a wake-up time alinged to
     * an integer multiple of the interval after a full hour.
     *
     * \return sleep duration in seconds
     */
    uint32_t sleepDuration(void)
    {
        uint32_t sleep_interval = sleepInterval();

        // If the real time is available, align the wake-up time to
        // next non-fractional multiple of sleep_interval past the hour
        if (isRtcSynched())
        {
            struct tm timeinfo;
            time_t t_now = time(nullptr);
            localtime_r(&t_now, &timeinfo);

            sleep_interval = sleep_interval - ((timeinfo.tm_min * 60) % sleep_interval + timeinfo.tm_sec);
        }

        sleep_interval = max(sleep_interval, static_cast<uint32_t>(SLEEP_INTERVAL_MIN));
        return sleep_interval;
    };

    /**
     * \brief LoRaWAN uplink delay
     *
     * Uses MCU sleep mode if possible, otherwise delays for the given time.
     *
     * \param timeUntilUplink   time until next uplink in milliseconds
     * \param uplinkInterval    planned uplink interval in seconds
     */
    void uplinkDelay(uint32_t timeUntilUplink, uint32_t uplinkInterval)
    {
        uint32_t uplinkIntervalMs = uplinkInterval * 1000UL;
        uint32_t delayMs = max(timeUntilUplink, uplinkIntervalMs); // cannot send faster than duty cycle allows

        log_d("Sending uplink in %u s", delayMs / 1000);
#if defined(ESP32)
        esp_sleep_enable_timer_wakeup(delayMs * 1000);
        esp_light_sleep_start();
#else
        delay(delayMs);
#endif
    };

    /**
     * \brief Enter sleep mode
     *
     * On wake-up, the MCU will soft-reset and start from the beginning.
     *
     * \param seconds Sleep duration in seconds
     */
    void gotoSleep(uint32_t seconds)
    {
#if defined(ARDUINO_ARCH_RP2040)
        gotoSleepRP2040(seconds);
#elif defined(ESP32)
        gotoSleepESP32(seconds);
#endif
    };

#if defined(ESP32)
    /**
     * \brief Enter sleep mode (ESP32 variant)
     *
     *  ESP32 deep sleep mode
     *
     * \param seconds sleep duration in seconds
     */
    void gotoSleepESP32(uint32_t seconds);
#endif

#if defined(ARDUINO_ARCH_RP2040)

    /*!
     * \brief Enter sleep mode (RP2040 variant)
     *
     * The RP2040 RTC is set up to keep the time during the sleep interval and
     * to wake up after the sleep interval.
     *
     * For compatibility with the ESP32 sleep mode, a SW reset is performed
     * after the sleep interval.
     *
     * The SW reset also resets the RTC, so the time (along with other data to be retained)
     * is saved in the watchdog scratch registers.
     *
     * \param seconds sleep duration in seconds
     */
    void gotoSleepRP2040(uint32_t seconds);

    /*!
     * \brief Restore RP2040 variables after sleep and SW reset
     */
    void restoreRP2040(void);
#endif

#if defined(ESP32)
    /**
     * \brief Print wakeup reason (ESP32 only)
     *
     * Abbreviated version from the Arduino-ESP32 package, see
     * https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/deepsleep.html
     * for the complete set of options.
     */
    void print_wakeup_reason()
    {
        esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
        if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER)
        {
            log_i("Wake from sleep");
        }
        else
        {
            log_i("Wake not caused by deep sleep: %u", wakeup_reason);
        }
    };
#endif

#if defined(ARDUINO_M5STACK_CORE2)
    void setupM5StackCore2(void);
#endif

private:
#if defined(EXT_RTC)
    /**
     * \brief Synchronize the internal RTC with the external RTC
     */
    void syncRTCWithExtRTC(void);
#endif

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
    void setupPowerFeather(struct sPowerFeatherCfg &cfg);
#endif

private:
#if defined(ARDUINO_ESP32S3_POWERFEATHER)
    struct sPowerFeatherCfg PowerFeatherCfg = {
        .battery_capacity = BATTERY_CAPACITY_MAH,
        .supply_maintain_voltage = PF_SUPPLY_MAINTAIN_VOLTAGE,
        .max_charge_current = PF_MAX_CHARGE_CURRENT_MAH,
        .temperature_measurement = PF_TEMPERATURE_MEASUREMENT,
        .battery_fuel_gauge = PF_BATTERY_FUEL_GAUGE};
#else
    struct sPowerFeatherCfg PowerFeatherCfg = {0};
#endif
};
