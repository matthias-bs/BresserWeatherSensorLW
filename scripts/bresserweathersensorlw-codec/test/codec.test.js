///////////////////////////////////////////////////////////////////////////////
// codec.test.js
//
// Test script for bresserweathersensorlw-codec LoRaWAN Payload encoder/decoder
//
//
// created: 09/2025
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
// 20250903 Created
//
///////////////////////////////////////////////////////////////////////////////

const test = require('node:test');
const assert = require('node:assert/strict');
const codec = require('../index');

/*
 * decodeUplink()
 */
test('decode uplink returns data object', () => {
    const uplinkBytes = Buffer.from([0xFF]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xC0 });
    assert.ok(res && typeof res === 'object', 'result should be an object');
    assert.ok('data' in res, 'result should contain data');
});

// test('decodeUplink() with unknown fPort', () => {
//     const uplinkBytes = Buffer.from([0xFF]);
//     const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xFF });
//     console.log('decodeUplink:', res); // Print the entire result object
//     assert.deepEqual(res.errors, ['unknown FPort'], 'should return unknown FPort error');
//     assert.ok(Object.keys(res.data).length === 0, 'data should be empty');
//     assert.ok(res.warnings.length === 0, 'should be no warnings');
// });

test('decodeUplink() -> CMD_GET_DATETIME response (RTC)', () => {
    const uplinkBytes = Buffer.from([0x64, 0x7E, 0xD4, 0x80, 0x01]); // Example bytes for datetime response
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x20 });
    assert.deepEqual(res.data, { bytes: { unixtime: 1686033536, rtc_source: 'RTC' } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_DATETIME response (LORA)', () => {
    const uplinkBytes = Buffer.from([0x64, 0x7E, 0xD4, 0x80, 0x02]); // Example bytes for datetime response
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x20 });
    assert.deepEqual(res.data, { bytes: { unixtime: 1686033536, rtc_source: 'LORA' } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_DATETIME response (unsynched)', () => {
    const uplinkBytes = Buffer.from([0x64, 0x7E, 0xD4, 0x80, 0x03]); // Example bytes for datetime response
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x20 });
    assert.deepEqual(res.data, { bytes: { unixtime: 1686033536, rtc_source: 'unsynched' } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_DATETIME response (set)', () => {
    const uplinkBytes = Buffer.from([0x64, 0x7E, 0xD4, 0x80, 0x04]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x20 });
    assert.deepEqual(res.data, { bytes: { unixtime: 1686033536, rtc_source: 'set (source unknown)' } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_LW_CONFIG response', () => {
    const uplinkBytes = Buffer.from([0x01, 0x2C, 0x02, 0x58, 0x80]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x36 });
    assert.deepEqual(res.data, { bytes: { sleep_interval: 300, sleep_interval_long: 600, lw_status_interval: 128 } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_LW_STATUS response', () => {
    const uplinkBytes = Buffer.from([0x74, 0x0E, 0x00]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x38 });
    assert.deepEqual(res.data, { bytes: { ubatt_mv: 3700, long_sleep: 0 } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_APP_STATUS_INTERVAL response', () => {
    const uplinkBytes = Buffer.from([0x40]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x40 });
    assert.deepEqual(res.data, { bytes: { app_status_interval: 64 } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_SENSORS_STAT response', () => {
    const uplinkBytes = Buffer.from([0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x20, 0x21, 0x30, 0x31, 0x32, 0x33,
        0x40, 0x41, 0x42, 0x43, 0x50, 0x51]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x42 });
    console.log('decodeUplink CMD_GET_SENSORS_STAT:', res.data.bytes.sensor_status);
    assert.deepEqual(res.data.bytes, {
        sensor_status: {
            bresser: [
                '0x00', '0x01', '0x02', '0x03', '0x04', '0x05', '0x06', '0x07',
                '0x08', '0x09', '0x0a', '0x0b', '0x0c', '0x0d', '0x0e', '0x0f'],
            ble: '0x4041'
        }
    }, 'data should match expected value');
});

test('decodeUplink() -> CMD_GET_WS_TIMEOUT response', () => {
    const uplinkBytes = Buffer.from([0xFF]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xC0 });
    assert.deepEqual(res.data, { bytes: { ws_timeout: 255 } }, 'data should match expected value');
    assert.ok(res.data.bytes.ws_timeout === 255, 'ws_timeout should be 255');
});

test('decodeUplink() -> CMD_GET_WS_POSTPROC response', () => {
    const uplinkBytes = Buffer.from([0x06]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xCC });
    assert.deepEqual(res.data, { bytes: { update_interval: 6 } }, 'data should match expected value');
});

test('decodeUplink() -> CMD_SCAN_SENSORS response', () => {
    const uplinkBytes = Buffer.from([0xFE, 0xED, 0xBE, 0xEF, 0x12, 0x01, 0x34, 0x12, 0x55]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xC4 });
    console.log('decodeUplink CMD_SCAN_SENSORS:', res.data.bytes.found_sensors);
    assert.deepEqual(res.data.bytes.found_sensors,
        [{ id: '0xfeedbeef', decoder: '6-in-1', type: 'Thermo-/Hygro-Sensor', ch: 1, flags: '0x1234', rssi: -85 }],
        'data should match expected values')
});

