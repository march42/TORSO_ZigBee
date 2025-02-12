/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for app_ui
 *
 * @author  Zigbee Group
 * @date    2021
 * 
 * @author	Marc Hefter
 * @date	2024-2025
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

#if (__PROJECT_TL_DIMMABLE_LIGHT__)

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "sampleLight.h"
#include "sampleLightCtrl.h"
#include "app_ui.h"
#include "gp.h"
/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * GLOBAL VARIABLES
 */
kb_data_t kb_buffer;		// buffer for kb_event handling

/**********************************************************************
 * FUNCTIONS
 */
extern void sampleLight_updateOnOff(void);
extern void sampleLight_updateLevel(void);
extern void sampleLight_updateColor(void);

extern void sampleLight_onOffInit(void);
extern void sampleLight_levelInit(void);
extern void sampleLight_colorInit(void);

/**********************************************************************
 * LOCAL FUNCTIONS
 */
void led_on(u32 pin){
#ifdef LED_POWER
	drv_gpio_write(pin, LED_ON);
#endif
}

void led_off(u32 pin){
#ifdef LED_PERMIT
	drv_gpio_write(pin, LED_OFF);
#endif
}

void led_init(void){
#ifdef LED_POWER
	led_off(LED_POWER);
#endif
#ifdef LED_PERMIT
	led_off(LED_PERMIT);
#endif
}

/*	localPermitJoinState is called regularly by app_task
*/
void localPermitJoinState(void){
	static bool assocPermit = 0;
	if(assocPermit != zb_getMacAssocPermit()){
		assocPermit = zb_getMacAssocPermit();
#ifdef LED_PERMIT
		if(assocPermit){
			led_on(LED_PERMIT);
		}else{
			led_off(LED_PERMIT);
		}
#else /* use the LED channels for PERMIT signalling */
		// TODO: implement PERMIT_SIGNALLING effect
		if(assocPermit){
			light_blink_start(32, 700, 1300);
		}else{
			light_blink_stop();
		}
#endif
	}
}

#if TELINK_BUTTON_HANDLING
void buttonKeepPressed(u8 btNum){
	if(btNum == VK_SW1){
		gLightCtx.state = APP_FACTORY_NEW_DOING;
		zb_factoryReset();
	}else if(btNum == VK_SW2){

	}
}

void buttonShortPressed(u8 btNum){
	if(btNum == VK_SW1){
		if(zb_isDeviceJoinedNwk()){
			gLightCtx.sta = !gLightCtx.sta;
			if(gLightCtx.sta){
				sampleLight_onoff(ZCL_ONOFF_STATUS_ON);
			}else{
				sampleLight_onoff(ZCL_ONOFF_STATUS_OFF);
			}
		}
	}else if(btNum == VK_SW2){
		/* toggle local permit Joining */
		u8 duration = zb_getMacAssocPermit() ? 0 : 180;
		zb_nlmePermitJoiningRequest(duration);

		gpsCommissionModeInvork();
	}
}

void keyScan_keyPressedCB(kb_data_t *kbEvt){
//	u8 toNormal = 0;
	u8 keyCode = kbEvt->keycode[0];
//	static u8 lastKeyCode = 0xff;

	buttonShortPressed(keyCode);

	if(keyCode == VK_SW1){
		gLightCtx.keyPressedTime = clock_time();
		gLightCtx.state = APP_FACTORY_NEW_SET_CHECK;
	}
}


void keyScan_keyReleasedCB(u8 keyCode){
	gLightCtx.state = APP_STATE_NORMAL;
}

/*	app_key_handler is called regularly by app_task
*/
volatile u8 T_keyPressedNum = 0;
void app_key_handler(void){
	static u8 valid_keyCode = 0xff;

	if(gLightCtx.state == APP_FACTORY_NEW_SET_CHECK){
		if(clock_time_exceed(gLightCtx.keyPressedTime, 5*1000*1000)){
			buttonKeepPressed(VK_SW1);
		}
	}

	if(kb_scan_key(0 , 1)){
		T_keyPressedNum++;
		if(kb_event.cnt){
			keyScan_keyPressedCB(&kb_event);
			if(kb_event.cnt == 1){
				valid_keyCode = kb_event.keycode[0];
			}
		}else{
			keyScan_keyReleasedCB(valid_keyCode);
			valid_keyCode = 0xff;
		}
	}
}

#else
/*	app_key_handler
**	regularly called by app_task in sampleLight.c
**	***
**	pressed_count	current counter of subsequent key presses
**	gLightCtx.keyPressedTime	last time key was pressed
**	gLightCtx.state				running state (APP_STATE_NORMAL, APP_FACTORY_NEW_SET_CHECK, APP_FACTORY_NEW_DOING)
*/
static volatile struct {						// need to ensure everything is 32bit alligned
	s8	keyState;		// current state of key press
	u8	keyCode;		// current pressed key
	u8	keyCount;		// counter of presses
	s8	levelState;		// current state for long pressing, 0=unknown <0=decrease >0=increase
	u32	changeTime;		// time of last key change
}	kb_state	= {-1, 0xFF, 0, 0, 0};			// initial data
ev_timer_event_t *pressed_resetEvent	= NULL;	// reset pressed counter and keyCode

