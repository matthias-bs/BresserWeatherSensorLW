///////////////////////////////////////////////////////////////////////////////
// DypR01cw.cpp
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

#include "DypR01cw.h"

#ifdef DYP_R01CW_EN

DypR01cw::DypR01cw(uint8_t addr)
    : m_addr(addr)
{
    m_sensor = new DYP_R01CW(addr);
}

DypR01cw::~DypR01cw()
{
    delete m_sensor;
}

void DypR01cw::begin(void)
{
    // Initialize I2C with custom pins if defined (only once)
    static bool wireInitialized = false;
    if (!wireInitialized)
    {
#if defined(DYP_R01CW_SDA) && defined(DYP_R01CW_SCL)
    #if defined(ARDUINO_ARCH_RP2040)
        // RP2040 requires setSDA/setSCL before begin()
        Wire.setSDA(DYP_R01CW_SDA);
        Wire.setSCL(DYP_R01CW_SCL);
        Wire.begin();
    #else
        // ESP32 and other platforms support begin(sda, scl)
        Wire.begin(DYP_R01CW_SDA, DYP_R01CW_SCL);
    #endif
#else
        Wire.begin();
#endif
        wireInitialized = true;
    }

    // Initialize the sensor
    if (!m_sensor->begin(&Wire))
    {
        log_e("DYP-R01CW sensor (0x%02X) initialization failed", m_addr);
    }
    else
    {
        log_i("DYP-R01CW sensor (0x%02X) initialized", m_addr);
    }
}

uint16_t DypR01cw::read(void)
{
    int16_t distance = m_sensor->readDistance();

    if (distance < 0)
    {
        log_e("DYP-R01CW sensor (0x%02X) read error", m_addr);
        return 0;
    }

    return static_cast<uint16_t>(distance);
}

#endif // DYP_R01CW_EN
