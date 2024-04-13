///////////////////////////////////////////////////////////////////////////////
// payload.cpp
//
// Create data payload from sensor or simulated data
//
// This implementation is specific for the BresserWeatherSensorLW project
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
// 20240402 Created
// 20240413 Refactored ADC handling
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "payload.h"
#include "WeatherSensorCfg.h"
#include <WeatherSensor.h>
#include <ESP32Time.h>
#include "BresserWeatherSensorLWCfg.h"
#include "adc/adc.h"


#if defined(MITHERMOMETER_EN)
// BLE Temperature/Humidity Sensor
#include <ATC_MiThermometer.h>
#endif
#if defined(THEENGSDECODER_EN)
#include "BleSensors/BleSensors.h"
#endif
#ifdef RAINDATA_EN
#include "RainGauge.h"
#endif
#ifdef LIGHTNINGSENSOR_EN
#include "Lightning.h"
#endif
#ifdef ONEWIRE_EN
// Dallas/Maxim OneWire Temperature Sensor
#include <DallasTemperature.h>
#endif
#ifdef DISTANCESENSOR_EN
// A02YYUW / DFRobot SEN0311 Ultrasonic Distance Sensor
#include <DistanceSensor_A02YYUW.h>
#endif

extern bool runtimeExpired;
extern bool longSleep;
extern bool rtcSyncReq;
extern time_t rtcLastClockSync;
extern ESP32Time rtc;
// FIXME
extern struct sPrefs
{
  uint8_t ws_timeout;           //!< preferences: weather sensor timeout
  uint16_t sleep_interval;      //!< preferences: sleep interval
  uint16_t sleep_interval_long; //!< preferences: sleep interval long
} prefs;

/// Bresser Weather Sensor Receiver
WeatherSensor weatherSensor;

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
std::vector<std::string> knownBLEAddresses = KNOWN_BLE_ADDRESSES;
#endif

#ifdef MITHERMOMETER_EN
// Setup BLE Temperature/Humidity Sensors
ATC_MiThermometer bleSensors(knownBLEAddresses); //!< Mijia Bluetooth Low Energy Thermo-/Hygrometer
#endif
#ifdef THEENGSDECODER_EN
BleSensors bleSensors(knownBLEAddresses);
#endif

#ifdef RAINDATA_EN
/// Rain data statistics
RainGauge rainGauge;
#endif

#ifdef LIGHTNINGSENSOR_EN
/// Lightning sensor post-processing
Lightning lightningProc;
#endif

#ifdef ONEWIRE_EN
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(PIN_ONEWIRE_BUS); //!< OneWire bus

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature temp_sensors(&oneWire); //!< Dallas temperature sensors connected to OneWire bus
#endif

#ifdef DISTANCESENSOR_EN
#if defined(ESP32)
DistanceSensor_A02YYUW distanceSensor(&Serial2);
#else
DistanceSensor_A02YYUW distanceSensor(&Serial1);
#endif
#endif

void genPayload(uint8_t port, LoraEncoder &encoder)
{
    // unused
    (void)port;
    weatherSensor.genMessage(0, 0xfff0, SENSOR_TYPE_WEATHER1);
    weatherSensor.genMessage(1, 0xfff1, SENSOR_TYPE_SOIL);
}

