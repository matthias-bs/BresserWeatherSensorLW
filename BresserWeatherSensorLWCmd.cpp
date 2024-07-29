///////////////////////////////////////////////////////////////////////////////
// BresserWeatherSensorLWCmd.h
//
// LoRaWAN Command Interface
//
// Definition of control/configuration commands and status responses for
// LoRaWAN node/network layer
//
// created: 07/2024
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
// 20240723 Extracted from BresserWeatherSensorLW.ino
// 20240729 Added PowerFeather specific status information
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "BresserWeatherSensorLWCfg.h"
#include "BresserWeatherSensorLWCmd.h"
#include <Preferences.h>
#include <RadioLib.h>
#include <ESP32Time.h>
#include "src/AppLayer.h"

#if defined(ARDUINO_ESP32S3_POWERFEATHER)
#include <PowerFeather.h>
using namespace PowerFeather;
#endif

/*
 * From config.h
 */
void debug(bool isFail, const char* message, int state, bool Freeze);
extern LoRaWANNode node;
extern const uint16_t uplinkIntervalSeconds;


/*
 * External variables (declared in BresserWeatherSensorLW.ino)
 */
extern struct sPrefs
{
  uint16_t sleep_interval;      //!< preferences: sleep interval
  uint16_t sleep_interval_long; //!< preferences: sleep interval long
  uint8_t lw_stat_interval;     //!< preferences: LoRaWAN node status uplink interval
} prefs;

/// Preferences (stored in flash memory)
extern Preferences preferences;

/// Real time clock
extern ESP32Time rtc;

/// Application layer
extern AppLayer appLayer;

extern bool longSleep;
extern time_t rtcLastClockSync;
extern E_TIME_SOURCE rtcTimeSource;


// Decode downlink
uint8_t decodeDownlink(uint8_t port, uint8_t *payload, size_t size)
{
  log_v("Port: %d", port);

  if ((port == CMD_GET_DATETIME) && (payload[0] == 0x00) && (size == 1))
  {
    log_d("Get date/time");
    return CMD_GET_DATETIME;
  }

  if ((port == CMD_SET_DATETIME) && (size == 4))
  {
    time_t set_time = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
    rtc.setTime(set_time);
    rtcLastClockSync = rtc.getLocalEpoch();
    rtcTimeSource = E_TIME_SOURCE::E_SET;

#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
    char tbuf[25];
    struct tm timeinfo;

    localtime_r(&set_time, &timeinfo);
    strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
    log_d("Set date/time: %s", tbuf);
#endif
    return 0;
  }

  if ((port == CMD_SET_SLEEP_INTERVAL) && (size == 2))
  {
    prefs.sleep_interval = (payload[0] << 8) | payload[1];
    log_d("Set sleep_interval: %u s", prefs.sleep_interval);
    preferences.begin("BWS-LW", false);
    preferences.putUShort("sleep_int", prefs.sleep_interval);
    preferences.end();
    return 0;
  }

  if ((port == CMD_SET_SLEEP_INTERVAL_LONG) && (size == 2))
  {
    prefs.sleep_interval_long = (payload[0] << 8) | payload[1];
    log_d("Set sleep_interval_long: %u s", prefs.sleep_interval_long);
    preferences.begin("BWS-LW", false);
    preferences.putUShort("sleep_int_long", prefs.sleep_interval_long);
    preferences.end();
    return 0;
  }

  if ((port == CMD_GET_LW_CONFIG) && (payload[0] == 0x00) && (size == 1))
  {
    log_d("Get config");
    return CMD_GET_LW_CONFIG;
  }

  if ((port == CMD_GET_LW_STATUS) && (payload[0] == 0x00) && (size == 1))
  {
    log_d("Get device status");
    return CMD_GET_LW_STATUS;
  }

  log_d("appLayer.decodeDownlink(port=%d, payload[0]=0x%02X, size=%d)", port, payload[0], size);
  return appLayer.decodeDownlink(port, payload, size);
}


