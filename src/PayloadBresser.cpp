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
//
// ToDo:
// - Add handling of Professional Rain Gauge
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadBresser.h"

void PayloadBresser::begin(void)
{
    weatherSensor.begin();
    weatherSensor.clearSlots();
    appPrefs.begin("BWS-LW-APP", false);
    uint8_t ws_timeout = appPrefs.getUChar("ws_timeout", WEATHERSENSOR_TIMEOUT);
    log_d("Preferences: weathersensor_timeout: %u s", ws_timeout);
    appPrefs.end();

    log_i("Waiting for Weather Sensor Data; timeout %u s", ws_timeout);
    bool decode_ok = weatherSensor.getData(ws_timeout * 1000, DATA_ALL_SLOTS);
    (void)decode_ok;
    log_i("Receiving Weather Sensor Data %s", decode_ok ? "o.k." : "failed");
}

void PayloadBresser::encodeBresser(uint8_t *appPayloadCfg, LoraEncoder &encoder)
{
    // Handle weather sensors first
    // SENSOR_TYPE_WEATHER0 and WEATHER_TYPE_WEATHER1 are treated as one.
    // Those sensors only have one channel (0).

    uint8_t flags = appPayloadCfg[0] | appPayloadCfg[1];
    if (flags & 1)
    {
        // Try to find SENSOR_TYPE_WEATHER1
        int idx = weatherSensor.findType(SENSOR_TYPE_WEATHER1);
        if (idx > 0)
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
#ifdef LIGHTNINGSENSOR_EN

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
#endif

            encodeLightningSensor(idx, appPayloadCfg[type], encoder);
            continue;
        }
#endif

        // Handle sensors with channel selection
        for (int bit = 7; bit >= 0; bit--)
        {
            // Check if channel is enabled
            if (!((appPayloadCfg[type] >> bit) & 0x1))
                continue;

            int idx = weatherSensor.findType(type, bit);

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
            else if (type == SENSOR_TYPE_WEATHER0)
            {
                // Note:
                // SENSOR_TYPE_RAIN is changed to SENSOR_TYPE_WEATHER0 by the decoder
                // to avoid mix-up with SENSOR_TYPE_LIGHTNING (same type code!).
                // FIXME
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
        encoder.writeUint16(INV_TEMP); // Temperature
        if (flags & PAYLOAD_WS_HUMIDITY)
            encoder.writeUint8(INV_UINT8); // Humidity
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
        if (flags & PAYLOAD_WS_RAINGAUGE)
            encoder.writeRawFloat(INV_FLOAT); // Rain
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
            encoder.writeUint16(INV_UINT16);
        }
        if (flags & PAYLOAD_WS_HUMIDITY)
        {
            if (weatherSensor.sensor[idx].w.humidity_ok)
            {
                log_i("Humidity:            %2d   %%", weatherSensor.sensor[idx].w.humidity);
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
            log_i("Rain past 60min:  %7.1f mm", rainGauge.pastHour());
            encoder.writeRawFloat(rainGauge.pastHour());
        }
        if (flags & PAYLOAD_WS_RAIN_DWM)
        {
            log_i("Rain curr. day:   %7.1f mm", rainGauge.currentDay());
            log_i("Rain curr. week:  %7.1f mm", rainGauge.currentWeek());
            log_i("Rain curr. month: %7.1f mm", rainGauge.currentMonth());
            encoder.writeRawFloat(rainGauge.currentDay());
            encoder.writeRawFloat(rainGauge.currentWeek());
            encoder.writeRawFloat(rainGauge.currentMonth());
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
        log_i("-- Thermo/Hygro Sensor Failure");
        // Invalidate
        encoder.writeUint16(INV_TEMP);
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
        log_i("-- Pool Thermometer Failure");
        // Invalidate
        encoder.writeUint16(INV_TEMP);
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
        log_i("-- Soil Sensor Failure");
        // Invalidate
        encoder.writeUint16(INV_TEMP);
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
        log_i("-- Leakage Sensor Failure");
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
        log_i("-- Air Quality (PM) Sensor Failure");
        // Invalidate
        encoder.writeUint16(INV_UINT16);
        encoder.writeUint16(INV_UINT16);
        encoder.writeUint16(INV_UINT16);
    }
    else
    {
        // Air Quality (Particular Matter) Sensor
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
void PayloadBresser::encodeLightningSensor(int idx, uint8_t flags, LoraEncoder &encoder)
{
    if (flags & (PAYLOAD_LIGHTNING_RAW | 1))
    {
        // Raw sensor values
        if (idx == -1)
        {
            log_i("-- Lightning Sensor Failure");
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

    if (flags & (PAYLOAD_LIGHTNING_PROC | 1))
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
        log_i("-- CO2 Sensor Failure");
        // Invalidate
        encoder.writeUint16(INV_UINT16);
    }
    else
    {
        // CO2 Sensor
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
        log_i("-- Air Quality (HCHO/VOC) Sensor Failure");
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