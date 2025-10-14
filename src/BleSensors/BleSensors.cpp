///////////////////////////////////////////////////////////////////////////////
// BleSensors.cpp
//
// Wrapper class for Theeengs Decoder (https://github.com/theengs/decoder)
//
// Intended for compatibility to the ATC_MiThermometer library
// (https://github.com/matthias-bs/ATC_MiThermometer)
//
// created: 02/2023
//
//
// MIT License
//
// Copyright (c) 2025 Matthias Prinke
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
// 20230211 Created
// 20240427 Added parameter activeScan to getData()
// 20250121 Updated for NimBLE-Arduino v2.x
// 20250725 Fixed potential buffer overflow
// 20250728 Fixed naming collision with ATC_MiThermometer
// 20250808 Added specific logging macros in scan callback to avoid WDT reset
// 20250926 Moved decoding from callback to getData() to prevent WDT reset
//          Modified getData() to return number of known sensors found
// 20251013 Added abort of scanning by touch sensor
// 20251014 Added optional callback to abort scanning early
//          Replaced TouchTriggered by callback function pointer
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ARCH_RP2040)

#include "BleSensors.h"

namespace BleSensorsCallbacks
{
  constexpr size_t JSON_SERIALIZATION_BUFFER_SIZE = 256; //!< Buffer size for JSON serialization

  /*!
   * \brief BLE scan callback class
   */
  class ScanCallbacks : public NimBLEScanCallbacks
  {
  public:
    std::vector<std::string> m_knownBLEAddresses; //!< MAC addresses of known sensors
    std::vector<ble_sensors_t> *m_sensorData;     //!< Sensor data
    NimBLEScan *m_pBLEScan;                       //!< Pointer to the BLE scan object
    bool (*m_stopScanCb)();                        //!< Pointer to optional callback function to stop scan early

    // Raw JSON payloads collected during scan for later decoding.
    std::vector<std::string> m_rawBLEJsons;
    std::vector<int> m_rawIndexes;                //!< corresponding known sensor index

    int m_devices_found = 0; //!< Number of known devices found

  private:
    void onDiscovered(const NimBLEAdvertisedDevice *advertisedDevice) override
    {
      cb_log_v("Discovered Advertised Device: %s", advertisedDevice->toString().c_str());
    }

    void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override
    {
      cb_log_v("Advertised Device Result: %s", advertisedDevice->toString().c_str());

      // Build JSON representation of advertised data
      JsonDocument doc;
      JsonObject BLEdata = doc.to<JsonObject>();
      String mac_adress = advertisedDevice->getAddress().toString().c_str();

      BLEdata["id"] = (char *)mac_adress.c_str();

      int found_index = -1;
      for (unsigned idx = 0; idx < m_knownBLEAddresses.size(); idx++)
      {
        if (mac_adress == m_knownBLEAddresses[idx].c_str())
        {
          cb_log_v("BLE device known at index %u", idx);
          found_index = (int)idx;
          break;
        }
      }

      if (advertisedDevice->haveName())
        BLEdata["name"] = (char *)advertisedDevice->getName().c_str();

      if (advertisedDevice->haveManufacturerData())
      {
        std::string manufacturerdata = advertisedDevice->getManufacturerData();
        std::string mfgdata_hex = NimBLEUtils::dataToHexString((const uint8_t *)manufacturerdata.c_str(), manufacturerdata.length());
        BLEdata["manufacturerdata"] = (char *)mfgdata_hex.c_str();
      }

      BLEdata["rssi"] = (int)advertisedDevice->getRSSI();

      if (advertisedDevice->haveTXPower())
        BLEdata["txpower"] = (int8_t)advertisedDevice->getTXPower();

      if (advertisedDevice->haveServiceData())
      {
        std::string servicedata = advertisedDevice->getServiceData(NimBLEUUID((uint16_t)0x181a));
        std::string servicedata_hex = NimBLEUtils::dataToHexString((const uint8_t *)servicedata.c_str(), servicedata.length());
        BLEdata["servicedata"] = (char *)servicedata_hex.c_str();
        BLEdata["servicedatauuid"] = "0x181a";
      }

      // If this is a known device, store the serialized JSON for decoding later
      if (found_index >= 0)
      {
        // Serialize into std::string
        std::string serialized;
        serialized.reserve(JSON_SERIALIZATION_BUFFER_SIZE);
        serializeJson(BLEdata, serialized);
        m_rawBLEJsons.push_back(std::move(serialized));
        m_rawIndexes.push_back(found_index);

        m_devices_found++;
        cb_log_v("Known BLE device queued for decoding at index %d", found_index);
      }

      // Abort scanning if requested by callback
      if (m_stopScanCb && m_stopScanCb())
      {
        cb_log_i("Scan aborted.");
        m_pBLEScan->stop();
      }

      // Abort scanning because all known devices have been found
      if (m_devices_found == (int)m_knownBLEAddresses.size())
      {
        cb_log_i("All devices found.");
        m_pBLEScan->stop();
      }
    }

