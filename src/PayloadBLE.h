///////////////////////////////////////////////////////////////////////////////
// PayloadBLE.h
//
// Get BLE temperature/humidity sensor values and encode as LoRaWAN payload
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
// 20240531 Moved from AppLayer.h
// 20240603 encodeBLE(): added appStatus parameter
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_PAYLOAD_BLE)
#define _PAYLOAD_BLE

#include "../BresserWeatherSensorLWCfg.h"

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)

#include <Preferences.h>

#if defined(MITHERMOMETER_EN)
// BLE Temperature/Humidity Sensor
#include <ATC_MiThermometer.h>
#endif
#if defined(THEENGSDECODER_EN)
#include "BleSensors/BleSensors.h"
#endif

#include <LoraMessage.h>
#include "logging.h"

class PayloadBLE
{
private:
    /// Preferences (stored in flash memory)
    Preferences appPrefs;

#ifdef MITHERMOMETER_EN
    /// BLE Temperature/Humidity Sensors
    ATC_MiThermometer bleSensors; //!< Mijia Bluetooth Low Energy Thermo-/Hygrometer
#endif
#ifdef THEENGSDECODER_EN
    /// Bluetooth Low Energy sensors
    BleSensors bleSensors;
#endif

    /// Default BLE MAC addresses
    std::vector<std::string> knownBLEAddressesDef;

public:
    /// Actual BLE MAC addresses; either from Preferences or from defaults
    std::vector<std::string> knownBLEAddresses;

public:
    /*!
     * \brief Constructor
     */
    PayloadBLE(){};

    /*!
     * \brief BLE startup code
     */
    void begin(void)
    {
        bleAddrInit();
    };

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
    void bleAddrInit(void);

    /*!
     * \brief Encode BLE temperature/humidity sensor values for LoRaWAN transmission
     *
     * \param appPayloadCfg LoRaWAN payload configuration bitmaps
     * \param encoder LoRaWAN payload encoder object
     */
    void encodeBLE(uint8_t *appPayloadCfg, uint8_t * appStatus, LoraEncoder &encoder);
};
#endif // defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
#endif //_PAYLOAD_BLE