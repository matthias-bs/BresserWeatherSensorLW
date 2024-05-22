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

#if !defined(_PAYLOAD_BRESSER)
#define _PAYLOAD_BRESSER

#include "../BresserWeatherSensorLWCfg.h"
#include "WeatherSensorCfg.h"
#include <WeatherSensor.h>

#ifdef RAINDATA_EN
#include "RainGauge.h"
#endif
#ifdef LIGHTNINGSENSOR_EN
#include "Lightning.h"
#endif

#include <LoraMessage.h>

class PayloadBresser
{
public:
    /// Bresser Weather Sensor Receiver
    WeatherSensor weatherSensor;

private:
#ifdef LIGHTNINGSENSOR_EN
    time_t lightn_ts;
    int lightn_events;
    uint8_t lightn_distance;

    /// Lightning sensor post-processing
    Lightning lightningProc;
#endif

public:
    /*!
     * \brief Constructor
     */
    PayloadBresser(){};

    /*!
     * \brief Bresser sensors startup code
     */
    void begin(void);

    /*!
     * \brief Encode Bresser sensor data for LoRaWAN transmission
     *
     * \param appPayloadCfg LoRaWAN payload configuration bitmaps
     * \param encoder LoRaWAN payload encoder object
     */
    void encodeBresser(uint8_t *appPayloadCfg, LoraEncoder &encoder);

private:
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
};
#endif //_PAYLOAD_BRESSER