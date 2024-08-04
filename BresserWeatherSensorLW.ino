///////////////////////////////////////////////////////////////////////////////
// BresserWeatherSensorLW.ino
//
// Bresser 868 MHz Weather Sensor Radio Receiver
// based on ESP32 and RFM95W/SX1276/SX1262 -
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// The radio transceiver is used with RadioLib
// in FSK mode to receive weather sensor data
// and
// in LoRaWAN mode to connect to a LoRaWAN network
//
// Based on:
// ---------
// RadioLib by Jan Gromeš (https://github.com/jgromes/RadioLib)
// Persistence support for RadioLib (https://github.com/radiolib-org/radiolib-persistence)
// Bresser5in1-CC1101 by Sean Siford (https://github.com/seaniefs/Bresser5in1-CC1101)
// rtl_433 (https://github.com/merbanan/rtl_433)
// BresserWeatherSensorTTN by Matthias Prinke (https://github.com/matthias-bs/BresserWeatherSensorTTN)
// Lora-Serialization by Joscha Feth (https://github.com/thesolarnomad/lora-serialization)
// ESP32Time by Felix Biego (https://github.com/fbiego/ESP32Time)
// OneWireNg by Piotr Stolarz (https://github.com/pstolarz/OneWireNg)
// DallasTemperature / Arduino-Temperature-Control-Library by Miles Burton (https://github.com/milesburton/Arduino-Temperature-Control-Library)
//
// Library dependencies (tested versions):
// ---------------------------------------
// (install via normal Arduino Library installer:)
// RadioLib                             6.6.0
// LoRa_Serialization                   3.2.1
// ESP32Time                            2.0.6
// BresserWeatherSensorReceiver         0.28.9
// OneWireNg                            0.13.3 (optional)
// DallasTemperature                    3.9.0 (optional)
// NimBLE-Arduino                       1.4.2 (optional)
// ATC MiThermometer                    0.4.2 (optional)
// Theengs Decoder                      1.7.8 (optional)
//
// (installed from ZIP file:)
// DistanceSensor_A02YYUW               1.0.2 (optional)
//
// created: 04/2024
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
// 20240407 Created from
//          https://github.com/matthias-bs/BresserWeatherSensorTTN (v0.12.2)
//          https://github.com/jgromes/RadioLib/blob/master/examples/LoRaWAN/LoRaWAN_Reference/LoRaWAN_Reference.ino
//          https://github.com/radiolib-org/radiolib-persistence/blob/main/examples/LoRaWAN_ESP32/LoRaWAN_ESP32.ino
// 20240410 Added RP2040 specific implementation
//          Added minimum sleep interval (and thus uplink interval)
//          Added M5Stack Core2 initialization
// 20240414 Added separation between LoRaWAN and application layer
//          Fixed battLevel calculation
// 20240415 Added ESP32-S3 PowerFeather
// 20240416 Added enabling of 3.3V power supply for FeatherWing on ESP32-S3 PowerFeather
// 20240423 Removed rtcSyncReq & runtimeExpired, added rtcTimeSource
// 20240424 Added appLayer.begin()
// 20240504 PowerFeather: added BATTERY_CAPACITY_MAH to init()
//          Added BresserWeatherSensorLWCmd.h
// 20240505 Implemented loading of LoRaWAN secrets from file on LittleFS (if available)
// 20230524 Modified PAYLOAD_SIZE: Moved define to header file, added small reserve
//          to uplinkPayload[], modified actual size in sendReceive()
// 20240528 Disabled uplink transmission of LoRaWAN node status flags
// 20242529 Fixed payload size calculation
// 20240530 Updated to RadioLib v6.6.0
// 20240603 Added AppLayer status uplink
// 20240606 Changed appStatusUplinkInterval from const to variable
// 20240608 Added LoRaWAN device status uplink
// 20240630 Switched to lwActivate() from radiolib-persistence/examples/LoRaWAN_ESP32
// 20240716 Modified port to allow modifications by appLayer.getPayloadStage<1|2>()
// 20240722 Added periodic uplink of LoRaWAN node status messages
// 20240723 Moved loadSecrets() to LoadSecrets.cpp/.h
//          Moved decodeDownlink() & sendCfgUplink() to BresserWeatherSensorLWCmd.cpp/.h
// 20240725 Added reading of hardware/deployment specific configuration node_config.json
//          from LittleFS (optional)
// 20240729 PowerFeather: Enabled battery temperature measurement, added specific configuration
// 20240730 PowerFeather: modified setSupplyMaintainVoltage()
// 20240804 PowerFeather: Added configuration of maximum battery charging current
//
// ToDo:
// -
//
// Notes:
// - Set "Core Debug Level: Debug" for initial testing
// - The lines with "#pragma message()" in the compiler output are not errors, but useful hints!
// - The default LoRaWAN credentials are read at compile time from secrets.h (included in config.h),
//   they can be overriden by the JSON file secrets.json placed in LittleFS
//   (Use https://github.com/earlephilhower/arduino-littlefs-upload for uploading.)
// - Pin mapping of radio transceiver module is done in two places:
//   - BresserWeatherSensorLW:       config.h
//   - BresserWeatherSensorReceiver: WeatherSensorCfg.h
// - After a successful transmission, the controller can go into deep sleep
// - If joining the network or transmitting uplink data fails,
//   the controller will go into deep sleep
// - For LoRaWAN Specification 1.1.0, a small set of data (the "nonces") have to be stored persistently -
//   this implementation uses Flash (via Preferences library
// - Storing LoRaWAN network session information speeds up the connection (join) after a restart -
//   this implementation uses the ESP32's RTC RAM or a variable located in the RP2040's RAM, respectively.
//   In the latter case, an uninitialzed linker section is used for this purpose.
// - The ESP32's Bluetooth LE interface is used to access sensor data (option)
// - settimeofday()/gettimeofday() must be used to access the ESP32's RTC time
// - Arduino ESP32 package has built-in time zone handling, see
//   https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32/blob/master/NTP_Example/NTP_Example.ino
//
///////////////////////////////////////////////////////////////////////////////
/*! \file BresserWeatherSensorLW.ino */

