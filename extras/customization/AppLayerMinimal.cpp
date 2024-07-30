///////////////////////////////////////////////////////////////////////////////
// AppLayerMinimal.cpp
//
// LoRaWAN node application layer -  Minimal template for customization
//
// - Create data payload from sensor or simulated data
// - Decode sensor specific commands
// - Encode sensor specific status responses
// - Retain sensor specific parameters
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
// 20240513 Created
// 20240730 Updated getPayloadStage1()/2()
//
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "AppLayer.h"

uint8_t
AppLayer::decodeDownlink(uint8_t port, uint8_t *payload, size_t size)
{
    return 0;
}

void AppLayer::genPayload(uint8_t port, LoraEncoder &encoder)
{
    (void)port; // suppress warning regarding unused parameter
    (void)encoder; // suppress warning regarding unused parameter
}

void AppLayer::getPayloadStage1(uint8_t &port, LoraEncoder &encoder)
{
    (void)port; // suppress warning regarding unused parameter

    // Sensor status flags
    encoder.writeBitmap(true,
                        false,
                        false,
                        false,
                        false,
                        false,
                        false,
                        false);

    // Example data
    encoder.writeUint8(0xaa);
    encoder.writeRawFloat(21.3);
}

void AppLayer::getPayloadStage2(uint8_t &port, LoraEncoder &encoder)
{
    (void)port; // suppress warning regarding unused parameter
    (void)encoder; // suppress warning regarding unused parameter
}

void AppLayer::getConfigPayload(uint8_t cmd, uint8_t &port, LoraEncoder &encoder)
{
    (void)cmd; // suppress warning regarding unused parameter
    (void)port; // suppress warning regarding unused parameter
    (void)encoder; // suppress warning regarding unused parameter
}
