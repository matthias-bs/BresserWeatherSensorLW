///////////////////////////////////////////////////////////////////////////////
// PayloadBresser.h
//
// Read Bresser sensor data and encode as LoRaWAN payload
//
// created: 05/2024
//
//
// MIT License
//
// Copyright (c) 2025 Matthias Prinke
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
// 20240524 Moved rainGauge, appPrefs and time members from AppLayer
//          into the class
//          Added isSpaceLeft(), payloadSize[] & sensorTypes[]
// 20240528 Moved encoding of invalid values to BresserWeatherSensorLWCmd.h
// 20240530 Added missing entries in sensorTypes[]
// 20240603 encodeBresser(): added appStatus parameter
// 20240716 Added ws_scantime and scanBresser()
// 20250209 Added Weather Station 8-in-1
// 20250318 Renamed PAYLOAD_SIZE to MAX_UPLINK_SIZE
// 20250828 Changed time functions to POSIX, added SystemContext
//          Added ws_postproc_int
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

/*! \file PayloadBresser.h
 *  \brief LoRaWAN node application layer - Bresser sensors
 */

#if !defined(_PAYLOAD_BRESSER)
#define _PAYLOAD_BRESSER

#include "../BresserWeatherSensorLWCfg.h"
#include "WeatherSensorCfg.h"
#include <WeatherSensor.h>
#include <time.h>
#include <Preferences.h>

#ifdef RAINDATA_EN
#include "RainGauge.h"
#endif
#ifdef LIGHTNINGSENSOR_EN
#include "Lightning.h"
#endif

#include <LoraMessage.h>
#include "SystemContext.h"
#include "logging.h"


/*!
 * \brief LoRaWAN node application layer - Bresser sensors
 *
 * Encodes data from Bresser sensors received via radio messages as LoRaWAN payload
 */
class PayloadBresser
{
public:
    /// Bresser Weather Sensor Receiver
    WeatherSensor weatherSensor;

    /// Weather Sensor Scan Request
    uint8_t ws_scantime = 0;

    /// Weather Sensor Post-Processing Update Rate (0: auto, 1..255: minutes)
    uint8_t ws_postproc_interval = 0;

    /// Payload size in bytes
    const uint8_t payloadSize[16] = {
        0,
        25, // SENSOR_TYPE_WEATHER<0|1|2> (max.)
        3, // SENSOR_TYPE_THERMO_HYGRO
        2, // SENSOR_TYPE_POOL_THERMO
        3, // SENSOR_TYPE_SOIL
        1, // SENSOR_TYPE_LEAKAGE
        0, // reserved
        0, // reserved
        6, // SENSOR_TYPE_AIR_PM
        3, // SENSOR_TYPE_LIGHTNING (min.)
        2, // SENSOR_TYPE_CO2
        3, // SENSOR_TYPE_HCHO_VOC
        0, // reserved
        0, // (Weather Station 8-in-1, included in [1])
        0, // reserved
        0 // reserved
    };

#if CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
    /// Map sensor type ID to name
    const char * sensorTypes[16] = {
        "Weather",
        "Weather",
        "Thermo/Hygro",
        "Pool Temperature",
        "Soil",
        "Leakage",
        "reserved",
        "reserved",
        "Air Quality (PM)",
        "Lightning",
        "CO2",
        "Air Quality (HCHO/VOC)",
        "reserved",
        "Weather", // Weather Station 8-in-1
        "reserved",
        "reserved"
    };
#endif

private:
    /// System context
    SystemContext *_sysCtx;

    /// Preferences (stored in flash memory)
    Preferences appPrefs;


#ifdef RAINDATA_EN
public:
    /// Rain data statistics
    RainGauge rainGauge;
#endif

#ifdef LIGHTNINGSENSOR_EN
public:
    /// Lightning sensor post-processing
    Lightning lightningProc;

private:
    time_t lightn_ts;
    int lightn_events;
    uint8_t lightn_distance;
#endif

public:
    /*!
     * \brief Constructor
     */
    PayloadBresser(SystemContext* sysCtx)
    {
        _sysCtx = sysCtx;
    };

    /*!
     * \brief Bresser sensors startup code
     */
    void begin(void);

    /*!
     * \brief Scan for Bresser sensors
     *
     * \param ws_scantime Scan time in seconds
     * \param encoder LoRaWAN payload encoder object
     */
    void scanBresser(uint8_t ws_scantime, LoraEncoder &encoder);

    /*!
     * \brief Encode Bresser sensor data for LoRaWAN transmission
     *
     * \param appPayloadCfg LoRaWAN payload configuration bitmaps
     * \param appStatus Application layer status (i.e. sensor battery status bits)
     * \param encoder LoRaWAN payload encoder object
     */
    void encodeBresser(uint8_t *appPayloadCfg, uint8_t *appStatus, LoraEncoder &encoder);

private:
    void encodeWeatherSensor(int idx, uint16_t flags, LoraEncoder &encoder);
    void encodeThermoHygroSensor(int idx, LoraEncoder &encoder);
    void encodePoolThermometer(int idx, LoraEncoder &encoder);
    void encodeSoilSensor(int idx, LoraEncoder &encoder);
    void encodeLeakageSensor(int idx, LoraEncoder &encoder);
    void encodeAirPmSensor(int idx, LoraEncoder &encoder);
#ifdef LIGHTNINGSENSOR_EN
    void encodeLightningSensor(int idx, uint8_t flags, LoraEncoder &encoder);
#endif
    void encodeCo2Sensor(int idx, LoraEncoder &encoder);
    void encodeHchoVocSensor(int idx, LoraEncoder &encoder);

    bool isSpaceLeft(LoraEncoder &encoder, uint8_t type)
    {
        return (encoder.getLength() + payloadSize[type] <= MAX_UPLINK_SIZE); 
    };
};
#endif //_PAYLOAD_BRESSER
