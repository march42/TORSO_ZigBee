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
#define PWM_FREQUENCY					4000
#define PWM_FULL_DUTYCYCLE				100
#define PMW_MAX_TICK		            (PWM_CLOCK_SOURCE / PWM_FREQUENCY)


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
 * @fn      pwmSetDuty
 *
 * @brief
 *
 * @param   ch			-	PWM channel
 * 			dutycycle	-	level * PWM_FULL_DUTYCYCLE
 *
 * @return  None
 */
void pwmSetDuty(u8 ch, u16 dutycycle)
{
#ifdef ZCL_LEVEL_CTRL
	u32 cmp_tick = ((u32)dutycycle * PMW_MAX_TICK) / (ZCL_LEVEL_ATTR_MAX_LEVEL * PWM_FULL_DUTYCYCLE);
	drv_pwm_cfg(ch, (u16)cmp_tick, PMW_MAX_TICK);
#endif
}

/*********************************************************************
 * @fn      pwmInit
 *
 * @brief
 *
 * @param   ch			-	PWM channel
 * 			dutycycle	-	level * PWM_FULL_DUTYCYCLE
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
	pwmInit(COLD_LIGHT_PWM_CHANNEL, 20);
#endif
#if (COLOR_RGB_SUPPORT)
	R_LIGHT_PWM_SET();
	G_LIGHT_PWM_SET();
	B_LIGHT_PWM_SET();
	pwmInit(R_LIGHT_PWM_CHANNEL, 20);
	pwmInit(G_LIGHT_PWM_CHANNEL, 20);
	pwmInit(B_LIGHT_PWM_CHANNEL, 20);
#endif
#if (COLOR_CCT_SUPPORT)
	WARM_LIGHT_PWM_SET();
	pwmInit(WARM_LIGHT_PWM_CHANNEL, 20);
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
	DEBUG(DEBUG_TRACE, "onOffUpdate(%x)\r", onOff);

	if(onOff){
		DEBUG(DEBUG_LED_PWM, "pwmStart\r");
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
		DEBUG(DEBUG_LED_PWM, "pwmStopp\r");
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
	DEBUG(DEBUG_TRACE, "levelUpdate(%x)\r", level);

#if (SINGLE_WHITE_SUPPORT) && (!COLOR_RGB_SUPPORT) /* && (!COLOR_CCT_SUPPORT) */
	level = (level < 0x10) ? 0x10 : level;

	LEDLIGHT_WHITE->OnOff	= (LEDLIGHT_WHITE->Value != 0);
	u16 tick = (LEDLIGHT_WHITE->Value * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL);
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL, tick);
	DEBUG(DEBUG_LED_PWM, "WHITE(%x)\r", level);
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

