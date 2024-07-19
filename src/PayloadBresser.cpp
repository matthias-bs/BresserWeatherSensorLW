///////////////////////////////////////////////////////////////////////////////
// PayloadBresser.cpp
//
// Read Bresser sensor data and encode as LoRaWAN payload
//
// created: 05/2024
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
// 20240521 Created
// 20240523 Added encodeWeatherSensor(),
//          added defines for signalling invalid data
// 20240524 Moved Weather Sensor and rain gauge/lightning post processing
//          from AppLayer into this class
//          Added handling of sensor feature flags
// 20240528 Fixes
// 20240529 Changed encoding of INV_TEMP
// 20240530 Weather sensor: Fixed encoding of invalid temperature
// 20240601 Added mapping of invalid RainGauge values to INV_FLOAT
// 20240603 Added encoding of sensor battery status
// 20240608 Modified default number of sensors
// 20240609 Fixed exception caused by max_num_sensors = 0
// 20240717 Fixed handling of rxFlags in begin(): getData()
//          Added scanBresser(), modified begin() to trigger scan
// 20240718 Fixed premature return from begin() leading to empty payload
// 20240719 Fixed enabling of all decoders in scanBresser()
//
// ToDo:
// - Add handling of Professional Rain Gauge
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadBresser.h"

void PayloadBresser::begin(void)
{

    appPrefs.begin("BWS-LW-APP", false);
    ws_scantime = appPrefs.getUChar("ws_scan_t", 0);

    // Clear scan time in Preferences set in previous run
    // (additionally used as scan request flag)
    appPrefs.putUChar("ws_scan_t", 0);
    appPrefs.end();
    if (ws_scantime > 0)
    {
        log_d("ws_scantime: %u s", ws_scantime);
        weatherSensor.begin(PAYLOAD_SIZE / 8, false);
        return;
    }

    weatherSensor.begin(MAX_NUM_868MHZ_SENSORS);

    if (weatherSensor.sensor.size() == 0)
        return;
    
    weatherSensor.clearSlots();
    appPrefs.begin("BWS-LW-APP", false);
    uint8_t ws_timeout = appPrefs.getUChar("ws_timeout", WEATHERSENSOR_TIMEOUT);
    log_d("Preferences: weathersensor_timeout: %u s", ws_timeout);
    appPrefs.end();

    log_i("Waiting for Weather Sensor Data; timeout %u s", ws_timeout);
    bool decode_ok = weatherSensor.getData(ws_timeout * 1000, weatherSensor.rxFlags);
    (void)decode_ok;
    log_i("Receiving Weather Sensor Data %s", decode_ok ? "o.k." : "failed");
}

