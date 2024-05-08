///////////////////////////////////////////////////////////////////////////////
// downlink_formatter.js
// 
// Bresser 868 MHz Weather Sensor Radio Receiver 
// based on ESP32 and SX1262/SX1276 - 
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// This script allows to send downlink data to the The Things Network using
// parameters encoded in JSON format.
// The FPort must be set to the command.
//
/// Commands:
// ----------
// (see constants for ports below)
// port = CMD_SET_WS_TIMEOUT, {"timeout": <timeout_in_seconds>}
// port = CMD_SET_SLEEP_INTERVAL, {"sleep_interval": <interval_in_seconds>}
// port = CMD_SET_SLEEP_INTERVAL_LONG, {"sleep_interval_long": <interval_in_seconds>}
// port = CMD_GET_DATETIME, {"cmd": "CMD_GET_DATETIME"} / payload = 0x00
// port = CMD_SET_DATETIME, {"epoch": <epoch>}
// port = CMD_RESET_RAINGAUGE, {"reset_flags": <flags>}
// port = CMD_GET_LW_CONFIG, {"cmd": "CMD_GET_LW_CONFIG"} / payload = 0x00
// port = CMD_GET_WS_TIMEOUT, {"cmd": "CMD_GET_WS_TIMEOUT" / payload = 0x00
// port = CMD_SET_WS_TIMEOUT, {"ws_timeout": <ws_timeout>}
// port = CMD_GET_SENSORS_INC, {"cmd": "CMD_GET_SENSORS_INC"} / payload = 0x00
// port = CMD_SET_SENSORS_INC, {"sensors_inc": [<sensors_inc0>, ..., <sensors_incN>]}
// port = CMD_GET_SENSORS_EXC, {"cmd": "CMD_GET_SENSORS_EXC"} / payload = 0x00
// port = CMD_SET_SENSORS_EXC, {"sensors_exc": [<sensors_exc0>, ..., <sensors_excN>]}
// port = CMD_GET_BLE_ADDR, {"cmd": "CMD_GET_BLE_ADDR"} / payload = 0x00
// port = CMD_SET_BLE_ADDR, {"ble_addr": [<ble_addr0>, ..., <ble_addrN>]}
// port = CMD_GET_BLE_CONFIG, {"cmd": "CMD_GET_BLE_CONFIG"} / payload = 0x00
// port = CMD_SET_BLE_CONFIG, {"ble_active": <ble_active>, "ble_scantime": <ble_scantime>}
//
// Responses:
// -----------
// (The response uses the same port as the request.)
// CMD_GET_LW_CONFIG {"sleep_interval": <sleep_interval>,
//                    "sleep_interval_long": <sleep_interval_long>}
// 
// CMD_GET_DATETIME {"epoch": <unix_epoch_time>, "rtc_source": <rtc_source>}
//
// CMD_GET_WS_TIMEOUT {"ws_timeout": <ws_timeout>}
//
// CMD_GET_SENSORS_INC {"sensors_inc": [<sensors_inc0>, ...]}
//
// CMD_GET_SENSORS_EXC {"sensors_exc"}: [<sensors_exc0>, ...]}
//
// CMD_GET_BLE_ADDR {"ble_addr": [<ble_addr0>, ...]}
//
// CMD_GET_BLE_CONFIG {"ble_active": <ble_active>, "ble_scantime": <ble_scantime>}
//
// <ws_timeout>         : 0...255
// <sleep_interval>     : 0...65535
// <sleep_interval>     : 0...65535
// <epoch>              : unix epoch time, see https://www.epochconverter.com/ (<integer> / "0x....")
// <reset_flags>        : 0...15 (1: hourly / 2: daily / 4: weekly / 8: monthly) / "0x0"..."0xF"
// <rtc_source>         : 0x00: GPS / 0x01: RTC / 0x02: LORA / 0x03: unsynched / 0x04: set (source unknown)
// <sensors_incN>       : e.g. "0xDEADBEEF"
// <sensors_excN>       : e.g. "0xDEADBEEF"
// <ble_active>         : BLE scan mode - 0: passive / 1: active
// <ble_scantime>       : BLE scan time in seconds (0...255)
// <ble_addrN>          : e.g. "DE:AD:BE:EF:12:23"
//
//
// Based on:
// ---------
// https://www.thethingsindustries.com/docs/integrations/payload-formatters/javascript/downlink/
//
// created: 08/2023
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
// 20230821 Created
// 20240420 Updated for BresserWeatherSensorLW, 
//          renamed from downlink_formatter.js
// 20240427 Added BLE configuration
// 20240507 Added CMD_GET_SENSORS_CFG/CMD_SET_SENSORS_CFG
// 20240508 Fixed decoding of raw data
//
// ToDo:
// -  
//
///////////////////////////////////////////////////////////////////////////////

