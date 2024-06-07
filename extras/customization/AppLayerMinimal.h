///////////////////////////////////////////////////////////////////////////////
// AppLayer.h
//
// LoRaWAN node application layer - Minimal template for customization
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
// 20240513 Created
// 20240607 Added getAppStatusUplinkInterval() for compatibility
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_APPLAYER_H)
#define _APPLAYER_H

#include <ESP32Time.h>
#include <Preferences.h> // keep this to store data persistently
#include "../BresserWeatherSensorLWCfg.h"
#include "../BresserWeatherSensorLWCmd.h" // keep this for adding your commands
#include "adc/adc.h" // keep this for using ADC functions
#include <LoraMessage.h> // see https://github.com/thesolarnomad/lora-serialization

/*!
 * \brief LoRaWAN node application layer
 *
 * Contains all device specific methods and attributes
 */
class AppLayer
{
private:
    ESP32Time *_rtc;
    time_t *_rtcLastClockSync;

    /// Preferences (stored in flash memory)
    Preferences appPrefs;

public:
    /*!
     * \brief Constructor
     *
     * \param rtc Real time clock object
     * \param clocksync Timestamp of last clock synchronization
     */
    AppLayer(ESP32Time *rtc, time_t *clocksync)
    {
        _rtc = rtc;
        _rtcLastClockSync = clocksync;
    };

    /*!
     * \brief AppLayer initialization
     *
     * Use this if needed
     */
    void begin(void)
    {
    };

    /*!
     * \brief Get sensor status message uplink interval
     *
     * \returns status uplink interval in frame counts (o: disabled)
     */
    uint8_t getAppStatusUplinkInterval(void)
    {
        return 0;
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
};
#endif // _APPLAYER_H