void PayloadBresser::scanBresser(uint8_t ws_scantime, LoraEncoder &encoder)
{
    weatherSensor.clearSlots();
    
    // Save enabled decoders
    uint8_t enabled_decoders = weatherSensor.enDecoders;

    // Enable all decoders
    weatherSensor.enDecoders = 0xFF;

    log_i("Scanning for 868 MHz sensors (max.: %u); timeout %u s", weatherSensor.sensor.size(), ws_scantime);
    weatherSensor.getData(ws_scantime * 1000, DATA_ALL_SLOTS | DATA_COMPLETE);

    for (size_t i = 0; i < weatherSensor.sensor.size(); i++)
    {
        if (!weatherSensor.sensor[i].valid)
            continue;

        // Convert decoder bitmap to decoder number
        uint8_t decoder = 0;
        for (int j = 0; j < 8; j++)
        {
            if ((weatherSensor.sensor[i].decoder >> j) & 0x1)
            {
                decoder = j;
                break;
            }
        }
        uint8_t flags = 0;
        if ((weatherSensor.sensor[i].s_type == SENSOR_TYPE_WEATHER0) ||
            (weatherSensor.sensor[i].s_type == SENSOR_TYPE_WEATHER1))
        {
            if (weatherSensor.sensor[i].w.temp_ok)
                flags |= 0x1;
            if (weatherSensor.sensor[i].w.humidity_ok)
                flags |= 0x2;
            if (weatherSensor.sensor[i].w.wind_ok)
                flags |= 0x4;
            if (weatherSensor.sensor[i].w.rain_ok)
                flags |= 0x8;
            if (weatherSensor.sensor[i].w.uv_ok)
                flags |= 0x10;
            if (weatherSensor.sensor[i].w.light_ok)
                flags |= 0x20;
        }

        encoder.writeUint8(weatherSensor.sensor[i].sensor_id >> 24);
        encoder.writeUint8((weatherSensor.sensor[i].sensor_id >> 16) & 0xFF);
        encoder.writeUint8((weatherSensor.sensor[i].sensor_id >> 8) & 0xFF);
        encoder.writeUint8(weatherSensor.sensor[i].sensor_id & 0xFF);
        encoder.writeUint8((decoder << 4) | weatherSensor.sensor[i].s_type);
        encoder.writeUint8(weatherSensor.sensor[i].chan);
        encoder.writeUint8(flags);
        encoder.writeUint8(static_cast<uint8_t>(-weatherSensor.sensor[i].rssi));
    }

    log_d("Size: %u", encoder.getLength());

    // Restore enabled decoders
    weatherSensor.enDecoders = enabled_decoders;
}

void PayloadBresser::encodeBresser(uint8_t *appPayloadCfg, uint8_t *appStatus, LoraEncoder &encoder)
{
    if (weatherSensor.sensor.size() == 0)
        return;

    // Handle weather sensors - which only have one channel (0) - first.
    // Configuration for SENSOR_TYPE_WEATHER0 is integrated into SENSOR_TYPE_WEATHER1.
    uint8_t flags = appPayloadCfg[1];
    if (flags & 1)
    {
        // Try to find SENSOR_TYPE_WEATHER1
        int idx = weatherSensor.findType(SENSOR_TYPE_WEATHER1);
        if (idx > -1)
        {
            rainGauge.set_max(100000);
        }
        else
        {
            // Try to find SENSOR_TYPE_WEATHER0
            idx = weatherSensor.findType(SENSOR_TYPE_WEATHER0);
            rainGauge.set_max(1000);
        }

#ifdef RAINDATA_EN
        // Check if time is valid
        if (*_rtcLastClockSync > 0)
        {
            // Get local date and time
            struct tm timeinfo;
            time_t tnow = _rtc->getLocalEpoch();
            localtime_r(&tnow, &timeinfo);

            // If weather sensor has be found and rain data is valid, update statistics
            if ((idx > -1) && weatherSensor.sensor[idx].valid && weatherSensor.sensor[idx].w.rain_ok)
            {
                rainGauge.update(tnow, weatherSensor.sensor[idx].w.rain_mm, weatherSensor.sensor[idx].startup);
            }
        }
#endif
        if ((idx > -1) && weatherSensor.sensor[idx].battery_ok)
        {
            appStatus[1] |= 1;
        }
        encodeWeatherSensor(idx, flags, encoder);
    }

    for (int type = 2; type < 16; type++)
    {
        // Skip if bitmap is zero
        if (appPayloadCfg[type] == 0)
            continue;

#ifdef LIGHTNINGSENSOR_EN
        // Lightning sensor has fixed channel (0)
        if (type == SENSOR_TYPE_LIGHTNING)
        {
            int idx = weatherSensor.findType(type, 0);
            if ((idx > -1) && weatherSensor.sensor[idx].battery_ok)
            {
                appStatus[type] |= 1;
            }

            // Check if time is valid
            if (*_rtcLastClockSync > 0)
            {
                // Get local date and time
                time_t tnow = _rtc->getLocalEpoch();

                // If lightning sensor has be found and data is valid, run post-processing
                if ((idx > -1) && weatherSensor.sensor[idx].valid)
                {
                    lightningProc.update(
                        tnow,
                        weatherSensor.sensor[idx].lgt.strike_count,
                        weatherSensor.sensor[idx].lgt.distance_km,
                        weatherSensor.sensor[idx].startup);
                }
            }

            encodeLightningSensor(idx, appPayloadCfg[type], encoder);
            continue;
        }
#endif

        // Handle sensors with channel selection
        for (uint8_t ch = 1; ch <= 7; ch++)
        {
            // Check if channel is enabled
            if (!((appPayloadCfg[type] >> ch) & 0x1))
                continue;

            if (!isSpaceLeft(encoder, type))
                break;

            log_i("%s Sensor Ch %u", sensorTypes[type], ch);
            int idx = weatherSensor.findType(type, ch);
            if (idx == -1)
            {
                log_i("-- Failure");
            }
            else if (weatherSensor.sensor[idx].battery_ok)
            {
                appStatus[type] |= (1 << ch);
            }

            if (type == SENSOR_TYPE_THERMO_HYGRO)
            {
                encodeThermoHygroSensor(idx, encoder);
            }
            else if (type == SENSOR_TYPE_POOL_THERMO)
            {
                encodePoolThermometer(idx, encoder);
            }
            else if (type == SENSOR_TYPE_SOIL)
            {
                encodeSoilSensor(idx, encoder);
            }
            else if (type == SENSOR_TYPE_LEAKAGE)
            {
                encodeLeakageSensor(idx, encoder);
            }
            else if (type == SENSOR_TYPE_AIR_PM)
            {
                encodeAirPmSensor(idx, encoder);
            }
            else if (type == SENSOR_TYPE_CO2)
            {
                encodeCo2Sensor(idx, encoder);
            }
            else if (type == SENSOR_TYPE_HCHO_VOC)
            {
                encodeHchoVocSensor(idx, encoder);
            }
        }
    }
}

