
///////////////////////////////////////////////////////////////////////////////
// uplink_formatter.js
// 
// Bresser 868 MHz Weather Sensor Radio Receiver 
// based on ESP32 and SX1262/SX1276 - 
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// This script allows to decode payload received from The Things Network -
// data (sent at fixed intervals) and responses (sent upon request) -
// from bytes to JSON.
//
// Commands:
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
// <ws_timeout>         : 0...255
// <sleep_interval>     : 0...65535
// <sleep_interval>     : 0...65535
// <epoch>              : unix epoch time, see https://www.epochconverter.com/ (<integer> / "0x....")
// <reset_flags>        : 0...15 (1: hourly / 2: daily / 4: weekly / 8: monthly) / "0x0"..."0xF"
// <rtc_source>         : 0x00: GPS / 0x01: RTC / 0x02: LORA / 0x03: unsynched / 0x04: set (source unknown)
// <sensors_incN>       : e.g. "0xDEADBEEF"
// <sensors_excN>       : e.g. "0xDEADBEEF"
// <ble_addrN>          : e.g. "DE:AD:BE:EF:12:23"

// Based on:
// ---------
// ttn_decoder_fp.js
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
//          renamed from ttn_uplink_formatter.js
//
// ToDo:
// -  
//
///////////////////////////////////////////////////////////////////////////////

