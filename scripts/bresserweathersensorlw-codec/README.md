# BresserWeatherSensorLW LoRaWAN Payload Codec

LoRaWAN Codec API compliant codec for [BresserWeatherSensorLW](https://github.com/matthias-bs/BresserWeatherSensorLW).

This package provides uplink and downlink encoding/decoding functions for use with The Things Network, The Things Stack, Helium, and other LoRaWAN platforms.

## Features

- **Uplink decoding:** Converts sensor payload bytes to JSON objects.
- **Downlink encoding:** Converts JSON commands to bytes for device control.
- **Downlink decoding:** Converts downlink bytes to JSON for verification.
- **LoRaWAN Codec API compliant:** Works with TTN, TTS, and similar platforms.

## Installation

```sh
npm install bresserweathersensorlw-codec
```

## Usage

```js
const codec = require('bresserweathersensorlw-codec');

// Decode uplink
const uplink = codec.decodeUplink({ bytes: Buffer.from([/* uplink bytes */]), fPort: 1 });
console.log(uplink);

// Encode downlink
const downlink = codec.encodeDownlink({ data: { sleep_interval: 300 }, fPort: 0x31 });
console.log(downlink);

// Decode downlink
const decodedDownlink = codec.decodeDownlink({ bytes: Buffer.from([0x01, 0x2C]), fPort: 0x31 });
console.log(decodedDownlink);
```

## API

### `decodeUplink({ bytes, fPort })`

Decodes uplink payload bytes to a JSON object.

### `encodeDownlink({ data, fPort })`

Encodes a JSON object to downlink payload bytes.

### `decodeDownlink({ bytes, fPort })`

Decodes downlink payload bytes to a JSON object.

## File Structure

```
bresserweathersensorlw-codec/
  ├── package.json
  ├── index.js
  ├── README.md
  ├── downlink_formatter.js
  ├── uplink_formatter.js
  └── test/
    └── codec.test.js
```

## License

MIT © 2025 Matthias Prinke

## Links

- [BresserWeatherSensorLW Project](https://github.com/matthias-bs/BresserWeatherSensorLW)
- [LoRaWAN Payload Codec API Specification](https://resources.lora-alliance.org/technical-specifications/ts013-1-0-0-payload-codec-api)