// Payload size: 2...17 bytes (ENCODE_AS_FLOAT == false) / 2...23 bytes (ENCODE_AS_FLOAT == true)
void PayloadBresser::encodeWeatherSensor(int idx, uint8_t flags, LoraEncoder &encoder)
{
    //                    Weather Stations     Professional  3-in-1 Professional
    //                 5-in-1  6-in-1  7-in-1   Rain Gauge       Wind Gauge
    //
    // Temperature       X       X       X         X                 X
    // Humidity          X       X       X                           X
    // Wind              X       X       X                           X
    // Rain              X       X       X         X
    // UV                        X       X
    // Light Intensity                   X

    if (idx == -1)
    {
        log_i("-- Weather Sensor Failure");
        // Invalidate
        encoder.writeTemperature(INV_TEMP); // Temperature
        if (flags & PAYLOAD_WS_HUMIDITY)
            encoder.writeUint8(INV_UINT8); // Humidity
        if (flags & PAYLOAD_WS_RAINGAUGE)
            encoder.writeRawFloat(INV_FLOAT); // Rain
        if (flags & PAYLOAD_WS_WIND)
        {
#ifdef ENCODE_AS_FLOAT
            encoder.writeRawFloat(INV_FLOAT); // Wind gust
            encoder.writeRawFloat(INV_FLOAT); // Wind avg
            encoder.writeRawFloat(INV_FLOAT); // Wind dir
#else
            encoder.writeUint16(INV_UINT16); // Wind gust
            encoder.writeUint16(INV_UINT16); // Wind avg
            encoder.writeUint16(INV_UINT16); // Wind dir
#endif
        }
        if (flags & PAYLOAD_WS_UV)
            encoder.writeUint8(INV_UINT8); // UV
        if (flags & PAYLOAD_WS_LIGHT)
            encoder.writeRawFloat(INV_UINT32); // Light
    }
    else
    {
        if (weatherSensor.sensor[idx].w.temp_ok)
        {
            log_i("Air Temperature:    %3.1f °C", weatherSensor.sensor[idx].w.temp_c);
            encoder.writeTemperature(weatherSensor.sensor[idx].w.temp_c);
        }
        else
        {
            log_i("Air Temperature:     --.- °C");
            encoder.writeTemperature(INV_TEMP);
        }
        if (flags & PAYLOAD_WS_HUMIDITY)
        {
            if (weatherSensor.sensor[idx].w.humidity_ok)
            {
                log_i("Humidity:            %2d   %%", weatherSensor.sensor[idx].w.humidity);
                encoder.writeUint8(weatherSensor.sensor[idx].w.humidity);
            }
            else
            {
                log_i("Humidity:            --   %%");
                encoder.writeUint8(INV_UINT8);
            }
        }
        if (flags & PAYLOAD_WS_RAINGAUGE)
        {
            if (weatherSensor.sensor[idx].w.rain_ok)
            {
                log_i("Rain Gauge:       %7.1f mm", weatherSensor.sensor[idx].w.rain_mm);
                encoder.writeRawFloat(weatherSensor.sensor[idx].w.rain_mm);
            }
            else
            {
                log_i("Rain Gauge:       ---.- mm");
                encoder.writeRawFloat(INV_FLOAT);
            }
        }
        if (flags & PAYLOAD_WS_WIND)
        {
            if (weatherSensor.sensor[idx].w.wind_ok)
            {
                log_i("Wind Speed (avg.):    %3.1f m/s", weatherSensor.sensor[idx].w.wind_avg_meter_sec_fp1 / 10.0);
                log_i("Wind Speed (max.):    %3.1f m/s", weatherSensor.sensor[idx].w.wind_gust_meter_sec_fp1 / 10.0);
                log_i("Wind Direction:     %4.1f °", weatherSensor.sensor[idx].w.wind_direction_deg_fp1 / 10.0);
                encoder.writeUint16(weatherSensor.sensor[idx].w.wind_avg_meter_sec_fp1);
                encoder.writeUint16(weatherSensor.sensor[idx].w.wind_gust_meter_sec_fp1);
                encoder.writeUint16(weatherSensor.sensor[idx].w.wind_direction_deg_fp1);
            }
            else
            {
                log_i("Wind Speed (avg.):     --.- m/s");
                log_i("Wind Speed (max.):     --.- m/s");
                log_i("Wind Direction:     ---.- °");
                encoder.writeUint16(INV_UINT16);
                encoder.writeUint16(INV_UINT16);
                encoder.writeUint16(INV_UINT16);
            }
        }
        if (flags & PAYLOAD_WS_UV)
        {
            if (weatherSensor.sensor[idx].w.uv_ok)
            {
                log_i("UV Index:            %3.1f", weatherSensor.sensor[idx].w.uv);
                encoder.writeUint8(static_cast<uint8_t>(weatherSensor.sensor[idx].w.uv * 10));
            }
            else
            {
                log_i("UV Index:            --.-");
                encoder.writeUint8(INV_UINT8);
            }
        }
        if (flags & PAYLOAD_WS_LIGHT)
        {
            if (weatherSensor.sensor[idx].w.light_ok)
            {
                log_i("Light intensity:  %06f lx", weatherSensor.sensor[idx].w.light_lux);
                encoder.writeUint32(static_cast<uint32_t>(weatherSensor.sensor[idx].w.light_lux));
            }
            else
            {
                log_i("Light intensity:   ------ lx");
                encoder.writeUint32(INV_UINT32);
            }
        }
    }

    // Rain data statistics
#ifdef RAINDATA_EN
    if ((idx) && weatherSensor.sensor[idx].valid && weatherSensor.sensor[idx].w.rain_ok)
    {
        if (flags & PAYLOAD_WS_RAIN_H)
        {
            bool valid = false;
            float rainPasthour = rainGauge.pastHour(&valid);
            log_i("Rain past 60min:  %7.1f mm", rainPasthour);
            if (!valid)
            {
                rainPasthour = INV_FLOAT;
            }
            encoder.writeRawFloat(rainPasthour);
        }
        if (flags & PAYLOAD_WS_RAIN_DWM)
        {
            float rain = rainGauge.currentDay();
            log_i("Rain curr. day:   %7.1f mm", rain);
            if (rain == -1)
            {
                rain = INV_FLOAT;
            }
            encoder.writeRawFloat(rain);

            rain = rainGauge.currentWeek();
            log_i("Rain curr. week:  %7.1f mm", rain);
            if (rain == -1)
            {
                rain = INV_FLOAT;
            }
            encoder.writeRawFloat(rain);

            rain = rainGauge.currentMonth();
            log_i("Rain curr. month: %7.1f mm", rain);
            if (rain == -1)
            {
                rain = INV_FLOAT;
            }
            encoder.writeRawFloat(rain);
        }
    }
    else
    {
        log_i("Current rain gauge statistics not valid.");
        if (flags & PAYLOAD_WS_RAIN_H)
            encoder.writeRawFloat(INV_FLOAT);
        if (flags & PAYLOAD_WS_RAIN_DWM)
        {
            encoder.writeRawFloat(INV_FLOAT);
            encoder.writeRawFloat(INV_FLOAT);
            encoder.writeRawFloat(INV_FLOAT);
        }
    }
#endif
}

