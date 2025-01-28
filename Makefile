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
CPPFLAGS				+= -D__LIGHT__MARCH42_TORSO__=1
###
# board settings
ifeq ($(MODULE),ZT3L)
#	-DMCU_CORE_8258=1 -DTUYA_ZT3L=1 -D__PROJECT_TL_BOOT_LOADER__=1
	MCU_CHIP			:= TLSR_8258
	MCU_CORE_8258		:= 1
	CPPFLAGS			+= -DMODULE_ZT3L=1 -DMCU_CORE_8258=1
	CHIP_TYPE			:= TLSR_8258_1M
	BOOT_LOADER_MODE	:= 1
	LED_RGBCCT_MODE		:= 1
#	src_files			+= 
else ifeq ($(MODULE),ZYZB010)
	MCU_CHIP			:= TLSR_8258
	MCU_CORE_8258		:= 1
	CPPFLAGS			+= -DMODULE_ZYZB010=1 -DMCU_CORE_8258=1
	CHIP_TYPE			:= TLSR_8258_512K
	BOOT_LOADER_MODE	:= 1
	LED_RGBCCT_MODE		:= 1
endif
CPPFLAGS		+= -DBOOT_LOADER_MODE=$(BOOT_LOADER_MODE) -D$(ZB_ROLE)=1
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
sdk_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_SOURCES:.c=.c.o))
sdk_ASMS		+= $(foreach dir,$(sdk_DIRS),$(wildcard $(dir)/*.S))
sdk_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_ASMS:.S=.S.o))
zigbee_SOURCES 	+= $(foreach dir,$(zigbee_DIRS),$(wildcard $(dir)/*.c))
zigbee_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_SOURCES:.c=.c.o))
zigbee_ASMS		+= $(foreach dir,$(zigbee_DIRS),$(wildcard $(dir)/*.S))
zigbee_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_ASMS:.S=.S.o))
###

###
	ldr_CPPFLAGS	= -D__PROJECT_TL_BOOT_LOADER__=1
	ldr_CPPFLAGS	+= -I$(PROJECTDIR)/bootloader -I$(SOURCEDIR) -I$(TL_ZIGBEE_SDK)/platform -I$(TL_ZIGBEE_SDK)/proj/common -I$(TL_ZIGBEE_SDK)/proj
	ldr_source		= $(PROJECTDIR)/bootloader/main.c $(PROJECTDIR)/bootloader/bootloader.c $(PROJECTDIR)/source/common/firmwareEncryptChk.c
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
	led_OBJS		+= $(subst $(PROJECTDIR),$(BUILDDIR)/led_rgbcct,$(led_source:.c=.c.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_SOURCES:.c=.c.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(sdk_ASMS:.S=.S.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_SOURCES:.c=.c.o))
	led_OBJS		+= $(subst $(TL_ZIGBEE_SDK),$(BUILDDIR)/sdk,$(zigbee_ASMS:.S=.S.o))
ifeq ($(LED_RGBCCT_MODE),1)
	extra_files		+= $(BUILDDIR)/boot.link $(BUILDDIR)/led_rgbcct.elf $(BUILDDIR)/led_rgbcct.lst
	bin_files		+= $(BUILDDIR)/led_rgbcct.bin
endif

clean:
	$(info removing built objects)
	rm -f $(sdk_OBJS) $(zigbee_OBJS) $(led_OBJS) $(ldr_OBJS) $(OBJS) $(extra_files)

distclean: clean
	$(info removing ALL built objects)
	rm -fR $(BUILDDIR) config.mk $(bin_files)

all: config.mk $(bin_files)

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
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) rf 0xFE000 0x02000 dump_FCFG_MAC.bin)
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) es 0x00000 0xFE000)
else ifeq ($(CHIP_TYPE),TLSR_8258_512K)
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) rf 0x76000 0x02000 dump_MAC_FCFG.bin)
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) es 0x00000 0x76000)
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) es 0x78000 0x08000)
endif
ifeq ($(BOOT_LOADER_MODE),1)
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) wf 0x00000 $(BUILDDIR)/bootloader.bin)
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) wf 0x08000 $(BUILDDIR)/led_rgbcct.bin)
else
	$(info python TlsrComSwireWriter/TLSR825xComFlasher.py $(FLASHER) wf 0x00000 $(BUILDDIR)/led_rgbcct.bin)
endif

###
# directories
ifneq ($(BUILDDIR),)
.PHONY: $(BUILDDIR)
$(BUILDDIR):
	$(info Creating $@)
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
#	led_rgbcct rules
$(BUILDDIR)/led_rgbcct.elf: $(BUILDDIR)/boot.link $(led_OBJS)
	$(info Linking	$@)
	$(LD) $(LDFLAGS) -T $(BUILDDIR)/boot.link -o $@ $(led_OBJS) $(LDLIBS)
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

.PHONY: $(BUILDDIR)/led_rgbcct
$(BUILDDIR)/led_rgbcct:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/led_rgbcct/%.c.o : %.c	| $(BUILDDIR)/led_rgbcct
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/led_rgbcct/%.S.o : %.S	| $(BUILDDIR)/led_rgbcct
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: $(BUILDDIR)/sdk
$(BUILDDIR)/sdk:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/sdk/%.c.o : $(TL_ZIGBEE_SDK)/%.c	| $(BUILDDIR)/sdk
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/sdk/%.S.o : $(TL_ZIGBEE_SDK)/%.S	| $(BUILDDIR)/sdk
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(led_ASFLAGS) $(led_CPPFLAGS) $(CPPFLAGS) $(led_CFLAGS) $(CFLAGS) -c $< -o $@

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

.PHONY: $(BUILDDIR)/bootloader
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

.PHONY: $(BUILDDIR)/sdk-bootloader
$(BUILDDIR)/sdk-bootloader:
	$(info Creating	$@)
	@mkdir -p $@

$(BUILDDIR)/sdk-bootloader/%.c.o : $(TL_ZIGBEE_SDK)/%.c	| $(BUILDDIR)/sdk-bootloader
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ldr_CPPFLAGS) $(CPPFLAGS) $(ldr_CFLAGS) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/sdk-bootloader/%.S.o : $(TL_ZIGBEE_SDK)/%.S	| $(BUILDDIR)/sdk-bootloader
	$(info Compiling	$<)
	mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) $(ldr_ASFLAGS) $(ldr_CPPFLAGS) $(CPPFLAGS) $(ldr_CFLAGS) $(CFLAGS) -c $< -o $@
