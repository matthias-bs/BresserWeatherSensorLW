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
// 20250820 Fixed rtcNeedsSync()
//
///////////////////////////////////////////////////////////////////////////////

#include "SystemContext.h"
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
#include <M5GFX.h>
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

// Time zone info
// const char *TZ_INFO = TZINFO_STR;

/// Preferences (stored in flash memory)
Preferences preferences;

#if defined(EXT_RTC)
// Create an instance of the external RTC class
EXT_RTC ext_rtc;
#endif

// Variables which must retain their values after deep sleep
#if defined(ESP32)
// Stored in RTC RAM
RTC_DATA_ATTR bool longSleep;              //!< last sleep interval; 0 - normal / 1 - long
RTC_DATA_ATTR time_t rtcLastClockSync = 0; //!< timestamp of last RTC synchonization to network time

// utilities & vars to support ESP32 deep-sleep. The RTC_DATA_ATTR attribute
// puts these in to the RTC memory which is preserved during deep-sleep
RTC_DATA_ATTR uint16_t bootCount = 1;
RTC_DATA_ATTR uint16_t bootCountSinceUnsuccessfulJoin = 0;
RTC_DATA_ATTR E_TIME_SOURCE rtcTimeSource = E_TIME_SOURCE::E_UNSYNCHED;

#else
// Saved to/restored from Watchdog SCRATCH registers
bool longSleep;          //!< last sleep interval; 0 - normal / 1 - long
time_t rtcLastClockSync; //!< timestamp of last RTC synchonization to network time

// utilities & vars to support deep-sleep
// Saved to/restored from Watchdog SCRATCH registers
uint16_t bootCount;
uint16_t bootCountSinceUnsuccessfulJoin;

/// RTC time source
E_TIME_SOURCE rtcTimeSource __attribute__((section(".uninitialized_data")));
#endif

void SystemContext::begin(void)
{
#if defined(ARDUINO_ARCH_RP2040)
  restoreRP2040();
#endif
  String timeZoneInfo(TZINFO_STR);

  // Load the node configuration from JSON file
  loadNodeCfg(
      timeZoneInfo,
      battery_weak,
      battery_low,
      battery_discharge_lim,
      battery_charge_lim,
      PowerFeatherCfg);

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
  setupPowerFeather(PowerFeatherCfg);
#endif
#if defined(ESP32)
  print_wakeup_reason();
#endif
  // Set time zone

  setenv("TZ", timeZoneInfo.c_str(), 1);
  tzset();
  printDateTime();

  log_i("Boot count: %u", bootCount);
  if (bootCount == 1)
  {
    rtcTimeSource = E_TIME_SOURCE::E_UNSYNCHED;
  }
  bootCount++;

#if defined(EXT_RTC)
  if (rtcNeedsSync())
  {
    getTimeFromExtRTC();
  }
#endif
  preferences.begin("BWS-LW", false);
  sleep_interval = preferences.getUShort("sleep_int", SLEEP_INTERVAL);
  sleep_interval_long = preferences.getUShort("sleep_int_long", SLEEP_INTERVAL_LONG);
  lw_stat_interval = preferences.getUChar("lw_stat_int", LW_STATUS_INTERVAL);
  preferences.end();
}

bool SystemContext::isFirstBoot(void)
{
  return (bootCount == 1);
}

void SystemContext::resetFailedJoinCount(void)
{
  bootCountSinceUnsuccessfulJoin = 0;
}

void SystemContext::sleepAfterFailedJoin(void)
{
  // how long to wait before join attempts. This is an interim solution pending
  // implementation of TS001 LoRaWAN Specification section #7 - this doc applies to v1.0.4 & v1.1
  // it sleeps for longer & longer durations to give time for any gateway issues to resolve
  // or whatever is interfering with the device <-> gateway airwaves.
  uint32_t sleepForSeconds = min((bootCountSinceUnsuccessfulJoin++ + 1UL) * 60UL, 3UL * 60UL);
  log_i("Boots since unsuccessful join: %u", bootCountSinceUnsuccessfulJoin);
  log_i("Retrying join in %u seconds", sleepForSeconds);

  gotoSleep(sleepForSeconds);
}

