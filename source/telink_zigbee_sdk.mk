#
###
#	telink_zigbee_sdk.mk
###
# TSLR8258 information
#	https://wiki.telink-semi.cn/wiki/chip-series/TLSR825x-Series/
#	https://wiki.telink-semi.cn/wiki/IDE-and-Tools/IDE-for-TLSR8-Chips/
#	https://github.com/telink-semi/telink_zigbee_sdk/releases/tag/V3.7.1.2
#	https://github.com/telink-semi/tc_platform_sdk/releases/tag/V2.0.0

###
# Telink IoT Studio
#	???\TelinkIoTStudio\bin				- tools like bash, make, 
#	???\TelinkIoTStudio\opt\tc32\bin	- tool chain GCC for tc32 platforms

# directory settings
TELINK_ZIGBEE_SDK ?= $(build_dir)/telink_zigbee_sdk
TL_ZIGBEE_SDK ?= $(TELINK_ZIGBEE_SDK)/tl_zigbee_sdk

#.NOTPARALLEL: $(TELINK_ZIGBEE_SDK)
#$(TELINK_ZIGBEE_SDK):
#	git clone --single-branch https://github.com/telink-semi/telink_zigbee_sdk.git $@
download_sdk:
	git clone --single-branch https://github.com/march42/telink_zigbee_sdk.git $(TELINK_ZIGBEE_SDK)

#$(TL_ZIGBEE_SDK): $(TELINK_ZIGBEE_SDK)
#	needs to be downloaded
$(TL_ZIGBEE_SDK):
	echo ERROR: SDK needs to be downloaded (make download_sdk)
	exit 1

PRE_BUILD		+= $(TL_ZIGBEE_SDK)
include_dirs	:= $(include_dirs) -I$(TL_ZIGBEE_SDK)/platform -I$(TL_ZIGBEE_SDK)/proj/common -I$(TL_ZIGBEE_SDK)/proj

sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/os/ev.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/os/ev_buffer.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/os/ev_poll.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/os/ev_queue.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/os/ev_rtc.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/os/ev_timer.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/usb/app/usbcdc.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/usb/app/usbkb.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/usb/app/usbmouse.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/usb/app/usbvendor.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/usb/usb.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_adc.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_calibration.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_flash.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_gpio.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_hw.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_i2c.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_keyboard.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_nv.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_pm.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_putchar.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_pwm.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_security.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_spi.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_timer.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/drivers/drv_uart.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/common/list.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/common/mempool.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/common/string.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/common/tlPrintf.c
sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/proj/common/utility.c

sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/boot/link_cfg.S

ifeq ($(MCU_CHIP),TLSR_8258)
	ASFLAGS			:= -DMCU_CORE_8258 -DMCU_STARTUP_8258 -I./bootLoader
	boot_link		:= $(TL_ZIGBEE_SDK)/platform/boot/8258/boot_8258.link

	CFLAGS			+= -I$(TL_ZIGBEE_SDK)/platform
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/platform/lib -ldrivers_8258

	CFLAGS			+= -I$(TL_ZIGBEE_SDK)/platform/tc32
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/platform/tc32 -lsoft-fp -lfirmware_encrypt

	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/services/b85m/irq_handler.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/boot/8258/cstartup_8258.S

	CFLAGS			+= -I$(TL_ZIGBEE_SDK)/platform/chip_8258
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_common.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid011460c8.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid1060c8.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid13325e.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid134051.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid136085.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid1360c8.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid1360eb.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid14325e.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash/flash_mid1460c8.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/adc.c
	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/chip_8258/flash.c

	sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/platform/tc32/div_mod.S
endif

#sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/apps/common/firmwareEncryptChk.c
#sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/apps/bootLoader/bootloader.c
#sdk_files		:= $(sdk_files) $(TL_ZIGBEE_SDK)/apps/bootLoader/main.c