test('decodeUplink() -> CMD_GET_SENSORS_INC response', () => {
    const uplinkBytes = Buffer.from([0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xC6 });
    assert.deepEqual(res.data.bytes.sensors_inc, ['0x00010203', '0x10111213'],
        'data should match expected value'
    )
});

test('decodeUplink() -> CMD_GET_SENSORS_EXC response', () => {
    const uplinkBytes = Buffer.from([0x20, 0x21, 0x22, 0x23, 0x30, 0x31, 0x32, 0x33]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xC8 });
    assert.deepEqual(res.data.bytes.sensors_exc, ['0x20212223', '0x30313233'],
        'data should match expected values')
});

test('decodeUplink() -> CMD_GET_SENSORS_CFG response', () => {
    const uplinkBytes = Buffer.from([0x04, 0x0F, 0x0A]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xCA });
    assert.deepEqual(res.data.bytes, { max_sensors: 4, rx_flags: 15, en_decoders: 10 },
        'data should match expected values')
});

test('decodeUplink() -> CMD_GET_BLE_CONFIG response', () => {
    const uplinkBytes = Buffer.from([0x01, 0x20]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xD0 });
    assert.deepEqual(res.data.bytes, { ble_active: 1, ble_scantime: 32 },
        'data should match expected values');
});

test('decodeUplink() -> CMD_GET_BLE_ADDR response', () => {
    const uplinkBytes = Buffer.from([0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0xD2 });
    assert.deepEqual(res.data.bytes.ble_addr, ["aa:ab:ac:ad:ae:af", "ba:bb:bc:bd:be:bf"],
        'data should match expected values');
});


test('decodeUplink() -> sensor data', () => {
    const uplinkBytes = Buffer.from(
        [
            7, 238, 42, // ws_temp_c, ws_humidity
            0x66, 0x66, 0x67, 0x44, // ws_rain_mm
            7, 0, 7, 0, 168, 7, // ws_wind_gust_ms, ws_wind_avg_ms, ws_wind_dir_deg,
            0x00, 0x00, 0x00, 0x00, // ws_rain_hourly_mm
            0x00, 0x00, 0x80, 0xbf, // ws_rain_daily_mm
            0x00, 0x00, 0x80, 0xbf, // ws_rain_weekly_mm
            0x00, 0x00, 0x80, 0xbf, // ws_rain_monthly_mm
            0x0B, 0xB4, 0x32, // th1_temp_c, th1_humidity
            0x0A, 0x00, 0x28, // soil1_temp_c, soil1_moisture
            0x83, 0xD5, 0xB9, 0x68, 0x10, 0x00, 0x08, // lgt_ev_time, lgt_ev_events, lgt_ev_dist_km
            0x12, 0x00, // ow0_temp_c
            0x49, 0x10, // a0_voltage_mv
            0x0C, 0x00, 0x1E // ble0_temp_c, ble0_humidity
        ]);
    const res = codec.decodeUplink({ bytes: uplinkBytes, fPort: 0x01 });
    console.log(res.data.bytes);
    assert.deepEqual(res.data.bytes, {
        "ws_temp_c": "20.3",
        "ws_humidity": 42,
        "ws_rain_mm": "925.6",
        "ws_wind_avg_ms": "0.7",
        "ws_wind_gust_ms": "0.7",
        "ws_wind_dir_deg": "196.0",
        "ws_rain_hourly_mm": "0.0",
        "ws_rain_daily_mm": "-1.0",
        "ws_rain_weekly_mm": "-1.0",
        "ws_rain_monthly_mm": "-1.0",
        "th1_temp_c": "30.0",
        "th1_humidity": 50,
        "soil1_temp_c": "25.6",
        "soil1_moisture": 40,
        "lgt_ev_time": {
            "time": '2025-09-04T18:08:03.000Z',
            "timestamp": 1757009283
        },
        "lgt_ev_events": 16,
        "lgt_ev_dist_km": 8,
        "ow0_temp_c": "46.1",
        "a0_voltage_mv": 4169,
        "ble0_temp_c": "30.7",
        "ble0_humidity": 30

    },
        'data should match expected values');
});

