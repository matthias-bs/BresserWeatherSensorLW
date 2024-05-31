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
// 20240504 Added BresserWeatherSensorLWCmd.h
// 20240515 Added getOneWireTemperature()
// 20240520 Moved 1-Wire sensor code to PayloadOneWire.h/.cpp
// 20240524 Added appPayloadCfgDef, setAppPayloadCfg() & getAppPayloadCfg()
//          Moved code to PayloadBresser, PayloadAnalog & PayloadDigital
// 20240530 Removed BleSensors as base class & from initializers
// 20240531 Moved BLE specific code to PayloadBLE.h
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
#include "../BresserWeatherSensorLWCmd.h"
#include "PayloadBresser.h"
#include "PayloadOneWire.h"
#include "PayloadAnalog.h"
#include "PayloadDigital.h"
#include "PayloadBLE.h"
#include <LoraMessage.h>


/// Default AppLayer payload configuration
const uint8_t appPayloadCfgDef[APP_PAYLOAD_CFG_SIZE] = {
    APP_PAYLOAD_CFG_TYPE00,
    APP_PAYLOAD_CFG_TYPE01,
    APP_PAYLOAD_CFG_TYPE02,
    APP_PAYLOAD_CFG_TYPE03,
    APP_PAYLOAD_CFG_TYPE04,
    APP_PAYLOAD_CFG_TYPE05,
    APP_PAYLOAD_CFG_TYPE06,
    APP_PAYLOAD_CFG_TYPE07,
    APP_PAYLOAD_CFG_TYPE08,
    APP_PAYLOAD_CFG_TYPE09,
    APP_PAYLOAD_CFG_TYPE10,
    APP_PAYLOAD_CFG_TYPE11,
    APP_PAYLOAD_CFG_TYPE12,
    APP_PAYLOAD_CFG_TYPE13,
    APP_PAYLOAD_CFG_TYPE14,
    APP_PAYLOAD_CFG_TYPE15,
    APP_PAYLOAD_CFG_ONEWIRE1, // onewire[15:8]
    APP_PAYLOAD_CFG_ONEWIRE0, // onewire[7:0]
    APP_PAYLOAD_CFG_ANALOG1,  // analog[15:0]
    APP_PAYLOAD_CFG_ANALOG0,  // analog[7:0]
    APP_PAYLOAD_CFG_DIGITAL3, // digital[31:24]
    APP_PAYLOAD_CFG_DIGITAL2, // digital[23:16]
    APP_PAYLOAD_CFG_DIGITAL1, // digital[15:8]
    APP_PAYLOAD_CFG_DIGITAL0  // digital[7:0]
};

/*!
 * \brief LoRaWAN node application layer
 *
 * Contains all device specific methods and attributes
 */
class AppLayer : public PayloadBresser, PayloadAnalog, PayloadDigital
#ifdef ONEWIRE_EN
    ,
                 PayloadOneWire
#endif
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    ,
                 PayloadBLE
#endif
{
private:
    ESP32Time *_rtc;
    time_t *_rtcLastClockSync;

    /// Preferences (stored in flash memory)
    Preferences appPrefs;

    /// AppLayer payload configuration
    uint8_t appPayloadCfg[APP_PAYLOAD_CFG_SIZE];

public:
    /*!
     * \brief Constructor with BLE sensors
     *
     * \param rtc Real time clock object
     * \param clocksync Timestamp of last clock synchronization
     */
    AppLayer(ESP32Time *rtc, time_t *clocksync) : PayloadBresser(rtc, clocksync), PayloadAnalog(), PayloadDigital()
#ifdef ONEWIRE_EN
                                                  ,
                                                  PayloadOneWire()
#endif
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
                                                  ,
                                                  PayloadBLE()
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
        // bleAddrInit();
        PayloadBresser::begin();
        PayloadAnalog::begin();
        PayloadDigital::begin();
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
        PayloadBLE::begin();
#endif

        if (!getAppPayloadCfg(appPayloadCfg, APP_PAYLOAD_CFG_SIZE))
        {
            memcpy(appPayloadCfg, appPayloadCfgDef, APP_PAYLOAD_CFG_SIZE);
        }
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
     * \brief Get payload before uplink
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

    /*!
     * Set AppLayer payload config in Preferences
     *
     * \param bytes buffer
     * \param size buffer size in bytes
     */
    void setAppPayloadCfg(uint8_t *bytes, uint8_t size);

    /*!
     * Get AppLayer payload config from Preferences
     *
     * \param bytes buffer
     * \param size buffer size in bytes
     *
     * \returns true if available in Preferences, else false
     */
    bool getAppPayloadCfg(uint8_t *bytes, uint8_t size);
};
#endif // _APPLAYER_H
