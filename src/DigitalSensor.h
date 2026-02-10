///////////////////////////////////////////////////////////////////////////////
// DigitalSensor.h
//
// Base class for digital sensor integration
//
// created: 02/2026
//
//
// MIT License
//
// Copyright (c) 2026 Matthias Prinke
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
// 20260210 Created
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

/*! \file DigitalSensor.h
 *  \brief Base class for digital sensor integration
 */

#if !defined(_DIGITAL_SENSOR_H)
#define _DIGITAL_SENSOR_H

#include <stdint.h>

/*!
 * \brief Base class for digital sensors
 *
 * Provides interface for sensor initialization and data acquisition
 */
class DigitalSensor
{
public:
    /*!
     * \brief Virtual destructor
     */
    virtual ~DigitalSensor() {}

    /*!
     * \brief Initialize the sensor
     */
    virtual void begin(void) = 0;

    /*!
     * \brief Read sensor data
     * 
     * \returns sensor reading (interpretation depends on sensor type)
     */
    virtual uint16_t read(void) = 0;
};

#endif // _DIGITAL_SENSOR_H
