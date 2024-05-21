///////////////////////////////////////////////////////////////////////////////
// PayloadAnalog.h
//
// Read analog input channels and encode as LoRaWAN payload
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

#if !defined(_PAYLOAD_ANALOG)
#define _PAYLOAD_ANALOG

#include "../BresserWeatherSensorLWCfg.h"
#include "adc/adc.h"
#include <LoraMessage.h>


class PayloadAnalog
{
public:
    /*!
     * \brief Constructor
     */
    PayloadAnalog(){};

    /*!
     * \brief Analog channel startup code
     */
    void begin(void);

    /*!
     * \brief Encode analog data channels for LoRaWAN transmission
     *
     * \param appPayloadCfg LoRaWAN payload configuration bitmaps
     * \param encoder LoRaWAN payload encoder object
     */
    void encodeAnalog(uint8_t *appPayloadCfg, LoraEncoder &encoder);
};
#endif //_PAYLOAD_ANALOG