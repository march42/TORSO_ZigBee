/********************************************************************************************************
 * @file	board_8258_ZYZB010.h
 *
 * @brief	This is the header file for eWeLink ZYZB010 module
 *
 * @version 1.0.release1	2024-12-20
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

/*	board specifications
 * MCU	TeLink TLSR8258 controller (32 pin, 512 KB flash)
 * PWR	5-24V DC
 *	***	pinout
 * 1	Vcc	(+3.3V, power supply)
 * 2	GND	(power supply)
 * 3	A7	SWS
 * 4	D2
 * 5	D3	FAC_RST (BUTTON1, active low, >5s pairing mode)
 * 6	C3	LED_WW, PWM2
 * 7	C2	LED_W, PWM1
 * 8	n.c.
 * 9	n.c.
 * 10	D4
 * 11	n.c.
 * 12	A0
 * 
 * 13	n.c.
 * 14	C4	LED_R, PWM3
 * 15	A1
 * 16	B4	LED_G, PWM4
 * 17	B5	LED_B, PWM5
 * 18	B1	
 * 19	B6
 * 20	D7	UART_TX
 * 21	B7	UART_RX
 * 22	C0
 * 23	C1
 * 24	RST	(RESET, active low)
*/

//	buttons
#define FAC_RST						GPIO_PD3	// factory reset, pairing button on board

//	LED channels
#define LED_W						GPIO_PC2
#define LED_WW						GPIO_PC3
#define LED_R						GPIO_PC4
#define LED_G						GPIO_PB4
#define LED_B						GPIO_PB5

//	additionals
#define UART_TX_PIN				 UART_TX_PD7
#define UART_RX_PIN				 UART_RX_PB7
#define UART_SWS					GPIO_PA7

//#define LED_POWER					NULL
//#define LED_PERMIT					NULL

//	setting IO functions
// FAC_RST
#define PD3_FUNC						AS_GPIO
#define PD3_OUTPUT_ENABLE				0
#define PD3_INPUT_ENABLE				1
#define	PULL_WAKEUP_SRC_PD3				PM_PIN_PULLUP_10K

#if (__PROJECT_TL_DIMMABLE_LIGHT__) || (__LIGHT__MARCH42_TORSO__)
	// LED_W
#	define PWM_W_CHANNEL			1
#	define PWM_W_CHANNEL_SET()		do{	\
										gpio_set_func(LED_W, AS_PWM1); 	\
									}while(0)
#	define COOL_LIGHT_PWM_CHANNEL	PWM_W_CHANNEL
#	define COOL_LIGHT_PWM_SET()		PWM_W_CHANNEL_SET()
	// LED_WW
#	define PWM_WW_CHANNEL			2
#	define PWM_WW_CHANNEL_SET()		do{	\
										gpio_set_func(LED_WW, AS_PWM2); 		\
									}while(0)
#	define WARM_LIGHT_PWM_CHANNEL	PWM_WW_CHANNEL
#	define WARM_LIGHT_PWM_SET()		PWM_WW_CHANNEL_SET()
	// LED_R
#	define PWM_R_CHANNEL			3
#	define PWM_R_CHANNEL_SET()		do{	\
										gpio_set_func(LED_R, AS_PWM3); 		\
									}while(0)
#	define R_LIGHT_PWM_CHANNEL		PWM_R_CHANNEL
#	define R_LIGHT_PWM_SET()		PWM_R_CHANNEL_SET()
	// LED_G
#	define PWM_G_CHANNEL			4
#	define PWM_G_CHANNEL_SET()		do{	\
										gpio_set_func(LED_G, AS_PWM4); 	\
									}while(0)
#	define G_LIGHT_PWM_CHANNEL		PWM_G_CHANNEL
#	define G_LIGHT_PWM_SET()		PWM_G_CHANNEL_SET()
	// LED_B
#	define PWM_B_CHANNEL			5
#	define PWM_B_CHANNEL_SET()		do{	\
										gpio_set_func(LED_B, AS_PWM5); 		\
									}while(0)
#	define B_LIGHT_PWM_CHANNEL		PWM_B_CHANNEL
#	define B_LIGHT_PWM_SET()		PWM_B_CHANNEL_SET()

#elif (__PROJECT_TL_BOOT_LOADER__)
	// LED_W
#	define PC2_FUNC					AS_GPIO
#	define PC2_OUTPUT_ENABLE		1
#	define PC2_INPUT_ENABLE			0
	// LED_WW
#	define PC3_FUNC					AS_GPIO
#	define PC3_OUTPUT_ENABLE		1
#	define PC3_INPUT_ENABLE			0
	// LED_R
#	define PC4_FUNC					AS_GPIO
#	define PC4_OUTPUT_ENABLE		1
#	define PC4_INPUT_ENABLE			0
	// LED_G
#	define PB4_FUNC					AS_GPIO
#	define PB4_OUTPUT_ENABLE		1
#	define PB4_INPUT_ENABLE			0
	// LED_B
#	define PB5_FUNC					AS_GPIO
#	define PB5_OUTPUT_ENABLE		1
#	define PB5_INPUT_ENABLE			0

//#elif (__PROJECT_TL_CONTACT_SENSOR__)
//#elif (__PROJECT_TL_SWITCH__)
#else
#	error	you need to specify your project
#endif

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
#		define	DEBUG_INFO_TX_PIN		GPIO_PB1
#	endif
#endif

enum{
	VK_SW1 = 0x01,
	VK_SW2 = 0x02,
};

#define	KB_MAP_NORMAL	{\
		{VK_SW1,}, \
		{VK_SW2,}, }

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL

#define KB_DRIVE_PINS	{0}
#define KB_SCAN_PINS	{BUTTON1, BUTTON2}


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
