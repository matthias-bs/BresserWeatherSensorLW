[
    {
        "type": "uplink",
        "description": "an example of an uplink frame",
        "input": {
            "bytes": [1, 35, 69, 103, 137, 171, 205, 239],
            "fPort": 15,
            "recvTime": "2020-08-02T20:00:00.000+05:00"
        },
        "output": {
            "data": {
                // decoded form of the uplink payload
                "temperature": 15,
                "humidity": 70
            },
            "warnings": ["warning while converting temperature"]
        }
    },
    {
        "type": "downlink-encode",
        "description": "an example of a downlink frame to encode",
        "input": {
            "data": {
                // decoded form of the downlink payload
                "sleep_interval": 360}
            }
        },
        "output": {
            "bytes": [1, 104],
            "fPort": 168,
            "warnings": []
        }
    },
    {
        "type": "downlink-decode",
        "description": "an example of a downlink frame to decode",
        "input": {
            "bytes": [1, 104],
            "fPort": 168,
            "recvTime": "2024-05-13T08:17:00.000+02:00"
        },
        "output": {
            "data": {
                // decoded form of the downlink payload
                "sleep_interval": 360
            },
            "warnings": []
        }
    },
    {
        "type": "uplink",
        "description": "an example of an uplink frame failed",
        "input": {
            "bytes": [1, 35, 69, 103, 137, 171, 205, 239],
            "fPort": 15,
            "recvTime": "2020-08-02T20:00:00.000+05:00"
        },
        "output": {
            "errors": [
                "error thrown while decoding temperature",
                "error thrown while decoding humidity"
            ],
            "warnings": ["warning while converting temperature"]
        }
    },
    {
        "type": "downlink-encode",
        "description": "an example of a downlink frame failed to encode",
        "input": {
            "data": {
                // decoded form of the downlink payload
                "cmd": "CMD_UNDEFINED",
            }
        },
        "output": {
            "errors": ["Unknown command"],
            "warnings": []
        }
    },
    {
        "type": "downlink-decode",
        "description": "an example of a downlink frame to decode",
        "input": {
            "bytes": [0],
            "fPort": 32,
            "recvTime": "2024-05-13T08:22:00.000+02:00"
        },
        "output": {
            "errors": ["Unknown FPort"],
            "warnings": []
        }
    }
]