void getPayloadStage1(uint8_t port, LoraEncoder &encoder)
{
#ifdef PIN_SUPPLY_IN
    uint16_t supply_voltage = getVoltage(PIN_SUPPLY_IN, SUPPLY_SAMPLES, SUPPLY_DIV);
#endif
    uint16_t battery_voltage = getBatteryVoltage();
    bool mithermometer_valid = false;
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    float indoor_temp_c;
    float indoor_humidity;

    // Set sensor data invalid
    bleSensors.resetData();

    // Get sensor data - run BLE scan for <bleScanTime>
    bleSensors.getData(BLE_SCAN_TIME);
#endif
#ifdef LIGHTNINGSENSOR_EN
    time_t lightn_ts;
    int lightn_events;
    uint8_t lightn_distance;
#endif

    weatherSensor.begin();
    weatherSensor.clearSlots();
    log_i("Waiting for Weather Sensor Data; timeout %u s", prefs.ws_timeout);
    bool decode_ok = weatherSensor.getData(prefs.ws_timeout * 1000, DATA_ALL_SLOTS);

    if (decode_ok)
    {
        log_v("Port: %d", port);
        log_i("Receiving Weather Sensor Data o.k.");

#ifdef RAINDATA_EN
        // Check if time is valid
        if (rtcLastClockSync > 0)
        {
            // Get local date and time
            struct tm timeinfo;
            time_t tnow = rtc.getLocalEpoch();
            localtime_r(&tnow, &timeinfo);

            // Find weather sensor and determine rain gauge overflow limit
            // Try to find SENSOR_TYPE_WEATHER0
            int ws = weatherSensor.findType(SENSOR_TYPE_WEATHER0);
            if (ws > -1)
            {
                rainGauge.set_max(1000);
            }
            else
            {
                // Try to find SENSOR_TYPE_WEATHER1
                ws = weatherSensor.findType(SENSOR_TYPE_WEATHER1);
                rainGauge.set_max(100000);
            }

            // If weather sensor has be found and rain data is valid, update statistics
            if ((ws > -1) && weatherSensor.sensor[ws].valid && weatherSensor.sensor[ws].w.rain_ok)
            {
                rainGauge.update(tnow, weatherSensor.sensor[ws].w.rain_mm, weatherSensor.sensor[ws].startup);
            }
        }
#endif

#ifdef LIGHTNINGSENSOR_EN
        // Check if time is valid
        if (rtcLastClockSync > 0)
        {
            // Get local date and time
            time_t tnow = rtc.getLocalEpoch();

            // Find lightning sensor
            int ls = weatherSensor.findType(SENSOR_TYPE_LIGHTNING);

            // If lightning sensor has be found and data is valid, run post-processing
            if ((ls > -1) && weatherSensor.sensor[ls].valid)
            {
                lightningProc.update(tnow, weatherSensor.sensor[ls].lgt.strike_count, weatherSensor.sensor[ls].lgt.distance_km, weatherSensor.sensor[ls].startup);
            }
        }
#endif
        //
        // Find Bresser sensor data in array
        //

        // Try to find SENSOR_TYPE_WEATHER0
        int ws = weatherSensor.findType(SENSOR_TYPE_WEATHER0);
        if (ws < 0)
        {
            // Try to find SENSOR_TYPE_WEATHER1
            ws = weatherSensor.findType(SENSOR_TYPE_WEATHER1);
        }

        int s1 = -1;
#ifdef SOILSENSOR_EN
        // Try to find SENSOR_TYPE_SOIL
        s1 = weatherSensor.findType(SENSOR_TYPE_SOIL, 1);
#endif

        int ls = -1;
#ifdef LIGHTNINGSENSOR_EN
        // Try to find SENSOR_TYPE_LIGHTNING
        ls = weatherSensor.findType(SENSOR_TYPE_LIGHTNING);
#endif

        log_i("--- Uplink Data ---");

        // Debug output for weather sensor data
        if (ws > -1)
        {
            if (weatherSensor.sensor[ws].w.temp_ok)
            {
                log_i("Air Temperature:    %3.1f °C", weatherSensor.sensor[ws].w.temp_c);
            }
            else
            {
                log_i("Air Temperature:     --.- °C");
            }
            if (weatherSensor.sensor[ws].w.humidity_ok)
            {
                log_i("Humidity:            %2d   %%", weatherSensor.sensor[ws].w.humidity);
            }
            else
            {
                log_i("Humidity:            --   %%");
            }
            if (weatherSensor.sensor[ws].w.rain_ok)
            {
                log_i("Rain Gauge:       %7.1f mm", weatherSensor.sensor[ws].w.rain_mm);
            }
            else
            {
                log_i("Rain Gauge:       ---.- mm");
            }
            log_i("Wind Speed (avg.):    %3.1f m/s", weatherSensor.sensor[ws].w.wind_avg_meter_sec_fp1 / 10.0);
            log_i("Wind Speed (max.):    %3.1f m/s", weatherSensor.sensor[ws].w.wind_gust_meter_sec_fp1 / 10.0);
            log_i("Wind Direction:     %4.1f °", weatherSensor.sensor[ws].w.wind_direction_deg_fp1 / 10.0);
        }
        else
        {
            log_i("-- Weather Sensor Failure");
        }

// Debug output for soil sensor data
#ifdef SOILSENSOR_EN
        if (s1 > -1)
        {
            log_i("Soil Temperature 1: %3.1f °C", weatherSensor.sensor[s1].soil.temp_c);
            log_i("Soil Moisture 1:     %2d   %%", weatherSensor.sensor[s1].soil.moisture);
        }
        else
        {
            log_i("-- Soil Sensor 1 Failure");
        }
#endif

// Debug output for lightning sensor data
#ifdef LIGHTNINGSENSOR_EN
        if (ls > -1)
        {
            log_i("Lightning counter: %4d", weatherSensor.sensor[ls].lgt.strike_count);
            log_i("Lightning distance:  %2d   km", weatherSensor.sensor[ls].lgt.distance_km);
        }
        else
        {
            log_i("-- Lightning Sensor Failure");
        }
        if (lightningProc.lastEvent(lightn_ts, lightn_events, lightn_distance))
        {
#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
            struct tm timeinfo;
            char tbuf[25];

            localtime_r(&lightn_ts, &timeinfo);
            strftime(tbuf, 25, "%Y-%m-%d %H:%M:%S", &timeinfo);
#endif
            log_i("Last lightning event @%s: %d events, %d km", tbuf, lightn_events, lightn_distance);
        }
        else
        {
            log_i("-- No Lightning Event Data Available");
        }
#endif

#ifdef ONEWIRE_EN
        // Debug output for auxiliary sensors/voltages
        if (water_temp_c != DEVICE_DISCONNECTED_C)
        {
            log_i("Water Temperature:  % 2.1f °C", water_temp_c);
        }
        else
        {
            log_i("Water Temperature:   --.- °C");
            water_temp_c = -30.0;
        }
#endif
#ifdef DISTANCESENSOR_EN
        if (distance_mm > 0)
        {
            log_i("Distance:          %4d mm", distance_mm);
        }
        else
        {
            log_i("Distance:         ---- mm");
        }
#endif
#ifdef ADC_EN
        log_i("Supply  Voltage:   %4d   mV", supply_voltage);
#endif
#if defined(ADC_EN) && defined(PIN_ADC3_IN)
        log_i("Battery Voltage:   %4d   mV", battery_voltage);
#endif

#if defined(MITHERMOMETER_EN)
        float div = 100.0;
#elif defined(THEENGSDECODER_EN)
        float div = 1.0;
#endif
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
        if (bleSensors.data[0].valid)
        {
            mithermometer_valid = true;
            indoor_temp_c = bleSensors.data[0].temperature / div;
            indoor_humidity = bleSensors.data[0].humidity / div;
            log_i("Indoor Air Temp.:   % 3.1f °C", bleSensors.data[0].temperature / div);
            log_i("Indoor Humidity:     %3.1f %%", bleSensors.data[0].humidity / div);
        }
        else
        {
            log_i("Indoor Air Temp.:    --.- °C");
            log_i("Indoor Humidity:     --   %%");
            indoor_temp_c = -30;
            indoor_humidity = 0;
        }
#endif
        log_v("-");

#ifdef SENSORID_EN
        if (ws > -1)
        {
            encoder.writeUint32(weatherSensor.sensor[ws].sensor_id);
        }
        else
        {
            encoder.writeUint32(0);
        }
#endif

        // LoRaWAN node status flags
        encoder.writeBitmap(0,
                            0,
                            0,
                            0,
                            0,
                            longSleep,
                            rtcSyncReq,
                            runtimeExpired);

        // Sensor status flags
        encoder.writeBitmap(0,
                            mithermometer_valid,
                            (ls > -1) ? weatherSensor.sensor[ls].valid : false,
                            (ls > -1) ? weatherSensor.sensor[ls].battery_ok : false,
                            (s1 > -1) ? weatherSensor.sensor[s1].valid : false,
                            (s1 > -1) ? weatherSensor.sensor[s1].battery_ok : false,
                            (ws > -1) ? weatherSensor.sensor[ws].valid : false,
                            (ws > -1) ? weatherSensor.sensor[ws].battery_ok : false);

        // Weather sensor data
        if (ws > -1)
        {
            // weather sensor data available
            if (weatherSensor.sensor[ws].w.temp_ok)
            {
                encoder.writeTemperature(weatherSensor.sensor[ws].w.temp_c);
            }
            else
            {
                encoder.writeTemperature(-30);
            }
            if (weatherSensor.sensor[ws].w.humidity_ok)
            {
                encoder.writeUint8(weatherSensor.sensor[ws].w.humidity);
            }
            else
            {
                encoder.writeUint8(0);
            }
#ifdef ENCODE_AS_FLOAT
            encoder.writeRawFloat(weatherSensor.sensor[ws].w.wind_gust_meter_sec);
            encoder.writeRawFloat(weatherSensor.sensor[ws].w.wind_avg_meter_sec);
            encoder.writeRawFloat(weatherSensor.sensor[ws].w.wind_direction_deg);
#else
            encoder.writeUint16(weatherSensor.sensor[ws].w.wind_gust_meter_sec_fp1);
            encoder.writeUint16(weatherSensor.sensor[ws].w.wind_avg_meter_sec_fp1);
            encoder.writeUint16(weatherSensor.sensor[ws].w.wind_direction_deg_fp1);
#endif
            if (weatherSensor.sensor[ws].w.rain_ok)
            {
                encoder.writeRawFloat(weatherSensor.sensor[ws].w.rain_mm);
            }
            else
            {
                encoder.writeRawFloat(0);
            }
        }
        else
        {
            // fill with suspicious dummy values
            encoder.writeTemperature(-30);
            encoder.writeUint8(0);
#ifdef ENCODE_AS_FLOAT
            encoder.writeRawFloat(0);
            encoder.writeRawFloat(0);
            encoder.writeRawFloat(0);
#else
            encoder.writeUint16(0);
            encoder.writeUint16(0);
            encoder.writeUint16(0);
#endif
            encoder.writeRawFloat(0);
        }

// Voltages / auxiliary sensor data
#ifdef ADC_EN
        encoder.writeUint16(supply_voltage);
#endif
#if defined(ADC_EN) && defined(PIN_ADC3_IN)
        encoder.writeUint16(battery_voltage);
#endif
#ifdef ONEWIRE_EN
        encoder.writeTemperature(water_temp_c);
#endif
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
        encoder.writeTemperature(indoor_temp_c);
        encoder.writeUint8((uint8_t)(indoor_humidity + 0.5));

        // BLE Tempoerature/Humidity Sensor: delete results fromBLEScan buffer to release memory
        bleSensors.clearScanResults();
#endif

// Soil sensor data
#ifdef SOILSENSOR_EN
        if (s1 > -1)
        {
            // soil sensor data available
            encoder.writeTemperature(weatherSensor.sensor[s1].soil.temp_c);
            encoder.writeUint8(weatherSensor.sensor[s1].soil.moisture);
        }
        else
        {
            // fill with suspicious dummy values
            encoder.writeTemperature(-30);
            encoder.writeUint8(0);
        }
#endif

// Rain data statistics
#ifdef RAINDATA_EN
        if ((ws > -1) && weatherSensor.sensor[ws].valid && weatherSensor.sensor[ws].w.rain_ok)
        {
            log_i("Rain past 60min:  %7.1f mm", rainGauge.pastHour());
            log_i("Rain curr. day:   %7.1f mm", rainGauge.currentDay());
            log_i("Rain curr. week:  %7.1f mm", rainGauge.currentWeek());
            log_i("Rain curr. month: %7.1f mm", rainGauge.currentMonth());
            encoder.writeRawFloat(rainGauge.pastHour());
            encoder.writeRawFloat(rainGauge.currentDay());
            encoder.writeRawFloat(rainGauge.currentWeek());
            encoder.writeRawFloat(rainGauge.currentMonth());
        }
        else
        {
            log_i("Current rain gauge statistics not valid.");
            encoder.writeRawFloat(-1);
            encoder.writeRawFloat(-1);
            encoder.writeRawFloat(-1);
            encoder.writeRawFloat(-1);
        }
#endif

// Distance sensor data
#ifdef DISTANCESENSOR_EN
        encoder.writeUint16(distance_mm);
#endif

// Lightning sensor data
#ifdef LIGHTNINGSENSOR_EN
        if (ls > -1)
        {
            // Lightning sensor data available
            encoder.writeUnixtime(lightn_ts);
            encoder.writeUint16(lightn_events);
            encoder.writeUint8(lightn_distance);
        }
        else
        {
            // Fill with suspicious dummy values
            encoder.writeUnixtime(0);
            encoder.writeUint16(0);
            encoder.writeUint8(0);
        }
#endif
    }
    else
    {
        log_i("Receiving Weather Sensor Data failed.");
    }
}

void getPayloadStage2(uint8_t port, LoraEncoder &encoder)
{
}

void deviceDecodeDownlink(uint8_t port, uint8_t *payload, size_t size)
{
#ifdef RAINDATA_EN
    if (port > 0)
    {
        if (payload[0] == CMD_RESET_RAINGAUGE)
        {
            if (size == 1)
            {
                log_d("Reset raingauge");
                rainGauge.reset();
            }
            else if (size == 2)
            {
                log_d("Reset raingauge - flags: 0x%X", payload[1]);
                rainGauge.reset(payload[1] & 0xF);
            }
        }
    }
#endif
}