s32 app_key_reset(void *arg)
{
	DEBUG(DEBUG_BUTTONS, "BUTTON reset\r");
	kb_state.keyCode	= 0xFF;
	kb_state.keyCount	= 0;
	pressed_resetEvent	= NULL;
	gLightCtx.state		= APP_STATE_NORMAL;
	return -1;
}

void app_key_reset_startTimer(u32 t_ms)
{
	if(pressed_resetEvent) {
		TL_ZB_TIMER_CANCEL(&pressed_resetEvent);
	}
	if (t_ms) {
		pressed_resetEvent = TL_ZB_TIMER_SCHEDULE(app_key_reset, NULL, t_ms);
	} else {
		pressed_resetEvent = NULL;
	}
}

void app_key_pressed(u8 pressed_keyCode, u8 pressed_count)
{
	DEBUG(DEBUG_BUTTONS, "BUTTON pressed key=%x count=%d\r", pressed_keyCode, pressed_count);
	if (pressed_keyCode == VK_SW1
#ifdef BUTTON2
		|| pressed_keyCode == VK_SW2
#endif
		) {
		// button pressed for the pressed_count time
		switch (pressed_count) {
			case 1:	// On/Off toggle
				DEBUG(DEBUG_BUTTONS, "BUTTON OnOff\r");
				sampleLight_onoff(ZCL_CMD_ONOFF_TOGGLE);	// toggle On/Off
				return;
			case 2:	// On and dim to initial default
				DEBUG(DEBUG_BUTTONS, "BUTTON Full On\r");
				zcl_levelAttr_t *pLevel		= zcl_levelAttrGet();
				pLevel->curLevel			= ZCL_LEVEL_ATTR_MAX_LEVEL;
				sampleLight_updateLevel();					// set level
				sampleLight_onoff(ZCL_CMD_ONOFF_ON);		// turn on
				return;
			case 3:
				DEBUG(DEBUG_BUTTONS, "BUTTON Join Network\r");
				// join network
				/* toggle local permit Joining */
				zb_nlmePermitJoiningRequest(zb_getMacAssocPermit() ? 0 : 180);
				gpsCommissionModeInvork();
				return;
			case 4:
				DEBUG(DEBUG_BUTTONS, "BUTTON Leave Network\r");
				// leave network
				return;
			case 5:
				DEBUG(DEBUG_BUTTONS, "BUTTON Factory Reset\r");
				// factory reset
				gLightCtx.state = APP_FACTORY_NEW_DOING;
				zb_factoryReset();
				return;
		}
	}
}

void app_key_longpress(u8 pressed_keyCode, int pressed_ms)
{
	// button long pressed
	DEBUG(DEBUG_BUTTONS, "BUTTON long press key=%x ms=%d\r", pressed_keyCode, pressed_ms);
	// BUTTON1
	if (pressed_keyCode == VK_SW1 && gLightCtx.state == APP_STATE_NORMAL && pressed_ms > 5000) {
		gLightCtx.state = APP_FACTORY_NEW_SET_CHECK;			// factory reset is done on release
		return;
	} else

	if (pressed_keyCode == VK_SW2) {		// BUTTON2
		//	dimming
		zcl_levelAttr_t *pLevel = zcl_levelAttrGet();
		if (kb_state.levelState == 0 && pLevel->curLevel == ZCL_LEVEL_ATTR_MIN_LEVEL) {
			kb_state.levelState = 1;		// start with dimming up, if at minimum
		}
		else if (kb_state.levelState == 0) {
			kb_state.levelState = -1;		// start with dimming down
		}

		if (kb_state.levelState < 0 && pLevel->curLevel == ZCL_LEVEL_ATTR_MIN_LEVEL) {
			return;		// stop at minimum
		}
		else if (kb_state.levelState > 0 && pLevel->curLevel == ZCL_LEVEL_ATTR_MAX_LEVEL) {
			return;		// stop at maximum
		}

		pLevel->curLevel	+= kb_state.levelState;
		hwLight_levelUpdate(pLevel->curLevel);		// set level
	}
}

void app_key_released(u8 pressed_keyCode, int pressed_ms)
{
	// button released after pressed_count presses
	DEBUG(DEBUG_BUTTONS, "BUTTON released key=%x time=%d\r", pressed_keyCode, pressed_ms);
	/* maybe save state after dimming, when released after long press
	if (pressed_keyCode == kb_state.keyCode && 0 != kb_state.levelState)
	if (pressed_keyCode == kb_state.keyCode && 0 != kb_state.colorState)
	*/

	if (pressed_keyCode == VK_SW1 && gLightCtx.state == APP_FACTORY_NEW_SET_CHECK && pressed_ms < 7500) {
		// factory reset
		gLightCtx.state = APP_FACTORY_NEW_DOING;
		zb_factoryReset();
		// join network
		zb_nlmePermitJoiningRequest(zb_getMacAssocPermit() ? 0 : 180);
		gpsCommissionModeInvork();
	}

	kb_state.levelState = 0;		// restart dimming on released button
}

