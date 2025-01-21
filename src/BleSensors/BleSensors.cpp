///////////////////////////////////////////////////////////////////////////////
// BleSensors.h
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
// Copyright (c) 2023 Matthias Prinke
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
// 20240427 Added paramter activeScan to getData()
// 20250121 Updated for NimBLE-Arduino v2.x
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ARCH_RP2040)

#include "BleSensors.h"

static NimBLEScan *pBLEScan;

class ScanCallbacks : public NimBLEScanCallbacks
{
public:
  std::vector<std::string> m_knownBLEAddresses; /// MAC addresses of known sensors
  std::vector<ble_sensors_t> *m_sensorData;     /// Sensor data

private:
  int m_devices_found = 0; /// Number of known devices found

  void onDiscovered(const NimBLEAdvertisedDevice *advertisedDevice) override
  {
    log_v("Discovered Advertised Device: %s", advertisedDevice->toString().c_str());
  }

  void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override
  {
    TheengsDecoder decoder;
    unsigned idx;
    bool device_found = false;
    JsonDocument doc;

    log_v("Advertised Device Result: %s", advertisedDevice->toString().c_str());
    JsonObject BLEdata = doc.to<JsonObject>();
    String mac_adress = advertisedDevice->getAddress().toString().c_str();

    BLEdata["id"] = (char *)mac_adress.c_str();
    for (idx = 0; idx < m_knownBLEAddresses.size(); idx++)
    {
      if (mac_adress == m_knownBLEAddresses[idx].c_str())
      {
        log_v("BLE device found at index %d", idx);
        device_found = true;
        m_devices_found++;
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

    if (decoder.decodeBLEJson(BLEdata) && device_found)
    {
      if (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG)
      {
        char buf[512];
        serializeJson(BLEdata, buf);
        log_d("TheengsDecoder found device: %s", buf);
      }

      // see https://stackoverflow.com/questions/5348089/passing-a-vector-between-functions-via-pointers
      (*m_sensorData)[idx].temperature = (float)BLEdata["tempc"];
      (*m_sensorData)[idx].humidity = (float)BLEdata["hum"];
      (*m_sensorData)[idx].batt_level = (uint8_t)BLEdata["batt"];
      (*m_sensorData)[idx].rssi = (int)BLEdata["rssi"];
      (*m_sensorData)[idx].valid = ((*m_sensorData)[idx].batt_level > 0);
      log_i("Temperature:       %.1f°C", (*m_sensorData)[idx].temperature);
      log_i("Humidity:          %.1f%%", (*m_sensorData)[idx].humidity);
      log_i("Battery level:     %d%%", (*m_sensorData)[idx].batt_level);
      log_i("RSSI:             %ddBm", (*m_sensorData)[idx].rssi = (int)BLEdata["rssi"]);
      log_d("BLE devices found: %d", m_devices_found);

      BLEdata.remove("manufacturerdata");
      BLEdata.remove("servicedata");
    }

    // Abort scanning because all known devices have been found
    if (m_devices_found == m_knownBLEAddresses.size())
    {
      log_i("All devices found.");
      pBLEScan->stop();
    }
  }

  void onScanEnd(const NimBLEScanResults &results, int reason) override
  {
    log_v("Scan Ended; reason = %d", reason);
  }
} scanCallbacks;

void BleSensors::clearScanResults(void)
{
  pBLEScan->clearResults();
}

// Set all array members invalid
void BleSensors::resetData(void)
{
  for (int i = 0; i < _known_sensors.size(); i++)
  {
    data[i].valid = false;
  }
}

/**
 * \brief Get BLE sensor data
 */
unsigned BleSensors::getData(uint32_t scanTime, bool activeScan)
{
  // From https://github.com/theengs/decoder/blob/development/examples/ESP32/ScanAndDecode/ScanAndDecode.ino:
  // MyAdvertisedDeviceCallbacks are still triggered multiple times; this makes keeping track of received
  // sensors difficult. Setting ScanFilterMode to CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE seems to
  // restrict callback invocation to once per device as desired.
  // NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);
  // NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE);
  // NimBLEDevice::setScanDuplicateCacheSize(200);
  // NimBLEDevice::init("");

  //_pBLEScan = NimBLEDevice::getScan(); //create new scan

  // Set the callback for when devices are discovered, no duplicates.
  //_pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
  // MyAdvertisedDeviceCallbacks *myCb = new MyAdvertisedDeviceCallbacks();

  // Copy some data required by the Callback
  // myCb->m_pBLEScan = _pBLEScan;
  // myCb->m_knownBLEAddresses = _known_sensors;
  // myCb->m_sensorData = &data;

  //_pBLEScan->setAdvertisedDeviceCallbacks(myCb);
  //_pBLEScan->setActiveScan(activeScan); // Set active scanning, this will get more data from the advertiser.
  //_pBLEScan->setInterval(97); // How often the scan occurs / switches channels; in milliseconds,
  //_pBLEScan->setWindow(37);  // How long to scan during the interval; in milliseconds.
  //_pBLEScan->setMaxResults(0); // do not store the scan results, use callback only.
  //_pBLEScan->start(duration, false /* is_continue */);

  // New
  scanCallbacks.m_knownBLEAddresses = _known_sensors;
  scanCallbacks.m_sensorData = &data;
  NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE);
  NimBLEDevice::setScanDuplicateCacheSize(200);

  NimBLEDevice::init("ble-scan");
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(&scanCallbacks);
  pBLEScan->setActiveScan(activeScan);
  pBLEScan->setInterval(97);
  pBLEScan->setWindow(37);
  // pBLEScan->start(scanTime * 1000, false);
  pBLEScan->getResults(scanTime * 1000, false);

  return 0;
}

#endif