void PayloadBresser::encodeThermoHygroSensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeTemperature(INV_TEMP);
        encoder.writeUint8(INV_UINT8);
    }
    else
    {
        log_i("Temperature: %3.1f °C", weatherSensor.sensor[idx].w.temp_c);
        log_i("Humidity:     %2d   %%", weatherSensor.sensor[idx].w.humidity);
        encoder.writeTemperature(weatherSensor.sensor[idx].w.temp_c);
        encoder.writeUint8(weatherSensor.sensor[idx].w.humidity);
    }
}

void PayloadBresser::encodePoolThermometer(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeTemperature(INV_TEMP);
    }
    else
    {
        log_i("Temperature: %3.1f °C", weatherSensor.sensor[idx].w.temp_c);
        encoder.writeTemperature(weatherSensor.sensor[idx].w.temp_c);
    }
}

void PayloadBresser::encodeSoilSensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeTemperature(INV_TEMP);
        encoder.writeUint8(INV_UINT8);
    }
    else
    {
        log_i("Soil Temperature: %3.1f °C", weatherSensor.sensor[idx].soil.temp_c);
        log_i("Soil Moisture:     %2d   %%", weatherSensor.sensor[idx].soil.moisture);
        encoder.writeTemperature(weatherSensor.sensor[idx].soil.temp_c);
        encoder.writeUint8(weatherSensor.sensor[idx].soil.moisture);
    }
}

