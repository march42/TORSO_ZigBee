#
# TORSO ZigBee
# (C) Copyright 2024 Marc Hefter, all rights reserved
# LICENSE GPLv3
#

###
# include config.mk fail without error
-include config.mk
###
#	variables
#
#	PROJECTDIR		base directory of the project
#	BUILDDIR		directory to build the binaries
#
#	MODULE			the hardware board (ZT3L, ZYZB010)
#	ZB_ROLE			the ZigBee device role (COORDINATOR, ROUTER, END_DEVICE)
#	
#
#	TC32DIR			the tc32 toolchain directory
#	TL_ZIGBEE_SDK	the ZigBee SDK
#	TOOLSPATH		extra PATH for tools
###

###
# build tools, add TOOLSPATH to the PATH
ifdef TOOLSPATH
	export PATH = $(shell echo ${TOOLSPATH}:$$PATH)
endif
#TC32DIR	:= $(shell echo $$TC32DIR)
# where the TC32 tools can be found
ifdef TC32DIR
	export PATH = $(shell echo ${TC32DIR}/tc32-elf/bin:${TC32DIR}/bin:$$PATH)
	CPPFLAGS	+= -I$(TC32DIR)/tc32-elf/include -I$(TC32DIR)/include
	LDFLAGS		+= -L$(TC32DIR)/tc32-elf/lib -L$(TC32DIR)/lib
endif
# tc32-elf- name of the gnu tools
AS		= tc32-elf-as
CC		= tc32-elf-gcc
LD		= tc32-elf-ld
OBJCOPY	= tc32-elf-objcopy
OBJDUMP	= tc32-elf-objdump
SIZE	= tc32-elf-size
###
# build variables
#	`PROJECTDIR :=`	set directly, expand all used variables
PROJECTDIR	:= $(realpath $(CURDIR))
SOURCEDIR	:= $(PROJECTDIR)/source
ifdef BUILDDIR
#	BUILDDIR set in config.mk or command line
	config_BUILDDIR		:= $(BUILDDIR)
else
#	`BUILDDIR ?= `	set, if not defined yet
	BUILDDIR	?= $(PROJECTDIR)/build/$(MODULE)
endif
###
# TARGET	= TS0501B,TS0502B,TS0503B,TS0504B,TS0505B, TORSO
ifdef TARGET
	config_TARGET		:= $(TARGET)
endif
# MODULE	= ZT3L, ZYZB010
ifdef MODULE
	config_MODULE		:= $(MODULE)
else
	MODULE				:= ZT3L
endif
###
# ZB_ROLE	= COORDINATOR, ROUTER, END_DEVICE
ifdef ZB_ROLE
	config_ZB_ROLE		:= $(ZB_ROLE)
else
	ZB_ROLE				:= ROUTER
endif
###
# the telink_zigbee_sdk git submodule
ifdef TL_ZIGBEE_SDK
	config_TL_ZIGBEE_SDK	:= $(TL_ZIGBEE_SDK)
else
	TL_ZIGBEE_SDK		:= $(PROJECTDIR)/telink_zigbee_sdk/tl_zigbee_sdk
endif
###
# the TlsrComSwireWriter git submodule
ifdef TLSR_FLASHER
	config_TLSR_FLASHER	:= $(TLSR_FLASHER)
else
	TLSR_FLASHER		:= python $(PROJECTDIR)/TlsrComSwireWriter/TLSR825xComFlasher.py
endif
###

###
# build settings
#include_dirs			+= $(SOURCEDIR) $(SOURCEDIR)/common
#CFLAGS					+= -std=gnu99 -Wall -Werror
CFLAGS					+= -Werror
###
#	LED_MODE channels WHITE	= 0x01,0x02,0x04,0x08
#	LED_MODE channels COLOR	= 0x10,0x20,0x40,0x80
LED_MODE_DIMMER		= 0x01
LED_MODE_CCT		= 0x03
LED_MODE_RGB		= 0x70
LED_MODE_RGBW		= 0x71
LED_MODE_RGBCCT		= 0x73
LED_MODE_MULTIPLE	= 0xFF
# board settings
ifdef LED_MODE
#	allready specified
else ifeq ($(TARGET),TORSO)
	CPPFLAGS		+= -D__LIGHT__MARCH42_TORSO__=1
	LED_MODE		= $(LED_MODE_RGBW)