// Send configuration uplink
void sendCfgUplink(uint8_t uplinkReq, uint32_t uplinkInterval)
{
  log_d("--- Uplink Configuration/Status ---");

  uint8_t uplinkPayload[48];
  uint8_t port = uplinkReq;

  //
  // Encode data as byte array for LoRaWAN transmission
  //
  LoraEncoder encoder(uplinkPayload);

  if (uplinkReq == CMD_GET_DATETIME)
  {
    log_d("Date/Time");
    time_t t_now = rtc.getLocalEpoch();
    encoder.writeUint8((t_now >> 24) & 0xff);
    encoder.writeUint8((t_now >> 16) & 0xff);
    encoder.writeUint8((t_now >> 8) & 0xff);
    encoder.writeUint8(t_now & 0xff);

    encoder.writeUint8(static_cast<uint8_t>(rtcTimeSource));
  }
  else if (uplinkReq == CMD_GET_LW_CONFIG)
  {
    log_d("LoRaWAN Config");
    encoder.writeUint8(prefs.sleep_interval >> 8);
    encoder.writeUint8(prefs.sleep_interval & 0xFF);
    encoder.writeUint8(prefs.sleep_interval_long >> 8);
    encoder.writeUint8(prefs.sleep_interval_long & 0xFF);
    encoder.writeUint8(prefs.lw_stat_interval);
  }
  else if (uplinkReq == CMD_GET_LW_STATUS)
  {
    uint8_t status = longSleep ? 1 : 0;
    log_d("Device Status: U_batt=%u mV, longSleep=%u", getBatteryVoltage(), status);
    encoder.writeUint16(getBatteryVoltage());
    encoder.writeUint8(status);
    #if defined(ARDUINO_ESP32S3_POWERFEATHER)
    Result res;
    uint16_t voltage;
    int16_t current;
    uint8_t battery_soc;
    uint8_t battery_soh;
    uint16_t battery_cycles;
    float battery_temp;
    int time_left;

    res = Board.getSupplyVoltage(voltage);
    if (res == Result::Ok)
    {
      encoder.writeUint16(voltage);
    }
    else
    {
      encoder.writeUint16(INV_UINT16);
    }
    
    res = Board.getSupplyCurrent(current);
    if (res == Result::Ok)
    {
      encoder.writeUint16(current + 0x8000);
    }
    else
    {
      encoder.writeUint16(INV_UINT16);
    }

    res = Board.getBatteryCurrent(current);
    if (res == Result::Ok)
    {
      encoder.writeUint16(current + 0x8000);
    }
    else
    {
      encoder.writeUint16(INV_UINT16);
    }

    res = Board.getBatteryCharge(battery_soc);
    if (res == Result::Ok)
    {
      encoder.writeUint8(battery_soc);
    }
    else
    {
      encoder.writeUint8(INV_UINT8);
    }

    res = Board.getBatteryHealth(battery_soh);
    if (res == Result::Ok)
    {
      encoder.writeUint8(battery_soh);
    }
    else
    {
      encoder.writeUint8(INV_UINT8);
    }

    res = Board.getBatteryCycles(battery_cycles);
    if (res == Result::Ok)
    {
      encoder.writeUint16(battery_cycles);
    }
    else
    {
      encoder.writeUint16(INV_UINT16);
    }

    res = Board.getBatteryTimeLeft(time_left);
    if (res == Result::Ok)
    {
      encoder.writeUint32(time_left + 0x80000000);
    }
    else
    {
      encoder.writeUint32(INV_UINT32);
    }

    res = Board.getBatteryTemperature(battery_temp);
    if (res == Result::Ok)
    {
      encoder.writeTemperature(battery_temp);
    }
    else
    {
      encoder.writeTemperature(INV_TEMP);
    }
    #endif
  }
  else
  {
    appLayer.getConfigPayload(uplinkReq, port, encoder);
  }
  log_d("Configuration uplink: port=%d, size=%d", port, encoder.getLength());

  for (int i = 0; i < encoder.getLength(); i++)
  {
    Serial.printf("%02X ", uplinkPayload[i]);
  }
  Serial.println();

  // wait before sending uplink
  uint32_t minimumDelay = uplinkInterval * 1000UL;
  uint32_t interval = node.timeUntilUplink();     // calculate minimum duty cycle delay (per FUP & law!)
  uint32_t delayMs = max(interval, minimumDelay); // cannot send faster than duty cycle allows

  log_d("Sending configuration uplink in %u s", delayMs / 1000);
  delay(delayMs);
  log_d("Sending configuration uplink now.");
  int16_t state = node.sendReceive(uplinkPayload, encoder.getLength(), port);
  debug((state != RADIOLIB_LORAWAN_NO_DOWNLINK) && (state != RADIOLIB_ERR_NONE), "Error in sendReceive", state, false);
}