void PayloadBresser::encodeLeakageSensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeUint8(INV_UINT8);
    }
    else
    {
        log_i("Leakage Alarm:      %u", weatherSensor.sensor[idx].leak.alarm);
        encoder.writeUint8(weatherSensor.sensor[idx].leak.alarm ? 1 : 0);
    }
}

void PayloadBresser::encodeAirPmSensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeUint16(INV_UINT16);
        encoder.writeUint16(INV_UINT16);
        encoder.writeUint16(INV_UINT16);
    }
    else
    {
        if (weatherSensor.sensor[idx].pm.pm_1_0_init)
        {
            log_i("PM1.0: init");
            encoder.writeUint16(INV_UINT16);
        }
        else
        {
            log_i("PM1.0: %u µg/m³", weatherSensor.sensor[idx].pm.pm_1_0);
            encoder.writeUint16(weatherSensor.sensor[idx].pm.pm_1_0);
        }
        if (weatherSensor.sensor[idx].pm.pm_2_5_init)
        {
            log_i("PM2.5: init");
            encoder.writeUint16(INV_UINT16);
        }
        else
        {
            log_i("PM2.5: %u µg/m³", weatherSensor.sensor[idx].pm.pm_2_5);
            encoder.writeUint16(weatherSensor.sensor[idx].pm.pm_2_5);
        }
        if (weatherSensor.sensor[idx].pm.pm_10_init)
        {
            log_i("PM10: init");
            encoder.writeUint16(INV_UINT16);
        }
        else
        {
            log_i("PM10: %u µg/m³", weatherSensor.sensor[idx].pm.pm_10);
            encoder.writeUint16(weatherSensor.sensor[idx].pm.pm_10);
        }
    }
}

