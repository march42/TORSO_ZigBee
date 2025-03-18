/********************************************************************************************************
 * @file    ledLightCtrl.c
 *
 * @brief   This is the source file for ledLightCtrl
 *
 * @author	Marc Hefter
 * @date	2024-2025
 * @par     Copyright (C) 2025, Marc Hefter (https://github.com/march42)
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

#if (__PROJECT_TL_DIMMABLE_LIGHT__)

/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zcl_include.h"
#include "ledLight.h"
#include "ledLightCtrl.h"
#include "color_calculations.h"


/**********************************************************************
 * LOCAL CONSTANTS
 */

/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * GLOBAL VARIABLES
 */

/**********************************************************************
 * FUNCTIONS
 */
extern void ledLight_onOffInit(void);
extern void ledLight_levelInit(void);
extern void ledLight_colorInit(void);

extern void ledLight_updateOnOff(void);
extern void ledLight_updateLevel(void);
extern void ledLight_updateColor(void);

/*********************************************************************
 * @fn      pwmSetDutyCount
 *
 * @brief
 *
 * @param   ch			- PWM channel
 * 			dutyCount	- PWM duty cycle counter (0 <= dutyCount <= PWM_MAX_TICK)
 *
 * @return  None
 */
void pwmSetDutyCount(u8 ch, u16 dutyCount)
{
#ifdef ZCL_LEVEL_CTRL
	DEBUG(DEBUG_LED_PWM, "PWM(%d,T=%x)\r", ch,dutyCount);
	drv_pwm_cfg(ch, (dutyCount>PWM_MAX_TICK ?PWM_MAX_TICK :dutyCount), PWM_MAX_TICK);
#endif
}

/*********************************************************************
 * @fn      pwmSetDuty
 *
 * @brief
 *
 * @param   ch			- PWM channel
 * 			dutycycle	- PWM duty cycle in percent (0 <= duty <= PWM_FULL_DUTYCYCLE)
 *
 * @return  None
 */
void pwmSetDuty(u8 ch, u16 dutycycle)
{
#ifdef ZCL_LEVEL_CTRL
	DEBUG(DEBUG_LED_PWM, "PWM(%d,P=%x)\r", ch,dutycycle);
	u32 cmp_tick = (u32)(dutycycle * PWM_MAX_TICK / PWM_FULL_DUTYCYCLE);	// convert fraction
	drv_pwm_cfg(ch, (u16)cmp_tick, PWM_MAX_TICK);
#endif
}

/*********************************************************************
 * @fn      pwmInit
 *
 * @brief
 *
 * @param   ch			- PWM channel
 * 			dutycycle	- PWM duty cycle in percent (0 <= duty <= PWM_FULL_DUTYCYCLE)
 *
 * @return  None
 */
void pwmInit(u8 ch, u16 dutycycle)
{
	pwmSetDuty(ch, dutycycle);
}

/*********************************************************************
 * @fn      hwLight_init
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void hwLight_init(void)
{
	DEBUG(DEBUG_TRACE, "hwLight_init\r");

	drv_pwm_init();

#if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	COLD_LIGHT_PWM_SET();
	pwmInit(COLD_LIGHT_PWM_CHANNEL, 1);
#endif
#if (COLOR_RGB_SUPPORT)
	R_LIGHT_PWM_SET();
	G_LIGHT_PWM_SET();
	B_LIGHT_PWM_SET();
	pwmInit(R_LIGHT_PWM_CHANNEL, 1);
	pwmInit(G_LIGHT_PWM_CHANNEL, 1);
	pwmInit(B_LIGHT_PWM_CHANNEL, 1);
#endif
#if (COLOR_CCT_SUPPORT)
	WARM_LIGHT_PWM_SET();
	pwmInit(WARM_LIGHT_PWM_CHANNEL, 1);
#endif
}

/*********************************************************************
 * @fn      hwLight_onOffUpdate
 *
 * @brief
 *
 * @param   onOff - onOff attribute value
 *
 * @return  None
 */