#	20:07:21 **** Build of configuration TORSO_bootloader for project tl_zigbee_sdk ****
#	make -j4 all 
#	"../../../tools/tl_link_load.sh" "../../../platform/boot/8258/boot_8258.link" "D:\development\telink_zigbee_sdk\tl_zigbee_sdk\build\tlsr_tc32/boot.link"
#	*****************************************************
#	this is post build!! current configure is :../../../platform/boot/8258/boot_8258.link
#	*****************************************************
#	tc32-elf-gcc -DMCU_CORE_8258 -DMCU_STARTUP_8258 -I../../../apps/bootLoader -c -o "platform/boot/826x/cstartup_826x.o" "../../../platform/boot/826x/cstartup_826x.S"
#	Invoking: TC32 CC/Assembler
#	tc32-elf-gcc -DMCU_CORE_8258 -DMCU_STARTUP_8258 -I../../../apps/bootLoader -c -o "platform/boot/8258/cstartup_8258.o" "../../../platform/boot/8258/cstartup_8258.S"
#	Invoking: TC32 CC/Assembler
#	tc32-elf-gcc -DMCU_CORE_8258 -DMCU_STARTUP_8258 -I../../../apps/bootLoader -c -o "platform/boot/link_cfg.o" "../../../platform/boot/link_cfg.S"
#	Invoking: TC32 CC/Assembler
#	tc32-elf-gcc -DMCU_CORE_8258 -DMCU_STARTUP_8258 -I../../../apps/bootLoader -c -o "div_mod.o" "../../../platform/tc32/div_mod.S"
#	*****************************************************
#TORSO_bootloader.elf
#	Invoking: TC32 C Linker
#	tc32-elf-ld --gc-sections -L../../../platform/lib -T ../boot.link -o "TORSO_bootloader.elf"  ./proj/os/ev.o ./proj/os/ev_buffer.o ./proj/os/ev_poll.o ./proj/os/ev_queue.o ./proj/os/ev_rtc.o ./proj/os/ev_timer.o  ./proj/drivers/usb/app/usbcdc.o ./proj/drivers/usb/app/usbkb.o ./proj/drivers/usb/app/usbmouse.o ./proj/drivers/usb/app/usbvendor.o  ./proj/drivers/usb/usb.o ./proj/drivers/usb/usbdesc.o  ./proj/drivers/drv_adc.o ./proj/drivers/drv_calibration.o ./proj/drivers/drv_flash.o ./proj/drivers/drv_gpio.o ./proj/drivers/drv_hw.o ./proj/drivers/drv_i2c.o ./proj/drivers/drv_keyboard.o ./proj/drivers/drv_nv.o ./proj/drivers/drv_pm.o ./proj/drivers/drv_putchar.o ./proj/drivers/drv_pwm.o ./proj/drivers/drv_security.o ./proj/drivers/drv_spi.o ./proj/drivers/drv_timer.o ./proj/drivers/drv_uart.o  ./proj/common/list.o ./proj/common/mempool.o ./proj/common/string.o ./proj/common/tlPrintf.o ./proj/common/utility.o  ./platform/services/irq_handler.o  ./platform/chip_8258/flash/flash_common.o ./platform/chip_8258/flash/flash_mid011460c8.o ./platform/chip_8258/flash/flash_mid1060c8.o ./platform/chip_8258/flash/flash_mid13325e.o ./platform/chip_8258/flash/flash_mid134051.o ./platform/chip_8258/flash/flash_mid136085.o ./platform/chip_8258/flash/flash_mid1360c8.o ./platform/chip_8258/flash/flash_mid1360eb.o ./platform/chip_8258/flash/flash_mid14325e.o ./platform/chip_8258/flash/flash_mid1460c8.o  ./platform/chip_8258/adc.o ./platform/chip_8258/flash.o  ./platform/boot/8278/cstartup_8278.o  ./platform/boot/826x/cstartup_826x.o  ./platform/boot/8258/cstartup_8258.o  ./platform/boot/link_cfg.o  ./apps/common/firmwareEncryptChk.o  ./apps/bootLoader/bootloader.o ./apps/bootLoader/main.o  ./div_mod.o   -ldrivers_8258 -ldrivers_8258
#	Finished building target: TORSO_bootloader.elf
#	 
#	Invoking: TC32 Create Extended Listing
#	tc32-elf-objdump -x -D -l -S TORSO_bootloader.elf  >"TORSO_bootloader.lst"
#	Invoking: Print Size
#	tc32-elf-size -t TORSO_bootloader.elf
#	   text	   data	    bss	    dec	    hex	filename
#	  19708	     68	   4632	  24408	   5f58	TORSO_bootloader.elf
#	  19708	     68	   4632	  24408	   5f58	(TOTALS)
#	Finished building: sizedummy
#	 
#	Finished building: TORSO_bootloader.lst
#	 
#	"../../../tools/tl_check_fw.sh" TORSO_bootloader tc32
#	*****************************************************
#	this is post build!! current configure is :TORSO_bootloader
#	copy from `TORSO_bootloader.elf' [elf32-littletc32] to `TORSO_bootloader.bin' [binary]
#	File size:27384
#	output done!
#	**************** end of post build ******************
#	 
#	
#	20:07:56 Build Finished. 0 errors, 0 warnings. (took 34s.597ms)
###