else ifeq ($(TARGET),TS0501B)
	LED_MODE		= $(LED_MODE_DIMMER)
else ifeq ($(TARGET),TS0502B)
	LED_MODE		= $(LED_MODE_CCT)
else ifeq ($(TARGET),TS0503B)
	LED_MODE		= $(LED_MODE_RGB)
else ifeq ($(TARGET),TS0504B)
	LED_MODE		= $(LED_MODE_RGBW)
else ifeq ($(TARGET),TS0505B)
	LED_MODE		= $(LED_MODE_RGBCCT)
else
	LED_MODE		= $(LED_MODE_DIMMER)
endif
# SOC module
ifeq ($(MODULE),ZT3L)
#	-DMCU_CORE_8258=1 -DTUYA_ZT3L=1 -D__PROJECT_TL_BOOT_LOADER__=1
	MCU_CHIP			:= TLSR_8258
	MCU_CORE_8258		:= 1
	CPPFLAGS			+= -DMODULE_ZT3L=1 -DMCU_CORE_8258=1
	CHIP_TYPE			:= TLSR_8258_1M
	BOOT_LOADER_MODE	:= 1
else ifeq ($(MODULE),ZYZB010)
	MCU_CHIP			:= TLSR_8258
	MCU_CORE_8258		:= 1
	CPPFLAGS			+= -DMODULE_ZYZB010=1 -DMCU_CORE_8258=1
	CHIP_TYPE			:= TLSR_8258_512K
	BOOT_LOADER_MODE	:= 0
endif
CPPFLAGS		+= -DBOOT_LOADER_MODE=$(BOOT_LOADER_MODE) -DTARGET=$(TARGET) -D$(ZB_ROLE)=1
CFLAGS			+= -ffunction-sections -fdata-sections -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions
LDFLAGS			+= --gc-sections
#bin_files		+= $(BUILDDIR)/torso-light.bin 

###
# ZigBee SDK
CPPFLAGS		+= -I$(TL_ZIGBEE_SDK)/zigbee/common/includes
#CFLAGS			+= -I$(TL_ZIGBEE_SDK)/platform -I$(TL_ZIGBEE_SDK)/proj/common -I$(TL_ZIGBEE_SDK)/proj
sdk_DIRS		+= $(TL_ZIGBEE_SDK)/platform/boot $(TL_ZIGBEE_SDK)/platform/tc32
sdk_DIRS		+= $(TL_ZIGBEE_SDK)/proj/common $(TL_ZIGBEE_SDK)/proj/drivers $(TL_ZIGBEE_SDK)/proj/os
zigbee_DIRS		+= $(TL_ZIGBEE_SDK)/zigbee/common $(TL_ZIGBEE_SDK)/zigbee/af $(TL_ZIGBEE_SDK)/zigbee/aps $(TL_ZIGBEE_SDK)/zigbee/bdb
zigbee_DIRS		+= $(TL_ZIGBEE_SDK)/zigbee/gp $(TL_ZIGBEE_SDK)/zigbee/mac $(TL_ZIGBEE_SDK)/zigbee/ota $(TL_ZIGBEE_SDK)/zigbee/ss
zigbee_DIRS		+= $(TL_ZIGBEE_SDK)/zigbee/wwah $(TL_ZIGBEE_SDK)/zigbee/zdo
zigbee_DIRS		+= $(TL_ZIGBEE_SDK)/zigbee/zcl $(TL_ZIGBEE_SDK)/zigbee/zcl/commissioning $(TL_ZIGBEE_SDK)/zigbee/zcl/general $(TL_ZIGBEE_SDK)/zigbee/zcl/light_color_control
zigbee_DIRS		+= $(TL_ZIGBEE_SDK)/zigbee/zcl/ota_upgrading $(TL_ZIGBEE_SDK)/zigbee/zcl/zcl_wwah $(TL_ZIGBEE_SDK)/zigbee/zcl/zll_commissioning
zigbee_DIRS		+= $(TL_ZIGBEE_SDK)/zbhci $(TL_ZIGBEE_SDK)/zbhci/uart
#zigbee_DIRS		+= $(shell find $(TL_ZIGBEE_SDK)/zigbee -type d) $(shell find $(TL_ZIGBEE_SDK)/zbhci -type d)
ifdef MCU_CORE_8258
	CPPFLAGS		+= -DMCU_CORE_8258=$(MCU_CORE_8258) -DMCU_STARTUP_8258=$(MCU_CORE_8258)
