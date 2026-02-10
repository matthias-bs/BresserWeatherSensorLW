///////////////////////////////////////////////////////////////////////////////
// DypR01cw.h
//
// DYP-R01CW laser distance sensor integration
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

/*! \file DypR01cw.h
 *  \brief DYP-R01CW laser distance sensor integration
 */

#if !defined(_DYP_R01CW_H)
#define _DYP_R01CW_H

#include "../DigitalSensor.h"
#include "../../BresserWeatherSensorLWCfg.h"

#ifdef DYP_R01CW_EN

#include <DYP_R01CW.h>
#include <Wire.h>
#include "../logging.h"

/*!
 * \brief DYP-R01CW laser distance sensor
 *
 * Handles initialization and data acquisition for DYP-R01CW laser distance sensor via I2C
 */
class DypR01cw : public DigitalSensor
{
public:
    /*!
     * \brief Constructor
     * 
     * \param addr I2C address of the sensor (8-bit format)
     */
    DypR01cw(uint8_t addr);

    /*!
     * \brief Destructor
     */
    ~DypR01cw();

    /*!
     * \brief Initialize DYP-R01CW laser distance sensor
     *
     * Initialize I2C interface and sensor
     */
    void begin(void) override;

    /*!
     * \brief Read DYP-R01CW laser distance sensor data
     * 
     * \returns distance in mm (0 if invalid)
     */
    uint16_t read(void) override;

private:
    DYP_R01CW *m_sensor; //!< Pointer to sensor object
    uint8_t m_addr;      //!< I2C address
};

#endif // DYP_R01CW_EN

#endif // _DYP_R01CW_H
