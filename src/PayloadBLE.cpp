///////////////////////////////////////////////////////////////////////////////
// PayloadBLE.cpp
//
// Get BLE temperature/humidity sensor values and encode as LoRaWAN payload
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
// 20240531 Moved from AppLayer.cpp
// 20240603 Added BLE sensor battery status
// 20240610 Fixed exception with empty list of BLE addresses
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "PayloadBLE.h"

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)

void PayloadBLE::setBleAddr(uint8_t *bytes, uint8_t size)
{
    appPrefs.begin("BWS-LW-APP", false);
    appPrefs.putBytes("ble", bytes, size);
    appPrefs.end();
}

uint8_t PayloadBLE::getBleAddr(uint8_t *payload)
{
    appPrefs.begin("BWS-LW-APP", false);
    uint8_t size = appPrefs.getBytesLength("ble");
    appPrefs.getBytes("ble", payload, size);
    appPrefs.end();

    return size;
}

std::vector<std::string> PayloadBLE::getBleAddr(void)
{
    std::vector<std::string> bleAddr;

    appPrefs.begin("BWS-LW-APP", false);
    uint8_t size = appPrefs.getBytesLength("ble");
    uint8_t addrBytes[48];
    appPrefs.getBytes("ble", addrBytes, size);
    appPrefs.end();

    if (size < 6)
    {
        // return empty list
        return bleAddr;
    }

    uint8_t check = 0;
    for (size_t i = 0; i < 6; i++)
    {
        check |= addrBytes[i];
    }
    if (check == 0)
    {
        // First address is 00:00:00:00:00:00, return empty list
        return bleAddr;
    }

    for (size_t i = 0; i < size; i += 6)
    {
        char addr[18];
        snprintf(addr, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
                 addrBytes[i], addrBytes[i + 1], addrBytes[i + 2], addrBytes[i + 3], addrBytes[i + 4], addrBytes[i + 5]);
        bleAddr.push_back(addr);
    }

    return bleAddr;
}

/*
 * Initialize list of known BLE addresses from defaults or Preferences
 */
void PayloadBLE::bleAddrInit(void)
{
    knownBLEAddressesDef = KNOWN_BLE_ADDRESSES;
    knownBLEAddresses = getBleAddr();
    if (knownBLEAddresses.size() != 0)
    {
        log_d("Using BLE addresses from Preferences:");
    }
    else if (knownBLEAddressesDef.size() != 0)
    {
        // No addresses stored in Preferences, use default
        knownBLEAddresses = knownBLEAddressesDef;
        log_d("Using BLE addresses from BresserWeatherSensorLWCfg.h:");
    }
    else
    {
        log_d("No BLE addresses specified.");
    }
    bleSensors = BleSensors(knownBLEAddresses);

    for (const std::string &s : knownBLEAddresses)
    {
        (void)s;
        log_d("%s", s.c_str());
    }
};

/*
 * Encode BLE temperature/humidity sensor values for LoRaWAN transmission
 */
void PayloadBLE::encodeBLE(uint8_t *appPayloadCfg, uint8_t *appStatus, LoraEncoder &encoder)
{
    if (bleSensors.data.size() == 0)
        return;

    float indoor_temp_c;
    float indoor_humidity;

    // Set sensor data invalid
    bleSensors.resetData();

    appPrefs.begin("BWS-LW-APP", false);
    uint8_t ble_active = appPrefs.getUChar("ble_active", BLE_SCAN_MODE);
    uint8_t ble_scantime = appPrefs.getUChar("ble_scantime", BLE_SCAN_TIME);
    log_d("Preferences: ble_active: %u", ble_active);
    log_d("Preferences: ble_scantime: %u s", ble_scantime);
    appPrefs.end();
    // Get sensor data - run BLE scan for <bleScanTime>
    bleSensors.getData(ble_scantime, ble_active);

    // BLE Temperature/Humidity Sensors
#if defined(MITHERMOMETER_EN)
    float div = 100.0;
#elif defined(THEENGSDECODER_EN)
    float div = 1.0;
#endif

    if (encoder.getLength() <= PAYLOAD_SIZE - 3)
    {
        if (bleSensors.data[0].valid)
        {
            indoor_temp_c = bleSensors.data[0].temperature / div;
            indoor_humidity = bleSensors.data[0].humidity / div;
            log_i("Indoor Air Temp.:   % 3.1f °C", bleSensors.data[0].temperature / div);
            log_i("Indoor Humidity:     %3.1f %%", bleSensors.data[0].humidity / div);
            encoder.writeTemperature(indoor_temp_c);
            encoder.writeUint8(static_cast<uint8_t>(indoor_humidity + 0.5));
            if (bleSensors.data[0].batt_level > BLE_BATT_OK)
            {
                appStatus[APP_PAYLOAD_OFFS_BLE + APP_PAYLOAD_BYTES_BLE - 1] |= 1;
            }
        }
        else
        {
            log_i("Indoor Air Temp.:    --.- °C");
            log_i("Indoor Humidity:     --   %%");
            encoder.writeTemperature(INV_TEMP);
            encoder.writeUint8(INV_UINT8);
        }
        // BLE Temperature/Humidity Sensors: delete results fromBLEScan buffer to release memory
        bleSensors.clearScanResults();
    }
}

#endif