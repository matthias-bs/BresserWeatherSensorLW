///////////////////////////////////////////////////////////////////////////////
// AppLayer.cpp
//
// LoRaWAN node application layer
//
// - Create data payload from sensor or simulated data
// - Decode sensor specific commands
// - Encode sensor specific status responses
// - Retain sensor specific parameters
//
// This implementation is specific for the BresserWeatherSensorLW project
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
// 20240413 Refactored ADC handling
// 20240414 Added separation between LoRaWAN and application layer
// 20240417 Added sensor configuration functions
// 20240419 Modified downlink decoding
// 20240424 Fixes in decodeDownlink()
//          Fixed getBleAddr()
//          Implemented resetting of knownBLEAddresses to defaults
// 20240426 Added BLE address initialization after updating via downlink
// 20240427 Added BLE configuration/status via LoRaWAN
// 20240507 Added configuration of max_sensors/rx_flags via LoRaWAN
// 20240508 Added configuration of en_decoders via LoRaWAN
// 20240515 Added getOneWireTemperature()
// 20240520 Moved 1-Wire sensor code to PayloadOneWire.h/.cpp
// 20240524 Added appPayloadCfgDef, setAppPayloadCfg() & getAppPayloadCfg()
//          Moved code to PayloadBresser, PayloadAnalog & PayloadDigital
// 20240529 Changed encoding of INV_TEMP for BLE sensors
// 20240530 Fixed CMD_SET_APP_PAYLOAD_CFG handling
// 20240531 Moved BLE specific code to PayloadBLE.cpp
// 20240603 Added encoding of sensor battery status flags
//          Added CMD_GET_SENSORS_STAT
// 20240606 Added CMD_GET_STATUS_INTERVAL/CMD_SET_STATUS_INTERVAL
// 20240614 Added lightning statistics reset
// 20240701 Fixed CMD_GET_BLE_ADDR (get default if Preferences are empty/zero)
// 20240716 Added CMD_SCAN_SENSORS
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "AppLayer.h"

void AppLayer::genPayload(uint8_t port, LoraEncoder &encoder)
{
    // unused
    (void)port;
    (void)encoder;
    weatherSensor.genMessage(0, 0xfff0, SENSOR_TYPE_WEATHER1);
    weatherSensor.genMessage(1, 0xfff1, SENSOR_TYPE_SOIL);
}

void AppLayer::getPayloadStage1(uint8_t &port, LoraEncoder &encoder)
{
    (void)port; // eventually suppress warning regarding unused parameter

    if (ws_scan_time)
    {
        log_i("Scan sensors");
        scanBresser(ws_scan_time, encoder);
        port = CMD_SCAN_SENSORS;
        ws_scan_time = 0;
        return;
    }

    log_v("Port: %d", port);

    log_i("--- Uplink Data ---");

    encodeBresser(appPayloadCfg, appStatus, encoder);

#ifdef ONEWIRE_EN
    encodeOneWire(appPayloadCfg, encoder);
#endif

    // Voltages / auxiliary analog sensor data
    encodeAnalog(appPayloadCfg, encoder);

    // Digital Sensors (GPIO, UART, I2C, SPI, ...)
    encodeDigital(appPayloadCfg, encoder);

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    // BLE Temperature/Humidity Sensors
    encodeBLE(appPayloadCfg, appStatus, encoder);
#endif

    // FIXME: To be removed later
    // Battery status flags for compatibility with BresserWeatherSensorTTN
    if ((appPayloadCfg[0] & 1) && (encoder.getLength() <= PAYLOAD_SIZE - 1))
    {
        log_i("Battery status flags: ws=%u, soil=%u, lgt=%u", appStatus[SENSOR_TYPE_WEATHER1] & 1,
              (appStatus[SENSOR_TYPE_SOIL] & 2) >> 1, appStatus[SENSOR_TYPE_LIGHTNING] & 1);
        encoder.writeBitmap(0,
                            0,
                            0,
                            (appStatus[SENSOR_TYPE_LIGHTNING] & 1) ? true : false,
                            0,
                            (appStatus[SENSOR_TYPE_SOIL] & 2) ? true : false,
                            0,
                            (appStatus[SENSOR_TYPE_WEATHER1] & 1) ? true : false);
    }
}

void AppLayer::getPayloadStage2(uint8_t &port, LoraEncoder &encoder)
{
    (void)port;
    (void)encoder;
}

