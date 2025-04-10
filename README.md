# TORSO lamp

The TORSO lamp is an artwork LED lamp.

The baseplate is cut from stainless steel.
The white plastic cover is a female semishell torso for hanging and presenting underwear.

It's designed to be wall mounted. It can also be mounted on the ceiling, if wished.
There are two (independend) light modules.
The main module is build with white LED COB modules. (I prefere WW/CW modules, to set color temperature.)
The background lighting is build with RGB LED COB modules.
The power supply needs to be a seperate power supply module. So the lamp doesn't need earthing.

## base plate

Made from stainless steel; 1.4301; 0.8 mm
Size is 480 x 770 mm (WxH)
The mounting ears give 25 mm space, after bending.
Mounting plates are designed, to fit the LED modules, power supply connector and buttons.

## Controller for TORSO lamp

ZigBee 3.0 LED controller

### configuration and handling

#### button functions

- short press to toggle on/off
- long press for dimming loop
- 2x short press to switch to full on
- 3x short press to join network
- 4x short press to leave network
- 5x short press to factory reset

- BUTTON1 short press in bootloader (first 2000ms after reset) stopp timeout and wait for UART data transfer.

- long press dimming down to minimum, long press at minimum dimming up to maximum

#### factory reset

- power cycle off/on the controller 5 times each under 2s (see source/common/factory_reset.c)
- press button 5 times each under 2s (source/app_ui.c)

- (TUYA default) long press button about 5s (source/app_ui.c)

### hardware

#### ZT3L module by Tuya

- TeLink TLSR8258 MCU (1 MB flash)
- 5 channel PWM CW,R,G,B,WW
- 1 button input
- 1 channel IR in

#### ZYZB010 module by eWeLight

- TeLink TLSR8258 MCU (512 KB flash)
- 5 channel PWM CW,WW,R,G,B
- 1 button input (for factory reset and pairing mode)

### firmware

#### LED Channels

- LED channel level set by `cmp_tick = (dutycycle * PMW_MAX_TICK) / (ZCL_LEVEL_ATTR_MAX_LEVEL * PWM_FULL_DUTYCYCLE)`
- hwLight_init initializes the PWM module and channels
- light_adjust initializes the light to default
- light_adjust calls sampleLight_colorInit, sampleLight_levelInit, sampleLight_onOffInit
- sampleLight_colorInit

### building firmware

```
/d/development/TelinkIoTStudio/bin/make BOARD=ZT3L TC32DIR=/d/development/TelinkIoTStudio/opt/tc32 TOOLSPATH=/d/development/TelinkIoTStudio/mingw/bin:/d/development/TelinkIoTStudio/bin

```

#### configuration

| variable          | meaning, setting                                                      |
| ----------------- | --------------------------------------------------------------------- |
| TC32DIR           | path to the compiler suite e.g. ???/TelinkIoTStudio/opt/tc32          |
| TOOLSPATH         | e.g. ???/TelinkIoTStudio/bin;???/TelinkIoTStudio/opt/tc32/bin         |
| CC                | C compiler, e.g. ???/TelinkIoTStudio/opt/tc32/bin/tc32-elf-gcc.exe    |
| PROJECTDIR        | project base directory                                                |
| SOURCEDIR         | source directories                                                    |
| TL_ZIGBEE_SDK     | path to the ZigBee SDK $(PROJECTDIR)/telink_zigbee_sdk/tl_zigbee_sdk  |
| BUILDDIR          | directory for building binaries $(PROJECTDIR)/build/$(MODULE)         |
| ----------------- | --------------------------------------------------------------------- |
| MODULE            | target hardware module (ZT3L,ZYZB010)                                 |
| ZB_ROLE           | ZigBee device role (COORDINATOR, ROUTER, END_DEVICE)                  |
| FLASHER           | SWire writer settings (--port COM10 --tact 300 --run)                 |
| ----------------- | --------------------------------------------------------------------- |
| MCU_CHIP          | target MCU (TLSR_8258)                                                |
| MCU_CORE_8258     | target MCU is TLSR_8258                                               |
| CHIP_TYPE         | target MCU variant (TLSR_8258_1M, TLSR_8258_512K)                     |
| BOOT_LOADER_MODE  | build for using bootloader                                            |
| boot_link         | linker script setting for target                                      |
| ----------------- | --------------------------------------------------------------------- |
| CPPFLAGS          | C pre processor flags                                                 |
| CFLAGS            | C compiler flags                                                      |
| ASFLAGS           | AS compiler flags                                                     |
| LDFLAGS           | linker flags                                                          |
| LDLIBS            | linker libraries                                                      |
| ----------------- | --------------------------------------------------------------------- |
| src_DIRS          | source directories                                                    |
| extra_files       | extra files to build                                                  |
| bin_files         | binary files to build                                                 |
| ----------------- | --------------------------------------------------------------------- |
| sdk_DIRS          | source directories for SDK                                            |
| sdk_LIBS          | library directories for SDK                                           |
| sdk_SOURCES       | source files C                                                        |
| sdk_ASMS          | source files ASM                                                      |
| sdk_OBJS          | object files to build                                                 |

