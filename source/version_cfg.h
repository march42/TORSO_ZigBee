/********************************************************************************************************
 * @file    version_cfg.h
 *
 * @brief   This is the header file for version_cfg
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/

#pragma once

#include "comm_cfg.h"

#if defined(MCU_CORE_826x)
	#if (CHIP_8269)
		#define CHIP_TYPE					TLSR_8269
	#else
		#define CHIP_TYPE					TLSR_8267
	#endif
#elif defined(MCU_CORE_8258)
#	if defined(CHIP_TYPE)
		// prevent warning: "CHIP_TYPE" redefined
#	elif (MODULE == ZT3L)
#		define CHIP_TYPE					TLSR_8258_1M
#	else
#		define CHIP_TYPE					TLSR_8258_512K
#	endif
//		#define CHIP_TYPE					TLSR_8258_512K//TLSR_8258_1M
#elif defined(MCU_CORE_8278)
		#define CHIP_TYPE					TLSR_8278
#elif defined(MCU_CORE_B91)
		#define CHIP_TYPE					TLSR_B91
#elif defined(MCU_CORE_B92)
		#define CHIP_TYPE					TLSR_B92
#elif defined(MCU_CORE_TL721X)
		#define CHIP_TYPE					TLSR_TL721X
#elif defined(MCU_CORE_TL321X)
		#define CHIP_TYPE					TLSR_TL321X
#endif

#define APP_RELEASE							0x10//app release 1.0
#define APP_BUILD							0x01//app build 01
#define STACK_RELEASE						0x30//stack release 3.0
#define STACK_BUILD							0x01//stack build 01

/*********************************************************************************************
 * During OTA upgrade, the upgraded device will check the rules of the following three fields.
 * Refer to ZCL OTA specification for details.
 */
#define MANUFACTURER_CODE_TELINK           	0x1141//Telink ID
#define	IMAGE_TYPE							((CHIP_TYPE << 8) | IMAGE_TYPE_LIGHT)
#define	FILE_VERSION					  	((APP_RELEASE << 24) | (APP_BUILD << 16) | (STACK_RELEASE << 8) | STACK_BUILD)

#define ZCL_BASIC_MFG_NAME     		{7,'m','a','r','c','h','4','2'}
#if (TARGET==TORSO)
#	define ZCL_BASIC_MODEL_ID	   	{11,'T','O','R','S','O',' ','l','i','g','h','t'}
#elif (TARGET==TS0501B)
#	define ZCL_BASIC_MODEL_ID	   	{7,'T','S','0','5','0','1','B'}
#elif (TARGET==TS0502B)
#	define ZCL_BASIC_MODEL_ID	   	{7,'T','S','0','5','0','2','B'}
#elif (TARGET==TS0503B)
#	define ZCL_BASIC_MODEL_ID	   	{7,'T','S','0','5','0','3','B'}
#elif (TARGET==TS0504B)
#	define ZCL_BASIC_MODEL_ID	   	{7,'T','S','0','5','0','4','B'}
#elif (TARGET==TS0505B)
#	define ZCL_BASIC_MODEL_ID	   	{7,'T','S','0','5','0','5','B'}
#else
#	define ZCL_BASIC_MODEL_ID	   	{13,'L','E','D','c','o','n','t','r','o','l','l','e','r'}
#endif
#define ZCL_BASIC_SW_BUILD_ID     	{10,'0','2','2','0','2','5','0','2','1','3'}

/* Pre-compiled link configuration. */
#define IS_BOOT_LOADER_IMAGE				0
#define RESV_FOR_APP_RAM_CODE_SIZE			0
#define IMAGE_OFFSET						APP_IMAGE_ADDR
