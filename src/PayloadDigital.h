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

/*! \file PayloadDigital.h
 *  \brief LoRaWAN node application layer - digital channels
 */

#if !defined(_PAYLOAD_DIGITAL)
#define _PAYLOAD_DIGITAL

#include "../BresserWeatherSensorLWCfg.h"

#include <LoraMessage.h>
#include "logging.h"
#include "DigitalSensor.h"

#ifdef A02YYUW_EN
#include "DistanceSensors/DistanceSensor.h"
#endif

#ifdef DYP_R01CW_EN
#include "DistanceSensors/DypR01cw.h"
#include <vector>
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
    PayloadDigital()
#ifdef A02YYUW_EN
        : m_distanceSensor(nullptr)
#endif
    {};

    /*!
     * \brief Destructor
     */
    ~PayloadDigital()
    {
#ifdef A02YYUW_EN
        delete m_distanceSensor;
#endif
#ifdef DYP_R01CW_EN
        for (auto sensor : m_dypR01cwSensors)
        {
            delete sensor;
        }
        m_dypR01cwSensors.clear();
#endif
    }

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
#ifdef A02YYUW_EN
    DigitalSensor *m_distanceSensor; //!< Distance sensor instance
#endif
#ifdef DYP_R01CW_EN
    std::vector<DigitalSensor *> m_dypR01cwSensors; //!< DYP-R01CW sensor instances
#endif
};
#endif //_PAYLOAD_DIGITAL
