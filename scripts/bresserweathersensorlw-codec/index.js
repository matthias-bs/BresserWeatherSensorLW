// LoRaWAN Codec API compliant implementation for BresserWeatherSensorLW
// Robust handling of different formatter export styles.

let uplinkFormatter;
let downlinkFormatter;
try { uplinkFormatter = require('./uplink_formatter.js'); } catch (e) { uplinkFormatter = undefined; }
try { downlinkFormatter = require('./downlink_formatter.js'); } catch (e) { downlinkFormatter = undefined; }

/**
 * Try calling formatter module with a target function name.
 * Supports many common export patterns and returns undefined if nothing matches.
 */
function callFormatter(mod, fnName, input) {
    if (!mod) return undefined;

    // module exported as function (common simple pattern)
    if (typeof mod === 'function') {
        try { return mod(input); } catch (e) { return undefined; }
    }

    // direct named export
    if (typeof mod[fnName] === 'function') {
        try { return mod[fnName](input); } catch (e) { return undefined; }
    }

    // default export as function or object
    if (mod.default) {
        if (typeof mod.default === 'function') {
            try { return mod.default(input); } catch (e) { /* ignore */ }
        }
        if (mod.default && typeof mod.default[fnName] === 'function') {
            try { return mod.default[fnName](input); } catch (e) { /* ignore */ }
        }
    }

    // generic encode/decode functions
    if (fnName.startsWith('decode') && typeof mod.decode === 'function') {
        try { return mod.decode(input); } catch (e) { return undefined; }
    }
    if (fnName.startsWith('encode') && typeof mod.encode === 'function') {
        try { return mod.encode(input); } catch (e) { return undefined; }
    }

    // some modules expose handler/process/run
    const alt = ['handler', 'process', 'run'];
    for (const a of alt) {
        if (typeof mod[a] === 'function') {
            try { return mod[a](input); } catch (e) { /* ignore */ }
        }
        if (mod[a] && typeof mod[a][fnName] === 'function') {
            try { return mod[a][fnName](input); } catch (e) { /* ignore */ }
        }
    }

    // nothing matched
    return undefined;
}

function normalizeBytes(x) {
    if (Buffer.isBuffer(x)) return x;
    if (!x) return Buffer.from([]);
    if (Array.isArray(x) || x instanceof Uint8Array) return Buffer.from(x);
    try { return Buffer.from(x); } catch (e) { return Buffer.from([]); }
}

/* Encode downlink (JSON -> bytes) */
function encodeDownlinkApi(input) {
    const result = callFormatter(downlinkFormatter, 'encodeDownlink', input) || {};
    return {
        bytes: normalizeBytes(result.bytes),
        fPort: result.fPort || (input && input.fPort),
        warnings: result.warnings || [],
        errors: result.errors || []
    };
}

/* Decode downlink (bytes -> JSON) */
function decodeDownlinkApi(input) {
    const bytesArr = Array.isArray(input.bytes) ? input.bytes : Array.from(input.bytes || []);
    const result = callFormatter(downlinkFormatter, 'decodeDownlink', { bytes: bytesArr, fPort: input.fPort }) || {};
    return {
        data: result.data || {},
        warnings: result.warnings || [],
        errors: result.errors || []
    };
}

/* Decode uplink (bytes -> JSON) */
function decodeUplinkApi(input) {
    const bytesArr = Array.isArray(input.bytes) ? input.bytes : Array.from(input.bytes || []);
    const result = callFormatter(uplinkFormatter, 'decodeUplink', { bytes: bytesArr, fPort: input.fPort }) || {};
    return {
        data: result.data || {},
        warnings: result.warnings || [],
        errors: result.errors || []
    };
}

module.exports = {
    encodeDownlink: encodeDownlinkApi,
    decodeDownlink: decodeDownlinkApi,
    decodeUplink: decodeUplinkApi
};