function decoder(bytes, port) {
    // bytes is of type Buffer

    const CMD_GET_DATETIME = 0x86;
    const CMD_GET_LW_CONFIG = 0xB1;
    const CMD_GET_WS_TIMEOUT = 0xC0;
    const CMD_GET_SENSORS_INC = 0xC4;
    const CMD_GET_SENSORS_EXC = 0xC6;
    const CMD_GET_BLE_ADDR = 0xC8;

    const ONEWIRE_EN = 0;

    var rtc_source_code = {
        0x00: "GPS",
        0x01: "RTC",
        0x02: "LORA",
        0x03: "unsynched",
        0x04: "set (source unknown)"
    };

    var rtc_source = function (bytes) {
        if (bytes.length !== rtc_source.BYTES) {
            throw new Error('rtc_source must have exactly 1 byte');
        }
        return rtc_source_code[bytes[0]];
    };
    rtc_source.BYTES = 1;

    var bytesToInt = function (bytes) {
        var i = 0;
        for (var x = 0; x < bytes.length; x++) {
            i |= +(bytes[x] << (x * 8));
        }
        return i;
    };

    // Big Endian
    var bytesToIntBE = function (bytes) {
        var i = 0;
        for (var x = 0; x < bytes.length; x++) {
            i |= +(bytes[x] << ((bytes.length - 1 - x) * 8));
        }
        return i;
    };

    var unixtime = function (bytes) {
        if (bytes.length !== unixtime.BYTES) {
            throw new Error('Unix time must have exactly 4 bytes');
        }
        return bytesToInt(bytes);
    };
    unixtime.BYTES = 4;

    var uint8 = function (bytes) {
        if (bytes.length !== uint8.BYTES) {
            throw new Error('int must have exactly 1 byte');
        }
        return bytesToInt(bytes);
    };
    uint8.BYTES = 1;

    var uint16 = function (bytes) {
        if (bytes.length !== uint16.BYTES) {
            throw new Error('int must have exactly 2 bytes');
        }
        return bytesToInt(bytes);
    };
    uint16.BYTES = 2;

    var uint16fp1 = function (bytes) {
        if (bytes.length !== uint16.BYTES) {
            throw new Error('int must have exactly 2 bytes');
        }
        var res = bytesToInt(bytes) * 0.1;
        return res.toFixed(1);
    };
    uint16fp1.BYTES = 2;

    var uint32 = function (bytes) {
        if (bytes.length !== uint32.BYTES) {
            throw new Error('int must have exactly 4 bytes');
        }
        return bytesToInt(bytes);
    };
    uint32.BYTES = 4;

    var uint16BE = function (bytes) {
        if (bytes.length !== uint16BE.BYTES) {
            throw new Error('int must have exactly 2 bytes');
        }
        return bytesToIntBE(bytes);
    };
    uint16BE.BYTES = 2;

    var uint32BE = function (bytes) {
        if (bytes.length !== uint32BE.BYTES) {
            throw new Error('int must have exactly 4 bytes');
        }
        return bytesToIntBE(bytes);
    };
    uint32BE.BYTES = 4;

    var mac48 = function (bytes) {
        var res = [];
        var size = bytes.length;
        var j = 0;
        for (var i = 0; i < bytes.length; i += 6) {
            res[j++] = bytes[i].toString(16) + ":" + bytes[i + 1].toString(16) + ":" + bytes[i + 2].toString(16) + ":" +
                bytes[i + 3].toString(16) + ":" + bytes[i + 4].toString(16) + ":" + bytes[i + 5].toString(16);
        }
        return res;
    }
    mac48.BYTES = bytes.length;

    var id32 = function (bytes) {
        var res = [];
        var size = bytes.length;
        var j = 0;
        for (var i = 0; i < bytes.length; i += 4) {
            res[j++] = "0x" + bytes[i].toString(16) + bytes[i + 1].toString(16) + bytes[i + 2].toString(16) +
                bytes[i + 3].toString(16);
        }
        return res;
    }
    id32.BYTES = bytes.length;

    var latLng = function (bytes) {
        if (bytes.length !== latLng.BYTES) {
            throw new Error('Lat/Long must have exactly 8 bytes');
        }

        var lat = bytesToInt(bytes.slice(0, latLng.BYTES / 2));
        var lng = bytesToInt(bytes.slice(latLng.BYTES / 2, latLng.BYTES));

        return [lat / 1e6, lng / 1e6];
    };
    latLng.BYTES = 8;

    var temperature = function (bytes) {
        if (bytes.length !== temperature.BYTES) {
            throw new Error('Temperature must have exactly 2 bytes');
        }
        var isNegative = bytes[0] & 0x80;
        var b = ('00000000' + Number(bytes[0]).toString(2)).slice(-8)
            + ('00000000' + Number(bytes[1]).toString(2)).slice(-8);
        if (isNegative) {
            var arr = b.split('').map(function (x) { return !Number(x); });
            for (var i = arr.length - 1; i > 0; i--) {
                arr[i] = !arr[i];
                if (arr[i]) {
                    break;
                }
            }
            b = arr.map(Number).join('');
        }
        var t = parseInt(b, 2);
        if (isNegative) {
            t = -t;
        }
        t = t / 1e2;
        return t.toFixed(1);
    };
    temperature.BYTES = 2;

    var humidity = function (bytes) {
        if (bytes.length !== humidity.BYTES) {
            throw new Error('Humidity must have exactly 2 bytes');
        }

        var h = bytesToInt(bytes);
        return h / 1e2;
    };
    humidity.BYTES = 2;

    // Based on https://stackoverflow.com/a/37471538 by Ilya Bursov
    // quoted by Arjan here https://www.thethingsnetwork.org/forum/t/decode-float-sent-by-lopy-as-node/8757
    function rawfloat(bytes) {
        if (bytes.length !== rawfloat.BYTES) {
            throw new Error('Float must have exactly 4 bytes');
        }
        // JavaScript bitwise operators yield a 32 bits integer, not a float.
        // Assume LSB (least significant byte first).
        var bits = bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0];
        var sign = (bits >>> 31 === 0) ? 1.0 : -1.0;
        var e = bits >>> 23 & 0xff;
        var m = (e === 0) ? (bits & 0x7fffff) << 1 : (bits & 0x7fffff) | 0x800000;
        var f = sign * m * Math.pow(2, e - 150);
        return f.toFixed(1);
    }
    rawfloat.BYTES = 4;

    var bitmap_node = function (byte) {
        if (byte.length !== bitmap_node.BYTES) {
            throw new Error('Bitmap must have exactly 1 byte');
        }
        var i = bytesToInt(byte);
        var bm = ('00000000' + Number(i).toString(2)).substr(-8).split('').map(Number).map(Boolean);

        return ['res7', 'res6', 'res5', 'res4', 'res3', 'res2', 'res1', 'res0']
            .reduce(function (obj, pos, index) {
                obj[pos] = bm[index];
                return obj;
            }, {});
    };
    bitmap_node.BYTES = 1;

    var bitmap_sensors = function (byte) {
        if (byte.length !== bitmap_sensors.BYTES) {
            throw new Error('Bitmap must have exactly 1 byte');
        }
        var i = bytesToInt(byte);
        var bm = ('00000000' + Number(i).toString(2)).substr(-8).split('').map(Number).map(Boolean);
        // Only Weather Sensor
        //return ['res5', 'res4', 'res3', 'res2', 'res1', 'res0', 'dec_ok', 'batt_ok']
        // Weather Sensor + MiThermo (BLE) Sensor
        //return ['res4', 'res3', 'res2', 'res1', 'res0', 'ble_ok', 'dec_ok', 'batt_ok']
        // Weather Sensor, Soil Sensor and MiThermo (BLE) Sensor
        return ['res0', 'ble_ok', 'ls_dec_ok', 'ls_batt_ok', 's1_dec_ok', 's1_batt_ok', 'ws_dec_ok', 'ws_batt_ok']
            .reduce(function (obj, pos, index) {
                obj[pos] = bm[index];
                return obj;
            }, {});
    };
    bitmap_sensors.BYTES = 1;

    var decode = function (bytes, mask, names) {

        var maskLength = mask.reduce(function (prev, cur) {
            return prev + cur.BYTES;
        }, 0);
        if (bytes.length < maskLength) {
            throw new Error('Mask length is ' + maskLength + ' whereas input is ' + bytes.length);
        }

        names = names || [];
        var offset = 0;
        return mask
            .map(function (decodeFn) {
                var current = bytes.slice(offset, offset += decodeFn.BYTES);
                return decodeFn(current);
            })
            .reduce(function (prev, cur, idx) {
                prev[names[idx] || idx] = cur;
                return prev;
            }, {});
    };

    if (typeof module === 'object' && typeof module.exports !== 'undefined') {
        module.exports = {
            unixtime: unixtime,
            uint8: uint8,
            uint16: uint16,
            uint32: uint32,
            uint16BE: uint16BE,
            uint32BE: uint32BE,
            mac48: mac48,
            temperature: temperature,
            humidity: humidity,
            latLng: latLng,
            bitmap_node: bitmap_node,
            bitmap_sensors: bitmap_sensors,
            rawfloat: rawfloat,
            uint16fp1: uint16fp1,
            rtc_source: rtc_source,
            decode: decode
        };
    }


    if (port === 1) {
        if (ONEWIRE_EN) {
            return decode(
                bytes,
                [bitmap_node, bitmap_sensors, temperature, uint8,
                    uint16fp1, uint16fp1, uint16fp1,
                    rawfloat, uint16, temperature,
                    temperature, uint8, temperature, uint8,
                    rawfloat, rawfloat, rawfloat, rawfloat,
                    unixtime, uint16, uint8
                ],
                ['status_node', 'status', 'air_temp_c', 'humidity',
                    'wind_gust_meter_sec', 'wind_avg_meter_sec', 'wind_direction_deg',
                    'rain_mm', 'supply_v', 'water_temp_c',
                    'indoor_temp_c', 'indoor_humidity', 'soil_temp_c', 'soil_moisture',
                    'rain_hr', 'rain_day', 'rain_week', 'rain_mon',
                    'lightning_time', 'lightning_events', 'lightning_distance_km'
                ]
            );
        } else {
            return decode(
                bytes,
                [bitmap_node, bitmap_sensors, temperature, uint8,
                    uint16fp1, uint16fp1, uint16fp1,
                    rawfloat, uint16,
                    temperature, uint8, temperature, uint8,
                    rawfloat, rawfloat, rawfloat, rawfloat,
                    unixtime, uint16, uint8
                ],
                ['status_node', 'status', 'air_temp_c', 'humidity',
                    'wind_gust_meter_sec', 'wind_avg_meter_sec', 'wind_direction_deg',
                    'rain_mm', 'supply_v',
                    'indoor_temp_c', 'indoor_humidity', 'soil_temp_c', 'soil_moisture',
                    'rain_hr', 'rain_day', 'rain_week', 'rain_mon',
                    'lightning_time', 'lightning_events', 'lightning_distance_km'
                ]
            );
        }

    } else if (port === CMD_GET_DATETIME) {
        return decode(
            bytes,
            [uint32BE, rtc_source
            ],
            ['unixtime', 'rtc_source'
            ]
        );
    } else if (port === CMD_GET_LW_CONFIG) {
        return decode(
            bytes,
            [uint16BE, uint16BE
            ],
            ['sleep_interval', 'sleep_interval_long'
            ]
        );
    } else if (port === CMD_GET_WS_TIMEOUT) {
        return decode(
            bytes,
            [uint8
            ],
            ['ws_timeout'
            ]
        );
    } else if (port === CMD_GET_SENSORS_INC) {
        return decode(
            bytes,
            [id32
            ],
            ['sensors_inc'
            ]
        );
    } else if (port === CMD_GET_SENSORS_EXC) {
        return decode(
            bytes,
            [id32
            ],
            ['sensors_exc'
            ]
        );
    } else if (port === CMD_GET_BLE_ADDR) {
        return decode(
            bytes,
            [mac48
            ],
            ['ble_addr'
            ]
        );
    }

}


function decodeUplink(input) {
    return {
        data: {
            bytes: decoder(input.bytes, input.fPort)
        },
        warnings: [],
        errors: []
    };
}