void hwLight_onOffUpdate(u8 onOff)
{
	DEBUG((DEBUG_TRACE), "onOff(%x)\r", onOff);
	if(onOff){
#if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
		drv_pwm_start(COLD_LIGHT_PWM_CHANNEL);
#endif
#if (COLOR_CCT_SUPPORT)
		drv_pwm_start(WARM_LIGHT_PWM_CHANNEL);
#endif
#if (COLOR_RGB_SUPPORT)
		drv_pwm_start(R_LIGHT_PWM_CHANNEL);
		drv_pwm_start(G_LIGHT_PWM_CHANNEL);
		drv_pwm_start(B_LIGHT_PWM_CHANNEL);
#endif

	}else{
#if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
		drv_pwm_stop(COLD_LIGHT_PWM_CHANNEL);
#endif
#if (COLOR_CCT_SUPPORT)
		drv_pwm_stop(WARM_LIGHT_PWM_CHANNEL);
#endif
#if (COLOR_RGB_SUPPORT)
		drv_pwm_stop(R_LIGHT_PWM_CHANNEL);
		drv_pwm_stop(G_LIGHT_PWM_CHANNEL);
		drv_pwm_stop(B_LIGHT_PWM_CHANNEL);
#endif
	}
}

/*********************************************************************
 * @fn      hwLight_levelUpdate
 *
 * @brief
 *
 * @param   level - level attribute value
 *
 * @return  None
 */
void hwLight_levelUpdate(u8 level)
{
#if (SINGLE_WHITE_SUPPORT) && (!COLOR_RGB_SUPPORT)
	DEBUG((DEBUG_LED_PWM || DEBUG_TRACE), "DIMMER(%x)\r", level);
	level = (level < 0x10) ? 0x10 : level;

#	if (0)	// no need to complicate things, for dimmer lights
	g_ledChannel_COLD.Value	= COLORCHANNEL_MAX * level / ZCL_LEVEL_ATTR_MAX_LEVEL;
	__LED_COLD_SETONOFF(1,1);
	pwmSetDutyCount(COLD_LIGHT_PWM_CHANNEL, __LED_PWM_COUNT(g_ledChannel_COLD));
#	else
	pwmSetDutyCount(COLD_LIGHT_PWM_CHANNEL, level * PWM_MAX_TICK / ZCL_LEVEL_ATTR_MAX_LEVEL);
#	endif
#endif
}

/*********************************************************************
 * @fn      hwLight_colorUpdate_colorTemperature
 *
 * @brief
 *
 * @param   colorTemperatureMireds	-	colorTemperatureMireds attribute value
 * 			level					-	level attribute value
 *
 * @return  None
 */
void hwLight_colorUpdate_colorTemperature(u16 colorTemperatureMireds, u8 level)
{
	DEBUG(DEBUG_LEDCOLOR, "CCT mired=%x, level=%x\r", colorTemperatureMireds, level);
	level = (level < 0x10) ? 0x10 : level;

#if (COLOR_CCT_SUPPORT) && (!COLOR_RGB_SUPPORT)
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();
	s32 warmCycle = 0;
	if (temperatureMireds >= pColor->colorTempPhysicalMaxMireds) {
		warmCycle = PWM_MAX_TICK;			// catch out of boundary values
	} else if (temperatureMireds <= pColor->colorTempPhysicalMinMireds) {
		warmCycle = 0;						// catch out of boundary values
	} else {
		warmCycle = (((temperatureMireds - pColor->colorTempPhysicalMinMireds) * PWM_MAX_TICK) / (pColor->colorTempPhysicalMaxMireds - pColor->colorTempPhysicalMinMireds));
	}
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL,	(u16)((PWM_MAX_TICK - warmCycle) * level / ZCL_LEVEL_ATTR_MAX_LEVEL));
	pwmSetDuty(WARM_LIGHT_PWM_CHANNEL,	(u16)((               warmCycle) * level / ZCL_LEVEL_ATTR_MAX_LEVEL));

#elif (COLOR_RGB_SUPPORT) && (!COLOR_CCT_SUPPORT) && (!SINGLE_WHITE_SUPPORT)
	LEDLIGHT_setMired (colorTemperatureMireds);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		__LED_RED_PWMCOUNT);
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		__LED_GREEN_PWMCOUNT);
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		__LED_BLUE_PWMCOUNT);

#elif (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)
// RGB+W light