#if (COLOR_RGB_SUPPORT) && (COLOR_CCT_SUPPORT) && (COLD_LIGHT_TEMPERATURE) && (WARM_LIGHT_TEMPERATURE)
	u16 RGB_CT = colorTemperatureMireds;
	float RGB_correct = 1;

	TODO("ensure min/max of LEDLIGHT_setMired function");
	if (colorTemperatureMireds < COLOR_TEMPERATURE_12000K) {				// LEDLIGHT_setMired minimum
		RGB_CT = COLOR_TEMPERATURE_12000K;
		LEDLIGHT_COLD->Value = (float)(((colorTemperatureMireds - RGB_CT)) / (COLD_LIGHT_TEMPERATURE - RGB_CT));
		RGB_correct = 1 - LEDLIGHT_COLD->Value;
	} else if (colorTemperatureMireds > COLOR_TEMPERATURE_1000K) {			// LEDLIGHT_setMired maximum
		RGB_CT = COLOR_TEMPERATURE_1000K;
		LEDLIGHT_WARM->Value = (float)(((RGB_CT - colorTemperatureMireds)) / (RGB_CT - WARM_LIGHT_TEMPERATURE));
		RGB_correct = 1 - LEDLIGHT_WARM->Value;
	} else

	if (colorTemperatureMireds <= COLD_LIGHT_TEMPERATURE)
	{
		RGB_CT -= (COLD_LIGHT_TEMPERATURE - colorTemperatureMireds);
		// 50% COLD + 50% RGB
		LEDLIGHT_COLD->Value = 1;
	} else
	if (colorTemperatureMireds >= WARM_LIGHT_TEMPERATURE)
	{
		RGB_CT += (colorTemperatureMireds - WARM_LIGHT_TEMPERATURE);
		// 50% WARM + 50% RGB
		LEDLIGHT_WARM->Value = 1;
	} else
	{
		// RGB_CT = colorTemperatureMireds
		// 50% RGB + W*50% WARM + C=50% COLD
		if (temperatureMireds >= WARM_LIGHT_TEMPERATURE) {
			LEDLIGHT_WARM->Value = 1;			// catch out of boundary values
		} else if (temperatureMireds <= COLD_LIGHT_TEMPERATURE) {
			LEDLIGHT_WARM->Value = 0;			// catch out of boundary values
		} else {
			LEDLIGHT_WARM->Value = (float)(((temperatureMireds - COLD_LIGHT_TEMPERATURE)) / (WARM_LIGHT_TEMPERATURE - COLD_LIGHT_TEMPERATURE));
		}
		LEDLIGHT_COLD->Value = 1 - LEDLIGHT_WARM->Value;
	}

	LEDLIGHT_setMired (RGB_CT);
	// (level / 2) to split 50% on RGB and 50% on CCT
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL,	((u16)(LEDLIGHT_COLD->Value                * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(WARM_LIGHT_PWM_CHANNEL,	((u16)(LEDLIGHT_WARM->Value                * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_RED->Value   * RGB_correct * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_GREEN->Value * RGB_correct * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_BLUE->Value  * RGB_correct * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_COLD->OnOff	= (LEDLIGHT_COLD->Value != 0);
	LEDLIGHT_WARM->OnOff	= (LEDLIGHT_WARM->Value != 0);
	LEDLIGHT_RED->OnOff		= (LEDLIGHT_RED->Value != 0);
	LEDLIGHT_GREEN->OnOff	= (LEDLIGHT_GREEN->Value != 0);
	LEDLIGHT_BLUE->OnOff	= (LEDLIGHT_BLUE->Value != 0);

#elif (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)

#	if (COLD_LIGHT_TEMPERATURE)
#		define WHITE_CT		COLD_LIGHT_TEMPERATURE
#	else /* just using COLOR_TEMPERATURE_NEUTRAL as assumed default, because not defined */
#		define WHITE_CT		COLOR_TEMPERATURE_NEUTRAL
#	endif

	LEDLIGHT_WHITE->Value = 1;
	float RGB_correct = 1;
	u16 RGB_CT = colorTemperatureMireds;
	if (colorTemperatureMireds < WHITE_CT) {				// colder WHITE
		RGB_CT -= (WHITE_CT - colorTemperatureMireds);
	} else if (colorTemperatureMireds > WHITE_CT) {			//	warmer WHITE
		RGB_CT += (colorTemperatureMireds - WHITE_CT);
	}

	// fix range limits
	if (RGB_CT < COLOR_TEMPERATURE_12000K) {				// LEDLIGHT_setMired minimum
		RGB_CT = COLOR_TEMPERATURE_12000K;
		LEDLIGHT_WHITE->Value = (float)(((colorTemperatureMireds - RGB_CT)) / (WHITE_CT - RGB_CT));
	} else if (RGB_CT > COLOR_TEMPERATURE_1000K) {			// LEDLIGHT_setMired maximum
		RGB_CT = COLOR_TEMPERATURE_1000K;
		LEDLIGHT_WHITE->Value = (float)(((RGB_CT - colorTemperatureMireds)) / (RGB_CT - WHITE_CT));
	}
	if (LEDLIGHT_WHITE->Value > 1) {
		LEDLIGHT_WHITE->Value = 1;
	} else if (LEDLIGHT_WHITE->Value != 1) {
		RGB_correct = 1 - LEDLIGHT_WHITE->Value;
	}

	// (level / 2) to split 50% on RGB and 50% on WHITE
	LEDLIGHT_setMired (RGB_CT);
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL,	((u16)(LEDLIGHT_WHITE->Value               * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_RED->Value   * RGB_correct * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_GREEN->Value * RGB_correct * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_BLUE->Value  * RGB_correct * PMW_MAX_TICK * (level / 2) / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_WHITE->OnOff	= (LEDLIGHT_WHITE->Value != 0);
	LEDLIGHT_RED->OnOff		= (LEDLIGHT_RED->Value != 0);
	LEDLIGHT_GREEN->OnOff	= (LEDLIGHT_GREEN->Value != 0);
	LEDLIGHT_BLUE->OnOff	= (LEDLIGHT_BLUE->Value != 0);

#elif (COLOR_CCT_SUPPORT)
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();
	if (temperatureMireds >= pColor->colorTempPhysicalMaxMireds) {
		LEDLIGHT_WARM->Value = 1;			// catch out of boundary values
	} else if (temperatureMireds <= pColor->colorTempPhysicalMinMireds) {
		LEDLIGHT_WARM->Value = 0;			// catch out of boundary values
	} else {
		LEDLIGHT_WARM->Value = (float)(((temperatureMireds - pColor->colorTempPhysicalMinMireds)) / (pColor->colorTempPhysicalMaxMireds - pColor->colorTempPhysicalMinMireds)) ;
	}
	LEDLIGHT_COLD->Value = 1 - LEDLIGHT_WARM->Value;
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL,	((u16)(LEDLIGHT_COLD->Value  * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(WARM_LIGHT_PWM_CHANNEL,	((u16)(LEDLIGHT_WARM->Value  * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_COLD->OnOff	= (LEDLIGHT_COLD->Value != 0);
	LEDLIGHT_WARM->OnOff	= (LEDLIGHT_WARM->Value != 0);

#elif (COLOR_RGB_SUPPORT)
	LEDLIGHT_setMired (colorTemperatureMireds);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_RED->Value   * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_GREEN->Value * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_BLUE->Value  * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_RED->OnOff		= (LEDLIGHT_RED->Value != 0);
	LEDLIGHT_GREEN->OnOff	= (LEDLIGHT_GREEN->Value != 0);
	LEDLIGHT_BLUE->OnOff	= (LEDLIGHT_BLUE->Value != 0);

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
	LEDLIGHT_setHSV (hue, saturation, ZCL_LEVEL_ATTR_MAX_LEVEL);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_RED->Value   * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_GREEN->Value * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_BLUE->Value  * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_RED->OnOff		= (LEDLIGHT_RED->Value != 0);
	LEDLIGHT_GREEN->OnOff	= (LEDLIGHT_GREEN->Value != 0);
	LEDLIGHT_BLUE->OnOff	= (LEDLIGHT_BLUE->Value != 0);
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
	LEDLIGHT_setEnhancedHSV (enhancedHue, saturation, ZCL_LEVEL_ATTR_MAX_LEVEL, hue);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_RED->Value   * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_GREEN->Value * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_BLUE->Value  * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_RED->OnOff		= (LEDLIGHT_RED->Value != 0);
	LEDLIGHT_GREEN->OnOff	= (LEDLIGHT_GREEN->Value != 0);
	LEDLIGHT_BLUE->OnOff	= (LEDLIGHT_BLUE->Value != 0);
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
	LEDLIGHT_setXY (x, y);
	pwmSetDuty(R_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_RED->Value   * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(G_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_GREEN->Value * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	pwmSetDuty(B_LIGHT_PWM_CHANNEL,		((u16)(LEDLIGHT_BLUE->Value  * PMW_MAX_TICK * level / ZCL_LEVEL_ATTR_MAX_LEVEL)));
	LEDLIGHT_RED->OnOff		= (LEDLIGHT_RED->Value != 0);
	LEDLIGHT_GREEN->OnOff	= (LEDLIGHT_GREEN->Value != 0);
	LEDLIGHT_BLUE->OnOff	= (LEDLIGHT_BLUE->Value != 0);
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
	g_PwmChannels.CH1_CycleTick = ZCL_LEVEL_ATTR_MAX_LEVEL * PWM_FULL_DUTYCYCLE;
	pwmSetDuty(COLD_LIGHT_PWM_CHANNEL, g_PwmChannels.CH1_CycleTick);
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
