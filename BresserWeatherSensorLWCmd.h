///////////////////////////////////////////////////////////////////////////////
// BresserWeatherSensorLWCmd.h
//
// LoRaWAN Command Interface
//
// Definition of control/configuration commands and status responses for
// LoRaWAN network layer and application layer
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
// 20240504 Extracted from BresserWeatherSensorLWCfg.h
// 20240507 Added CMD_GET_SENSORS_CFG/CMD_SET_SENSORS_CFG
// 20240508 Updated description of CMD_GET_SENSORS_CFG/CMD_SET_SENSORS_CFG
// 20240603 Added CMD_GET_SENSORS_STAT
// 20240606 Added CMD_GET_STATUS_INTERVAL/CMD_SET_STATUS_INTERVAL
// 20240608 Added CMD_GET_LW_STATUS
// 20240609 Refactored command encoding
// 20240614 Renamed CMD_RESET_RAINGAUGE to CMD_RESET_WS_POSTPROC
// 20240716 Added CMD_SCAN_SENSORS
// 20240722 Added CMD_SET_LW_STATUS_INTERVAL, renamed 
//          CMD_<GET|SET>_STATUS_INTERVAL to CMD_<GET|SET>_APP_STATUS_INTERVAL
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_LWCMD_H)
#define _LWCMD_H

#include <Arduino.h>
#include <LoraEncoder.h>

// ===========================
// LoRaWAN command interface
// ===========================

// ---------------------------
// -- LoRaWAN network layer --
// ---------------------------

// CMD_GET_DATETIME
// -----------------
#define CMD_GET_DATETIME 0x20

// Downlink (command):
// byte0:   0x00

// Uplink (response):
// byte0: unixtime[31:24]
// byte1: unixtime[23:16]
// byte2: unixtime[15: 8]
// byte3: unixtime[ 7: 0]
// byte4: rtc_source[ 7: 0]


// CMD_SET_DATETIME
// -----------------
// Port: CMD_SET_DATETIME
#define CMD_SET_DATETIME 0x21

// Downlink (command):
// byte0: unixtime[31:24]
// byte1: unixtime[23:16]
// byte2: unixtime[15: 8]
// byte3: unixtime[ 7: 0]

// Uplink: n.a.

// CMD_SET_SLEEP_INTERVAL
// -----------------------
// Note: Set normal sleep interval in seconds
// Port: CMD_SET_SLEEP_INTERVAL
#define CMD_SET_SLEEP_INTERVAL 0x31

// Downlink (command):
// byte0: sleep_interval[15:8]
// byte1: sleep_interval[ 7:0]

// Response: n.a.

// CMD_SET_SLEEP_INTERVAL_LONG
// ----------------------------
// Note: Set long sleep interval in seconds (energy saving mode)
// Port: CMD_SET_SLEEP_INTERVAL_LONG
#define CMD_SET_SLEEP_INTERVAL_LONG 0x33

// Downlink (command):
// byte0: sleep_interval_long[15:8]
// byte1: sleep_interval_long[ 7:0]

// Uplink: n.a.

// CMD_SET_LW_STATUS_INTERVAL
// ---------------------------
// Note: Set LoRaWAN status interval in frame counts
// Port: CMD_SET_LW_STATUS_INTERVAL
#define CMD_SET_LW_STATUS_INTERVAL 0x35

// Downlink (command):
// byte0: lw_status_interval[7:0]

// Uplink: n.a.

// CMD_GET_LW_CONFIG
// ------------------
// Port: CMD_GET_LW_CONFIG
#define CMD_GET_LW_CONFIG 0x36

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: sleep_interval[15: 8]
// byte1: sleep_interval[ 7:0]
// byte2: sleep_interval_long[15:8]
// byte3: sleep_interval_long[ 7:0]

// CMD_GET_LW_STATUS
// ------------------
// Port: CMD_GET_LW_STATUS
// Note: Get LoRaWAN device status
#define CMD_GET_LW_STATUS 0x38

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: u_batt[15:8]
// byte1: u_batt[ 7:0]
// byte2: flags[ 7:0]

// -----------------------
// -- Application layer --
// -----------------------

// CMD_GET_APP_STATUS_INTERVAL
// ------------------------
// Note: Get status interval in frame counts
// Port: CMD_GET_APP_STATUS_INTERVAL
#define CMD_GET_APP_STATUS_INTERVAL 0x40

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: status_interval[7:0]

// CMD_SET_APP_STATUS_INTERVAL
// ------------------------
// Note: Set status interval in frame counts
// Port: CMD_SET_APP_STATUS_INTERVAL
#define CMD_SET_APP_STATUS_INTERVAL 0x41