#	if (COLD_LIGHT_TEMPERATURE)
#		define WHITE_CT		COLD_LIGHT_TEMPERATURE
#	else /* just using COLOR_TEMPERATURE_NEUTRAL as assumed default, because not defined */
#		define WHITE_CT		COLOR_TEMPERATURE_NEUTRAL
#	endif

	LEDLIGHT_setMired_RGBW (colorTemperatureMireds, WHITE_CT);
	pwmSetDutyCount(COLD_LIGHT_PWM_CHANNEL,	(g_ledChannel_COLD.Value  * g_ledChannel_COLD.OnOff  * level / COLORCHANNEL_MAX / COLORONOFF_MAX / ZCL_LEVEL_ATTR_MAX_LEVEL));
	pwmSetDutyCount(R_LIGHT_PWM_CHANNEL,	(g_ledChannel_RED.Value   * g_ledChannel_RED.OnOff   * level / COLORCHANNEL_MAX / COLORONOFF_MAX / ZCL_LEVEL_ATTR_MAX_LEVEL));
	pwmSetDutyCount(G_LIGHT_PWM_CHANNEL,	(g_ledChannel_GREEN.Value * g_ledChannel_GREEN.OnOff * level / COLORCHANNEL_MAX / COLORONOFF_MAX / ZCL_LEVEL_ATTR_MAX_LEVEL));
	pwmSetDutyCount(B_LIGHT_PWM_CHANNEL,	(g_ledChannel_BLUE.Value  * g_ledChannel_BLUE.OnOff  * level / COLORCHANNEL_MAX / COLORONOFF_MAX / ZCL_LEVEL_ATTR_MAX_LEVEL));

#elif (COLOR_RGB_SUPPORT) && (COLOR_CCT_SUPPORT)
	// RGB+CCT
#	ifndef COLD_LIGHT_TEMPERATURE
#		define COLD_LIGHT_TEMPERATURE		COLOR_TEMPERATURE_6500K
#	endif
#	ifndef WARM_LIGHT_TEMPERATURE
#		define WARM_LIGHT_TEMPERATURE		COLOR_TEMPERATURE_2200K
#	endif

	LEDLIGHT_setMired_RGBCCT (RGB_CT, COLD_LIGHT_TEMPERATURE, WARM_LIGHT_TEMPERATURE);

	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL,	__LED_COLD_PWMCOUNT);
	pwmSetDuty(WARM_LIGHT_PWM_CHANNEL,	__LED_WARM_PWMCOUNT);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		__LED_RED_PWMCOUNT);
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		__LED_GREEN_PWMCOUNT);
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		__LED_BLUE_PWMCOUNT);

#endif
}

/*********************************************************************
 * @fn      hwLight_colorUpdate_HSV2RGB
 *
 * @brief
 *
 * @param   hue			-	hue attribute value
 * 			saturation	-	saturation attribute value
 * 			level		-	level attribute value
 *
 * @return  None
 */
void hwLight_colorUpdate_HSV2RGB(u8 hue, u8 saturation, u8 level)
{
#if (COLOR_RGB_SUPPORT)
	level = (level < 0x10) ? 0x10 : level;
	LEDLIGHT_setHSV (hue, saturation, level);	// use MAX level and apply actual level later
	pwmSetDuty(R_LIGHT_PWM_CHANNEL, g_ledChannel_RED.Value);
	pwmSetDuty(G_LIGHT_PWM_CHANNEL, g_ledChannel_GREEN.Value);
	pwmSetDuty(B_LIGHT_PWM_CHANNEL, g_ledChannel_BLUE.Value);
	LEDLIGHT_setOnOff_fromValue();
#endif
}

/*********************************************************************
 * @fn      hwLight_colorUpdate_enhancedHSV2RGB
 *
 * @brief
 *
 * @param   [in]enhancedHue		-	hue attribute value
 * 			[in]saturation		-	saturation attribute value
 * 			[in]level			-	level attribute value
 * 			[out]hue			-	hue attribute value
 *
 * @return  None
 */