#### TODO

- [ ] colorLoop feature

source/zcl_colorCtrlCb.c:303:12: warning: 'sampleLight_colorLoopTimerEvtCb' defined but not used
source/zcl_colorCtrlCb.c:326:13: warning: 'sampleLight_colorLoopTimerStop' defined but not used

- [ ] fix debug output (maybe interrupt problem)
- [ ] fix the color controll handling (RGB, CCT or RGB and white or RGB and CCT)
- [ ] switching main/backlight with power on/off
- [ ] handle X/Y color setting
- [ ] handle enhanced hue color setting
- [ ] split TORSO_LIGHT to multiple end points handling main, background, extra light
- [ ] ZCL BASIC cluster ProductCode attribute (0x00 =none) wird auf HomeAssistant falsch angezeigt
- [ ] store serialNumber and productCode in NV (maybe with MAC or in F_Cfg)

### color calculations

For the correct calculations, there are a few technical details to be known.
The exact values should be available from the LED manufacturers datasheet.

- emiting wavelength of 5050 RGB LED is red=630nm, green=525nm, blue=465nm
- intensity of 5050 RGB LED is red=1000mcd, green=1500mcd, blue=800mcd

Kelvin and Mired used for specification of color temperature.
Color temperature is achieved by mixing cold and warm white light.
The color temperature of LED should be available from the manufacturers datasheet.

- typical color temperature of CCT LED is cold=6,000K-6,500K, warm=2,700K-3,000K
- injection LED module build with 5730 SMD LEDs are mostly sold as warm 3,000K, neutral 6,500K, cold 10,000K

Hue is an angle between 0 and 360. CurrentHue attribute is an integer value between 0 and 254, because 255 is don't change value.
`Hue(degrees) = CurrentHue x 360 / 254`
`CurrentHue = Hue x 254 / 360`

#### PWM Cycle

The PWM cycle is defined by giving a cycle counter and a maximum cycle.
- on starting the state is HI and the clock ticks are counted
- on reaching the cycle counter the state switches to LO
- on reaching the maximum the counter is reset and the cycle starts again

The maximum is defined by the PWM clock and the PWM frequency `MAXIMUM = CLOCK / FREQUENCY`

```
CPU clock = 48MHz
PWM frequency = 4kHz
MAXIMUM = 48MHz / 4kHz = 12,000
```

The intensity is `INTENSITY = COUNTER / MAXIMUM`

#### Level Attribute

ZigBee LEVEL is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)

The level attribute is 8bit ranging from 1 (ZCL_LEVEL_ATTR_MIN_LEVEL) to 254 (ZCL_LEVEL_ATTR_MAX_LEVEL).

#### HSV Color Space

Typical H is 0-360°, S and V are each 0%-100%