// Commands
const CMD_SET_SLEEP_INTERVAL = 0xA8;
const CMD_SET_SLEEP_INTERVAL_LONG = 0xA9;
const CMD_GET_DATETIME = 0x86;
const CMD_SET_DATETIME = 0x88;
const CMD_GET_LW_CONFIG = 0xB1;
const CMD_GET_WS_TIMEOUT = 0xC0;
const CMD_SET_WS_TIMEOUT = 0xC1;
const CMD_RESET_RAINGAUGE = 0xC3;
const CMD_GET_SENSORS_INC = 0xC4;
const CMD_SET_SENSORS_INC = 0xC5;
const CMD_GET_SENSORS_EXC = 0xC6;
const CMD_SET_SENSORS_EXC = 0xC7;
const CMD_GET_SENSORS_CFG = 0xCC;
const CMD_SET_SENSORS_CFG = 0xCD;
const CMD_GET_BLE_ADDR = 0xC8;
const CMD_SET_BLE_ADDR = 0xC9;
const CMD_GET_BLE_CONFIG = 0xCA;
const CMD_SET_BLE_CONFIG = 0xCB;


// Source of Real Time Clock setting
var rtc_source_code = {
    0x00: "GPS",
    0x01: "RTC",
    0x02: "LORA",
    0x03: "unsynched",
    0x04: "set (source unknown)"
};

function bytesToInt(bytes) {
    var i = 0;
    for (var x = 0; x < bytes.length; x++) {
        i |= +(bytes[x] << (x * 8));
    }
    return i;
}

// Big Endian
function bytesToIntBE(bytes) {
    var i = 0;
    for (var x = 0; x < bytes.length; x++) {
        i |= +(bytes[x] << ((bytes.length - 1 - x) * 8));
    }
    return i;
}

function uint8(bytes) {
    if (bytes.length !== 1) {
        throw new Error('uint8 must have exactly 1 byte');
    }
    return bytesToInt(bytes);
}

function uint16BE(bytes) {
    if (bytes.length !== 2) {
        throw new Error('uint16BE must have exactly 2 bytes');
    }
    return bytesToIntBE(bytes);
}

function uint32BE(bytes) {
    if (bytes.length !== 4) {
        throw new Error('uint32BE must have exactly 4 bytes');
    }
    return bytesToIntBE(bytes);
}

function byte2hex(byte) {
    return ('0' + byte.toString(16)).slice(-2);
}

function mac48(bytes) {
    var res = [];
    var j = 0;
    for (var i = 0; i < bytes.length; i += 6) {
        res[j++] = byte2hex(bytes[i]) + ":" + byte2hex(bytes[i + 1]) + ":" + byte2hex(bytes[i + 2]) + ":" +
            byte2hex(bytes[i + 3]) + ":" + byte2hex(bytes[i + 4]) + ":" + byte2hex(bytes[i + 5]);
    }
    return res;
}

function id32(bytes) {
    var res = [];
    var j = 0;
    for (var i = 0; i < bytes.length; i += 4) {
        res[j++] = "0x" + byte2hex(bytes[i]) + byte2hex(bytes[i + 1]) + byte2hex(bytes[i + 2]) + byte2hex(bytes[i + 3]);
    }
    return res;
}