/*
 * encodeDownlink() - CMD_GET_* commands
 */
test('encode downlink returns bytes Buffer and fPort', () => {
    const downlinkData = { sleep_interval: 300 };
    const res = codec.encodeDownlink({ data: downlinkData });
    console.log('Encoded result:', res); // Print the entire result object
    console.log('Encoded bytes:', Array.from(res.bytes)); // Print the bytes
    assert.ok(res && typeof res === 'object', 'result should be an object');
    assert.ok(Buffer.isBuffer(res.bytes), 'bytes should be a Buffer');
    assert.ok(res.fPort !== undefined, 'fPort should be set');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});


test('encodeDownlink({"cmd": "CMD_GET_DATETIME"})', () => {
    const downlinkData = { "cmd": "CMD_GET_DATETIME" };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x20, 'fPort should be 0x20');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_LW_CONFIG")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_LW_CONFIG" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0x36, 'fPort should be 0x36');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_LW_STATUS")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_LW_STATUS" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0x38, 'fPort should be 0x38');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_APP_STATUS_INTERVAL")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_APP_STATUS_INTERVAL" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0x40, 'fPort should be 0x40');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_SENSORS_STAT")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_SENSORS_STAT" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0x42, 'fPort should be 0x42');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_APP_PAYLOAD_CFG")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_APP_PAYLOAD_CFG" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0x46, 'fPort should be 0x46');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_WS_TIMEOUT")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_WS_TIMEOUT" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0xC0, 'fPort should be 0xC0');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_SENSORS_INC")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_SENSORS_INC" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0xC6, 'fPort should be 0xC6');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_SENSORS_EXC")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_SENSORS_EXC" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0xC8, 'fPort should be 0xC8');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_SENSORS_CFG")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_SENSORS_CFG" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0xCA, 'fPort should be 0xCA');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_BLE_CONFIG")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_BLE_CONFIG" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0xD0, 'fPort should be 0xD0');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(cmd: "CMD_GET_BLE_ADDR")', () => {
    const res = codec.encodeDownlink({ data: { cmd: "CMD_GET_BLE_ADDR" } });
    assert.ok(res.bytes.equals(Buffer.from([0x00])), 'bytes should be [0x00]');
    assert.ok(res.fPort === 0xD2, 'fPort should be 0xD2');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

/*
 * encodeDownlink() - CMD_SET_* commands
 */

