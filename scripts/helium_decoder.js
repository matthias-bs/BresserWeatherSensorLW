///////////////////////////////////////////////////////////////////////////////
// helium_decoder.js
// 
// Bresser 868 MHz Weather Sensor Radio Receiver 
// based on ESP32 and SX1262/SX1276/LR1121 -
// sends data to a LoRaWAN network (e.g. The Things Network)
//
// This script allows to decode payload received from a LoRaWAN device -
// data (sent at fixed intervals) and responses (sent upon request) -
// from bytes to JSON.
//
// Commands:
// ----------
// (see constants for ports below)
// port = CMD_SET_WS_TIMEOUT, {"ws_timeout": <timeout_in_seconds>}
// port = CMD_SET_SLEEP_INTERVAL, {"sleep_interval": <interval_in_seconds>}
// port = CMD_SET_SLEEP_INTERVAL_LONG, {"sleep_interval_long": <interval_in_seconds>}
// port = CMD_SET_LW_STATUS_INTERVAL, {"lw_status_interval": <interval_in_frames>}
// port = CMD_GET_DATETIME, {"cmd": "CMD_GET_DATETIME"} / payload = 0x00
// port = CMD_SET_DATETIME, {"epoch": <epoch>}
// port = CMD_RESET_WS_POSTPROC, {"reset_flags": <flags>}
// port = CMD_GET_WS_POSTPROC, {"cmd": "CMD_GET_WS_POSTPROC"} / payload = 0x00
// port = CMD_SET_WS_POSTPROC, {"update_interval": <update_interval>}
// port = CMD_GET_LW_CONFIG, {"cmd": "CMD_GET_LW_CONFIG"} / payload = 0x00
// port = CMD_GET_WS_TIMEOUT, {"cmd": "CMD_GET_WS_TIMEOUT" / payload = 0x00
// port = CMD_SET_WS_TIMEOUT, {"ws_timeout": <ws_timeout>}
// port = CMD_GET_APP_STATUS_INTERVAL, {"cmd": "CMD_GET_APP_STATUS_INTERVAL"} / payload = 0x00
// port = CMD_SET_APP_STATUS_INTERVAL, {"app_status_interval": <app_status_interval>}
// port = CMD_GET_SENSORS_STAT, {"cmd": "CMD_GET_SENSORS_STAT"} / payload = 0x00
// port = CMD_GET_SENSORS_INC, {"cmd": "CMD_GET_SENSORS_INC"} / payload = 0x00
// port = CMD_SET_SENSORS_INC, {"sensors_inc": [<sensors_inc0>, ..., <sensors_incN>]}
// port = CMD_GET_SENSORS_EXC, {"cmd": "CMD_GET_SENSORS_EXC"} / payload = 0x00
// port = CMD_SET_SENSORS_EXC, {"sensors_exc": [<sensors_exc0>, ..., <sensors_excN>]}
// port = CMD_GET_BLE_ADDR, {"cmd": "CMD_GET_BLE_ADDR"} / payload = 0x00
// port = CMD_SET_BLE_ADDR, {"ble_addr": [<ble_addr0>, ..., <ble_addrN>]}
// port = CMD_GET_BLE_CONFIG, {"cmd": "CMD_GET_BLE_CONFIG"} / payload = 0x00
// port = CMD_SET_BLE_CONFIG, {"ble_active": <ble_active>, "ble_scantime": <ble_scantime>}
// port = CMD_GET_APP_PAYLOAD_CFG, {"cmd": "CMD_GET_APP_PAYLOAD_CFG"} / payload = 0x00
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
// CMD_GET_WS_POSTPROC {"update_interval": <update_interval>}
//
// CMD_GET_APP_STATUS_INTERVAL {"app_status_interval": <app_status_interval>}
//
// CMD_GET_SENSORS_STAT {"sensor_status": {bresser: [<bresser_stat0>, ..., <bresser_stat15>], "ble_stat": <ble_stat>}}
//
// CMD_GET_SENSORS_INC {"sensors_inc": [<sensors_inc0>, ...]}
//
// CMD_GET_SENSORS_EXC {"sensors_exc"}: [<sensors_exc0>, ...]}
//
// CMD_GET_SENSORS_CFG {"max_sensors": <max_sensors>, "rx_flags": <rx_flags>, "en_decoders": <en_decoders>}
//
// CMD_GET_BLE_ADDR {"ble_addr": [<ble_addr0>, ...]}
//
// CMD_GET_BLE_CONFIG {"ble_active": <ble_active>, "ble_scantime": <ble_scantime>}
//
// CMD_GET_APP_PAYLOAD_CFG {"bresser": [<type0>, <type1>, ..., <type15>], "onewire": <onewire>, "analog": <analog>, "digital": <digital>}
//
// <ws_timeout>         : 0...255
// <sleep_interval>     : 0...65535
// <sleep_interval_long>: 0...65535
// <lw_status_interval> : LoRaWAN node status message uplink interval in no. of frames (0...255, 0: disabled)
// <epoch>              : unix epoch time, see https://www.epochconverter.com/ (<integer> / "0x....")
// <reset_flags>        : 0...15 (1: hourly / 2: daily / 4: weekly / 8: monthly) / "0x0"..."0xF"
// <update_interval>    : Rain gauge / lightning counter post processing interval in minutes (1...255, 0: auto)
// <rtc_source>         : 0x00: GPS / 0x01: RTC / 0x02: LORA / 0x03: unsynched / 0x04: set (source unknown)
// <app_status_interval>: Sensor status message uplink interval in no. of frames (0...255, 0: disabled)
// <sensors_incN>       : e.g. "0xDEADBEEF"
// <sensors_excN>       : e.g. "0xDEADBEEF"
// <max_sensors>        : max. number of Bresser sensors per receive cycle; 1...8
// <rx_flags>           : Flags for getData(); see BresserWeatherSensorReceiver
// <en_decoders>        : Enabled decoders; see BresserWeatherSensorReceiver
// <ble_active>         : BLE scan mode - 0: passive / 1: active
// <ble_scantime>       : BLE scan time in seconds (0...255)
// <ble_addrN>          : e.g. "DE:AD:BE:EF:12:23"
// <typeN>              : Bitmap for enabling Bresser sensors of type N; each bit position corresponds to a channel, e.g. bit 0 controls ch0; 
//                        unused bits can be used to select features
// <onewire>            : Bitmap for enabling 1-Wire sensors; each bit position corresponds to an index
// <analog>             : Bitmap for enabling analog input channels; each bit positions corresponds to a channel
// <digital>            : Bitmap for enabling digital input channels in a broad sense &mdash; GPIO, SPI, I2C, UART, ...
//
// created: 06/2025
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
// 20250624 Created from datacake_decoder.js
// 20250828 Added CMD_GET_WS_POSTPROC/CMD_SET_WS_POSTPROC
//
// ToDo:
// -  
//
///////////////////////////////////////////////////////////////////////////////