uint8_t
AppLayer::decodeDownlink(uint8_t port, uint8_t *payload, size_t size)
{
    if ((port == CMD_RESET_WS_POSTPROC) && (size == 1))
    {
#ifdef RAINDATA_EN
        if (payload[0] & 0xF)
        {
            log_d("Reset rain statistics - flags: 0x%X", payload[0]);
            rainGauge.reset(payload[0] & 0xF);
        }
#endif
#ifdef LIGHTNINGSENSOR_EN
        if (payload[0] & 0x10)
        {
            log_d("Reset lightning statistics");
            lightningProc.reset();
        }
#endif
        return 0;
    }

    if ((port == CMD_SCAN_SENSORS) && (size == 1))
    {
        log_d("Scan sensors - time: %u s", payload[0]);
        // 1. Set flag in Preferences to trigger sensor scan and set scan time
        // 2. If flag is set, perform sensors scan instead of normal operation in 
        //    PayloadBresser::begin(void)
        // 3. Reset flag after scan
        // 4. Uplink scan results instead of normal sensor data
        appPrefs.begin("BWS-LW-APP", false);
        appPrefs.putUChar("ws_scan_t", payload[0]);
        appPrefs.end();
        return 0;
    }

    if ((port == CMD_GET_WS_TIMEOUT) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get weathersensor_timeout");
        return CMD_GET_WS_TIMEOUT;
    }

    if ((port == CMD_SET_WS_TIMEOUT) && (size == 1))
    {
        log_d("Set weathersensor_timeout: %u s", payload[0]);
        appPrefs.begin("BWS-LW-APP", false);
        appPrefs.putUChar("ws_timeout", payload[0]);
        appPrefs.end();
        return 0;
    }

    if ((port == CMD_GET_STATUS_INTERVAL) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get status_interval");
        return CMD_GET_STATUS_INTERVAL;
    }

    if ((port == CMD_SET_STATUS_INTERVAL) && (size == 1))
    {
        log_d("Set status_interval: %u frames", payload[0]);
        appPrefs.begin("BWS-LW-APP", false);
        appPrefs.putUChar("stat_interval", payload[0]);
        appPrefs.end();
        return 0;
    }

    if ((port == CMD_GET_SENSORS_STAT) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get sensors' status");
        return CMD_GET_SENSORS_STAT;
    }

    if ((port == CMD_GET_SENSORS_INC) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get sensors include list");
        return CMD_GET_SENSORS_INC;
    }

    if ((port == CMD_SET_SENSORS_INC) && (size % 4 == 0))
    {
        log_d("Set sensors include list");
        for (size_t i = 0; i < size; i += 4)
        {
            log_d("%08X:",
                  (payload[i] << 24) |
                      (payload[i + 1] << 16) |
                      (payload[i + 2] << 8) |
                      payload[i + 3]);
        }
        weatherSensor.setSensorsInc(payload, size);
        return 0;
    }

    if ((port == CMD_GET_SENSORS_EXC) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get sensors exclude list");
        return CMD_GET_SENSORS_EXC;
    }

    if ((port == CMD_SET_SENSORS_EXC) && (size % 4 == 0))
    {
        log_d("Set sensors exclude list");
        for (size_t i = 0; i < size - 1; i += 4)
        {
            log_d("%08X:",
                  (payload[i] << 24) |
                      (payload[i + 1] << 16) |
                      (payload[i + 2] << 8) |
                      payload[i + 3]);
        }
        weatherSensor.setSensorsExc(payload, size);
        return 0;
    }

    if ((port == CMD_GET_SENSORS_CFG) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get sensors configuration");
        return CMD_GET_SENSORS_CFG;
    }

    if ((port == CMD_SET_SENSORS_CFG) && (size == 3))
    {
        log_d("Set sensors configuration - max_sensors: %u, rx_flags: %u, en_decoders: %u",
              payload[0], payload[1], payload[2]);
        weatherSensor.setSensorsCfg(payload[0], payload[1], payload[2]);
        return 0;
    }

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    if ((port == CMD_GET_BLE_CONFIG) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get BLE config");
        return CMD_GET_BLE_CONFIG;
    }

    if ((port == CMD_SET_BLE_CONFIG) && (size == 2))
    {
        appPrefs.begin("BWS-LW-APP", false);
        appPrefs.putUChar("ble_active", payload[0]);
        appPrefs.putUChar("ble_scantime", payload[1]);
        appPrefs.end();
        return 0;
    }

    if ((port == CMD_GET_BLE_ADDR) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get BLE sensors MAC addresses");
        return CMD_GET_BLE_ADDR;
    }

    if ((port == CMD_SET_BLE_ADDR) && (size % 6 == 0))
    {
        log_d("Set BLE sensors MAC addresses");
        for (size_t i = 0; i < size - 1; i += 6)
        {
            log_d("%02X:%02X:%02X:%02X:%02X:%02X",
                  payload[i],
                  payload[i + 1],
                  payload[i + 2],
                  payload[i + 3],
                  payload[i + 4],
                  payload[i + 5]);
        }

        setBleAddr(payload, size);
        bleAddrInit();

        return 0;
    }

    if ((port == CMD_GET_APP_PAYLOAD_CFG) && (payload[0] == 0x00) && (size == 1))
    {
        log_d("Get AppLayer payload configuration");
        return CMD_GET_APP_PAYLOAD_CFG;
    }

    if ((port == CMD_SET_APP_PAYLOAD_CFG) && (size == 24))
    {
        log_d("Set AppLayer payload configuration");
        for (size_t i = 0; i < 16; i++)
        {
            log_d("Type%02d: 0x%X", i, payload[i]);
        }
        log_d("1-Wire:  0x%04X", payload[16] << 8 | payload[17]);
        log_d("Analog:  0x%04X", (payload[18] << 8) | payload[19]);
        log_d("Digital: 0x%08X", (payload[20] << 24) | (payload[21] << 16) | (payload[22] << 8) | payload[23]);

        setAppPayloadCfg(payload, APP_PAYLOAD_CFG_SIZE);
        return 0;
    }

#endif
    return 0;
}