// Encode Downlink from JSON to bytes
function encodeDownlink(input) {
    var i;
    var j;
    var k;
    var output = [];
    var value;
    if (input.data.hasOwnProperty('cmd')) {
        if (input.data.cmd == "CMD_GET_DATETIME") {
            return {
                bytes: [0],
                fPort: CMD_GET_DATETIME,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_LW_CONFIG") {
            return {
                bytes: [0],
                fPort: CMD_GET_LW_CONFIG,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_WS_TIMEOUT") {
            return {
                bytes: [0],
                fPort: CMD_GET_WS_TIMEOUT,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_SENSORS_INC") {
            return {
                bytes: [0],
                fPort: CMD_GET_SENSORS_INC,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_SENSORS_EXC") {
            return {
                bytes: [0],
                fPort: CMD_GET_SENSORS_EXC,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_SENSORS_CFG") {
            return {
                bytes: [0],
                fPort: CMD_GET_SENSORS_CFG,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_BLE_ADDR") {
            return {
                bytes: [0],
                fPort: CMD_GET_BLE_ADDR,
                warnings: [],
                errors: []
            };
        }
        else if (input.data.cmd == "CMD_GET_BLE_CONFIG") {
            return {
                bytes: [0],
                fPort: CMD_GET_BLE_CONFIG,
                warnings: [],
                errors: []
            };
        }
    }
    if (input.data.hasOwnProperty('sleep_interval')) {
        return {
            bytes: [
                input.data.sleep_interval >> 8,
                input.data.sleep_interval & 0xFF
            ],
            fPort: CMD_SET_SLEEP_INTERVAL,
            warnings: [],
            errors: []
        };
    }
    if (input.data.hasOwnProperty('sleep_interval_long')) {
        return {
            bytes: [
                input.data.sleep_interval_long >> 8,
                input.data.sleep_interval_long & 0xFF
            ],
            fPort: CMD_SET_SLEEP_INTERVAL_LONG,
            warnings: [],
            errors: []
        };
    }
    else if (input.data.hasOwnProperty('epoch')) {
        if (typeof input.data.epoch == "string") {
            if (input.data.epoch.substr(0, 2) == "0x") {
                value = parseInt(input.data.epoch.substr(2), 16);
            } else {
                return {
                    bytes: [],
                    warnings: [],
                    errors: ["Invalid hex value"]
                };
            }
        } else {
            value = input.data.epoch;
        }
        return {
            bytes: [
                value >> 24,
                (value >> 16) & 0xFF,
                (value >> 8) & 0xFF,
                (value & 0xFF)],
            fPort: CMD_SET_DATETIME,
            warnings: [],
            errors: []
        };
    }
    else if (input.data.hasOwnProperty('ws_timeout')) {
        return {
            bytes: [input.data.ws_timeout],
            fPort: CMD_SET_WS_TIMEOUT,
            warnings: [],
            errors: []
        };
    } else if (input.data.hasOwnProperty('reset_flags')) {
        if (typeof input.data.epoch == "string") {
            if (input.data.reset_flags.substr(0, 2) == "0x") {
                value = parseInt(input.data.reset_flags.substr(2), 16);
            } else {
                return {
                    bytes: [],
                    warnings: [],
                    errors: ["Invalid hex value"]
                };
            }
        } else {
            value = input.data.reset_flags;
        }
        return {
            bytes: [value],
            fPort: CMD_RESET_RAINGAUGE,
            warnings: [],
            errors: []
        };
    } else if (input.data.hasOwnProperty('sensors_inc')) {
        k = 0;
        for (i = 0; i < input.data.sensors_inc.length; i++) {
            for (j = 2; j < 10; j += 2) {
                output[k++] = parseInt(input.data.sensors_inc[i].substr(j, 2), 16);
            }
        }
        return {
            bytes: output,
            fPort: CMD_SET_SENSORS_INC,
            warnings: [],
            errors: []
        };
    } else if (input.data.hasOwnProperty('sensors_exc')) {
        k = 0;
        for (i = 0; i < input.data.sensors_exc.length; i++) {
            for (j = 2; j < 10; j += 2) {
                output[k++] = parseInt(input.data.sensors_exc[i].substr(j, 2), 16);
            }
        }
        return {
            bytes: output,
            fPort: CMD_SET_SENSORS_EXC,
            warnings: [],
            errors: []
        };
    } else if (input.data.hasOwnProperty('max_sensors') && input.data.hasOwnProperty('rx_flags')) {
        return {
            bytes: [input.data.max_sensors, input.data.rx_flags],
            fPort: CMD_SET_SENSORS_CFG,
            warnings: [],
            errors: []
        };
    } else if (input.data.hasOwnProperty('ble_addr')) {
        output = [];
        k = 0;
        for (i = 0; i < input.data.ble_addr.length; i++) {
            var tmp = input.data.ble_addr[i].replace(/([^:]*):/g, '$1');
            for (j = 0; j < 12; j += 2) {
                output[k++] = parseInt(tmp.substr(j, 2), 16);
            }
        }
        return {
            bytes: output,
            fPort: CMD_SET_BLE_ADDR,
            warnings: [],
            errors: []
        };
    }
    else if (input.data.hasOwnProperty('ble_active') && input.data.hasOwnProperty('ble_scantime')) {
        return {
            bytes: [input.data.ble_active, input.data.ble_scantime],
            fPort: CMD_SET_BLE_CONFIG,
            warnings: [],
            errors: []
        };
    }
    else {
        return {
            bytes: [],
            errors: ["unknown command"],
            fPort: 1,
            warnings: []
        };
    }
}

// Decode Downlink from bytes to JSON
function decodeDownlink(input) {
    switch (input.fPort) {
        case CMD_GET_DATETIME:
        case CMD_GET_LW_CONFIG:
        case CMD_GET_WS_TIMEOUT:
        case CMD_GET_SENSORS_INC:
        case CMD_GET_SENSORS_EXC:
        case CMD_GET_SENSORS_CFG:
        case CMD_GET_BLE_ADDR:
        case CMD_GET_BLE_CONFIG:
            return {
                data: [0],
                warnings: [],
                errors: []
            };
        case CMD_SET_DATETIME:
            return {
                data: {
                    unixtime: "0x" + uint32BE(input.bytes.slice(0, 4)).toString(16)
                }
            };
        case CMD_SET_SLEEP_INTERVAL:
            return {
                data: {
                    sleep_interval: uint16BE(input.bytes)
                }
            };
        case CMD_SET_SLEEP_INTERVAL_LONG:
            return {
                data: {
                    sleep_interval_long: uint16BE(input.bytes)
                }
            };
        case CMD_SET_WS_TIMEOUT:
            return {
                data: {
                    ws_timeout: uint8(input.bytes)
                }
            };
        case CMD_RESET_RAINGAUGE:
            return {
                data: {
                    reset_flags: "0x" + uint8(input.bytes).toString(16)
                }
            };
        case CMD_SET_SENSORS_INC:
            return {
                data: {
                    sensors_inc: id32(input.bytes)
                }
            };
        case CMD_SET_SENSORS_EXC:
            return {
                data: {
                    sensors_exc: id32(input.bytes)
                }
            };
        case CMD_SET_SENSORS_CFG:
            return {
                data: {
                    max_sensors: uint8(input.bytes.slice(0, 1)),
                    rx_flags: uint8(input.bytes.slice(1, 2))
                }
            };
        case CMD_SET_BLE_ADDR:
            return {
                data: {
                    ble_addr: mac48(input.bytes)
                }
            };
        case CMD_SET_LW_CONFIG:
            return {
                data: {
                    sleep_interval: uint16BE(input.bytes.slice(0, 2)),
                    sleep_interval_long: uint16BE(input.bytes.slice(2, 4))
                }
            };
        case CMD_SET_BLE_CONFIG:
            return {
                data: {
                    ble_active: uint8(input.bytes.slice(0, 1)),
                    ble_timeout: uint8(input.bytes.slice(1, 2))
                }
            };
        default:
            return {
                errors: ["unknown FPort"]
            };
    }
}