#	define	MIN_MS_LONG_PRESS		2000
#	define	MIN_MS_ANTIGLITCH		5
#	define	MAX_MS_SHORT_PRESS		750
#	define	MAX_MS_RECURRING_PRESS	1000
void app_key_handler(void)
{
	static int longPressMs	= MIN_MS_LONG_PRESS;			// next ms counter for long press handling

	if (-1 == kb_state.keyState) {							// this will only be true on startup
		DEBUG(DEBUG_TRACE, "app_key_handler initialize\r");
		kb_state.keyState		= 0;						// no key pressed
	}

	if (kb_scan_key(0, 1)) {								// scan keyboard, returns TRUE if keys changed
		DEBUG(DEBUG_KEYHANDLER, "key changed\r");
		memcpy4(&kb_buffer, &kb_event, sizeof(kb_data_t));	// buffer changed keyboard data
	}

	/* keyState==0 waiting to be pressed */
	if ((kb_state.keyState == 0 || kb_state.keyState == 2) && kb_buffer.cnt == 1) {
		kb_state.changeTime	= clock_time();					// remember time of key change
		DEBUG(DEBUG_KEYHANDLER, "key pressed\r");
		sleep_ms(MIN_MS_ANTIGLITCH);
		if (!kb_scan_key(0, 1) || kb_event.cnt == 1) {		// ensure key still pressed after MIN_MS_ANTIGLITCH
			DEBUG(DEBUG_KEYHANDLER, "key pressed (%dms)\r", MIN_MS_ANTIGLITCH);
			kb_state.keyState		= 1;					// key pressed
			gLightCtx.keyPressedTime = kb_state.changeTime;	// remember initial time of pressing
			if (kb_buffer.keycode[0] == kb_state.keyCode) {	// pressed before
				kb_state.keyCount++;						// count pressing
			}
			else /* if (0xFF == kb_state.keyCode) */ {		// first pressed key
				kb_state.keyCode	= kb_buffer.keycode[0];	// remember pressed key
				kb_state.keyCount	= 1;					// count pressed key
				longPressMs			= MIN_MS_LONG_PRESS;	// minimum long press time
			}
		}
		return;
	}

	/* keyState==1 waiting to be released or long pressed */
	else if (kb_state.keyState == 1 && kb_buffer.cnt != 1) {	// key released
		DEBUG(DEBUG_KEYHANDLER, "key released\r");
		sleep_ms(MIN_MS_ANTIGLITCH);
		if (!kb_scan_key(0, 1) || kb_event.cnt == 1) {		// ensure key still released after MIN_MS_ANTIGLITCH
			DEBUG(DEBUG_KEYHANDLER, "key released (%dms)\r", MIN_MS_ANTIGLITCH);
			//kb_state.changeTime		= clock_time();			// remember time of key change
			kb_state.keyState		= 2;					// waiting for recurring press
			app_key_reset_startTimer(MAX_MS_RECURRING_PRESS);	// clear after timeout, to ensure subsequent presses are same key
		}
	}
	else if (kb_state.keyState == 1 && clock_time_exceed(kb_state.changeTime, MAX_MS_SHORT_PRESS*1000)) {
		DEBUG(DEBUG_KEYHANDLER, "key pressed (%dms)\r", MAX_MS_SHORT_PRESS);
		kb_state.keyState			= 5;					// key pressed long
		return;
	}

	/* keyState==2 waiting for recurring press */
	else if (kb_state.keyState == 2 && clock_time_exceed(kb_state.changeTime, MAX_MS_SHORT_PRESS*1000)) {
		kb_state.keyState			= 0;					// reset to not pressed
		app_key_pressed(kb_state.keyCode, kb_state.keyCount);	// short pressed, timeout for recurring press
		return;
	}

	/* keyState==5 long pressed */
	else if (kb_state.keyState == 5 && kb_buffer.cnt != 1) {
		DEBUG(DEBUG_KEYHANDLER, "key released (%d ms)\r", longPressMs);
		// released after long press
		sleep_ms(MIN_MS_ANTIGLITCH);
		if (!kb_scan_key(0, 1) || kb_event.cnt == 0) {		// ensure key still released after MIN_MS_ANTIGLITCH
			kb_state.keyState		= 0;					// reset to not pressed
			app_key_reset_startTimer(MAX_MS_RECURRING_PRESS);	// clear after timeout, to ensure subsequent presses are same key
			app_key_released(kb_state.keyCode, longPressMs);	// button pressed_keyCode released after pressed_count
		}
		return;
	}
	else if (kb_state.keyState == 5 && clock_time_exceed(kb_state.changeTime, longPressMs*1000)) {
		DEBUG(DEBUG_KEYHANDLER, "key longpress (%d ms)\r", longPressMs);
		app_key_longpress(kb_state.keyCode, longPressMs);
		longPressMs += 100;									// next long press handling after 100ms
		return;
	}

	return;
}
#endif

#endif  /* __PROJECT_TL_DIMMABLE_LIGHT__ */
