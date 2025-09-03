///////////////////////////////////////////////////////////////////////////////
// PayloadDigital.h
//
// Read digital input channels and encode as LoRaWAN payload
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
// 20240520 Created
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_PAYLOAD_DIGITAL)
#define _PAYLOAD_DIGITAL

#include "../BresserWeatherSensorLWCfg.h"

#include <LoraMessage.h>
#include "logging.h"

#ifdef DISTANCESENSOR_EN
// A02YYUW / DFRobot SEN0311 Ultrasonic Distance Sensor
#include <DistanceSensor_A02YYUW.h>
#endif

/*!
 * \brief LoRaWAN node application layer - digital channels
 *
 * Encodes data from digital input channels as LoRaWAN payload
 */
class PayloadDigital
{
public:
    /*!
     * \brief Constructor
     */
    PayloadDigital(){};

    /*!
     * \brief Digital channel startup code
     */
    void begin(void);

    /*!
     * \brief Encode digital data channels for LoRaWAN transmission
     *
     * \param appPayloadCfg LoRaWAN payload configuration bitmaps
     * \param encoder LoRaWAN payload encoder object
     */
    void encodeDigital(uint8_t *appPayloadCfg, LoraEncoder &encoder);

private:
#ifdef DISTANCESENSOR_EN
    /*!
     * \brief Initialize ultrasonic distance sensor A02YYUW
     *
     * Initialize UART and power enable pin
     */
    void initDistanceSensor(void);

    /*!
     * \brief Read ultrasonic distance sensor (A02YYUW) data
     * 
     * \returns distance in mm (0 if invalid)
     */
    uint16_t readDistanceSensor(void)
#endif
};
#endif //_PAYLOAD_DIGITAL