#	ASFLAGS			:= -DMCU_CORE_8258 -DMCU_STARTUP_8258
	boot_link		:= $(TL_ZIGBEE_SDK)/platform/boot/8258/boot_8258.link
#	CFLAGS			+= -I$(TL_ZIGBEE_SDK)/platform/chip_8258
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/platform/tc32
	LDLIBS			+= -lsoft-fp -lfirmware_encrypt
	sdk_DIRS		+= $(TL_ZIGBEE_SDK)/platform/boot/8258 $(TL_ZIGBEE_SDK)/platform/chip_8258 $(TL_ZIGBEE_SDK)/platform/chip_8258/flash
	sdk_DIRS		+= $(TL_ZIGBEE_SDK)/platform/services/b85m
#	sdk_LIBS		+= $(TL_ZIGBEE_SDK)/platform/lib
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/platform/lib
	LDLIBS			+= -ldrivers_8258
endif
ifeq ($(ZB_ROLE),COORDINATOR)
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/zigbee/lib/tc32
	LDLIBS			+= -lzb_coordinator
else ifeq ($(ZB_ROLE),ROUTER)
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/zigbee/lib/tc32
	LDLIBS			+= -lzb_router
else ifeq ($(ZB_ROLE),END_DEVICE)
	LDFLAGS			+= -L$(TL_ZIGBEE_SDK)/zigbee/lib/tc32
	LDLIBS			+= -lzb_ed
