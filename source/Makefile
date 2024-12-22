#
# TORSO ZigBee
# (C) Copyright 2024 Marc Hefter, all rights reserved
# LICENSE GPLv3
#

# include, fail without error
-include config.mk

# include SDK build rules
include telink_zigbee_sdk.mk

# build variables
build_dir	= ../build
PRE_BUILD	= $(build_dir) config.mk
src_files	= 
asm_files	= 
obj_files	= 
bin_files	= 
#bin_files	+= $(build_dir)/$(BOARD)-application.bin

###
# settings
CC		?= tc32-elf-gcc
LD		?= tc32-elf-ld
OBJDUMP	?= tc32-elf-objdump
SIZE	?= tc32-elf-size
###
ifeq ($(OS),Windows_NT)
	RM			:= del /F /Q
	RMDIR		:= RMDIR /S /Q
	MKDIR		:= mkdir
else
	RM			:= rm -f
	RMDIR		:= rm -fR
	MKDIR		:= mkdir -p
endif
###
include_dirs	+= -I./common -I./bootLoader
CFLAGS			+= $(CFLAGS) -ffunction-sections -fdata-sections $(include_dirs) 
#CFLAGS			+= $(CFLAGS) -DTUYA_ZT3L=1 -DMCU_CORE_8258=1 -D__PROJECT_TL_BOOT_LOADER__=1 
CFLAGS			+= $(CFLAGS) -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions 

ifeq ($(BOARD),ZT3L)
#	-DMCU_CORE_8258=1 -DTUYA_ZT3L=1 -D__PROJECT_TL_BOOT_LOADER__=1
	MCU_CHIP			:= TLSR_8258
	MCU_CORE_8258		:= 1
	CHIP_TYPE			:= TLSR_8258_1M
	BOOT_LOADER_MODE	:= 1
	ldr_source	= bootLoader/main.c bootLoader/bootloader.c common/firmwareEncryptChk.c
	bin_files	+= $(build_dir)/$(BOARD)-bootloader.bin
	C_EXTRA		+= -DMODULE_ZT3L=1 -DMCU_CORE_8258=1 -DCHIP_TYPE=$(CHIP_TYPE) -DBOOT_LOADER_MODE=$(BOOT_LOADER_MODE)
	src_files	+= 
else ifeq ($(BOARD),ZYZB010)
	MCU_CHIP			:= TLSR_8258
	MCU_CORE_8258		:= 1
	CHIP_TYPE			:= TLSR_8258_512K
	BOOT_LOADER_MODE	:= 0
	C_EXTRA		+= -DMODULE_ZYZB010=1 -DMCU_CORE_8258=1 -DCHIP_TYPE=$(CHIP_TYPE) -DBOOT_LOADER_MODE=$(BOOT_LOADER_MODE)
endif

###
#.SUFFIXES: .c .o
###
# Default target
.PHONY: all clean distclean
all: $(bin_files)

clean:
	rm -f $(obj_files) $(bin_files)
	rm -fR $(build_dir)/$(BOARD)

distclean: clean
	rm -f config.mk
	rm -fR $(build_dir)

.PHONY: config
config: config.mk
config.mk:
ifndef BOARD
	echo ERROR: BOARD must be defined as ZT3L or ZYZB010
	exit 1
else ifndef MCU_CHIP
	echo ERROR: MCU_CHIP must be defined as TLSR_8258
	exit 1
else
	echo '# config.mk' > config.mk
	echo '# $(MAKE)' >> config.mk
	echo 'BOARD            := $(BOARD)' >> config.mk
	echo 'MCU_CHIP         := $(MCU_CHIP)' >> config.mk
	echo 'MCU_CORE_8258    := $(MCU_CORE_8258)' >> config.mk
	echo 'CHIP_TYPE        := $(CHIP_TYPE)' >> config.mk
	echo 'BOOT_LOADER_MODE := $(BOOT_LOADER_MODE)' >> config.mk
endif

ifneq ($(build_dir),)
$(build_dir): config.mk
	$(MKDIR) $(build_dir)
$(build_dir)/$(BOARD): config.mk $(build_dir)
	$(MKDIR) $(build_dir)/$(BOARD)
endif

# compile *.c
%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# compile *.S
%.o : %.S
	$(CC) -c $(ASFLAGS) $< -o $@
