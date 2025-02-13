/********************************************************************************************************
 * @file	board_8258_ZT3L.h
 *
 * @brief	This is the header file for Tuya ZT3L module
 *
 * @version 1.0.release7	2024-12-20
 * 
 * 			derrived from Telink ZigBee 3.0 SDK
 * 
 * @author	Marc Hefter
 * @date	2024
 * 
 * @author	Zigbee Group
 * @date	2021
 *
 * @par	 Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
 *
 *			Licensed under the Apache License, Version 2.0 (the "License");
 *			you may not use this file except in compliance with the License.
 *			You may obtain a copy of the License at
 *
 *				http://www.apache.org/licenses/LICENSE-2.0
 *
 *			Unless required by applicable law or agreed to in writing, software
 *			distributed under the License is distributed on an "AS IS" BASIS,
 *			WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *			See the License for the specific language governing permissions and
 *			limitations under the License.
 *
 *******************************************************************************************************/

#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/*	board specifications ZT3L
 * MCU	TeLink TLSR8258 controller (32 pin, 1 MB flash)
 * PWR	5-24V DC
 *	***	pinout
 * 1	RST (RESET, active low, connected to EN)
 * 2	C4	(ADC)
 * 3	EN	(ENABLE, active high, connected to RST)
 * 4	D7	IR_IN (? DEBUG_INFO_TX_PIN)
 * 5	D2	(PWM) LED_CH4, LED_B
 * 6	C3	(PWM) LED_CH3, LED_G
 * 7	C2	(PWM) LED_CH5, LED_WW
 * 8	+3V3 voltage, power supply
 * 
 * 9	GND ground, power supply
 * 10	C0	BUTTON1, on board, active low
 * 11	D4	BUTTON2
 * 12	A0	LED_POWER
 * 13	B4	(PWM) LED_CH2, LED_R
 * 14	B5	(PWM) LED_CH1, LED_W
 * 15	B7	(UART) RX
 * 16	B1	(UART) TX
 * 
 * 17	A7	SWS
*/

//	buttons
#define BUTTON1							GPIO_PC0	// pairing button on board
//#define BUTTON2						GPIO_PD4
//#define	KB_ONLY_SINGLEKEY_SUPP		1

//	LED channels
#define LED_CH1							GPIO_PB5	// either single white or cold white
#define LED_CH2							GPIO_PB4	// red
#define LED_CH3							GPIO_PC3	// green
#define LED_CH4							GPIO_PD2	// blue
#define LED_CH5							GPIO_PC2	// warm white

//	additionals
#define IR_IN							GPIO_PD7
#define UART_TX_PIN						UART_TX_PB1
#define UART_RX_PIN						UART_RX_PB7
#define UART_SWS						GPIO_PA7

//#define LED_POWER						GPIO_PA0
//#define LED_PERMIT					GPIO_PC2

#if (TARGET == TORSO)
#	define BUTTON2						GPIO_PD4
#	define LED_PERMIT					GPIO_PC2
#endif

//	setting IO functions
// BUTTON1
#define PC0_FUNC						AS_GPIO
#define PC0_OUTPUT_ENABLE				0
#define PC0_INPUT_ENABLE				1
#define	PULL_WAKEUP_SRC_PC0				PM_PIN_PULLUP_10K
// BUTTON2
#ifdef BUTTON2
#   define PD4_FUNC						AS_GPIO
#   define PD4_OUTPUT_ENABLE			0
#   define PD4_INPUT_ENABLE				1
#   define PULL_WAKEUP_SRC_PD4			PM_PIN_PULLUP_10K
#endif
// LED_POWER
#if defined(LED_POWER) && (LED_POWER == GPIO_PA0)
#	define PA0_FUNC						AS_GPIO
#	define PA0_OUTPUT_ENABLE			1
#	define PA0_INPUT_ENABLE				0
//#	define PULL_WAKEUP_SRC_PA0			PM_PIN_PULLDOWN_100K
#endif
// LED_PERMIT
/* #if defined(LED_PERMIT) && (LED_PERMIT == GPIO_PC2)
#	define PC2_FUNC					AS_GPIO
#	define PC2_OUTPUT_ENABLE		1
#	define PC2_INPUT_ENABLE			0
#	define PULL_WAKEUP_SRC_PC2		PM_PIN_PULLDOWN_100K
#endif */