endif
sdk_SOURCES 	+= $(foreach dir,$(sdk_DIRS),$(wildcard $(dir)/*.c))
sdk_HEADERS 	+= $(foreach dir,$(sdk_DIRS),$(wildcard $(dir)/*.h))
sdk_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_SOURCES:.c=.c.o))
sdk_ASMS		+= $(foreach dir,$(sdk_DIRS),$(wildcard $(dir)/*.S))
sdk_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_ASMS:.S=.S.o))
zigbee_SOURCES 	+= $(foreach dir,$(zigbee_DIRS),$(wildcard $(dir)/*.c))
zigbee_HEADERS 	+= $(foreach dir,$(zigbee_DIRS),$(wildcard $(dir)/*.h))
zigbee_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_SOURCES:.c=.c.o))
zigbee_ASMS		+= $(foreach dir,$(zigbee_DIRS),$(wildcard $(dir)/*.S))
zigbee_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_ASMS:.S=.S.o))
###

###
	ldr_CPPFLAGS	= -D__PROJECT_TL_BOOT_LOADER__=1
	ldr_CPPFLAGS	+= -I$(PROJECTDIR)/bootloader -I$(SOURCEDIR) -I$(TL_ZIGBEE_SDK)/platform -I$(TL_ZIGBEE_SDK)/proj/common -I$(TL_ZIGBEE_SDK)/proj
	ldr_source		= $(PROJECTDIR)/bootloader/main.c $(PROJECTDIR)/bootloader/bootloader.c $(PROJECTDIR)/source/common/firmwareEncryptChk.c
	ldr_HEADERS		= $(wildcard $(PROJECTDIR)/bootloader/*.h) $(wildcard $(PROJECTDIR)/source/*.h) $(wildcard $(PROJECTDIR)/source/common/*.h) $(sdk_HEADERS) $(zigbee_HEADERS)
	ldr_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/bootloader,$(ldr_source:.c=.c.o))
	ldr_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk-bootloader,$(sdk_SOURCES:.c=.c.o))
	ldr_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk-bootloader,$(sdk_ASMS:.S=.S.o))
ifeq ($(BOOT_LOADER_MODE),1)
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/bootloader.elf $(BUILDDIR)/bootloader.lst
	bin_files		+= $(BUILDDIR)/bootloader.bin
endif

#	sdk_DIRS		+= $(TL_ZIGBEE_SDK)/zigbee/common
	led_CPPFLAGS	= -D__PROJECT_TL_DIMMABLE_LIGHT__=1
	led_CPPFLAGS	+= -I$(SOURCEDIR) -I$(SOURCEDIR)/common -I$(TL_ZIGBEE_SDK)/platform -I$(TL_ZIGBEE_SDK)/proj/common -I$(TL_ZIGBEE_SDK)/proj
	led_CPPFLAGS	+= -I$(TL_ZIGBEE_SDK)/zigbee/zbapi -I$(TL_ZIGBEE_SDK)/zigbee/zcl -I$(TL_ZIGBEE_SDK)/zigbee/gp -I$(TL_ZIGBEE_SDK)/zigbee/bdb/includes
	led_CPPFLAGS	+= -I$(TL_ZIGBEE_SDK)/zigbee/ota
	led_CPPFLAGS	+= -I$(TL_ZIGBEE_SDK)/zbhci
	led_source		= $(wildcard $(SOURCEDIR)/*.c) $(wildcard $(SOURCEDIR)/common/*.c)
	led_HEADERS		= $(wildcard $(PROJECTDIR)/source/*.h) $(wildcard $(PROJECTDIR)/source/common/*.h) $(sdk_HEADERS) $(zigbee_HEADERS)
	led_dimmer_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/led_dimmer,$(led_source:.c=.c.o))
	led_cct_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/led_cct,$(led_source:.c=.c.o))
	led_rgb_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/led_rgb,$(led_source:.c=.c.o))
	led_rgbw_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/led_rgbw,$(led_source:.c=.c.o))
	led_rgbcct_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/led_rgbcct,$(led_source:.c=.c.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_SOURCES:.c=.c.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_ASMS:.S=.S.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_SOURCES:.c=.c.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_ASMS:.S=.S.o))
	torso_OBJS		= $(subst $(PROJECTDIR),$(BUILDDIR)/torso,$(led_source:.c=.c.o))

ifeq ($(TARGET),TORSO)
#	special LED controller
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/torso.elf $(BUILDDIR)/torso.lst
	firmware_app	+= $(BUILDDIR)/torso.bin
else ifeq ($(LED_MODE),0x01)
#	single color WHITE
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/led_dimmer.elf $(BUILDDIR)/led_dimmer.lst
	firmware_app	+= $(BUILDDIR)/led_dimmer.bin
else ifeq ($(LED_MODE),0x03)
#	dual channel WHITE
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/led_cct.elf $(BUILDDIR)/led_cct.lst
	firmware_app	+= $(BUILDDIR)/led_cct.bin
else ifeq ($(LED_MODE),0x70)
#	3 channel color
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/led_rgb.elf $(BUILDDIR)/led_rgb.lst
	firmware_app	+= $(BUILDDIR)/led_rgb.bin
else ifeq ($(LED_MODE),0x71)
#	3 channel color and single color WHITE
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/led_rgbw.elf $(BUILDDIR)/led_rgbw.lst
	firmware_app	+= $(BUILDDIR)/led_rgbw.bin
else ifeq ($(LED_MODE),0x73)
#	3 channel RGB and 2 channel WHITE
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/led_rgbcct.elf $(BUILDDIR)/led_rgbcct.lst
	firmware_app	= $(BUILDDIR)/led_rgbcct.bin
else
#	unplanned for now, this should not happen
endif
bin_files			+= $(firmware_app)

.PHONY: all clean allclean distclean
###
# all is first, so it is done if none specified
all: build_info config.mk $(bin_files)

clean:
	$(info removing built objects)
	rm -f $(sdk_OBJS) $(zigbee_OBJS) $(led_OBJS) $(ldr_OBJS) $(OBJS) $(extra_files)

allclean:
	$(info removing built dir)
	rm -fR $(BUILDDIR)

distclean: clean
	$(info removing ALL built objects)
	rm -fR $(BUILDDIR) config.mk $(bin_files)

.PHONY: build_info
build_info:
	$(info )
	$(info BUILDING INFO)
	$(info MODULE           = $(MODULE))
	$(info CHIP_TYPE        = $(CHIP_TYPE))
	$(info BOOT_LOADER_MODE = $(BOOT_LOADER_MODE))
	$(info LED_MODE         = $(LED_MODE))
	$(info TARGET           = $(TARGET))
	$(info )

###
# config
config.mk:
ifndef MODULE
	$(error MODULE must be defined as ZT3L or ZYZB010)
else ifndef MCU_CHIP
	$(error MCU_CHIP must be defined as TLSR_8258)
else
	echo '# config.mk' > config.mk
	echo '# $(MAKE)' >> config.mk
	echo '# project settings' >> config.mk
ifndef config_TARGET
	echo '#TARGET' >> config.mk
else
	echo 'TARGET            := $(TARGET)' >> config.mk
endif
ifndef config_MODULE
	echo '#MODULE' >> config.mk
else
	echo 'MODULE            := $(MODULE)' >> config.mk
endif
ifndef config_ZB_ROLE
	echo '#ZB_ROLE' >> config.mk
else
	echo 'ZB_ROLE          := $(config_ZB_ROLE)' >> config.mk
endif
	echo '# directory settings' >> config.mk
ifndef config_BUILDDIR
	echo '#BUILDDIR' >> config.mk
else
	echo 'BUILDDIR         := $(config_BUILDDIR)' >> config.mk
endif
ifndef config_TL_ZIGBEE_SDK
	echo '#TL_ZIGBEE_SDK' >> config.mk
else
	echo 'TL_ZIGBEE_SDK    := $(config_TL_ZIGBEE_SDK)' >> config.mk
endif
ifndef TC32DIR
	echo '#TC32DIR' >> config.mk
else
	echo 'TC32DIR          := $(TC32DIR)' >> config.mk
endif
ifndef TOOLSPATH
	echo '#TOOLSPATH' >> config.mk
else
	echo 'TOOLSPATH        := $(TOOLSPATH)' >> config.mk
endif
ifndef config_TLSR_FLASHER
	echo '#TLSR_FLASHER' >> config.mk
else
	echo 'TLSR_FLASHER     := $(config_TLSR_FLASHER)' >> config.mk
endif
ifndef FLASHER
	echo '#FLASHER' >> config.mk
else
	echo 'FLASHER          := $(FLASHER)' >> config.mk
endif
endif

###
# write firmware
.PHONY: write_firmware
write_firmware: $(bin_files)
ifeq ($(CHIP_TYPE),TLSR_8258_1M)
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) rf 0xFE000 0x02000 dump_FCFG_MAC.bin
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) es 0x00000 0xFE000
else ifeq ($(CHIP_TYPE),TLSR_8258_512K)
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) rf 0x76000 0x02000 dump_MAC_FCFG.bin
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) es 0x00000 0x76000
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) es 0x78000 0x08000
endif
ifeq ($(BOOT_LOADER_MODE),1)
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) wf 0x00000 $(BUILDDIR)/bootloader.bin
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) wf 0x08000 $(firmware_app)
else
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) wf 0x00000 $(firmware_app)
endif

.PHONY: run_firmware
run_firmware: write_firmware
	python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) --run

###
# directories
ifneq ($(BUILDDIR),)
#.PHONY: $(BUILDDIR)
$(BUILDDIR):
	$(info Creating	BUILDDIR	$@)
	@mkdir -p $@
endif
#
.PHONY: $(TL_ZIGBEE_SDK)
$(TL_ZIGBEE_SDK):
	$(info TL_ZIGBEE_SDK in $(TL_ZIGBEE_SDK))
	$(error SDK needs to be downloaded `git submodules update --init`)

###
# ZigBee SDK rules
$(BUILDDIR)/boot.link: $(boot_link) | $(BUILDDIR)
	$(info Creating	$@)
#	"../../../tools/tl_link_load.sh" "../../../platform/boot/8258/boot_8258.link" "${workspace_loc:/${ProjName}}/boot.link"
	cp -f $< $@

###
#	torso rules
$(BUILDDIR)/torso.elf: $(BUILDDIR)/boot.link $(led_OBJS) $(torso_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(torso_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/torso.lst: $(BUILDDIR)/torso.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	torso
torso: $(BUILDDIR)/torso.bin
	$(info building $@)
$(BUILDDIR)/torso.bin: $(BUILDDIR)/torso.elf $(BUILDDIR)/torso.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/torso
$(BUILDDIR)/torso:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/torso/%.c.o : %.c $(led_HEADERS)	| $(BUILDDIR)/torso
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE) -D__LIGHT__MARCH42_TORSO__=1 $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/torso/%.S.o : %.S $(led_HEADERS)	| $(BUILDDIR)/torso
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE) -D__LIGHT__MARCH42_TORSO__=1 $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

###
#	led_rgbcct rules
$(BUILDDIR)/led_rgbcct.elf: $(BUILDDIR)/boot.link $(led_OBJS) $(led_rgbcct_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(led_rgbcct_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/led_rgbcct.lst: $(BUILDDIR)/led_rgbcct.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	led_rgbcct
led_rgbcct: $(BUILDDIR)/led_rgbcct.bin
	$(info building $@)
$(BUILDDIR)/led_rgbcct.bin: $(BUILDDIR)/led_rgbcct.elf $(BUILDDIR)/led_rgbcct.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/led_rgbcct
$(BUILDDIR)/led_rgbcct:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/led_rgbcct/%.c.o : %.c $(led_HEADERS)	| $(BUILDDIR)/led_rgbcct
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGBCCT) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/led_rgbcct/%.S.o : %.S $(led_HEADERS)	| $(BUILDDIR)/led_rgbcct
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGBCCT) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

###
#	led_cct rules
$(BUILDDIR)/led_cct.elf: $(BUILDDIR)/boot.link $(led_OBJS) $(led_cct_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(led_cct_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/led_cct.lst: $(BUILDDIR)/led_cct.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	led_cct
led_cct: $(BUILDDIR)/led_cct.bin
	$(info building $@)
$(BUILDDIR)/led_cct.bin: $(BUILDDIR)/led_cct.elf $(BUILDDIR)/led_cct.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/led_cct
$(BUILDDIR)/led_cct:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/led_cct/%.c.o : %.c $(led_HEADERS)	| $(BUILDDIR)/led_cct
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_CCT) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/led_cct/%.S.o : %.S $(led_HEADERS)	| $(BUILDDIR)/led_cct
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_CCT) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

###
#	led_dimmer rules
$(BUILDDIR)/led_dimmer.elf: $(BUILDDIR)/boot.link $(led_OBJS) $(led_dimmer_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(led_dimmer_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/led_dimmer.lst: $(BUILDDIR)/led_dimmer.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	led_dimmer
led_dimmer: $(BUILDDIR)/led_dimmer.bin
	$(info building $@)
$(BUILDDIR)/led_dimmer.bin: $(BUILDDIR)/led_dimmer.elf $(BUILDDIR)/led_dimmer.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/led_dimmer
$(BUILDDIR)/led_dimmer:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/led_dimmer/%.c.o : %.c $(led_HEADERS)	| $(BUILDDIR)/led_dimmer
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_DIMMER) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/led_dimmer/%.S.o : %.S $(led_HEADERS)	| $(BUILDDIR)/led_dimmer
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_DIMMER) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

###
#	led_rgb rules
$(BUILDDIR)/led_rgb.elf: $(BUILDDIR)/boot.link $(led_OBJS) $(led_rgb_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(led_rgb_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/led_rgb.lst: $(BUILDDIR)/led_rgb.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	led_rgb
led_rgb: $(BUILDDIR)/led_rgb.bin
	$(info building $@)
$(BUILDDIR)/led_rgb.bin: $(BUILDDIR)/led_rgb.elf $(BUILDDIR)/led_rgb.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/led_rgb
$(BUILDDIR)/led_rgb:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/led_rgb/%.c.o : %.c $(led_HEADERS)	| $(BUILDDIR)/led_rgb
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGB) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/led_rgb/%.S.o : %.S $(led_HEADERS)	| $(BUILDDIR)/led_rgb
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGB) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

###
#	led_rgbw rules
$(BUILDDIR)/led_rgbw.elf: $(BUILDDIR)/boot.link $(led_OBJS) $(led_rgbw_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(led_rgbw_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/led_rgbw.lst: $(BUILDDIR)/led_rgbw.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	led_rgbw
led_rgbw: $(BUILDDIR)/led_rgbw.bin
	$(info building $@)
$(BUILDDIR)/led_rgbw.bin: $(BUILDDIR)/led_rgbw.elf $(BUILDDIR)/led_rgbw.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/led_rgbw
$(BUILDDIR)/led_rgbw:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/led_rgbw/%.c.o : %.c $(led_HEADERS)	| $(BUILDDIR)/led_rgbw
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGBW) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/led_rgbw/%.S.o : %.S $(led_HEADERS)	| $(BUILDDIR)/led_rgbw
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGBW) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@


###
# SDK
#	compile with LED_MODE=LED_MODE_RGBCCT to enable building the neccessary routines
#.PHONY: $(BUILDDIR)/sdk
$(BUILDDIR)/sdk:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/sdk/%.c.o : $(TL_ZIGBEE_SDK)/%.c $(led_HEADERS)	| $(BUILDDIR)/sdk
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGBCCT) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/sdk/%.S.o : $(TL_ZIGBEE_SDK)/%.S $(led_HEADERS)	| $(BUILDDIR)/sdk
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) -DLED_MODE=$(LED_MODE_RGBCCT) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

###
#	bootloader rules
$(BUILDDIR)/bootloader.elf: $(BUILDDIR)/boot.link $(ldr_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(ldr_OBJS) $(LDLIBS)
	$(SIZE) -t $@

$(BUILDDIR)/bootloader.lst: $(BUILDDIR)/bootloader.elf
	$(info Creating	$@)
	$(OBJDUMP) -x -D -l -S $< >$@

.PHONY:	bootloader
bootloader: $(BUILDDIR)/bootloader.bin
	$(info building $@)
$(BUILDDIR)/bootloader.bin: $(BUILDDIR)/bootloader.elf $(BUILDDIR)/bootloader.lst
	$(info Creating	$@)
#	$(TL_ZIGBEE_SDK)/tools/tl_check_fw.sh" bootloader tc32
	$(OBJCOPY) -v -O binary $< $@
	$(TL_ZIGBEE_SDK)/tools/tl_check_fw2 $@

#.PHONY: $(BUILDDIR)/bootloader
$(BUILDDIR)/bootloader:
	$(info Creating	$@)
	@mkdir -p $@
ifeq ($(BOOT_LOADER_MODE),1)
#	building for bootloader
else
	$(error ERROR: Your configuration is set to build without bootloader.)
endif

$(BUILDDIR)/bootloader/%.c.o : %.c	| $(BUILDDIR)/bootloader
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ldr_CPPFLAGS) $(CPPFLAGS) $(ldr_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/bootloader/%.S.o : %.S	| $(BUILDDIR)/bootloader
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(ldr_ASFLAGS) $(ldr_CPPFLAGS) $(CPPFLAGS) $(ldr_CFLAGS) $(CFLAGS) -c $< -o $@

#.PHONY: $(BUILDDIR)/sdk-bootloader
$(BUILDDIR)/sdk-bootloader:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/sdk-bootloader/%.c.o : $(TL_ZIGBEE_SDK)/%.c $(ldr_HEADERS)	| $(BUILDDIR)/sdk-bootloader
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ldr_CPPFLAGS) $(CPPFLAGS) $(ldr_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/sdk-bootloader/%.S.o : $(TL_ZIGBEE_SDK)/%.S $(ldr_HEADERS)	| $(BUILDDIR)/sdk-bootloader
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(ldr_ASFLAGS) $(ldr_CPPFLAGS) $(CPPFLAGS) $(ldr_CFLAGS) $(CFLAGS) -c $< -o $@