void hwLight_colorUpdate_enhancedHSV2RGB(u16 enhancedHue, u8 saturation, u8 level, u8 *hue)
{
#if (COLOR_RGB_SUPPORT)
	level = (level < 0x10) ? 0x10 : level;
	LEDLIGHT_setEnhancedHSV (enhancedHue, saturation, level, hue);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL, g_ledChannel_RED.Value);
	pwmSetDuty(G_LIGHT_PWM_CHANNEL, g_ledChannel_GREEN.Value);
	pwmSetDuty(B_LIGHT_PWM_CHANNEL, g_ledChannel_BLUE.Value);
	LEDLIGHT_setOnOff_fromValue();
#endif
}

/*********************************************************************
 * @fn      hwLight_colorUpdate_xyY2RGB
 *
 * @brief	set PWM cycle for RGB LEDs from CIE xyY color value
 *
 * @param   X			-	X attribute value		ZCL_COLOR_ATTR_XY_MIN ... ZCL_COLOR_ATTR_XY_MAX
 * 			Y			-	Y attribute value		ZCL_COLOR_ATTR_XY_MIN ... ZCL_COLOR_ATTR_XY_MAX
 * 			level		-	level attribute value	ZCL_LEVEL_ATTR_MIN_LEVEL ... ZCL_LEVEL_ATTR_MAX_LEVEL
 *
 * @return  None
 */
void hwLight_colorUpdate_xyY2RGB(u16 x, u16 y, u8 level)
{
#if (COLOR_RGB_SUPPORT)
	level = (level < 0x10) ? 0x10 : level;
	LEDLIGHT_setXY (x, y, level);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL, g_ledChannel_RED.Value);
	pwmSetDuty(G_LIGHT_PWM_CHANNEL, g_ledChannel_GREEN.Value);
	pwmSetDuty(B_LIGHT_PWM_CHANNEL, g_ledChannel_BLUE.Value);
	LEDLIGHT_setOnOff_fromValue();
#endif
}

/*********************************************************************
 * @fn      light_adjust
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void light_adjust(void)
{
	DEBUG(DEBUG_TRACE, "light_adjust\r");
#if defined(ZCL_LIGHT_COLOR_CONTROL)
	// if CCT or RGB supported
	ledLight_colorInit();
#elif defined(ZCL_LEVEL_CTRL)
	// for LED_MODE_DIMMER
	ledLight_levelInit();
#endif
	ledLight_onOffInit();
}

/*********************************************************************
 * @fn      light_fresh
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void light_fresh(void)
{
	DEBUG(DEBUG_TRACE, "light_fresh\r");
#if defined(ZCL_LIGHT_COLOR_CONTROL)
	// if CCT or RGB supported
	ledLight_updateColor();
#elif defined(ZCL_LEVEL_CTRL)
	// for LED_MODE_DIMMER
	ledLight_updateLevel();
#else
	// always set LED_CH1 to maximum, without ZCL_LEVEL_CTRL and ZCL_LIGHT_COLOR_CONTROL
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL, PWM_MAX_TICK);
#endif
	ledLight_updateOnOff();

	gLightCtx.lightAttrsChanged = TRUE;
}

/*********************************************************************
 * @fn      light_applyUpdate
 *
 * @brief
 *
 * @param
 *
 * @return  None
 */
void light_applyUpdate(u8 *curLevel, u16 *curLevel256, s32 *stepLevel256, u16 *remainingTime, u8 minLevel, u8 maxLevel, bool wrap)
{
	DEBUG(DEBUG_LEDCOLOR, "applUpd\r");
	if((*stepLevel256 > 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) > maxLevel)){
		*curLevel256 = (wrap) ? ((u16)minLevel * 256 + ((*curLevel256 + *stepLevel256) - (u16)maxLevel * 256) - 256)
							  : ((u16)maxLevel * 256);
	}else if((*stepLevel256 < 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) < minLevel)){
		*curLevel256 = (wrap) ? ((u16)maxLevel * 256 - ((u16)minLevel * 256 - ((s32)*curLevel256 + *stepLevel256)) + 256)
							  : ((u16)minLevel * 256);
	}else{
		*curLevel256 += *stepLevel256;
	}

	if(*stepLevel256 > 0){
		*curLevel = (*curLevel256 + 127) / 256;
	}else{
		*curLevel = *curLevel256 / 256;
	}

	if(*remainingTime == 0){
		*curLevel256 = ((u16)*curLevel) * 256;
		*stepLevel256 = 0;
	}else if(*remainingTime != 0xFFFF){
		*remainingTime = *remainingTime -1;
	}

	light_fresh();
}

