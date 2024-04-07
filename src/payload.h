///////////////////////////////////////////////////////////////////////////////
// payload.h
//
// Create data payload from sensor or simulated data
//
// created: 04/2024
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
// 20240402 Created
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_PAYLOAD_H)
#define _PAYLOAD_H
#include <LoraMessage.h>
//#include "settings.h"

/*!
 * \brief Generate payload (by emulation)
 * 
 * \param port LoRaWAN port
 * \param encoder uplink encoder object
*/
void genPayload(uint8_t port, LoraEncoder & encoder);

/*!
 * \brief Prepare / get payload at startup
 *
 * Use this if 
 * - A sensor needs some time for warm-up or data acquisition
 * - The data aquisition has to be done directly after startup
 * - The radio transceiver is used for sensor communication
 *   before starting LoRaWAN activities.
 * 
 * \param port LoRaWAN port
 * \param encoder uplink encoder object
 */
void getPayloadStage1(uint8_t port, LoraEncoder & encoder);

/*!
 * \brief Get payload befor uplink
 *
 * Use this if 
 * - The radio transceiver is NOT used for sensor communication
 * - The sensor preparation has been started in stage1
 * - The data aquistion has to be done immediately before uplink
 * 
 * \param port LoRaWAN port
 * \param encoder uplink encoder object
 */
void getPayloadStage2(uint8_t port, LoraEncoder & encoder);

/*!
 * \brief Decode device specific downlink messages
 * 
 * \param port LoRaWAN port
 * \param payload downlink message payload
 * \param size downlink message size (bytes)
 */
void deviceDecodeDownlink(uint8_t port, uint8_t *payload, size_t size);

#endif // PAYLOAD_H