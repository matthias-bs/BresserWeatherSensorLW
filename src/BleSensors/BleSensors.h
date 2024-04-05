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
//
// ToDo:
// - 
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(BLE_SENSORS) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) \
                          && !defined(ARDUINO_ARCH_RP2040)
#define BLE_SENSORS

#include <Arduino.h>
#include <NimBLEDevice.h>       //!< https://github.com/h2zero/NimBLE-Arduino
#include <decoder.h>            //!< https://github.com/theengs/decoder

// Local Sensor Data
struct BleDataS {
      bool     valid;              //!< data valid
      float    temperature;        //!< temperature in degC
      float    humidity;           //!< humidity in %
      uint8_t  batt_level;         //!< battery level in %
      int      rssi;               //!< RSSI in dBm
};

typedef struct BleDataS ble_sensors_t; //!< Shortcut for struct BleDataS


/*!
  \class Ble_Sensors
  \brief BLE Sensor (e.g. thermometer/hygrometer) client
*/
class BleSensors {
    public:
        /*!
        \brief Constructor.
        
        \param known_sensors    Vector of BLE MAC addresses of known sensors, e.g. {"11:22:33:44:55:66", "AA:BB:CC:DD:EE:FF"}
        */
        BleSensors(std::vector<std::string> known_sensors) {
            _known_sensors = known_sensors;
            data.resize(known_sensors.size());
        };

        /*!
        \brief Initialization.
        */
        void begin(void) {
        };
        
        /*!
        \brief Delete results from BLEScan buffer to release memory.
        */        
        void clearScanResults(void) {
            _pBLEScan->clearResults();
        };
        
        /*!
        \brief Get data from sensors by running a BLE scan.
        
        \param duration     Scan duration in seconds
        */                
        unsigned getData(uint32_t duration);
        
        /*!
        \brief Set sensor data invalid.
        */                        
        void resetData(void);
        
        /*!
        \brief Sensor data.
        */
        std::vector<ble_sensors_t>  data;
        
    protected:
        std::vector<std::string> _known_sensors;
        NimBLEScan*              _pBLEScan;
};
#endif