// Set RTC from epoch and store source & sync timestamp
void SystemContext::setTime(time_t epoch, E_TIME_SOURCE source)
{
  timeval epoch_tv = {epoch, 0};
  const timeval *tv = &epoch_tv;
  timezone utc = {0, 0};
  const timezone *tz = &utc;
  settimeofday(tv, tz);

  rtcTimeSource = source;
  rtcLastClockSync = epoch;
}

/**
 * \brief Save preferences to flash memory
 */
void SystemContext::savePreferences(void)
{
  preferences.begin("BWS-LW", false);
  preferences.putUShort("sleep_int", sleep_interval);
  preferences.putUShort("sleep_int_long", sleep_interval_long);
  preferences.putUChar("lw_stat_int", lw_stat_interval);
  preferences.end();
}

#if defined(EXT_RTC)
// Synchronize the internal RTC with the external RTC
void SystemContext::syncRTCWithExtRTC(void)
{
  DateTime now = ext_rtc.now();

  // Convert DateTime to time_t
  struct tm timeinfo;
  timeinfo.tm_year = now.year() - 1900;
  timeinfo.tm_mon = now.month() - 1;
  timeinfo.tm_mday = now.day();
  timeinfo.tm_hour = now.hour();
  timeinfo.tm_min = now.minute();
  timeinfo.tm_sec = now.second();

  time_t t = mktime(&timeinfo);

  // Set the MCU's internal RTC (ESP32) or SW RTC (RP2040)
  struct timeval tv = {t, 0}; // `t` is seconds, 0 is microseconds
  settimeofday(&tv, nullptr);
}
#endif // EXT_RTC

#if defined(EXT_RTC)
// Get the time from external RTC
void SystemContext::getTimeFromExtRTC(void)
{
  if (!ext_rtc.begin())
  {
    log_w("External RTC not available");
  }
  else if (ext_rtc.lostPower())
  {
    log_w("External RTC lost power");
  }
  else
  {
    syncRTCWithExtRTC();
    rtcLastClockSync = time(nullptr);
    rtcTimeSource = E_TIME_SOURCE::E_RTC;
    log_i("Set time and date from external RTC");
  }
}
#endif

/**
 * \brief Check if the RTC is synchronized to a time source
 *
 * \return true     if the RTC is synchronized to a time source
 * \return false    if the RTC is not synchronized
 */
bool SystemContext::isRtcSynched(void)
{
  return rtcTimeSource != E_TIME_SOURCE::E_UNSYNCHED;
}

E_TIME_SOURCE SystemContext::getRtcTimeSource(void)
{
  return rtcTimeSource;
}

// Check if the RTC needs to be synchronized to a time source
bool SystemContext::rtcNeedsSync(void)
{
  // Check if the RTC is not synchronized to a time source
  // or the last clock sync is older than CLOCK_SYNC_INTERVAL
  return (rtcTimeSource == E_TIME_SOURCE::E_UNSYNCHED) ||
         ((time(nullptr) - rtcLastClockSync) > (CLOCK_SYNC_INTERVAL * 60));
}

#if defined(ESP32)
// Enter sleep mode (ESP32 variant)
void SystemContext::gotoSleepESP32(uint32_t seconds)
{
  esp_sleep_enable_timer_wakeup(seconds * 1000UL * 1000UL); // function uses uS
  log_i("Sleeping for %lu s", seconds);
  Serial.flush();

  esp_deep_sleep_start();

  // if this appears in the serial debug, we didn't go to sleep!
  // so take defensive action so we don't continually uplink
  log_w("\n\n### Sleep failed ###");
  delay(5UL * 60UL * 1000UL);
  ESP.restart();
}
#endif // defined(ESP32)