// Downlink (command):
// byte0: status_interval[7:0]

// Uplink: n.a.

// CMD_GET_SENSORS_STAT
// ---------------------
// Port: CMD_GET_SENSORS_STAT
#define CMD_GET_SENSORS_STAT 0x42

// CMD_GET_APP_PAYLOAD_CFG
// -----------------------
// Port: CMD_GET_APP_PAYLOAD_CFG
#define CMD_GET_APP_PAYLOAD_CFG 0x46

// Downlink (command):
// byte0: 0x00

// Response: n.a.
// Uplink (command):
// byte00: type00[7:0]
// byte01: type01[7:0]
// byte02: type02[7:0]
// byte03: type03[7:0]
// byte04: type04[7:0]
// byte05: type05[7:0]
// byte06: type06[7:0]
// byte07: type07[7:0]
// byte08: type08[7:0]
// byte09: type09[7:0]
// byte10: type10[7:0]
// byte11: type11[7:0]
// byte12: type12[7:0]
// byte13: type13[7:0]
// byte14: type14[7:0]
// byte15: type15[7:0]
// byte16: onewire[15:8]
// byte17: onewire[7:0]
// byte18: analog[15:8]
// byte19: analog[7:0]
// byte20: digital[31:24]
// byte21: digital[23:16]
// byte22: digital[15:8]
// byte23: digital[7:0]

// CMD_SET_APP_PAYLOAD_CFG
// Port: CMD_SET_APP_PAYLOAD_CFG
#define CMD_SET_APP_PAYLOAD_CFG 0x47

// Uplink (command):
// byte00: type00[7:0]
// byte01: type01[7:0]
// byte02: type02[7:0]
// byte03: type03[7:0]
// byte04: type04[7:0]
// byte05: type05[7:0]
// byte06: type06[7:0]
// byte07: type07[7:0]
// byte08: type08[7:0]
// byte09: type09[7:0]
// byte10: type10[7:0]
// byte11: type11[7:0]
// byte12: type12[7:0]
// byte13: type13[7:0]
// byte14: type14[7:0]
// byte15: type15[7:0]
// byte16: onewire[15:8]
// byte17: onewire[7:0]
// byte18: analog[15:8]
// byte19: analog[7:0]
// byte20: digital[31:24]
// byte21: digital[23:16]
// byte22: digital[15:8]
// byte23: digital[7:0]

// Response: n.a.

// CMD_GET_WS_TIMEOUT
// -------------------
// Note: Get weather sensor RX timeout in seconds
// Port: CMD_GET_WS_TIMEOUT
#define CMD_GET_WS_TIMEOUT              0xC0

// Downlink (command)
// byte0: 0x00

// Uplink (response):
// byte0: ws_timeout[ 7: 0]

// CMD_SET_WS_TIMEOUT
// -------------------
// Note: Set weather sensor RX timeout in seconds
// Port: CMD_SET_WS_TIMEOUT
#define CMD_SET_WS_TIMEOUT 0xC1

// Downlink (command):
// byte0: ws_timeout[ 7: 0]

// Uplink: n.a.

// CMD_RESET_WS_POSTPROC
// ----------------------
// Port: CMD_RESET_WS_POSTPROC
#define CMD_RESET_WS_POSTPROC 0xC3

// Downlink (command):
// byte0: flags[ 7: 0]

// Uplink: n.a.

// CMD_SCAN_SENSORS
// -----------------
// Note: Scan for 868 MHz sensors
// Port: CMD_SCAN_SENSORS
#define CMD_SCAN_SENSORS 0xC4

// Downlink (command):
// byte0: ws_scantime[ 7: 0]

// Uplink (response):
// byte0: id0[31:24]
// byte1: id0[23:16]
// byte2: id0[15: 8]
// byte3: id0[ 7: 0]
// byte4: decoder0[3:0] << 4 | type0[3:0]
// byte5: ch0[7:0]
// byte6: data_flags0[7:0]
// byte7: rssi0[7:0]
// ...

// CMD_GET_SENSORS_INC
// --------------------
// Note: Get sensors include list (0...12 IDs)
// Port: CMD_GET_SENSORS_INC
#define CMD_GET_SENSORS_INC 0xC6

// Downlink (command):
// byte0: 0x00

// Uplink (response): 
// byte0: sensors_inc0[31:24]
// byte1: sensors_inc0[23:16]
// byte2: sensors_inc0[15: 8]
// byte3: sensors_inc0[ 7: 0]
// ...

