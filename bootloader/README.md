### bootloader

The bootloader has following functions

- checks new firmware and eventually installs it (bootloader_with_ota_check)
- checks firmware and eventually runs it (bootloader_with_ota_check)
- if no firmware available, bootloader_with_ota_check sets noAppFlg
- waits 2000ms for UART message on startup (timer is cancelled, if BUTTON1 pressed)

Uses LEDs LED_POWER and LED_PERMIT, if configured, for status message.

- LED_POWER initialized to 1, on startup (bootloader_init)
- LED_POWER toggled, on receiving message over UART (bootloader_uartRxDataProc)
- LED_POWER toggled, every 100ms, if noAppFlg set (bootloader_loop)

- LED_PERMIT initialized to 1, on startup (bootloader_init)
- LED_PERMIT toggled, for every 256 byte of new image copied (bootloader_with_ota_check)
- LED_PERMIT toggled, every 100ms, if noAppFlg set (bootloader_loop)

Display error on configured LED outputs, in case no valid firmware is found.
Loops through the LED outputs LED_W, LED_R, LED_G, LED_B, LED_WW one at a time.

Shutdown LED outputs before RESET (bootloader_with_ota_check).

#### specifications

- UART 115k200 Baud, 8 bit data, no parity, 1 stop bit, no flow control

#### technical

- bootloader_uartRxDataProc     called for UART_PROC events
- bootloader_keyPressProc       called for KEY_PRESS events

##### OTA image information

The images have following informations.

- manufacturer code (16 bit, 0x1141 =Telink)
- image type (16 bit, HI=chip type, LO=image type)
- file version (32 bit, app release, app build, stack release, stack build)

```
#define MANUFACTURER_CODE_TELINK           	0x1141//Telink ID
#define	IMAGE_TYPE							((CHIP_TYPE << 8) | IMAGE_TYPE_BOOTLOADER)
#define	FILE_VERSION					  	((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)
```

###### Chip IDs

| Chip           | ID   |
| -------------- | ---- |
| TLSR_8267      | 0x00 |
| TLSR_8269      | 0x01 |
| TLSR_8258_512K | 0x02 |
| TLSR_8258_1M   | 0x03 |
| TLSR_8278      | 0x04 |
| TLSR_B91       | 0x05 |
| TLSR_B92       | 0x06 |
| TLSR_TL721X    | 0x07 |
| TLSR_TL321X    | 0x08 |

###### Image types

| Image                           | ID   |
| ------------------------------- | ---- |
| gateway                         | 0x00 |
| light                           | 0x01 |
| switch                          | 0x02 |
| contact sensor                  | 0x03 |
| bootloader                      | 0xFF |
| gateway, for bootloader         | 0x80 |
| light, for bootloader           | 0x81 |
| switch, for bootloader          | 0x82 |
| contact sensor, for bootloader  | 0x83 |

#### protocol

##### message format

- start flag (0x55)
- message type (HI)
- message type (LO)
- message length (HI)
- message length (LO)
- check sum (CRC8, calculated over data block)
- data block (variable length counts message length)
- end flag (0xAA)

##### message codes

#define MSG_START_FLAG						0x55
#define MSG_END_FLAG						0xAA

#define MSG_CMD_OTA_START_REQUEST			0x0210
#define MSG_CMD_OTA_START_RESPONSE			0x8210
#define MSG_CMD_OTA_BLOCK_RESPONSE			0x0211
#define MSG_CMD_OTA_BLOCK_REQUEST			0x8211
#define MSG_CMD_OTA_END_STATUS				0x8212
#define MSG_CMD_ACKNOWLEDGE					0x8000

##### response codes

	MSG_STA_SUCCESS,

	MSG_STA_OTA_GET_BLOCK_TIMEOUT,
	MSG_STA_OTA_IN_PROGRESS,
	MSG_STA_OTA_INCORRECT_OFFSET,
	MSG_STA_OTA_FILE_OVERSIZE,
	MSG_STA_OTA_ERR_BLOCK_RSP,

	MSG_STA_ERROR_START_CHAR	= 0xE0,
	MSG_STA_ERROR_END_CHAR		= 0xE2,
	MSG_STA_BAD_MSG				= 0xE3,
	MSG_STA_CRC_ERROR			= 0xE5,
