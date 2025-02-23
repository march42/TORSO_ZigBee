### Tuya WZ5 5-in-1 controller (unsupported)

- 5 channel
- 5-24 V/DC
- 5x 3A

| mode    | output                     |
| ------- | -------------------------- |
| DIM     |  W |  W |  W |  W |  W | + |
| CCT     | CW | WW |  / | CW | WW | + |
| RGB     |  / |  / |  B |  G |  R | + |
| RGBW    |  / |  W |  B |  G |  R | + |
| RGB+CCT | CW | WW |  B |  G |  R | + |

After changing the mode, it seems the manufacturer always changes with it, maybe it's generated.

- RGB or white lighting, not simultaneously

#### DIM mod (LED=white)

IEEE: a4:c1:38:78:f1:97:01:51

{
  "node_descriptor": {
    "logical_type": 1,
    "complex_descriptor_available": 0,
    "user_descriptor_available": 0,
    "reserved": 0,
    "aps_flags": 0,
    "frequency_band": 8,
    "mac_capability_flags": 142,
    "manufacturer_code": 4417,
    "maximum_buffer_size": 66,
    "maximum_incoming_transfer_size": 66,
    "server_mask": 10752,
    "maximum_outgoing_transfer_size": 66,
    "descriptor_capability_field": 0
  },
  "endpoints": {
    "1": {
      "profile_id": "0x0104",
      "device_type": "0x0101",
      "input_clusters": [
        "0x0000",
        "0x0003",
        "0x0004",
        "0x0005",
        "0x0006",
        "0x0008",
        "0x0300",
        "0x1000",
        "0xef00"
      ],
      "output_clusters": [
        "0x000a",
        "0x0019"
      ]
    },
    "242": {
      "profile_id": "0xa1e0",
      "device_type": "0x0061",
      "input_clusters": [],
      "output_clusters": [
        "0x0021"
      ]
    }
  },
  "manufacturer": "_TZB210_rkgngb5o",
  "model": "TS0502B",
  "class": "zigpy.device.Device"
}

#### CCT mode (LED=yellow)

IEEE: a4:c1:38:78:f1:97:01:51

{
  "node_descriptor": {
    "logical_type": 1,
    "complex_descriptor_available": 0,
    "user_descriptor_available": 0,
    "reserved": 0,
    "aps_flags": 0,
    "frequency_band": 8,
    "mac_capability_flags": 142,
    "manufacturer_code": 4417,
    "maximum_buffer_size": 66,
    "maximum_incoming_transfer_size": 66,
    "server_mask": 10752,
    "maximum_outgoing_transfer_size": 66,
    "descriptor_capability_field": 0
  },
  "endpoints": {
    "1": {
      "profile_id": "0x0104",
      "device_type": "0x010c",
      "input_clusters": [
        "0x0000",
        "0x0003",
        "0x0004",
        "0x0005",
        "0x0006",
        "0x0008",
        "0x0300",
        "0x1000",
        "0xef00"
      ],
      "output_clusters": [
        "0x000a",
        "0x0019"
      ]
    },
    "242": {
      "profile_id": "0xa1e0",
      "device_type": "0x0061",
      "input_clusters": [],
      "output_clusters": [
        "0x0021"
      ]
    }
  },
  "manufacturer": "_TZB210_nfzrlz29",
  "model": "TS0503B",
  "class": "zigpy.device.Device"
}

#### RGB mode (LED=red)

IEEE: a4:c1:38:78:f1:97:01:51
"manufacturer_code": 4417,
"server_mask": 10752,
"manufacturer": "_TZB210_zdvrsts8",
"model": "TS0504B",