#if !defined(ESP32) && !defined(ARDUINO_ARCH_RP2040)
#pragma error("This is not the example your device is looking for - ESP32 & RP2040 only")
#endif

// LoRa_Serialization
#include <LoraMessage.h>

// ##### load the ESP32 preferences facilites
#include <Preferences.h>
static Preferences store;

/// Preferences (stored in flash memory)
Preferences preferences;

struct sPrefs
{
  uint16_t sleep_interval;      //!< preferences: sleep interval
  uint16_t sleep_interval_long; //!< preferences: sleep interval long
  uint8_t lw_stat_interval;     //!< preferences: LoRaWAN node status uplink interval
} prefs;

// Logging macros for RP2040
#include "src/logging.h"

#if defined(ARDUINO_ARCH_RP2040)
#include "src/rp2040/pico_rtc_utils.h"
#include <hardware/rtc.h>
#endif

#if defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORE2)
#include <M5Unified.h>
#endif

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
#include <PowerFeather.h>
using namespace PowerFeather;
#endif

// LoRaWAN config, credentials & pinmap
#include "config.h"

#include <RadioLib.h>
#include <ESP32Time.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "BresserWeatherSensorLWCfg.h"
#include "BresserWeatherSensorLWCmd.h"
#include "src/LoadSecrets.h"
#include "src/LoadNodeCfg.h"
#include "src/AppLayer.h"
#include "src/adc/adc.h"

// Time zone info
const char *TZ_INFO = TZINFO_STR;

// Variables which must retain their values after deep sleep
#if defined(ESP32)
// Stored in RTC RAM
RTC_DATA_ATTR bool longSleep;              //!< last sleep interval; 0 - normal / 1 - long
RTC_DATA_ATTR time_t rtcLastClockSync = 0; //!< timestamp of last RTC synchonization to network time

// utilities & vars to support ESP32 deep-sleep. The RTC_DATA_ATTR attribute
// puts these in to the RTC memory which is preserved during deep-sleep
RTC_DATA_ATTR uint16_t bootCount = 1;
RTC_DATA_ATTR uint16_t bootCountSinceUnsuccessfulJoin = 0;
RTC_DATA_ATTR E_TIME_SOURCE rtcTimeSource;
RTC_DATA_ATTR bool appStatusUplinkPending = false;
RTC_DATA_ATTR bool lwStatusUplinkPending = false;
RTC_DATA_ATTR uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];