void AppLayer::getConfigPayload(uint8_t cmd, uint8_t &port, LoraEncoder &encoder)
{
    if (cmd == CMD_GET_WS_TIMEOUT)
    {
        appPrefs.begin("BWS-LW-APP", false);
        uint8_t ws_timeout = appPrefs.getUChar("ws_timeout", WEATHERSENSOR_TIMEOUT);
        appPrefs.end();
        encoder.writeUint8(ws_timeout);
        port = CMD_GET_WS_TIMEOUT;
    }
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    else if (cmd == CMD_GET_BLE_CONFIG)
    {
        appPrefs.begin("BWS-LW-APP", false);
        uint8_t ble_active = appPrefs.getUChar("ble_active", BLE_SCAN_MODE);
        uint8_t ble_scantime = appPrefs.getUChar("ble_scantime", BLE_SCAN_TIME);
        appPrefs.end();
        encoder.writeUint8(ble_active);
        encoder.writeUint8(ble_scantime);
        port = CMD_GET_BLE_CONFIG;
    }
#endif
    else if (cmd == CMD_GET_STATUS_INTERVAL)
    {
        encoder.writeUint8(getAppStatusUplinkInterval());
        port = CMD_GET_STATUS_INTERVAL;
    }
    else if (cmd == CMD_GET_SENSORS_STAT)
    {
        for (size_t i = 0; i < APP_STATUS_SIZE; i++)
        {
            encoder.writeUint8(appStatus[i]);
        }
        port = CMD_GET_SENSORS_STAT;
    }
    else if (cmd == CMD_GET_SENSORS_STAT)
    {
        for (size_t i = 0; i < APP_STATUS_SIZE; i++)
        {
            encoder.writeUint8(appStatus[i]);
        }
        port = CMD_GET_SENSORS_STAT;
    }
    else if (cmd == CMD_GET_SENSORS_INC)
    {
        uint8_t payload[48];
        uint8_t size = weatherSensor.getSensorsInc(payload);
        for (size_t i = 0; i < min(size, static_cast<uint8_t>(48)); i++)
        {
            encoder.writeUint8(payload[i]);
        }
        port = CMD_GET_SENSORS_INC;
    }
    else if (cmd == CMD_GET_SENSORS_EXC)
    {
        uint8_t payload[48];
        uint8_t size = weatherSensor.getSensorsExc(payload);
        for (size_t i = 0; i < min(size, static_cast<uint8_t>(48)); i++)
        {
            encoder.writeUint8(payload[i]);
        }
        port = CMD_GET_SENSORS_EXC;
    }
    else if (cmd == CMD_GET_SENSORS_CFG)
    {
        uint8_t maxSensors;
        uint8_t rxFlags;
        uint8_t enDecoders;
        weatherSensor.getSensorsCfg(maxSensors, rxFlags, enDecoders);
        encoder.writeUint8(maxSensors);
        encoder.writeUint8(rxFlags);
        encoder.writeUint8(enDecoders);
        port = CMD_GET_SENSORS_CFG;
    }
#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
    else if (cmd == CMD_GET_BLE_ADDR)
    {
        for (const std::string &addr : knownBLEAddresses)
        {
            for (int i = 0; i < 6; i++)
            {
                uint8_t byte = std::stoi(addr.substr(3 * i, 2), nullptr, 16);
                encoder.writeUint8(byte);
            }
        }
        port = CMD_GET_BLE_ADDR;
    }
#endif
    else if (cmd == CMD_GET_APP_PAYLOAD_CFG)
    {
        uint8_t payload[APP_PAYLOAD_CFG_SIZE];
        getAppPayloadCfg(payload, APP_PAYLOAD_CFG_SIZE);
        for (size_t i = 0; i < APP_PAYLOAD_CFG_SIZE; i++)
        {
            encoder.writeUint8(payload[i]);
        }
        port = CMD_GET_APP_PAYLOAD_CFG;
    }
}

bool AppLayer::getAppPayloadCfg(uint8_t *bytes, uint8_t size)
{
    bool res = false;
    appPrefs.begin("BWS-LW-APP", false);
    if (appPrefs.isKey("payloadcfg"))
    {
        appPrefs.getBytes("payloadcfg", bytes, size);
        res = true;
    }
    appPrefs.end();
    return res;
}

void AppLayer::setAppPayloadCfg(uint8_t *bytes, uint8_t size)
{
    appPrefs.begin("BWS-LW-APP", false);
    appPrefs.putBytes("payloadcfg", bytes, size);
    appPrefs.end();
    memcpy(appPayloadCfg, bytes, size);
}
