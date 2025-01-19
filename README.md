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
- 2x short press to switch on full
- 3x short press to join network
- 3x short press to leave network
- 5x short press to factory reset


#### factory reset

- power cycle off/on the controller 5 times each under 2s (see source/common/factory_reset.c)
- press button 5 times each under 2s (source/app_ui.c)

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

source/zcl_colorCtrlCb.c:303:12: warning: 'sampleLight_colorLoopTimerEvtCb' defined but not used
source/zcl_colorCtrlCb.c:326:13: warning: 'sampleLight_colorLoopTimerStop' defined but not used