#else
// Saved to/restored from Watchdog SCRATCH registers
bool longSleep;          //!< last sleep interval; 0 - normal / 1 - long
time_t rtcLastClockSync; //!< timestamp of last RTC synchonization to network time

// utilities & vars to support deep-sleep
// Saved to/restored from Watchdog SCRATCH registers
uint16_t bootCount;
uint16_t bootCountSinceUnsuccessfulJoin;

/// RP2040 RAM is preserved during sleep; we just have to ensure that it is not initialized at startup (after reset)
uint8_t LWsession[RADIOLIB_LORAWAN_SESSION_BUF_SIZE] __attribute__((section(".uninitialized_data")));

/// RTC time source
E_TIME_SOURCE rtcTimeSource __attribute__((section(".uninitialized_data")));

/// AppLayer status uplink pending
bool appStatusUplinkPending __attribute__((section(".uninitialized_data")));

/// LoRaWAN Node status uplink pending
bool lwStatusUplinkPending __attribute__((section(".uninitialized_data")));
#endif

/// Real time clock
ESP32Time rtc;

/// Application layer
AppLayer appLayer(&rtc, &rtcLastClockSync);

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
struct sPowerFeatherCfg PowerFeatherCfg = {
    .battery_capacity = BATTERY_CAPACITY_MAH,
    .supply_maintain_voltage = PF_SUPPLY_MAINTAIN_VOLTAGE,
    .max_charge_current = PF_MAX_CHARGE_CURRENT_MAH,
    .temperature_measurement = PF_TEMPERATURE_MEASUREMENT,
    .battery_fuel_gauge = PF_BATTERY_FUEL_GAUGE
};
#else
struct sPowerFeatherCfg PowerFeatherCfg = {0};
#endif

#if defined(ESP32)
/*!
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
}
#endif

/*!
 * \brief Compute sleep duration
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
 * \returns sleep duration in seconds
 */
uint32_t sleepDuration(uint16_t battery_weak)
{
  uint32_t sleep_interval = prefs.sleep_interval;
  longSleep = false;

  uint16_t voltage = getBatteryVoltage();
  // Long sleep interval if battery is weak
  if (voltage && voltage <= battery_weak)
  {
    sleep_interval = prefs.sleep_interval_long;
    longSleep = true;
  }

  // If the real time is available, align the wake-up time to the
  // to next non-fractional multiple of sleep_interval past the hour
  if (rtcLastClockSync)
  {
    struct tm timeinfo;
    time_t t_now = rtc.getLocalEpoch();
    localtime_r(&t_now, &timeinfo);

    sleep_interval = sleep_interval - ((timeinfo.tm_min * 60) % sleep_interval + timeinfo.tm_sec);
  }

  sleep_interval = max(sleep_interval, static_cast<uint32_t>(SLEEP_INTERVAL_MIN));
  return sleep_interval;
}

#if defined(ESP32)
/*!
 * \brief Enter sleep mode (ESP32 variant)
 *
 *  ESP32 deep sleep mode
 *
 * \param seconds sleep duration in seconds
 */
void gotoSleep(uint32_t seconds)
{
  esp_sleep_enable_timer_wakeup(seconds * 1000UL * 1000UL); // function uses uS
  log_i("Sleeping for %lu s", seconds);
  Serial.flush();

  esp_deep_sleep_start();

  // if this appears in the serial debug, we didn't go to sleep!
  // so take defensive action so we don't continually uplink
  log_w("\n\n### Sleep failed, delay of 5 minutes & then restart ###");
  delay(5UL * 60UL * 1000UL);
  ESP.restart();
}

#else
/*!
 * \brief Enter sleep mode (RP2040 variant)
 *
 * \param seconds sleep duration in seconds
 */
