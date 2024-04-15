///////////////////////////////////////////////////////////////////////////////
// payload.h
//
// Create data payload from sensor or simulated data
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
// 20240414 Added separation between LoRaWAN and application layer
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_APPLAYER_H)
#define _APPLAYER_H

#include "WeatherSensorCfg.h"
#include <WeatherSensor.h>
#include <ESP32Time.h>
#include <Preferences.h>
#include "../BresserWeatherSensorLWCfg.h"
#include "adc/adc.h"
#include <LoraMessage.h>

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

class AppLayer
{
private:
    ESP32Time *rtc;
    time_t *rtcLastClockSync;

    /// Preferences (stored in flash memory)
    Preferences appPrefs;

    /// Bresser Weather Sensor Receiver
    WeatherSensor weatherSensor;

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    std::vector<std::string> knownBLEAddresses;
#endif

#ifdef MITHERMOMETER_EN
    // Setup BLE Temperature/Humidity Sensors
    ATC_MiThermometer bleSensors; //!< Mijia Bluetooth Low Energy Thermo-/Hygrometer
#endif
#ifdef THEENGSDECODER_EN
    BleSensors bleSensors;
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

public:
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    AppLayer(ESP32Time *rtc, time_t *clocksync) : bleSensors(KNOWN_BLE_ADDRESSES)
#else
    AppLayer(ESP32Time *rtc, time_t *clocksync)
#endif
    {
        rtc = rtc;
        rtcLastClockSync = clocksync;

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
        knownBLEAddresses = KNOWN_BLE_ADDRESSES;
#endif
    };

    /*!
     * \brief Decode app layer specific downlink messages
     *
     * \param payload downlink message payload
     * \param size payload size in bytes
     *
     * \returns config uplink request or 0
     */
    uint8_t decodeDownlink(uint8_t *payload, size_t size);

    /*!
     * \brief Generate payload (by emulation)
     *
     * \param port LoRaWAN port
     * \param encoder uplink encoder object
     */
    void genPayload(uint8_t port, LoraEncoder &encoder);

    /*!
     * \brief Prepare / get payload at startup
     *
     * Use this if
     * - A sensor needs some time for warm-up or data acquisition
     * - The data aquisition has to be done directly after startup
     * - The radio transceiver is used for sensor communication
     *   before starting LoRaWAN activities.
     *
     * \param port LoRaWAN port
     * \param encoder uplink encoder object
     */
    void getPayloadStage1(uint8_t port, LoraEncoder &encoder);

    /*!
     * \brief Get payload befor uplink
     *
     * Use this if
     * - The radio transceiver is NOT used for sensor communication
     * - The sensor preparation has been started in stage1
     * - The data aquistion has to be done immediately before uplink
     *
     * \param port LoRaWAN port
     * \param encoder uplink encoder object
     */
    void getPayloadStage2(uint8_t port, LoraEncoder &encoder);

    void getConfigPayload(uint8_t cmd, uint8_t &port, LoraEncoder &encoder);
};

#endif // _APPLAYER_H