// Commands
var CMD_GET_DATETIME = 0x20;
var CMD_SET_DATETIME = 0x21;
var CMD_SET_SLEEP_INTERVAL = 0x31;
var CMD_SET_SLEEP_INTERVAL_LONG = 0x33;
var CMD_SET_LW_STATUS_INTERVAL = 0x35;
var CMD_GET_LW_CONFIG = 0x36;
var CMD_GET_LW_STATUS = 0x38;
var CMD_GET_APP_STATUS_INTERVAL = 0x40;
var CMD_SET_APP_STATUS_INTERVAL = 0x41;
var CMD_GET_SENSORS_STAT = 0x42;
var CMD_GET_APP_PAYLOAD_CFG = 0x46;
var CMD_SET_APP_PAYLOAD_CFG = 0x47;
var CMD_GET_WS_TIMEOUT = 0xC0;
var CMD_SET_WS_TIMEOUT = 0xC1;
var CMD_RESET_WS_POSTPROC = 0xC3;
var CMD_GET_WS_POSTPROC = 0xCC;
var CMD_SET_WS_POSTPROC = 0xCD;
var CMD_SCAN_SENSORS = 0xC4;
var CMD_GET_SENSORS_INC = 0xC6;
var CMD_SET_SENSORS_INC = 0xC7;
var CMD_GET_SENSORS_EXC = 0xC8;
var CMD_SET_SENSORS_EXC = 0xC9;
var CMD_GET_SENSORS_CFG = 0xCA;
var CMD_SET_SENSORS_CFG = 0xCB;
var CMD_GET_BLE_CONFIG = 0xD0;
var CMD_SET_BLE_CONFIG = 0xD1;
var CMD_GET_BLE_ADDR = 0xD2;
var CMD_SET_BLE_ADDR = 0xD3;