#if (__PROJECT_TL_DIMMABLE_LIGHT__) || (__LIGHT__MARCH42_TORSO__)
#	define LED_CH1_PWM				5//PWM5
#	define LED_CH1_PWM_SET()		do{	\
										gpio_set_func(LED_CH1, AS_PWM5); 	\
									}while(0)
#	define LED_CH2_PWM				4//PWM4
#	define LED_CH2_PWM_SET()		do{	\
										gpio_set_func(LED_CH2, AS_PWM4); 		\
									}while(0)
#	define LED_CH3_PWM				1//PWM1
#	define LED_CH3_PWM_SET()		do{	\
										gpio_set_func(LED_CH3, AS_PWM1); 	\
									}while(0)
#	define LED_CH4_PWM				3//PWM3
#	define LED_CH4_PWM_SET()		do{	\
										gpio_set_func(LED_CH4, AS_PWM3); 		\
									}while(0)
#	define LED_CH5_PWM				0//PWM0
#	define LED_CH5_PWM_SET()		do{	\
										gpio_set_func(LED_CH5, AS_PWM0); 		\
									}while(0)
	// LED_W
#	define COOL_LIGHT_PWM_CHANNEL	LED_CH1_PWM
#	define COOL_LIGHT_PWM_SET()		LED_CH1_PWM_SET()
	// LED_R
#	define R_LIGHT_PWM_CHANNEL		LED_CH2_PWM
#	define R_LIGHT_PWM_SET()		LED_CH2_PWM_SET()
	// LED_G
#	define G_LIGHT_PWM_CHANNEL		LED_CH3_PWM
#	define G_LIGHT_PWM_SET()		LED_CH3_PWM_SET()
	// LED_B
#	define B_LIGHT_PWM_CHANNEL		LED_CH4_PWM
#	define B_LIGHT_PWM_SET()		LED_CH4_PWM_SET()
	// LED_WW
#	define WARM_LIGHT_PWM_CHANNEL	LED_CH5_PWM
#	define WARM_LIGHT_PWM_SET()		LED_CH5_PWM_SET()

#elif (__PROJECT_TL_BOOT_LOADER__)
	// LED_W
#	define PB5_FUNC					AS_GPIO
#	define PB5_OUTPUT_ENABLE		1
#	define PB5_INPUT_ENABLE			0
	// LED_R
#	define PB4_FUNC					AS_GPIO
#	define PB4_OUTPUT_ENABLE		1
#	define PB4_INPUT_ENABLE			0
	// LED_G
#	define PC3_FUNC					AS_GPIO
#	define PC3_OUTPUT_ENABLE		1
#	define PC3_INPUT_ENABLE			0
	// LED_B
#	define PD2_FUNC					AS_GPIO
#	define PD2_OUTPUT_ENABLE		1
#	define PD2_INPUT_ENABLE			0
	// LED_WW
#	define PC2_FUNC					AS_GPIO
#	define PC2_OUTPUT_ENABLE		1
#	define PC2_INPUT_ENABLE			0

#elif (__PROJECT_TL_CONTACT_SENSOR__)
#elif (__PROJECT_TL_SWITCH__)
#else
/*#	error	you need to specify your project*/
#endif

//	ADC PIN C4 is fed out on module pin 2
#define VOLTAGE_DETECT_PIN			GPIO_PC5

// UART
#if ZBHCI_UART || UART_ENABLE
#	if ZBHCI_UART
#		error SURE ABOUT ZBHCI MODE ???
#	endif
#	define UART_PIN_CFG()			uart_gpio_set(UART_TX_PIN, UART_RX_PIN);
#endif

// DEBUG
#if UART_PRINTF_MODE
#	if ZBHCI_UART || UART_ENABLE
#		error sure to use UART_ENABLE,ZBHI_MODE and UART_PRINTF_MODE ??? need to change DEBUG_INFO_TX_PIN
		// GPIO_PD7 can be used, but is assigned to IR_IN
#		define	DEBUG_INFO_TX_PIN		GPIO_PD7
#	else
#		define	DEBUG_INFO_TX_PIN		UART_TX_PIN
#	endif
#endif

enum{
	VK_SW1 = 0x01,
	VK_SW2 = 0x02,
};

#define KB_DRIVE_PINS	{0}

#ifdef BUTTON2
#	define KB_MAP_NORMAL	{ {VK_SW1,}, {VK_SW2,}, }
#	define KB_SCAN_PINS	{BUTTON1,BUTTON2}
#else
#	define KB_MAP_NORMAL	{ {VK_SW1,}, }
#	define KB_SCAN_PINS	{BUTTON1}
#endif

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