#if defined(ARDUINO_ARCH_RP2040)
// Enter sleep mode (RP2040 variant)
void SystemContext::gotoSleepRP2040(uint32_t seconds)
{
  log_i("Sleeping for %lu s", seconds);
  time_t t_now = time(nullptr);
  datetime_t dt;
  epoch_to_datetime(&t_now, &dt);
  rtc_set_datetime(&dt);
  sleep_us(64);
  pico_sleep(seconds);

  // Save variables to be retained after reset
  watchdog_hw->scratch[3] = (bootCountSinceUnsuccessfulJoin << 16) | bootCount;
  watchdog_hw->scratch[2] = rtcLastClockSync;

  if (longSleep)
  {
    watchdog_hw->scratch[1] |= 2;
  }
  else
  {
    watchdog_hw->scratch[1] &= ~2;
  }
  // Save the current time, because RTC will be reset (SIC!)
  rtc_get_datetime(&dt);
  time_t now = datetime_to_epoch(&dt, NULL);
  watchdog_hw->scratch[0] = now;
  log_i("Now: %llu", now);

  rp2040.restart();
}

// Restore RP2040 variables after sleep and SW reset
void SystemContext::restoreRP2040(void)
{
  // see pico-sdk/src/rp2_common/hardware_rtc/rtc.c
  rtc_init();

  // Restore variables and RTC after reset
  time_t time_saved = watchdog_hw->scratch[0];
  datetime_t dt;
  epoch_to_datetime(&time_saved, &dt);

  // Set HW clock (only used in sleep mode)
  rtc_set_datetime(&dt);

  // Restore SW clock after reset
  timeval epoch = {time_saved, 0};
  const timeval *tv = &epoch;
  timezone utc = {0, 0};
  const timezone *tz = &utc;
  settimeofday(tv, tz);

  longSleep = ((watchdog_hw->scratch[1] & 2) == 2);
  rtcLastClockSync = watchdog_hw->scratch[2];
  bootCount = watchdog_hw->scratch[3] & 0xFFFF;
  if (bootCount == 0)
  {
    bootCount = 1;
  }
  bootCountSinceUnsuccessfulJoin = watchdog_hw->scratch[3] >> 16;
}
#endif

#if defined(ARDUINO_M5STACK_CORE2)
void SystemContext::setupM5StackCore2(void)
{
  auto cfg = M5.config();
  cfg.clear_display = true; // default=true. clear the screen when begin.
  cfg.output_power = true;  // default=true. use external port 5V output.
  cfg.internal_imu = false; // default=true. use internal IMU.
  cfg.internal_rtc = true;  // default=true. use internal RTC.
  cfg.internal_spk = false; // default=true. use internal speaker.
  cfg.internal_mic = false; // default=true. use internal microphone.
  M5.begin(cfg);
}
#endif

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
void SystemContext::setupPowerFeather(struct sPowerFeatherCfg &cfg)
{
  delay(2000);
  // Note: Battery capacity / type has to be set for voltage measurement
  Board.init(cfg.battery_capacity);
  Board.enable3V3(true);                                     // Power supply for FeatherWing
  Board.enableVSQT(true);                                    // Power supply for battery management chip (voltage measurement)
  Board.enableBatteryTempSense(cfg.temperature_measurement); // Enable battery temperature measurement
  Board.enableBatteryFuelGauge(cfg.battery_fuel_gauge);      // Enable battery fuel gauge
  if (cfg.supply_maintain_voltage)
  {
    Board.setSupplyMaintainVoltage(cfg.supply_maintain_voltage); // Set supply maintain voltage
  }
  Board.enableBatteryCharging(true);                          // Enable battery charging
  Board.setBatteryChargingMaxCurrent(cfg.max_charge_current); // Set max charging current
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
  int16_t current;
  Board.getBatteryCurrent(current);
  log_d("Battery current: %d mA", current);
#endif
}
#endif