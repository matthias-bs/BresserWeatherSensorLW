///////////////////////////////////////////////////////////////////////////////
// DistanceSensor.h
//
// Ultrasonic distance sensor integration (A02YYUW / DFRobot SEN0311)
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
// 20260210 Created from PayloadDigital.cpp
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

/*! \file DistanceSensor.h
 *  \brief Ultrasonic distance sensor integration
 */

#if !defined(_DISTANCE_SENSOR_H)
#define _DISTANCE_SENSOR_H

#include "../DigitalSensor.h"
#include "../../BresserWeatherSensorLWCfg.h"

#ifdef DISTANCESENSOR_EN

#include <DistanceSensor_A02YYUW.h>
#include "../logging.h"

/*!
 * \brief Ultrasonic distance sensor (A02YYUW / DFRobot SEN0311)
 *
 * Handles initialization and data acquisition for ultrasonic distance sensor
 */
class DistanceSensor : public DigitalSensor
{
public:
    /*!
     * \brief Constructor
     */
    DistanceSensor();

    /*!
     * \brief Destructor
     */
    ~DistanceSensor();

    /*!
     * \brief Initialize ultrasonic distance sensor
     *
     * Initialize UART and power enable pin
     */
    void begin(void) override;

    /*!
     * \brief Read ultrasonic distance sensor data
     * 
     * \returns distance in mm (0 if invalid)
     */
    uint16_t read(void) override;

private:
    DistanceSensor_A02YYUW *m_sensor; //!< Pointer to sensor object
};

#endif // DISTANCESENSOR_EN

#endif // _DISTANCE_SENSOR_H
