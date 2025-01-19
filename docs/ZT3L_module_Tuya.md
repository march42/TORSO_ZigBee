### Tuya ZT3L module

- 5 channel PWM
- 5-24 V/DC
- 1 button input
- 1 IR input (or second button)
- TeLink TLSR8258 controller (32 pin, 1 MB flash)

#### MCU

Chip version: 02
Chip product: 5562 (825x ?)
Flash ID: c86014
Flash size: 1024 KB

- Clock 48 MHz

#### ports

| port | function, usage          |
| ---- | ------------------------ |
| B5   | white, cold white, PWM5  |
| B4   | red, PWM4                |
| D2   | blue, PWM3               |
| C3   | green, PWM1              |
| C2   | warm white, PWM0         |
| C0   | button 1, pull-up        |
| D4   | button 2, pull-up        |

| port | function, usage          |
| ---- | ------------------------ |
| B1   | UART_TX, PWM4            |
| B7   | UART_RX                  |
| D7   | IR in                    |

#### signature TS0505B

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
  "manufacturer": "_TZ3210_3lbtuxgp",
  "model": "TS0505B",
  "class": "zigpy.device.Device"
}

##### signature TS0501B

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
        "0x1000"
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
  "manufacturer": "_TZ3210_4zinq6io",
  "model": "TS0501B",
  "class": "zhaquirks.tuya.ts0501bs.DimmableLedController"
}

#### flash organization

| address | sectors |    size | usage        |
| ------- | ------- | ------- | ------------ |
| 0x00000 |     256 | 1048576 | flash memory |
| 0x00000 |       8 |   32768 | boot loader  |
| 0x08000 |     111 |  454656 | application  |
| 0x77000 |     111 |  454656 | OTA image    |
| 0xE6000 |      22 |   90112 | NV storage   |
| 0xFC000 |       2 |    8192 | U_Cfg_Info   |
| 0xFE000 |       1 |    4096 | F_Cfg_Info   |
| 0xFF000 |       1 |    4096 | MAC address  |

check tl_zigbee_sdk\proj\drivers\drv_nv.h

##### burning

python TLSR825xComFlasher.py --port com10 --tact 300 rf 0x00000 0x100000 dump-flash.bin

python TLSR825xComFlasher.py --port com10 --tact 300 rf 0x00000 32768 dump-bootloader.bin
python TLSR825xComFlasher.py --port com10 --tact 300 rf 0x08000 454656 dump-application.bin
python TLSR825xComFlasher.py --port com10 --tact 300 rf 0x77000 454656 dump-ota-image.bin
python TLSR825xComFlasher.py --port com10 --tact 300 rf 0xe6000 90112 dump-nv-storage.bin
python TLSR825xComFlasher.py --port com10 --tact 300 rf 0xfc000 8192 dump-U_Cfg_Info.bin
python TLSR825xComFlasher.py --port com10 --tact 300 rf 0xfe000 4096 dump-F_Cfg_info.bin
python TLSR825xComFlasher.py --port com10 --tact 300 rf 0xff000 4096 dump-mac-address.bin

python TLSR825xComFlasher.py --port com10 --tact 300 we 0x00000 bootloader.bin

python TLSR825xComFlasher.py --port com10 --tact 300 we 0x08000 application.bin

erase OTA image
python TLSR825xComFlasher.py --port com10 --tact 300 es 0x77000 454656

erase NV storage
python TLSR825xComFlasher.py --port com10 --tact 300 es 0xe6000 90112

erase U_Cfg_Info
python TLSR825xComFlasher.py --port com10 --tact 300 es 0xfc000 8192

erase F_Cfg_info
python TLSR825xComFlasher.py --port com10 --tact 300 es 0xfe000 4096

erase MAX address - CAUTION !!! - after this, a new MAC will be generated
python TLSR825xComFlasher.py --port com10 --tact 300 es 0xff000 4096 dump-mac-address.bin

##### IEEE MAC address

a4:c1:38 ... Telink

@0xFF000 =F5 00 30 38 C1 A4 73 0E
MAC =a4:c1:38:30:00:f5:0e:73

@0xFF000 =B7 13 7C 38 C1 A4 01 E9
MAC =a4:c1:38:7c:13:b7:e9:01


### Telink TLSR binary images

- FLASH_TLNK_FLAG_OFFSET =0x8 (defined in drv_nv.h)
- u32 @FLASH_TLNK_FLAG_OFFSET =FW_START_UP_FLAG_WHOLE 0x544c4e4b (bootloader.c)
- u32 @0x18 =image size
