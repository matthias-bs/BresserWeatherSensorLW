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
                "setCounter": true,
                "range": 7
            }
        },
        "output": {
            "bytes": [254, 220, 186, 152, 118, 84, 50],
            "fPort": 8,
            "warnings": ["warning while converting temperature"]
        }
    },
    {
        "type": "downlink-decode",
        "description": "an example of a downlink frame to decode",
        "input": {
            "bytes": [254, 220, 186, 152, 118, 84, 50],
            "fPort": 8,
            "recvTime": "2020-09-06T15:00:00.000+03:00"
        },
        "output": {
            "data": {
                // decoded form of the downlink payload
                "setCounter": true,
                "range": 7
            },
            "warnings": ["warning while converting temperature"]
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
                "setCounter": true,
                "range": 7
            }
        },
        "output": {
            "errors": ["error thrown while encoding configuration"],
            "warnings": ["warning while converting temperature"]
        }
    },
    {
        "type": "downlink-decode",
        "description": "an example of a downlink frame to decode",
        "input": {
            "bytes": [254, 220, 186, 152, 118, 84, 50],
            "fPort": 8,
            "recvTime": "2020-09-06T15:00:00.000+03:00"
        },
        "output": {
            "errors": ["error thrown while decoding configuration"],
            "warnings": ["warning while converting configuration"]
        }
    }
]