test('encodeDownlink({"epoch": <epoch>})', () => {
    const downlinkData = { epoch: 1757009283 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x68, 0xB9, 0xD5, 0x83])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x21, 'fPort should be 0x21');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ sleep_interval: 300 })', () => {
    const downlinkData = { sleep_interval: 300 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x01, 0x2C])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x31, 'fPort should be 0x31');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ sleep_interval_long: 600 })', () => {
    const downlinkData = { sleep_interval_long: 600 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x02, 0x58])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x33, 'fPort should be 0x33');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ lw_status_interval: 30 })', () => {
    const downlinkData = { lw_status_interval: 30 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x1E])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x35, 'fPort should be 0x35');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ app_status_interval: 60 })', () => {
    const downlinkData = { app_status_interval: 60 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x3C])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x41, 'fPort should be 0x41');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink(<CMD_SET_APP_PAYLOAD_CFG>)', () => {
    const downlinkData = {
        bresser: [
            "0x00", "0x01", "0x02", "0x03", "0x04", "0x05", "0x06", "0x07",
            "0x08", "0x09", "0x0A", "0x0B", "0x0C", "0x0D", "0x0E", "0x0F"
        ], 
        onewire: "0x1011", 
        analog: "0x2021", 
        digital: "0x30313233"
    };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11,
        0x20, 0x21,
        0x30, 0x31, 0x32, 0x33
    ])), 'bytes should match expected value');
    assert.ok(res.fPort === 0x47, 'fPort should be 0x47');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ ws_timeout: 128 })', () => {
    const downlinkData = { ws_timeout: 128 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x80])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xC1, 'fPort should be 0xC1');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ reset_flags: 15 })', () => {
    const downlinkData = { reset_flags: 15 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0x0F])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xC3, 'fPort should be 0xC3');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink({ ws_scantime: 180 })', () => {
    const downlinkData = { ws_scantime: 180 };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([0xB4])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xC4, 'fPort should be 0xC4');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink( <CMD_SET_SENSORS_INC> )', () => {
    const downlinkData = { sensors_inc: [
        "0x10111213", "0x20212223"
    ] };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([
        0x10, 0x11, 0x12, 0x13, 0x20, 0x21, 0x22, 0x23
    ])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xC7, 'fPort should be 0xC7');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink( <CMD_SET_SENSORS_EXC> )', () => {
    const downlinkData = { sensors_exc: [
        "0x30313233", "0x40414243"
    ] };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([
        0x30, 0x31, 0x32, 0x33, 0x40, 0x41, 0x42, 0x43
    ])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xC9, 'fPort should be 0xC9');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink( <CMD_SET_SENSORS_CFG> )', () => {
    const downlinkData = {
        max_sensors: 4, rx_flags: 10, en_decoders: 15
    };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([
        0x04, 0x0A, 0x0F
    ])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xCB, 'fPort should be 0xCB');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink( <CMD_SET_BLE_CONFIG> )', () => {
    const downlinkData = {
        ble_active: 1, ble_scantime: 20
    };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([
        0x01, 0x14
    ])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xD1, 'fPort should be 0xD1');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('encodeDownlink( <CMD_SET_BLE_ADDR> )', () => {
    const downlinkData = { ble_addr: [
        "A0:B0:C0:D0:E0:F0", "0A:0B:0C:0D:0E:0F"
    ] };
    const res = codec.encodeDownlink({ data: downlinkData });
    assert.ok(res.bytes.equals(Buffer.from([
        0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    ])), 'bytes should match expected value');
    assert.ok(res.fPort === 0xD3, 'fPort should be 0xD3');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

/*
 * decodeDownlink()
 */
test('decode downlink returns data object', () => {
    const downlinkBytes = Buffer.from([0x01, 0x2C]);
    const res = codec.decodeDownlink({ bytes: downlinkBytes, fPort: 0x31 });
    console.log('decodeDownlink:', res); // Print the entire result object
    assert.ok(res && typeof res === 'object', 'result should be an object');
    assert.ok('data' in res, 'result should contain data');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});

test('decodeDownlink() with unknown fPort', () => {
    const downlinkBytes = Buffer.from([0x01, 0x02]);
    const res = codec.decodeDownlink({ bytes: downlinkBytes, fPort: 0xff });
    console.log('decodeDownlink:', res); // Print the entire result object
    assert.deepEqual(res.errors, ['unknown FPort'], 'should return unknown FPort error');
    assert.ok(Object.keys(res.data).length === 0, 'data should be empty');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
});

test('decodeDownlink() -> { sleep_interval: 300 }', () => {
    const downlinkBytes = Buffer.from([0x01, 0x2C]);
    const res = codec.decodeDownlink({ bytes: downlinkBytes, fPort: 0x31 });
    console.log('decodeDownlink:', res); // Print the entire result object
    assert.deepEqual(res.data, { sleep_interval: 300 }, 'data should match expected value');
    assert.ok(res.warnings.length === 0, 'should be no warnings');
    assert.ok(res.errors.length === 0, 'should be no errors');
});