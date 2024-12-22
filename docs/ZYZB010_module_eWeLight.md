### ZYZB010 module by eWeLight

- 5 channel PWM
- 5-24 V/DC
- 1 button input
- TeLink TLSR8258 controller (32 pin, 512KB flash)
- ?without bootloader

#### ports 1-5 channel PWM output

| port | function, usage           |
| ---- | ------------------------- |
| A7   | debug und download, SWS   |
| D3   | FAC_RST, pairing, pull-up |
| C3   | warm white, PWM2          |
| C2   | white, cold white, PWM1   |
| C4   | red, PWM3                 |
| B4   | green, PWM4               |
| B5   | blue, PWM5                |

| port | function, usage          ´|
| ---- | ------------------------- |
| D7   | UART_TX                   |
| B7   | UART_RX                   |

| port | function, usage           |
| ---- | ------------------------- |
| D2   |                           |
| D4   |                           |
| A0   |                           |
| A1   |                           |
| B1   |                           |
| B6   |                           |
| C0   |                           |
| C1   |                           |

#### signature ZB-CL01 by eWeLight

{
  "node_descriptor": {
    "logical_type": 1,
    "complex_descriptor_available": 0,
    "user_descriptor_available": 0,
    "reserved": 0,
    "aps_flags": 0,
    "frequency_band": 8,
    "mac_capability_flags": 142,
    "manufacturer_code": 4107,
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
        "0x0b05",
        "0x1000"
      ],
      "output_clusters": []
    }
  },
  "manufacturer": "eWeLight",
  "model": "ZB-CL01",
  "class": "zigpy.device.Device"
}

#### flash layout

IEEE: a4:c1:38:34:07:7d:a0:f6
0x76000 =7D 07 34 38 C1 A4 F6 A0

##### without BOOT_LOADER_MODE (or ==0)

| address | sectors |    size | usage        |
| ------- | ------- | ------- | ------------ |
| 0x00000 |         |         | firmware     |
| 0x34000 |         |         | NV_1 storage |
| 0x40000 |         |         | OTA_Image    |
| ------- | ------- | ------- | ------------ |
| 0x76000 |         |         | MAC_Addr     |
| 0x77000 |         |         | F_CFG_Info   |
| 0x78000 |         |         | U_CFG_Info   |
| 0x7A000 |         |         | NV_2 storage |

##### BOOT_LOADER_MODE==1

| address | sectors |    size | usage        |
| ------- | ------- | ------- | ------------ |
| 0x00000 |         |         | bootloader   |
| 0x08000 |         |         | firmware     |
| 0x39000 |         |         | OTA_Image    |
| 0x6A000 |         |         | NV_1 storage |
| ------- | ------- | ------- | ------------ |
| 0x76000 |         |         | MAC_Addr     |
| 0x77000 |         |         | F_CFG_Info   |
| 0x78000 |         |         | U_CFG_Info   |
| 0x7A000 |         |         | NV_2 storage |

check tl_zigbee_sdk\proj\drivers\drv_nv.h