#ifdef LIGHTNINGSENSOR_EN
// Payload size: 3 bytes (raw) / 7 bytes (pre-processed) / 10 bytes (both)
void PayloadBresser::encodeLightningSensor(int idx, uint8_t flags, LoraEncoder &encoder)
{
    if (flags & PAYLOAD_LIGHTNING_RAW)
    {
        // Raw sensor values
        if (idx == -1)
        {
            // Invalidate
            encoder.writeUint8(INV_UINT8);
            encoder.writeUint16(INV_UINT16);
        }
        else
        {
            log_i("Lightning Distance:      %2u km", weatherSensor.sensor[idx].lgt.distance_km);
            log_i("Lightning Strike Count %4u", weatherSensor.sensor[idx].lgt.strike_count);
            encoder.writeUint8(weatherSensor.sensor[idx].lgt.distance_km);
            encoder.writeUint16(weatherSensor.sensor[idx].lgt.strike_count);
        }
    }

    if (flags & PAYLOAD_LIGHTNING_PROC)
    {
        // Post-processed sensor values
        if (lightningProc.lastEvent(lightn_ts, lightn_events, lightn_distance))
        {
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
            struct tm timeinfo;
            char tbuf[25];

            localtime_r(&lightn_ts, &timeinfo);
            strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
#endif
            log_i("Last lightning event @%s: %d events, %d km", tbuf, lightn_events, lightn_distance);
            encoder.writeUnixtime(lightn_ts);
            encoder.writeUint16(lightn_events);
            encoder.writeUint8(lightn_distance);
        }
        else
        {
            log_i("-- No Lightning Event Data Available");
            encoder.writeUint32(INV_UINT32);
            encoder.writeUint16(INV_UINT16);
            encoder.writeUint8(INV_UINT8);
        }
    }
}
#endif

void PayloadBresser::encodeCo2Sensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeUint16(INV_UINT16);
    }
    else
    {
        if (weatherSensor.sensor[idx].co2.co2_init)
        {
            log_i("CO2: init");
            encoder.writeUint16(INV_UINT16);
        }
        else
        {
            log_i("CO2: %4u", weatherSensor.sensor[idx].co2.co2_ppm);
            encoder.writeUint16(weatherSensor.sensor[idx].co2.co2_ppm);
        }
    }
}

void PayloadBresser::encodeHchoVocSensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        // Invalidate
        encoder.writeUint16(INV_UINT16);
        encoder.writeUint8(INV_UINT8);
    }
    else
    {
        if (weatherSensor.sensor[idx].voc.hcho_init)
        {
            log_i("HCHO: init");
            encoder.writeUint16(INV_UINT16);
        }
        else
        {
            log_i("HCHO: %u", weatherSensor.sensor[idx].voc.hcho_ppb);
            encoder.writeUint16(weatherSensor.sensor[idx].voc.hcho_ppb);
        }

        if (weatherSensor.sensor[idx].voc.voc_init)
        {
            log_i("VOC: init");
            encoder.writeUint8(INV_UINT8);
        }
        else
        {
            log_i("VOC: %u", weatherSensor.sensor[idx].voc.voc_level);
            encoder.writeUint8(weatherSensor.sensor[idx].voc.voc_level);
        }
    }
}