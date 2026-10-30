
## ZigBee OTA file header structure

| Octets | Field Name | Data Type | Notes |
| ---:|:--- |:--- | --- |
| 4 | OTA upgrade file identifier | uint32 | default 0x0BEEF11E |
| 2 | OTA Header version | uint16 | default 0x0100 |
| 2 | OTA Header length | uint16 | default 56, plus length of optional fields |
| 2 | OTA Header Field control | uint16 | default 0x0000, bitmask of presenmt optional fields |
| 2 | Manufacturer code | uint16 | usually assigned by CSA-IOT |
| 2 | Image type | uint16 | manufacturer specific, ensure correct image for different hardware |
| 4 | File version | uint32 | manufacturer specific, Telink suggests APP_RELEASE/APP_BUILD/STACK_RELEASE/STACK_BUILD |
| 2 | ZigBee Stack version | uint16 | specifications 0x0000=ZigBee 2006, 0x0001=ZigBee 2007, 0x0002=ZigBee Pro, 0x0003=ZigBee IP |
| 32 | OTA Header string | ASCII string | manufacturer or SDK platform specific |
| 4 | Total Image size | uint32 | header + firmware size |
| 0/1 | Required Security Credentials | uint8 | (OPTIONAL) Field control 0x0001 |
| 0/8 | EUI64 (8-Byte hex) of the device intended | EUI-64 | (OPTIONAL) Field control 0x0002 |
| 0/2 | Minimum hardware version | uint16 | (OPTIONAL) Field control 0x0004 |
| 0/2 | Maximum hardware version | uint16 | (OPTIONAL) Field control 0x0004 |

## Espressif Sub-Elements

| Octets | Field Name | Data Type | Notes |
| ---:|:--- |:--- | --- |
| 2 | Tag ID | uint16 | denotes the type and format of the data contained within the sub-element |
| 4 | Length field | uint32 | dictates the length of the rest of the data within the sub-element in bytes, does not include Tag ID and Length field |
| variable | Data | depends | Data of the Sub-Element |

| Tag ID | Description |
| ---:|:--- |
| 0x0000 | Upgrade Image |
| 0x0001 | ECDSA Signature (Crypto Suite 1) |
| 0x0002 | ECDSA Signing Certificate (Crypto Suite 1) |
| 0x0003 | Image Integrity Code |
| 0x0004 | Picture Data |
| 0x0005 | ECDSA Signature (Crypto Suite 2) |
| 0x0006 | ECDSA Signing Certificate (Crypto Suite 2) |
| 0xf000 - 0xffff | Manufacturer Specific Use |