    void onScanEnd(const NimBLEScanResults &results, int reason) override
    {
      cb_log_v("Scan Ended; reason = %d", reason);
    }
  } scanCallbacks;

} // namespace BleSensorsCallbacks

void BleSensors::clearScanResults(void)
{
  _pBLEScan->clearResults();
}

// Set all array members invalid
void BleSensors::resetData(void)
{
  for (int i = 0; i < _known_sensors.size(); i++)
  {
    data[i].valid = false;
  }
}

using namespace BleSensorsCallbacks;

/**
 * \brief Get BLE sensor data
 *
 * Note: Decoding using TheengsDecoder is performed here after scanning,
 *       instead of during the NimBLE callback, to avoid heavy processing
 *       in the callback context, which can lead to the watchdog being
 *       triggered.
 */
unsigned BleSensors::getData(uint32_t scanTime, bool activeScan)
{
  NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE);

  NimBLEDevice::init("ble-scan");
  _pBLEScan = NimBLEDevice::getScan();
  _pBLEScan->setScanCallbacks(&scanCallbacks);
  _pBLEScan->setActiveScan(activeScan);
  _pBLEScan->setInterval(97);
  _pBLEScan->setWindow(37);
  scanCallbacks.m_knownBLEAddresses = _known_sensors;
  scanCallbacks.m_sensorData = &data;
  scanCallbacks.m_pBLEScan = _pBLEScan;
  scanCallbacks.m_stopScanCb = _stopScanCb;

  // Ensure previous scan data cleared
  scanCallbacks.m_rawBLEJsons.clear();
  scanCallbacks.m_rawIndexes.clear();
  scanCallbacks.m_devices_found = 0;

  // Start scanning
  // Blocks until all known devices are found or scanTime is expired
  _pBLEScan->getResults(scanTime * 1000, false);

  // Now perform decoding of the collected JSON payloads (outside callback)
  TheengsDecoder decoder;
  for (size_t i = 0; i < scanCallbacks.m_rawBLEJsons.size(); ++i)
  {
    const std::string &serialized = scanCallbacks.m_rawBLEJsons[i];
    int idx = scanCallbacks.m_rawIndexes[i];
    if (idx < 0 || idx >= (int)scanCallbacks.m_sensorData->size())
      continue;

    // Parse JSON
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, serialized);
    if (err)
    {
      cb_log_w("Failed to parse stored advert JSON: %s", err.c_str());
      continue;
    }

    JsonObject BLEdata = doc.as<JsonObject>();

    if (decoder.decodeBLEJson(BLEdata))
    {
      if (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG) {
        constexpr size_t DEBUG_BUFFER_SIZE = 512;
        char buf[DEBUG_BUFFER_SIZE];
        serializeJson(BLEdata, buf, sizeof(buf));
        cb_log_d("TheengsDecoder decoded device: %s", buf);
      }

      // Update sensor data vector
      (*scanCallbacks.m_sensorData)[idx].temperature = (float)BLEdata["tempc"];
      (*scanCallbacks.m_sensorData)[idx].humidity = (float)BLEdata["hum"];
      (*scanCallbacks.m_sensorData)[idx].batt_level = (uint8_t)BLEdata["batt"];
      (*scanCallbacks.m_sensorData)[idx].rssi = (int)BLEdata["rssi"];
      (*scanCallbacks.m_sensorData)[idx].valid = ((*scanCallbacks.m_sensorData)[idx].batt_level > 0);

      cb_log_i("Temperature:       %.1f°C", (*scanCallbacks.m_sensorData)[idx].temperature);
      cb_log_i("Humidity:          %.1f%%", (*scanCallbacks.m_sensorData)[idx].humidity);
      cb_log_i("Battery level:     %d%%", (*scanCallbacks.m_sensorData)[idx].batt_level);
      cb_log_i("RSSI:             %ddBm", (*scanCallbacks.m_sensorData)[idx].rssi);
      cb_log_d("BLE devices found: %d", scanCallbacks.m_devices_found);

      // remove bulky fields from doc to keep log/output clean if needed
      BLEdata.remove("manufacturerdata");
      BLEdata.remove("servicedata");
    }
    else
    {
      cb_log_v("TheengsDecoder could not decode stored advert for index %d", idx);
    }
  }

  // Clear stored raw adverts to free memory
  scanCallbacks.m_rawBLEJsons.clear();
  scanCallbacks.m_rawIndexes.clear();
  unsigned devices_found = scanCallbacks.m_devices_found;
  scanCallbacks.m_devices_found = 0;

  return devices_found;
}

#endif // !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ARCH_RP2040)