// Bresser Weather Sensor Decoder
function bws_decoder(bytes, port) {
    var bytesToInt = function (bytes) {
        var i = 0;
        for (var x = 0; x < bytes.length; x++) {
            i |= +(bytes[x] << (x * 8));
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

    function byte2hex(byte) {
        return ('0' + byte.toString(16)).slice(-2);
    }

    var mac48 = function (bytes) {
        var res = [];
        var j = 0;
        for (var i = 0; i < bytes.length; i += 6) {
            res[j++] = byte2hex(bytes[i]) + ":" + byte2hex(bytes[i + 1]) + ":" + byte2hex(bytes[i + 2]) + ":" +
                byte2hex(bytes[i + 3]) + ":" + byte2hex(bytes[i + 4]) + ":" + byte2hex(bytes[i + 5]);
        }
        return res;
    };
    mac48.BYTES = 6;

    var bresser_bitmaps = function (bytes) {
        var res = [];
        for (var i = 0; i < 16; i++) {
            res[i] = "0x" + byte2hex(bytes[i]);
        }
        return res;
    };
    bresser_bitmaps.BYTES = 16;

    var hex16 = function (bytes) {
        var res = "0x" + byte2hex(bytes[0]) + byte2hex(bytes[1]);
        return res;
    };
    hex16.BYTES = 2;

    var hex32 = function (bytes) {
        var res = "0x" + byte2hex(bytes[0]) + byte2hex(bytes[1]) + byte2hex(bytes[2]) + byte2hex(bytes[3]);
        return res;
    };
    hex32.BYTES = 4;

    var id32 = function (bytes) {
        var res = [];
        var j = 0;
        for (var i = 0; i < bytes.length; i += 4) {
            res[j++] = "0x" + byte2hex(bytes[i]) + byte2hex(bytes[i + 1]) + byte2hex(bytes[i + 2]) + byte2hex(bytes[i + 3]);
        }
        return res;
    };
    id32.BYTES = 4;

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


    if (port === 1) {
        // see assignment to 'bitmap' variable for status bit names 
        //    return decode(
        //    bytes,
        //    [bitmap,   temperature,  uint8,       uint16,       uint16      ], // types
        //    ['status', 'air_temp_c', 'humidity',  'supply_v',   'battery_v' ]  // JSON elements
        //  );
        return decode(
            bytes,
            [ 
            temperature,
            uint8,
            rawfloat,
            uint16fp1,
            uint16fp1,
            uint16fp1,
            rawfloat,
            rawfloat,
            rawfloat,
            rawfloat,
            //temperature,
            //uint8,
            temperature,
            uint8,
            //unixtime,
            //uint16,
            //uint8,
            temperature,
            uint16,
            temperature,
            uint8
            ],
            [ 
            'ws_temp_c',
            'ws_humidity',
            'ws_rain_mm',
            'ws_wind_gust_ms',
            'ws_wind_avg_ms',
            'ws_wind_dir_deg',
            'ws_rain_hourly_mm',
            'ws_rain_daily_mm',
            'ws_rain_weekly_mm',
            'ws_rain_monthly_mm',
            //'th1_temp_c',
            //'th1_humidity',
            'soil1_temp_c',
            'soil1_moisture',
            //'lgt_ev_time',
            //'lgt_ev_events',
            //'lgt_ev_dist_km',
            'ow0_temp_c',
            'a0_voltage_mv',
            'ble0_temp_c',
            'ble0_humidity'
            ]
        );
    } else if (port === CMD_GET_DATETIME) {
        return decode(
            port,
            bytes,
            [uint32BE, rtc_source
            ],
            ['unixtime', 'rtc_source'
            ]
        );
    } else if (port === CMD_GET_LW_CONFIG) {
        return decode(
            port,
            bytes,
            [uint16BE, uint16BE
            ],
            ['sleep_interval', 'sleep_interval_long'
            ]
        );
    } else if (port === CMD_GET_LW_STATUS) {
        return decode(
            port,
            bytes,
            [uint16, uint8
            ],
            ['ubatt_mv', 'long_sleep'
            ]
        );
    } else if (port === CMD_GET_WS_TIMEOUT) {
        return decode(
            port,
            bytes,
            [uint8
            ],
            ['ws_timeout'
            ]
        );
    } else if (port === CMD_GET_SENSORS_INC) {
        return decode(
            port,
            bytes,
            [id32
            ],
            ['sensors_inc'
            ]
        );
    } else if (port === CMD_GET_SENSORS_EXC) {
        return decode(
            port,
            bytes,
            [id32
            ],
            ['sensors_exc'
            ]
        );
    } else if (port === CMD_GET_SENSORS_CFG) {
        return decode(
            port,
            bytes,
            [bits8, bits8, bits8
            ],
            ['max_sensors', 'rx_flags', 'en_decoders'
            ]
        );
    } else if (port === CMD_GET_BLE_ADDR) {
        return decode(
            port,
            bytes,
            [mac48
            ],
            ['ble_addr'
            ]
        );
    } else if (port === CMD_GET_BLE_CONFIG) {
        return decode(
            port,
            bytes,
            [bits8, bits8
            ],
            ['ble_active', 'ble_scantime']
        );
    } else if (port === CMD_GET_APP_PAYLOAD_CFG) {
        return decode(
            port,
            bytes,
            [bresser_bitmaps, hex16, hex16, hex32
            ],
            ['bresser', 'onewire', 'analog', 'digital']
        );
    } else if (port === CMD_GET_STATUS_INTERVAL) {
        return decode(
            port,
            bytes,
            [bits8
            ],
            ['status_interval'
            ]
        );
    } else if (port === CMD_GET_SENSORS_STAT) {
        return decode(
            port,
            bytes,
            [sensor_status
            ],
            ['sensor_status'
            ]
        );
    } else if (port === CMD_GET_WS_POSTPROC) {
        return decode(
            port,
            bytes,
            [bits8
            ],
            ['update_interval'
            ]
        );
    } 
}


function Decoder(bytes, port, uplink_info) {
    var decoded = {};

    if (port != 1) {
      return {};
    }

    decoded = bws_decoder(bytes, port);

    /*
      The uplink_info variable is an OPTIONAL third parameter that provides the following:
    
      uplink_info = {
        type: "join",
        uuid: <UUIDv4>,
        id: <device id>,
        name: <device name>,
        dev_eui: <dev_eui>,
        app_eui: <app_eui>,
        metadata: {...},
        fcnt: <integer>,
        reported_at: <timestamp>,
        port: <integer>,
        devaddr: <devaddr>,
        hotspots: {...},
        hold_time: <integer>
      }
    */

    if (uplink_info) {
        // do something with uplink_info fields
    }

    return {
      decoded
    };
}