{
  "node_descriptor": {
    "logical_type": 1,
    "complex_descriptor_available": 0,
    "user_descriptor_available": 0,
    "reserved": 0,
    "aps_flags": 0,
    "frequency_band": 8,
    "mac_capability_flags": 142,
    "manufacturer_code": 4417,
    "maximum_buffer_size": 66,
    "maximum_incoming_transfer_size": 66,
    "server_mask": 10752,
    "maximum_outgoing_transfer_size": 66,
    "descriptor_capability_field": 0
  },
  "endpoints": {
    "1": {
      "profile_id": "0x0104",
      "device_type": "0x010d",
      "input_clusters": [
        "0x0000",
        "0x0003",
        "0x0004",
        "0x0005",
        "0x0006",
        "0x0008",
        "0x0300",
        "0x1000",
        "0xef00"
      ],
      "output_clusters": [
        "0x000a",
        "0x0019"
      ]
    },
    "242": {
      "profile_id": "0xa1e0",
      "device_type": "0x0061",
      "input_clusters": [],
      "output_clusters": [
        "0x0021"
      ]
    }
  },
  "manufacturer": "_TZB210_zdvrsts8",
  "model": "TS0505B",
  "class": "zigpy.device.Device"
}

#### RGBW mode (LED=green)

IEEE: a4:c1:38:78:f1:97:01:51
"manufacturer_code": 4417,
"server_mask": 10752,
"manufacturer": "_TZB210_5jn6an2y",
"model": "TS0505B",

{
  "node_descriptor": {
    "logical_type": 1,
    "complex_descriptor_available": 0,
    "user_descriptor_available": 0,
    "reserved": 0,
    "aps_flags": 0,
    "frequency_band": 8,
    "mac_capability_flags": 142,
    "manufacturer_code": 4417,
    "maximum_buffer_size": 66,
    "maximum_incoming_transfer_size": 66,
    "server_mask": 10752,
    "maximum_outgoing_transfer_size": 66,
    "descriptor_capability_field": 0
  },
  "endpoints": {
    "1": {
      "profile_id": "0x0104",
      "device_type": "0x010d",
      "input_clusters": [
        "0x0000",
        "0x0003",
        "0x0004",
        "0x0005",
        "0x0006",
        "0x0008",
        "0x0300",
        "0x1000",
        "0xef00"
      ],
      "output_clusters": [
        "0x000a",
        "0x0019"
      ]
    },
    "242": {
      "profile_id": "0xa1e0",
      "device_type": "0x0061",
      "input_clusters": [],
      "output_clusters": [
        "0x0021"
      ]
    }
  },
  "manufacturer": "_TZB210_5jn6an2y",
  "model": "TS0505B",
  "class": "zigpy.device.Device"
}

#### RGB-CCT mode (LED=blue)

IEEE: a4:c1:38:78:f1:97:01:51
"manufacturer_code": 4417,
"manufacturer": "_TZB210_3zfp8mki",
"model": "TS0505B",

{
  "node_descriptor": {
    "logical_type": 1,
    "complex_descriptor_available": 0,
    "user_descriptor_available": 0,
    "reserved": 0,
    "aps_flags": 0,
    "frequency_band": 8,
    "mac_capability_flags": 142,
    "manufacturer_code": 4417,
    "maximum_buffer_size": 66,
    "maximum_incoming_transfer_size": 66,
    "server_mask": 10752,
    "maximum_outgoing_transfer_size": 66,
    "descriptor_capability_field": 0
  },
  "endpoints": {
    "1": {
      "profile_id": "0x0104",
      "device_type": "0x010d",
      "input_clusters": [
        "0x0000",
        "0x0003",
        "0x0004",
        "0x0005",
        "0x0006",
        "0x0008",
        "0x0300",
        "0x1000",
        "0xef00"
      ],
      "output_clusters": [
        "0x000a",
        "0x0019"
      ]
    },
    "242": {
      "profile_id": "0xa1e0",
      "device_type": "0x0061",
      "input_clusters": [],
      "output_clusters": [
        "0x0021"
      ]
    }
  },
  "manufacturer": "_TZB210_3zfp8mki",
  "model": "TS0505B",
  "class": "zigpy.device.Device"
}
