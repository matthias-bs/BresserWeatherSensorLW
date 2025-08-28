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
// 20240818 Replaced delay() with light sleep for ESP32
// 20240829 Added missing implementation of CMD_SET_LW_STATUS_INTERVAL
// 20240920 Changed sendCfgUplink() to encodeCfgUplink()
// 20241227 Removed delay from encodeCfgUplink()
// 20250806 Refactored by adding SystemContext class,
//          replaced getLocalEpoch() (ESP32Time) with time() (POSIX)
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "BresserWeatherSensorLWCfg.h"
#include "BresserWeatherSensorLWCmd.h"
#include <Preferences.h>
#include <RadioLib.h>
#include "src/AppLayer.h"
#include "src/SystemContext.h"
#if defined(ARDUINO_ESP32S3_POWERFEATHER)
#include <PowerFeather.h>
using namespace PowerFeather;
#endif

/*
 * From config.h
 */
void debug(bool isFail, const char* message, int state, bool Freeze);

/// Application layer
extern AppLayer appLayer;

/// System context
extern SystemContext sysCtx;


// Decode downlink
uint8_t decodeDownlink(uint8_t port, uint8_t *payload, size_t size)
{
  if ((port == CMD_GET_DATETIME) && (payload[0] == 0x00) && (size == 1))
  {
    log_i("Get date/time");
    return CMD_GET_DATETIME;
  }

  if ((port == CMD_SET_DATETIME) && (size == 4))
  {
    time_t set_time = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
    sysCtx.setTime(set_time, E_TIME_SOURCE::E_SET);

#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
    char tbuf[25];
    struct tm timeinfo;

    localtime_r(&set_time, &timeinfo);
    strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
    log_i("Set date/time: %s", tbuf);
#endif
    return 0;
  }

  if ((port == CMD_SET_SLEEP_INTERVAL) && (size == 2))
  {
    sysCtx.sleep_interval = (payload[0] << 8) | payload[1];
    sysCtx.savePreferences();
    log_i("Set sleep_interval: %u s", sysCtx.sleep_interval);
    return 0;
  }

  if ((port == CMD_SET_SLEEP_INTERVAL_LONG) && (size == 2))
  {
    sysCtx.sleep_interval_long = (payload[0] << 8) | payload[1];
    sysCtx.savePreferences();
    log_i("Set sleep_interval_long: %u s", sysCtx.sleep_interval_long);
    return 0;
  }

  if ((port == CMD_SET_LW_STATUS_INTERVAL) && (size == 1))
  {
    sysCtx.lw_stat_interval = payload[0];
    sysCtx.savePreferences();
    log_i("Set lw_stat_interval: %u", sysCtx.lw_stat_interval);
    return 0;
  }

  if ((port == CMD_GET_LW_CONFIG) && (payload[0] == 0x00) && (size == 1))
  {
    log_i("Get config");
    return CMD_GET_LW_CONFIG;
  }

  if ((port == CMD_GET_LW_STATUS) && (payload[0] == 0x00) && (size == 1))
  {
    log_i("Get device status");
    return CMD_GET_LW_STATUS;
  }

  log_d("appLayer.decodeDownlink(port=%d, payload[0]=0x%02X, size=%d)", port, payload[0], size);
  return appLayer.decodeDownlink(port, payload, size);
}


// Encode configuration/status uplink
void encodeCfgUplink(uint8_t port, uint8_t *uplinkPayload, uint8_t &payloadSize)
{
  log_d("--- Uplink Configuration/Status ---");

  uint8_t uplinkReq = port;

  //
  // Encode data as byte array for LoRaWAN transmission
  //
  LoraEncoder encoder(uplinkPayload);

  if (uplinkReq == CMD_GET_DATETIME)
  {
    log_i("Date/Time");
    time_t t_now = time(nullptr);
    encoder.writeUint8((t_now >> 24) & 0xff);
    encoder.writeUint8((t_now >> 16) & 0xff);
    encoder.writeUint8((t_now >> 8) & 0xff);
    encoder.writeUint8(t_now & 0xff);

    encoder.writeUint8(static_cast<uint8_t>(sysCtx.getRtcTimeSource()));
  }
  else if (uplinkReq == CMD_GET_LW_CONFIG)
  {
    log_i("LoRaWAN Config");
    encoder.writeUint8(sysCtx.sleep_interval >> 8);
    encoder.writeUint8(sysCtx.sleep_interval & 0xFF);
    encoder.writeUint8(sysCtx.sleep_interval_long >> 8);
    encoder.writeUint8(sysCtx.sleep_interval_long & 0xFF);
    encoder.writeUint8(sysCtx.lw_stat_interval);
  }
  else if (uplinkReq == CMD_GET_LW_STATUS)
  {
    uint8_t status = sysCtx.longSleepActive() ? 1 : 0;
    log_i("Device Status: U_batt=%u mV, longSleep=%u", getBatteryVoltage(), status);
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

  payloadSize = encoder.getLength();
}