// CMD_SET_SENSORS_INC
// --------------------
// Note: Set sensors include list (0...12 IDs)
// Port: CMD_SET_SENSORS_INC
#define CMD_SET_SENSORS_INC 0xC7

// Downlink (command):
// byte0: sensors_inc0[31:24]
// byte1: sensors_inc0[23:16]
// byte2: sensors_inc0[15: 8]
// byte3: sensors_inc0[ 7: 0]
// ...

// Uplink: n.a.

// CMD_GET_SENSORS_EXC
// --------------------
// Note: Get sensors exclude list (0...12 * 4 bytes)
// Port: CMD_GET_SENSORS_EXC
#define CMD_GET_SENSORS_EXC 0xC8

// Downlink (command):
// byte0: 0x00

// Uplink (response): 
// byte0: sensors_exc0[31:24]
// byte1: sensors_exc0[23:16]
// byte2: sensors_exc0[15: 8]
// byte3: sensors_exc0[ 7: 0]
// ...

// CMD_SET_SENSORS_EXC
// --------------------
// Note: Set sensors exclude list (0...12 * 4 bytes)
// Port: CMD_SET_SENSORS_EXC
#define CMD_SET_SENSORS_EXC 0xC9

// Downlink (command):
// byte0: sensors_exc0[31:24]
// byte1: sensors_exc0[23:16]
// byte2: sensors_exc0[15: 8]
// byte3: sensors_exc0[ 7: 0]
// ...

// Uplink: n.a.

// CMD_GET_SENSORS_CFG
// --------------------
// Port: CMD_GET_SENSORS_CFG
#define CMD_GET_SENSORS_CFG 0xCA

// Downlink (command):
// byte0: 0x00

// Uplink (response): 
// byte0: max_sensors
// byte1: rx_flags
// byte2: en_decoders

// CMD_SET_SENSORS_CFG
// --------------------
// Port: CMD_SET_SENSORS_CFG
#define CMD_SET_SENSORS_CFG 0xCB

// Downlink (command):
// byte0: max_sensors
// byte1: rx_flags
// byte2: en_decoders

// Uplink: n.a.

// CMD_GET_BLE_CONFIG
// -------------------
// Note: Scan time in seconds
// Port: CMD_GET_BLE_CONFIG
#define CMD_GET_BLE_CONFIG 0xD0

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: 0x01 (active scan) / 0x00 (passive scan)
// byte1: scan_time[7:0]

// CMD_SET_BLE_CONFIG
// -------------------
// Note: Scan time in seconds
// Port: CMD_SET_BLE_CONFIG
#define CMD_SET_BLE_CONFIG 0xD1

// Uplink (command):
// byte0: active_scan - 0x01 (active scan) / 0x00 (passive scan)
// byte1: scan_time[7:0]

// Response: n.a.

// CMD_GET_BLE_ADDR
// -----------------
// Note: Get BLE sensors MAC addresses (0..8 * 6 bytes)
// Port: CMD_GET_BLE_ADDR
#define CMD_GET_BLE_ADDR 0xD2

// Downlink (command):
// byte0: 0x00

// Uplink (response):
// byte0: ble_addr0[47:24]
// byte1: ble_addr0[23:32]
// byte2: ble_addr0[31:24]
// byte3: ble_addr0[23:16]
// byte4: ble_addr0[15: 8]
// byte5: ble_addr0[ 7: 0]
// ...

// CMD_SET_BLE_ADDR
// -----------------
// Note: Set BLE sensors MAC addresses (0..8 * 6 bytes)
// Port: CMD_SET_BLE_ADDR
#define CMD_SET_BLE_ADDR 0xD3

// Downlink (command):
// byte0: ble_addr0[47:24]
// byte1: ble_addr0[23:32]
// byte2: ble_addr0[31:24]
// byte3: ble_addr0[23:16]
// byte4: ble_addr0[15: 8]
// byte5: ble_addr0[ 7: 0]
// ...

// Response: n.a.

// ===========================

/*!
 * \brief Decode downlink
 *
 * \param port      downlink message port
 * \param payload   downlink message payload
 * \param size      downlink message size in bytes
 *
 * \returns command ID, if downlink message requests a response, otherwise 0
 */
uint8_t decodeDownlink(uint8_t port, uint8_t *payload, size_t size);

/*!
 * \brief Send configuration uplink
 *
 * \param uplinkRequest command ID of uplink request
 */
void sendCfgUplink(uint8_t uplinkReq, uint32_t uplinkInterval);

#endif