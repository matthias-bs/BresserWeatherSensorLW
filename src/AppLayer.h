///////////////////////////////////////////////////////////////////////////////
// AppLayer.h
//
// LoRaWAN node application layer
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
// 20240417 Added sensor configuration functions
// 20240419 Modified downlink decoding
// 20240424 Fixed BLE address initialization from Preferences, added begin()
// 20240426 Moved bleAddrInit() out of begin()
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

/*!
 * LoRaWAN node application layer
 *
 * Contails all device specific methods and attributes
 */
class AppLayer
{
private:
    ESP32Time *_rtc;
    time_t *_rtcLastClockSync;

    /// Preferences (stored in flash memory)
    Preferences appPrefs;

    /// Bresser Weather Sensor Receiver
    WeatherSensor weatherSensor;

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    /// Default BLE MAC addresses
    std::vector<std::string> knownBLEAddressesDef;
    /// Actual BLE MAC addresses; either from Preferences or from defaults
    std::vector<std::string> knownBLEAddresses;
#endif

#ifdef MITHERMOMETER_EN
    /// BLE Temperature/Humidity Sensors
    ATC_MiThermometer bleSensors; //!< Mijia Bluetooth Low Energy Thermo-/Hygrometer
#endif
#ifdef THEENGSDECODER_EN
    /// Bluetooth Low Energy sensors
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
    /// Ultrasonic distance sensor
    DistanceSensor_A02YYUW distanceSensor(&Serial2);
#else
    /// Ultrasonic distance sensor
    DistanceSensor_A02YYUW distanceSensor(&Serial1);
#endif
#endif

public:
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    /*!
     * \brief Constructor with BLE sensors
     *
     * \param rtc Real time clock object
     * \param clocksync Timestamp of last clock synchronization
     */
    AppLayer(ESP32Time *rtc, time_t *clocksync) : bleSensors()
#else
    /*!
     * \brief Constructor without BLE sensors
     *
     * \param rtc Real time clock object
     * \param clocksync Timestamp of last clock synchronization
     */
    AppLayer(ESP32Time *rtc, time_t *clocksync)
#endif
    {
        _rtc = rtc;
        _rtcLastClockSync = clocksync;
    };

    /*!
     * \brief AppLayer initialization
     *
     */
    void begin(void)
    {
        bleAddrInit();
    };

    /*!
     * \brief Initialize list of known BLE addresses from defaults or Preferences
     *
     * If available, addresses from Preferences are used, otherwise defaults from
     * BresserWeatherSensorLWCfg.h.
     * 
     * BleSensors() requires Preferences, which uses the Flash FS,
     * which is not available before the sketches' begin() is called -
     * thus the following cannot be handled by the constructor!
     */
    void bleAddrInit(void)
    {
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
        knownBLEAddressesDef = KNOWN_BLE_ADDRESSES;
        knownBLEAddresses = getBleAddr();
        if (knownBLEAddresses.size() == 0)
        {
            // No addresses stored in Preferences, use default
            knownBLEAddresses = knownBLEAddressesDef;
            log_d("Using BLE addresses from BresserWeatherSensorLWCfg.h:");
        } else {
            log_d("Using BLE addresses from Preferences:");
        }
        bleSensors = BleSensors(knownBLEAddresses);
        
        for (const std::string &s : knownBLEAddresses)
        {
            (void)s;
            log_d("%s", s.c_str());
        }
#endif
    };


    /*!
     * \brief Decode app layer specific downlink messages
     *
     * \param port downlink message port
     * \param payload downlink message payload
     * \param size payload size in bytes
     *
     * \returns config uplink request or 0
     */
    uint8_t decodeDownlink(uint8_t port, uint8_t *payload, size_t size);

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

    /*!
     * Get configuration data for uplink
     *
     * Get the configuration data requested in a downlink command and
     * prepare it as payload in a uplink response.
     *
     * \param cmd command
     * \param port uplink port
     * \param encoder uplink data encoder object
     */
    void getConfigPayload(uint8_t cmd, uint8_t &port, LoraEncoder &encoder);

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    /*!
     * Set BLE addresses in Preferences and bleSensors object
     *
     * \param bytes MAC addresses (6 bytes per address)
     * \param size size in bytes
     */
    void setBleAddr(uint8_t *bytes, uint8_t size);

    /*!
     * Get BLE addresses from Preferences
     *
     * \param bytes buffer for addresses
     *
     * \returns number of bytes copied into buffer
     */
    uint8_t getBleAddr(uint8_t *bytes);

    /*!
     * Get BLE addresses from Preferences
     *
     * \returns BLE addresses
     */
    std::vector<std::string> getBleAddr(void);
#endif
};
#endif // _APPLAYER_H