ZigBee Hue is fractions of 254 (ZCL_COLOR_ATTR_HUE_MAX)
ZigBee Saturation is fractions of 254 (ZCL_COLOR_ATTR_SATURATION_MAX)
ZigBee Value is LEVEL, Value/LEVEL is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)

#### Enhanced Hue Color Space

ZigBee Enhanced Hue is fractions of 65535 (ZCL_COLOR_ATTR_ENHANCED_HUE_MAX)
ZigBee Saturation is fractions of 254 (ZCL_COLOR_ATTR_SATURATION_MAX)
ZigBee Value is LEVEL, Value/LEVEL is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)

#### XY Color Space

ZigBee X is fractions of 65535, maximum of 65279 (ZCL_COLOR_ATTR_XY_MAX)
ZigBee Y is fractions of 65535, maximum of 65279 (ZCL_COLOR_ATTR_XY_MAX)
ZigBee LEVEL, LEVEL is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)

#### RGB Color Space

Typical RGB values are 0-255

#### xyY Color Space

#### XYZ Color Space

#### Color Temperature

Color Temperature given in Mired or Kelvin.
`1 Mired = 1,000,000 / 1 Kelvin  <==> 1 Kelvin = 1,000,000 / 1 Mired`

ZigBee Mired maximum is 65279 (ZCL_COLOR_ATTR_TEMPERATURE_MIRDES_MAX)

### firmware functions

#### light_applyUpdate

`void light_applyUpdate(u8 *curLevel, u16 *curLevel256, s32 *stepLevel256, u16 *remainingTime, u8 minLevel, u8 maxLevel, bool wrap)`


#### light_applyUpdate_16

`void light_applyUpdate_16(u16 *curLevel, u32 *curLevel256, s32 *stepLevel256, u16 *remainingTime, u16 minLevel, u16 maxLevel, bool wrap)`

#### ZCL clusters and commands

- BASIC cluster
- OnOff cluster
- Level cluster
- ColorControl cluster

##### ZCL cluster command handling

The command handling functions are set in `source\ledLightEpCfg.c`

###### ledLight_sceneCb

###### ledLight_onOffCb

###### ledLight_levelCb

###### ledLight_colorCtrlCb

Setting `EXTENDED_COLOR_LIGHT=1` will activate additional features and synchronization.

- ColorTemperature on RGB LEDs
- Color Loop function
- enhanced Hue capability

- ZCL command MOVE_TO_HUE
  - ledLight_moveToHueProcess
- ZCL command MOVE_TO_SATURATION
  - ledLight_moveToSaturationProcess
- ZCL command MOVE_TO_HUE_AND_SATURATION
  - ledLight_moveToHueAndSaturationProcess
    - ledLight_moveToHueProcess
    - ledLight_moveToSaturationProcess

- ZCL command MOVE_TO_COLOR
  - ledLight_moveToColorProcess X,Y ,t

- ZCL command ENHANCED_MOVE_TO_HUE_AND_SATURATION
  - ledLight_enhancedMoveToHueAndSaturationProcess
    - ledLight_enhancedMoveToHueProcess
    - ledLight_moveToSaturationProcess

#### ZCL cluster support setting

In `source/app_cfg.h` the desired ZCL clusters are specified (ZCL_ ... _SUPPORT).

- ZCL_ON_OFF_SUPPORT
- ZCL_LEVEL_CTRL_SUPPORT
- ZCL_LIGHT_COLOR_CONTROL_SUPPORT
- ZCL_GROUP_SUPPORT
- ZCL_SCENE_SUPPORT
- ZCL_OTA_SUPPORT
- ZCL_GP_SUPPORT
- ZCL_WWAH_SUPPORT
- ZCL_ZLL_COMMISSIONING_SUPPORT

In `tl_zigbee_sdk/zigbee/zcl/zcl_config.h` the (ZCL_ ...) macros are defined accordingly.

The actual handling is then enabled in the according SDK files (e.g. `tl_zigbee_sdk/zigbee/zcl/general/zcl_level.c`).

