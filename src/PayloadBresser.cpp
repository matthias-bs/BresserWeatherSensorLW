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
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadBresser.h"

void PayloadBresser::begin(void)
{
}

void PayloadBresser::encodeBresser(uint8_t *appPayloadCfg, LoraEncoder &encoder)
{
    for (int type = 0; type < 16; type++)
    {
        // Skip if bitmap is zero
        if (appPayloadCfg[type] == 0)
            continue;

#ifdef LIGHTNINGSENSOR_EN
        if (type == SENSOR_TYPE_LIGHTNING)
        {
            int idx = -1;
            idx = weatherSensor.findType(type, 0);

            encodeLightningSensor(idx, appPayloadCfg[type], encoder);
            continue;
        }
#endif

        for (int bit = 7; bit >= 0; bit--)
        {
            // Check if channel is enabled
            if (!((appPayloadCfg[type] >> bit) & 0x1))
                continue;

            int idx = -1;
            idx = weatherSensor.findType(type, bit);

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

void PayloadBresser::encodeThermoHygroSensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        log_i("-- Thermo/Hygro Sensor Failure");
        // fill with suspicious dummy values
        encoder.writeUint16(0x7FFF);
        encoder.writeUint8(0xFF);
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
        // fill with suspicious dummy values
        encoder.writeUint16(0x7FFF);
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
        // fill with suspicious dummy values
        encoder.writeUint16(0x7FFF);
        encoder.writeUint8(0xFF);
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
        // fill with suspicious dummy values
        encoder.writeUint8(0xFF);
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
        // fill with suspicious dummy values
        encoder.writeUint16(0xFFFF);
        encoder.writeUint16(0xFFFF);
        encoder.writeUint16(0xFFFF);
    }
    else
    {
        // Air Quality (Particular Matter) Sensor
        if (weatherSensor.sensor[idx].pm.pm_1_0_init)
        {
            log_i("PM1.0: init");
            encoder.writeUint16(0xFFFF);
        }
        else
        {
            log_i("PM1.0: %u µg/m³", weatherSensor.sensor[idx].pm.pm_1_0);
            encoder.writeUint16(weatherSensor.sensor[idx].pm.pm_1_0);
        }
        if (weatherSensor.sensor[idx].pm.pm_2_5_init)
        {
            log_i("PM2.5: init");
            encoder.writeUint16(0xFFFF);
        }
        else
        {
            log_i("PM2.5: %u µg/m³", weatherSensor.sensor[idx].pm.pm_2_5);
            encoder.writeUint16(weatherSensor.sensor[idx].pm.pm_2_5);
        }
        if (weatherSensor.sensor[idx].pm.pm_10_init)
        {
            log_i("PM10: init");
            encoder.writeUint16(0xFFFF);
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
    if (flags & 0x21)
    {
        // Raw sensor values
        if (idx == -1)
        {
            log_i("-- Lightning Sensor Failure");
            // fill with suspicious dummy values
            encoder.writeUint8(0xFF);
            encoder.writeUint16(0xFFFF);
        }
        else
        {
            log_i("Lightning Distance:      %2u km", weatherSensor.sensor[idx].lgt.distance_km);
            log_i("Lightning Strike Count %4u", weatherSensor.sensor[idx].lgt.strike_count);
            encoder.writeUint8(weatherSensor.sensor[idx].lgt.distance_km);
            encoder.writeUint16(weatherSensor.sensor[idx].lgt.strike_count);
        }
    }

    if (flags & 0x20)
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
            encoder.writeUint32(0xFFFFFFFF);
            encoder.writeUint16(0xFFFF);
            encoder.writeUint8(0xFF);
        }
    }
}
#endif

void PayloadBresser::encodeCo2Sensor(int idx, LoraEncoder &encoder)
{
    if (idx == -1)
    {
        log_i("-- CO2 Sensor Failure");
        // fill with suspicious dummy values
        encoder.writeUint16(0xFFFF);
    }
    else
    {
        // CO2 Sensor
        if (weatherSensor.sensor[idx].co2.co2_init)
        {
            log_i("CO2: init");
            encoder.writeUint16(0xFFFF);
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
        // fill with suspicious dummy values
        encoder.writeUint16(0xFFFF);
        encoder.writeUint8(0xFF);
    }
    else
    {
        if (weatherSensor.sensor[idx].voc.hcho_init)
        {
            log_i("HCHO: init");
            encoder.writeUint16(0xFFFF);
        }
        else
        {
            log_i("HCHO: %u", weatherSensor.sensor[idx].voc.hcho_ppb);
            encoder.writeUint16(weatherSensor.sensor[idx].voc.hcho_ppb);
        }

        if (weatherSensor.sensor[idx].voc.voc_init)
        {
            log_i("VOC: init");
            encoder.writeUint8(0xFF);
        }
        else
        {
            log_i("VOC: %u", weatherSensor.sensor[idx].voc.voc_level);
            encoder.writeUint8(weatherSensor.sensor[idx].voc.voc_level);
        }
    }
}