/*********************************************************************
 * @fn      light_applyUpdate_16
 *
 * @brief
 *
 * @param
 *
 * @return  None
 */
void light_applyUpdate_16(u16 *curLevel, u32 *curLevel256, s32 *stepLevel256, u16 *remainingTime, u16 minLevel, u16 maxLevel, bool wrap)
{
	DEBUG(DEBUG_LEDCOLOR, "applUpd16\r");
	if((*stepLevel256 > 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) > maxLevel)){
		*curLevel256 = (wrap) ? ((u32)minLevel * 256 + ((*curLevel256 + *stepLevel256) - (u32)maxLevel * 256) - 256)
							  : ((u32)maxLevel * 256);
	}else if((*stepLevel256 < 0) && ((((s32)*curLevel256 + *stepLevel256) / 256) < minLevel)){
		*curLevel256 = (wrap) ? ((u32)maxLevel * 256 - ((u32)minLevel * 256 - ((s32)*curLevel256 + *stepLevel256)) + 256)
							  : ((u32)minLevel * 256);
	}else{
		*curLevel256 += *stepLevel256;
	}

	if(*stepLevel256 > 0){
		*curLevel = (*curLevel256 + 127) / 256;
	}else{
		*curLevel = *curLevel256 / 256;
	}

	if(*remainingTime == 0){
		*curLevel256 = ((u32)*curLevel) * 256;
		*stepLevel256 = 0;
	}else if(*remainingTime != 0xFFFF){
		*remainingTime = *remainingTime -1;
	}

	light_fresh();
}

/*********************************************************************
 * @fn      light_blink_TimerEvtCb
 *
 * @brief
 *
 * @param   arg
 *
 * @return  0: timer continue on; -1: timer will be canceled
 */
s32 light_blink_TimerEvtCb(void *arg)
{
	u32 interval = 0;

	if(gLightCtx.sta == gLightCtx.oriSta){
		if(gLightCtx.times){
			gLightCtx.times--;
			if(gLightCtx.times <= 0){
				if(gLightCtx.oriSta){
					hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
				}else{
					hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
				}

				gLightCtx.timerLedEvt = NULL;
				return -1;
			}
		}
	}

	gLightCtx.sta = !gLightCtx.sta;
	if(gLightCtx.sta){
		hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
		interval = gLightCtx.ledOnTime;
	}else{
		hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
		interval = gLightCtx.ledOffTime;
	}

	return interval;
}

/*********************************************************************
 * @fn      light_blink_start
 *
 * @brief
 *
 * @param   times 		- counts
 * @param   ledOnTime	- on times, ms
 * @param   ledOffTime	- off times, ms
 *
 * @return  None
 */
void light_blink_start(u8 times, u16 ledOnTime, u16 ledOffTime)
{
	DEBUG(DEBUG_LEDEFFECT, "light_blink_start\r");
	u32 interval = 0;
	zcl_onOffAttr_t *pOnoff = zcl_onoffAttrGet();

	gLightCtx.oriSta = pOnoff->onOff;
	gLightCtx.times = times;

	if(!gLightCtx.timerLedEvt){
		if(gLightCtx.oriSta){
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
			gLightCtx.sta = 0;
			interval = ledOffTime;
		}else{
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
			gLightCtx.sta = 1;
			interval = ledOnTime;
		}
		gLightCtx.ledOnTime = ledOnTime;
		gLightCtx.ledOffTime = ledOffTime;

		gLightCtx.timerLedEvt = TL_ZB_TIMER_SCHEDULE(light_blink_TimerEvtCb, NULL, interval);
	}
}

/*********************************************************************
 * @fn      light_blink_stop
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void light_blink_stop(void)
{
	DEBUG(DEBUG_LEDEFFECT, "light_blink_stop\r");
	if(gLightCtx.timerLedEvt){
		TL_ZB_TIMER_CANCEL(&gLightCtx.timerLedEvt);

		gLightCtx.times = 0;
		if(gLightCtx.oriSta){
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_ON);
		}else{
			hwLight_onOffUpdate(ZCL_CMD_ONOFF_OFF);
		}
	}
}

#endif	/* __PROJECT_TL_DIMMABLE_LIGHT__ */