void gotoSleep(uint32_t seconds)
{
  log_i("Sleeping for %lu s", seconds);
  time_t t_now = rtc.getLocalEpoch();
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
#endif

/// Print date and time (i.e. local time)
void printDateTime(void)
{
  struct tm timeinfo;
  char tbuf[25];

  time_t tnow = rtc.getLocalEpoch();
  localtime_r(&tnow, &timeinfo);
  strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
  log_i("%s", tbuf);
}

/*!
 * \brief Activate node by restoring session or otherwise joining the network
 *
 * \return RADIOLIB_LORAWAN_NEW_SESSION or RADIOLIB_LORAWAN_SESSION_RESTORED
 */
int16_t lwActivate(void)
{
  int16_t state = RADIOLIB_ERR_UNKNOWN;

  // setup the OTAA session information
  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  log_d("Recalling LoRaWAN nonces & session");
  // ##### setup the flash storage
  store.begin("radiolib");
  // ##### if we have previously saved nonces, restore them and try to restore session as well
  if (store.isKey("nonces"))
  {
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
    store.getBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // get them from the store
    state = node.setBufferNonces(buffer);                               // send them to LoRaWAN
    debug(state != RADIOLIB_ERR_NONE, "Restoring nonces buffer failed", state, false);

    // recall session from RTC deep-sleep preserved variable
    state = node.setBufferSession(LWsession); // send them to LoRaWAN stack

    // if we have booted more than once we should have a session to restore, so report any failure
    // otherwise no point saying there's been a failure when it was bound to fail with an empty LWsession var.
    debug((state != RADIOLIB_ERR_NONE) && (bootCount > 1), "Restoring session buffer failed", state, false);

    // if Nonces and Session restored successfully, activation is just a formality
    // moreover, Nonces didn't change so no need to re-save them
    if (state == RADIOLIB_ERR_NONE)
    {
      log_d("Succesfully restored session - now activating");
      state = node.activateOTAA();
      debug((state != RADIOLIB_LORAWAN_SESSION_RESTORED), "Failed to activate restored session", state, true);

      // ##### close the store before returning
      store.end();
      return (state);
    }
  }
  else
  { // store has no key "nonces"
    log_d("No Nonces saved - starting fresh.");
  }

  // if we got here, there was no session to restore, so start trying to join
  state = RADIOLIB_ERR_NETWORK_NOT_JOINED;
  while (state != RADIOLIB_LORAWAN_NEW_SESSION)
  {
    log_d("Join ('login') to the LoRaWAN Network");
    state = node.activateOTAA();

    // ##### save the join counters (nonces) to permanent store
    log_d("Saving nonces to flash");
    uint8_t buffer[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];                   // create somewhere to store nonces
    uint8_t *persist = node.getBufferNonces();                          // get pointer to nonces
    memcpy(buffer, persist, RADIOLIB_LORAWAN_NONCES_BUF_SIZE);          // copy in to buffer
    store.putBytes("nonces", buffer, RADIOLIB_LORAWAN_NONCES_BUF_SIZE); // send them to the store

    // we'll save the session after an uplink

    if (state != RADIOLIB_LORAWAN_NEW_SESSION)
    {
      log_d("Join failed: %d", state);

      // how long to wait before join attempts. This is an interim solution pending
      // implementation of TS001 LoRaWAN Specification section #7 - this doc applies to v1.0.4 & v1.1
      // it sleeps for longer & longer durations to give time for any gateway issues to resolve
      // or whatever is interfering with the device <-> gateway airwaves.
      uint32_t sleepForSeconds = min((bootCountSinceUnsuccessfulJoin++ + 1UL) * 60UL, 3UL * 60UL);
      log_i("Boots since unsuccessful join: %u", bootCountSinceUnsuccessfulJoin);
      log_i("Retrying join in %u seconds", sleepForSeconds);

      gotoSleep(sleepForSeconds);

    } // if activateOTAA state
  } // while join

  log_d("Joined");

  // reset the failed join count
  bootCountSinceUnsuccessfulJoin = 0;

  delay(1000); // hold off off hitting the airwaves again too soon - an issue in the US

  // ##### close the store
  store.end();
  return (state);
}

// setup & execute all device functions ...
void setup()
{
#if defined(ARDUINO_M5STACK_Core2) || defined(ARDUINO_M5STACK_CORE2)
  auto cfg = M5.config();
  cfg.clear_display = true; // default=true. clear the screen when begin.
  cfg.output_power = true;  // default=true. use external port 5V output.
  cfg.internal_imu = false; // default=true. use internal IMU.
  cfg.internal_rtc = true;  // default=true. use internal RTC.
  cfg.internal_spk = false; // default=true. use internal speaker.
  cfg.internal_mic = false; // default=true. use internal microphone.
  M5.begin(cfg);
#endif

  Serial.begin(115200);
  delay(2000); // give time to switch to the serial monitor
  log_i("Setup");

  String timeZoneInfo(TZ_INFO);
  uint16_t battery_weak = BATTERY_WEAK;
  uint16_t battery_low = BATTERY_LOW;
  uint16_t battery_discharge_lim = BATTERY_DISCHARGE_LIM;
  uint16_t battery_charge_lim = BATTERY_CHARGE_LIM;

  loadNodeCfg(
      timeZoneInfo,
      battery_weak,
      battery_low,
      battery_discharge_lim,
      battery_charge_lim,
      PowerFeatherCfg);

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
  delay(2000);
  // Note: Battery capacity / type has to be set for voltage measurement
  Board.init(PowerFeatherCfg.battery_capacity);                            
  Board.enable3V3(true);  // Power supply for FeatherWing
  Board.enableVSQT(true); // Power supply for battery management chip (voltage measurement)
  Board.enableBatteryTempSense(PowerFeatherCfg.temperature_measurement);   // Enable battery temperature measurement
  Board.enableBatteryFuelGauge(PowerFeatherCfg.battery_fuel_gauge);        // Enable battery fuel gauge
  if (PowerFeatherCfg.supply_maintain_voltage) {
      Board.setSupplyMaintainVoltage(PowerFeatherCfg.supply_maintain_voltage); // Set supply maintain voltage
  }
  Board.enableBatteryCharging(true); // Enable battery charging
  Board.setBatteryChargingMaxCurrent(PowerFeatherCfg.max_charge_current); // Set max charging current
#endif

#if defined(ARDUINO_ARCH_RP2040)
  // see pico-sdk/src/rp2_common/hardware_rtc/rtc.c
  rtc_init();

  // Restore variables and RTC after reset
  time_t time_saved = watchdog_hw->scratch[0];
  datetime_t dt;
  epoch_to_datetime(&time_saved, &dt);

  // Set HW clock (only used in sleep mode)
  rtc_set_datetime(&dt);

  // Set SW clock
  rtc.setTime(time_saved);

  longSleep = ((watchdog_hw->scratch[1] & 2) == 2);
  rtcLastClockSync = watchdog_hw->scratch[2];
  bootCount = watchdog_hw->scratch[3] & 0xFFFF;
  if (bootCount == 0)
  {
    bootCount = 1;
  }
  bootCountSinceUnsuccessfulJoin = watchdog_hw->scratch[3] >> 16;
#else
  print_wakeup_reason();
#endif
  log_i("Boot count: %u", bootCount++);

  if (bootCount == 1)
  {
    rtcTimeSource = E_TIME_SOURCE::E_UNSYNCHED;
    appStatusUplinkPending = 0;
    lwStatusUplinkPending = 0;
  }

  // Set time zone
  setenv("TZ", timeZoneInfo.c_str(), 1);
  printDateTime();

  // Try to load LoRaWAN secrets from LittleFS file, if available
  loadSecrets(joinEUI, devEUI, nwkKey, appKey);

  // Initialize Application Layer
  appLayer.begin();

  preferences.begin("BWS-LW", false);
  prefs.sleep_interval = preferences.getUShort("sleep_int", SLEEP_INTERVAL);
  log_d("Preferences: sleep_interval:        %u s", prefs.sleep_interval);
  prefs.sleep_interval_long = preferences.getUShort("sleep_int_long", SLEEP_INTERVAL_LONG);
  log_d("Preferences: sleep_interval_long:   %u s", prefs.sleep_interval_long);
  prefs.lw_stat_interval = preferences.getUChar("lw_stat_int", LW_STATUS_INTERVAL);
  log_d("Preferences: lw_stat_interval:      %u cycles", prefs.lw_stat_interval);
  preferences.end();

  uint16_t voltage = getBatteryVoltage();
  if (voltage && voltage <= battery_low)
  {
    log_i("Battery low!");
    gotoSleep(sleepDuration(battery_weak));
  }

  // build payload byte array (+ reserve to prevent overflow with configuration at run-time)
  uint8_t uplinkPayload[PAYLOAD_SIZE + 8];

  LoraEncoder encoder(uplinkPayload);

  uint8_t port = 1;
  appLayer.getPayloadStage1(port, encoder);

  int16_t state = 0; // return value for calls to RadioLib

  // setup the radio based on the pinmap (connections) in config.h
  log_v("Initalise the radio");
  radio.reset();
  state = radio.begin();
  debug(state != RADIOLIB_ERR_NONE, "Initalise radio failed", state, true);

  // activate node by restoring session or otherwise joining the network
  state = lwActivate();
  // state is one of RADIOLIB_LORAWAN_NEW_SESSION or RADIOLIB_LORAWAN_SESSION_RESTORED

  // Set battery fill level -
  // the LoRaWAN network server may periodically request this information
  // 0 = external power source
  // 1 = lowest (empty battery)
  // 254 = highest (full battery)
  // 255 = unable to measure
  uint8_t battLevel;
  if (voltage == 0)
  {
    battLevel = 255;
  }
  else if (voltage > battery_charge_lim)
  {
    battLevel = 0;
  }
  else
  {
    battLevel = static_cast<uint8_t>(
        static_cast<float>(voltage - battery_discharge_lim) / static_cast<float>(battery_charge_lim - battery_discharge_lim) * 255);
    battLevel = (battLevel == 0) ? 1 : battLevel;
    battLevel = (battLevel == 255) ? 254 : battLevel;
  }
  log_d("Battery level: %u", battLevel);
  node.setDeviceStatus(battLevel);

  // Check if clock was never synchronized or sync interval has expired
  if ((rtcLastClockSync == 0) || ((rtc.getLocalEpoch() - rtcLastClockSync) > (CLOCK_SYNC_INTERVAL * 60)))
  {
    log_i("RTC sync required");
    node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_DEVICE_TIME);
  }

  // Retrieve the last uplink frame counter
  uint32_t fCntUp = node.getFCntUp();

  // Send a confirmed uplink every 64th frame
  // and also request the LinkCheck command
  if (fCntUp % 64 == 0)
  {
    log_i("[LoRaWAN] Requesting LinkCheck");
    node.sendMacCommandReq(RADIOLIB_LORAWAN_MAC_LINK_CHECK);
  }

  // Set appStatusUplink flag if required
  uint8_t appStatusUplinkInterval = appLayer.getAppStatusUplinkInterval();
  if (appStatusUplinkInterval && (fCntUp % appStatusUplinkInterval == 0))
  {
    appStatusUplinkPending = true;
  }

  // Set lwStatusUplink flag if required
  if (prefs.lw_stat_interval && (fCntUp % prefs.lw_stat_interval == 0))
  {
    lwStatusUplinkPending = true;
  }

  // get payload immediately before uplink - not used here
  appLayer.getPayloadStage2(port, encoder);

  uint8_t downlinkPayload[MAX_DOWNLINK_SIZE]; // Make sure this fits your plans!
  size_t downlinkSize;                        // To hold the actual payload size rec'd
  LoRaWANEvent_t uplinkDetails;
  LoRaWANEvent_t downlinkDetails;

  uint8_t payloadSize = encoder.getLength();
  if (payloadSize > PAYLOAD_SIZE)
  {
    log_w("Payload size exceeds maximum of %u bytes - truncating", PAYLOAD_SIZE);
    payloadSize = PAYLOAD_SIZE;
  }

  // ----- and now for the main event -----
  log_i("Sending uplink; port %u, size %u", port, payloadSize);

  // perform an uplink & optionally receive downlink
  if (fCntUp % 64 == 0)
  {
    state = node.sendReceive(
        uplinkPayload,
        payloadSize,
        port,
        downlinkPayload,
        &downlinkSize,
        true,
        &uplinkDetails,
        &downlinkDetails);
  }
  else
  {
    state = node.sendReceive(
        uplinkPayload,
        payloadSize,
        port,
        downlinkPayload,
        &downlinkSize,
        false,
        nullptr,
        &downlinkDetails);
  }
  debug((state != RADIOLIB_LORAWAN_NO_DOWNLINK) && (state != RADIOLIB_ERR_NONE), "Error in sendReceive", state, false);

  /// Uplink request - command received via downlink
  uint8_t uplinkReq = 0;

  // Check if downlink was received
  if (state != RADIOLIB_LORAWAN_NO_DOWNLINK)
  {
    // Did we get a downlink with data for us
    if (downlinkSize > 0)
    {
      log_i("Downlink port %u, data: ", downlinkDetails.fPort);
      arrayDump(downlinkPayload, downlinkSize);

      if (downlinkDetails.fPort > 0)
      {
        uplinkReq = decodeDownlink(downlinkDetails.fPort, downlinkPayload, downlinkSize);
      }
    }
    else
    {
      log_d("<MAC commands only>");
    }

    // print RSSI (Received Signal Strength Indicator)
    log_d("[LoRaWAN] RSSI:\t\t%f dBm", radio.getRSSI());

    // print SNR (Signal-to-Noise Ratio)
    log_d("[LoRaWAN] SNR:\t\t%f dB", radio.getSNR());

    // print frequency error
    log_d("[LoRaWAN] Frequency error:\t%f Hz", radio.getFrequencyError());

    // print extra information about the event
    log_d("[LoRaWAN] Event information:");
    log_d("[LoRaWAN] Confirmed:\t%d", downlinkDetails.confirmed);
    log_d("[LoRaWAN] Confirming:\t%d", downlinkDetails.confirming);
    log_d("[LoRaWAN] Datarate:\t%d", downlinkDetails.datarate);
    log_d("[LoRaWAN] Frequency:\t%7.3f MHz", downlinkDetails.freq);
    log_d("[LoRaWAN] Output power:\t%d dBm", downlinkDetails.power);
    log_d("[LoRaWAN] Frame count:\t%u", downlinkDetails.fCnt);
    log_d("[LoRaWAN] fPort:\t\t%u", downlinkDetails.fPort);
  }

  uint32_t networkTime = 0;
  uint8_t fracSecond = 0;
  if (node.getMacDeviceTimeAns(&networkTime, &fracSecond, true) == RADIOLIB_ERR_NONE)
  {
    log_i("[LoRaWAN] DeviceTime Unix:\t %ld", networkTime);
    log_i("[LoRaWAN] DeviceTime second:\t1/%u", fracSecond);

    // Update the system time with the time read from the network
    rtc.setTime(networkTime);

    // Save clock sync timestamp and clear flag
    rtcLastClockSync = rtc.getLocalEpoch();
    rtcTimeSource = E_TIME_SOURCE::E_LORA;
    log_d("RTC sync completed");
    printDateTime();
  }

  uint8_t margin = 0;
  uint8_t gwCnt = 0;
  if (node.getMacLinkCheckAns(&margin, &gwCnt) == RADIOLIB_ERR_NONE)
  {
    log_d("[LoRaWAN] LinkCheck margin:\t%d", margin);
    log_d("[LoRaWAN] LinkCheck count:\t%u", gwCnt);
  }

  if (appStatusUplinkPending)
  {
    log_i("App status uplink pending");
  }

  if (lwStatusUplinkPending)
  {
    log_i("LoRaWAN node status uplink pending");
  }

  if (uplinkReq)
  {
    sendCfgUplink(uplinkReq, uplinkIntervalSeconds);
  }
  else if (lwStatusUplinkPending)
  {
    sendCfgUplink(CMD_GET_LW_STATUS, uplinkIntervalSeconds);
    lwStatusUplinkPending = false;
  }
  else if (appStatusUplinkPending)
  {
    sendCfgUplink(CMD_GET_SENSORS_STAT, uplinkIntervalSeconds);
    appStatusUplinkPending = false;
  }

  log_d("FcntUp: %u", node.getFCntUp());

  // now save session to RTC memory
  uint8_t *persist = node.getBufferSession();
  memcpy(LWsession, persist, RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

  // wait until next uplink - observing legal & TTN Fair Use Policy constraints
  gotoSleep(sleepDuration(battery_weak));
}

// The ESP32 wakes from deep-sleep and starts from the very beginning.
// It then goes back to sleep, so loop() is never called and which is
// why it is empty.

void loop() {}
