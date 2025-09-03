const codec = require('../index');

// Example uplink bytes (replace with real test data)
const uplinkBytes = Buffer.from([0x01, 0x2C, 0x00, 0xFF]);
const uplinkResult = codec.decodeUplink({ bytes: uplinkBytes, fPort: 1 });
console.log('Uplink decode:', uplinkResult);

// Example downlink encoding
const downlinkData = { sleep_interval: 300 };
const downlinkResult = codec.encodeDownlink({ data: downlinkData, fPort: 0x31 });
console.log('Downlink encode:', downlinkResult);

// Example downlink decoding
const downlinkBytes = Buffer.from([0x01, 0x2C]);
const decodedDownlink = codec.decodeDownlink({ bytes: downlinkBytes, fPort: 0x31 });
console.log('Downlink decode:', decodedDownlink);

// Simple assertions (replace with your preferred test framework)
if (!uplinkResult.data) throw new Error('Uplink decode failed');
if (!downlinkResult.bytes) throw new Error('Downlink encode failed');
if (!decodedDownlink.data) throw new Error('Downlink decode failed');

console.log('All codec tests passed.');
