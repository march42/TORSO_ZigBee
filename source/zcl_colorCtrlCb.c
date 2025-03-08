/********************************************************************************************************
 * @file    zcl_colorCtrlCb.c
 *
 * @brief   This is the source file for zcl_colorCtrlCb
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
#include "zb_api.h"
#include "zcl_include.h"
#include "ledLight.h"
#include "ledLightCtrl.h"

#ifdef ZCL_LIGHT_COLOR_CONTROL

/**********************************************************************
 * LOCAL CONSTANTS
 */
#define ZCL_COLOR_CHANGE_INTERVAL		100



/**********************************************************************
 * TYPEDEFS
 */
typedef struct{
#if (COLOR_RGB_SUPPORT)
	// ZCL_COLOR_CAPABILITIES_BIT_HUE_SATURATION
	s32 stepHue256;
	u16	currentHue256;
	u16	hueRemainingTime;

	s32 stepSaturation256;
	u16 currentSaturation256;
	u16 saturationRemainingTime;

	s32 stepX;
	u32 currentX;
	s32 stepY;
	u32 currentY;
	u16 XYRemainingTime;
#endif

#if (EXTENDED_COLOR_LIGHT)
	u16 loopRemainingTime;
#endif

#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
	// ZCL_COLOR_CAPABILITIES_BIT_COLOR_TEMPERATURE
	s32 stepColorTemp256;
	u32	currentColorTemp256;
	u16	colorTempRemainingTime;
	u16	colorTempMinMireds;
	u16	colorTempMaxMireds;
#endif
}zcl_colorInfo_t;

/**********************************************************************
 * LOCAL VARIABLES
 */
static zcl_colorInfo_t colorInfo = {
#if (COLOR_RGB_SUPPORT)
	.stepHue256 				= 0,
	.currentHue256				= 0,
	.hueRemainingTime			= 0,

	.stepSaturation256			= 0,
	.currentSaturation256		= 0,
	.saturationRemainingTime	= 0,

	.stepX						= 0,
	.currentX					= 0,
	.stepY						= 0,
	.currentY					= 0,
	.XYRemainingTime			= 0,
#endif

#if (EXTENDED_COLOR_LIGHT)
	.loopRemainingTime			= 0,
#endif

#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
	.stepColorTemp256			= 0,
	.currentColorTemp256		= 0,
	.colorTempRemainingTime		= 0,
	.colorTempMinMireds			= 0,
	.colorTempMaxMireds			= 0,
#endif
};

static ev_timer_event_t *colorTimerEvt = NULL;
#if (COLOR_RGB_SUPPORT) && (EXTENDED_COLOR_LIGHT)
static ev_timer_event_t *colorLoopTimerEvt = NULL;
#endif


/**********************************************************************
 * FUNCTIONS
 */
void ledLight_updateColorMode(u8 colorMode);


/*********************************************************************
 * @fn      ledLight_colorInit
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void ledLight_colorInit(void)
{
	DEBUG(DEBUG_TRACE, "ledLight_colorInit\r");

	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	pColor->colorCapabilities = 0x0000
#if (COLOR_RGB_SUPPORT)
		| ZCL_COLOR_CAPABILITIES_BIT_HUE_SATURATION
		| ZCL_COLOR_CAPABILITIES_BIT_X_Y_ATTRIBUTES
#	if (EXTENDED_COLOR_LIGHT)
		| ZCL_COLOR_CAPABILITIES_BIT_ENHANCED_HUE
		| ZCL_COLOR_CAPABILITIES_BIT_COLOR_LOOP
#	endif
#endif
#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
		| ZCL_COLOR_CAPABILITIES_BIT_COLOR_TEMPERATURE
#endif
		;
#if (COLOR_RGB_SUPPORT)
	colorInfo.currentHue256 = (u16)(pColor->currentHue) << 8;
	colorInfo.currentSaturation256 = (u16)(pColor->currentSaturation) << 8;
	colorInfo.hueRemainingTime = 0;
	colorInfo.saturationRemainingTime = 0;
#endif
#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
	colorInfo.currentColorTemp256 = (u32)(pColor->colorTemperatureMireds) << 8;
	colorInfo.colorTempRemainingTime = 0;
#endif
	
#if (COLOR_RGB_SUPPORT)
	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	light_applyUpdate(&pColor->currentHue, &colorInfo.currentHue256, &colorInfo.stepHue256, &colorInfo.hueRemainingTime,
						ZCL_COLOR_ATTR_HUE_MIN, ZCL_COLOR_ATTR_HUE_MAX, TRUE);

	light_applyUpdate(&pColor->currentSaturation, &colorInfo.currentSaturation256, &colorInfo.stepSaturation256, &colorInfo.saturationRemainingTime,
						ZCL_COLOR_ATTR_SATURATION_MIN, ZCL_COLOR_ATTR_SATURATION_MAX, FALSE);
#elif (COLOR_CCT_SUPPORT)
	pColor->colorMode = ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS;

	light_applyUpdate_16(&pColor->colorTemperatureMireds, &colorInfo.currentColorTemp256, &colorInfo.stepColorTemp256, &colorInfo.colorTempRemainingTime,
							pColor->colorTempPhysicalMinMireds, pColor->colorTempPhysicalMaxMireds, FALSE);
#endif
}

/*********************************************************************
 * @fn      ledLight_updateColorMode
 *
 * @brief
 *
 * @param   ZCL_COLOR_MODE_CURRENT_HUE_SATURATION
 * 			ZCL_COLOR_MODE_CURRENT_X_Y
 * 			ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS
 * 			ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION
 *
 * @return  None
 */
void ledLight_updateColorMode(u8 colorMode)
{
	DEBUG(DEBUG_TRACE, "ledLight_updateColorMode\r");

	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	if(colorMode != pColor->colorMode){
#if (COLOR_RGB_SUPPORT)
		if (colorMode == ZCL_COLOR_MODE_CURRENT_HUE_SATURATION) {
			TODO("synchronize color data ??? maybe better when changing");
			pColor->colorMode = colorMode;
			pColor->enhancedColorMode = colorMode;

		} else if (colorMode == ZCL_COLOR_MODE_CURRENT_X_Y) {
			TODO("synchronize color data ??? maybe better when changing");
			pColor->colorMode = colorMode;
			pColor->enhancedColorMode = colorMode;

		} else
#	if (EXTENDED_COLOR_LIGHT)
		if (colorMode == ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION) {
			TODO("synchronize color data ??? maybe better when changing");
			pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;		/* for compatibility reasons (ZCL rev.8) */
			pColor->enhancedColorMode = colorMode;

		} else
#	endif
#endif

#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
		if (colorMode == ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS) {
			TODO("synchronize color data ??? maybe better when changing");
			pColor->colorMode = colorMode;
			pColor->enhancedColorMode = colorMode;

		} else
#endif
		{
			pColor->colorMode = colorMode;
			pColor->enhancedColorMode = colorMode;
		}
	}
}

/*********************************************************************
 * @fn      ledLight_updateColor
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
void ledLight_updateColor(void)
{
	DEBUG(DEBUG_TRACE, "updateColor\r");

	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();
	zcl_levelAttr_t *pLevel = zcl_levelAttrGet();

#if (COLOR_RGB_SUPPORT)
	if (pColor->colorMode == ZCL_COLOR_MODE_CURRENT_HUE_SATURATION) {
		hwLight_colorUpdate_HSV2RGB(pColor->currentHue, pColor->currentSaturation, pLevel->curLevel);

	} else if (pColor->colorMode == ZCL_COLOR_MODE_CURRENT_X_Y) {
		hwLight_colorUpdate_xyY2RGB(pColor->currentX, pColor->currentY, pLevel->curLevel);

	} else
#endif

#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
	if (pColor->colorMode == ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS) {
		hwLight_colorUpdate_colorTemperature(pColor->colorTemperatureMireds, pLevel->curLevel);
	} else
#endif

#if (EXTENDED_COLOR_LIGHT)
	if (pColor->colorMode == ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION) {
		pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;		/* for compatibility reasons (ZCL rev.8) */
		hwLight_colorUpdate_enhancedHSV2RGB(pColor->enhancedCurrentHue, pColor->currentSaturation, pLevel->curLevel, &pColor->currentHue);
	} else
#endif

	{
		// ERROR: this should not happen
		TRACE("INVALID colorMode=%x\r",pColor->colorMode);
	}
}

/*********************************************************************
 * @fn      ledLight_colorTimerEvtCb
 *
 * @brief
 *
 * @param   arg
 *
 * @return  0: timer continue on; -1: timer will be canceled
 */
static s32 ledLight_colorTimerEvtCb(void *arg)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

#if (COLOR_RGB_SUPPORT)
	if( (pColor->enhancedColorMode == ZCL_COLOR_MODE_CURRENT_HUE_SATURATION) ||
		(pColor->enhancedColorMode == ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION) ){
		if(colorInfo.saturationRemainingTime){
			light_applyUpdate(&pColor->currentSaturation, &colorInfo.currentSaturation256, &colorInfo.stepSaturation256, &colorInfo.saturationRemainingTime,
									ZCL_COLOR_ATTR_SATURATION_MIN, ZCL_COLOR_ATTR_SATURATION_MAX, FALSE);
		}

		if(colorInfo.hueRemainingTime){
			light_applyUpdate(&pColor->currentHue, &colorInfo.currentHue256, &colorInfo.stepHue256, &colorInfo.hueRemainingTime,
									ZCL_COLOR_ATTR_HUE_MIN, ZCL_COLOR_ATTR_HUE_MAX, TRUE);
		}
	}
	if (pColor->enhancedColorMode == ZCL_COLOR_MODE_CURRENT_X_Y) {
		if(colorInfo.XYRemainingTime){
			light_applyUpdate_16(&pColor->currentX, &colorInfo.currentX, &colorInfo.stepX, &colorInfo.XYRemainingTime,
				ZCL_COLOR_ATTR_XY_MIN, ZCL_COLOR_ATTR_XY_MAX, FALSE);
			light_applyUpdate_16(&pColor->currentY, &colorInfo.currentY, &colorInfo.stepY, &colorInfo.XYRemainingTime,
				ZCL_COLOR_ATTR_XY_MIN, ZCL_COLOR_ATTR_XY_MAX, FALSE);
		}
	}
#endif
#if (COLOR_CCT_SUPPORT)
	if(pColor->enhancedColorMode == ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS){
		if(colorInfo.colorTempRemainingTime){
			light_applyUpdate_16(&pColor->colorTemperatureMireds, &colorInfo.currentColorTemp256, &colorInfo.stepColorTemp256, &colorInfo.colorTempRemainingTime,
									colorInfo.colorTempMinMireds, colorInfo.colorTempMaxMireds, FALSE);
		}
	}
#endif

#if (COLOR_RGB_SUPPORT)
	if(colorInfo.saturationRemainingTime || colorInfo.hueRemainingTime || colorInfo.XYRemainingTime){
		return 0;
	} else
#endif
#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
	if(colorInfo.colorTempRemainingTime){
		return 0;
	} else
#endif
	{
		colorTimerEvt = NULL;
		return -1;
	}
}

/*********************************************************************
 * @fn      ledLight_colorTimerStop
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
static void ledLight_colorTimerStop(void)
{
	if(colorTimerEvt){
		TL_ZB_TIMER_CANCEL(&colorTimerEvt);
	}
}

#if (COLOR_RGB_SUPPORT)
/*********************************************************************
 * @fn      ledLight_colorLoopTimerEvtCb
 *
 * @brief
 *
 * @param   arg
 *
 * @return  0: timer continue on; -1: timer will be canceled
 */
static s32 ledLight_colorLoopTimerEvtCb(void *arg)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	if(pColor->colorLoopActive){
		/* TODO: colorLoop */
	}else{
		colorLoopTimerEvt = NULL;
		return -1;
	}

	return 0;
}

/*********************************************************************
 * @fn      ledLight_colorLoopTimerStop
 *
 * @brief
 *
 * @param   None
 *
 * @return  None
 */
static void ledLight_colorLoopTimerStop(void)
{
	if(colorLoopTimerEvt){
		TL_ZB_TIMER_CANCEL(&colorLoopTimerEvt);
	}
	// TODO: ??? restore previous (before color loop) setting
}


/*********************************************************************
 * @fn      ledLight_moveToHueProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveToHueProcess(zcl_colorCtrlMoveToHueCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	colorInfo.currentHue256 = (u16)(pColor->currentHue) << 8;

	s16 hueDiff = (s16)cmd->hue - pColor->currentHue;

	switch(cmd->direction){
		case COLOR_CTRL_DIRECTION_SHORTEST_DISTANCE:
			if(hueDiff > (ZCL_COLOR_ATTR_HUE_MAX / 2)){
				hueDiff -= (ZCL_COLOR_ATTR_HUE_MAX + 1);
			}else if(hueDiff < -ZCL_COLOR_ATTR_HUE_MAX / 2){
				hueDiff += (ZCL_COLOR_ATTR_HUE_MAX + 1);
			}
			break;
		case COLOR_CTRL_DIRECTION_LONGEST_DISTANCE:
			if((hueDiff > 0) && (hueDiff < (ZCL_COLOR_ATTR_HUE_MAX / 2))){
				hueDiff -= (ZCL_COLOR_ATTR_HUE_MAX + 1);
			}else if((hueDiff < 0) && (hueDiff > -ZCL_COLOR_ATTR_HUE_MAX / 2)){
				hueDiff += (ZCL_COLOR_ATTR_HUE_MAX + 1);
			}
			break;
		case COLOR_CTRL_DIRECTION_UP:
			if(hueDiff < 0){
				hueDiff += ZCL_COLOR_ATTR_HUE_MAX;
			}
			break;
		case COLOR_CTRL_DIRECTION_DOWN:
			if(hueDiff > 0){
				hueDiff -= ZCL_COLOR_ATTR_HUE_MAX;
			}
			break;
		default:
			break;
	}

	colorInfo.hueRemainingTime = (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;
	colorInfo.stepHue256 = ((s32)hueDiff) << 8;
	colorInfo.stepHue256 /= (s32)colorInfo.hueRemainingTime;

	light_applyUpdate(&pColor->currentHue, &colorInfo.currentHue256, &colorInfo.stepHue256, &colorInfo.hueRemainingTime,
							ZCL_COLOR_ATTR_HUE_MIN, ZCL_COLOR_ATTR_HUE_MAX, TRUE);

	if(colorInfo.hueRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_moveHueProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveHueProcess(zcl_colorCtrlMoveHueCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	colorInfo.currentHue256 = (u16)(pColor->currentHue) << 8;

	switch(cmd->moveMode){
		case COLOR_CTRL_MOVE_STOP:
			colorInfo.stepHue256 = 0;
			colorInfo.hueRemainingTime = 0;
			break;
		case COLOR_CTRL_MOVE_UP:
			colorInfo.stepHue256 = (((s32)cmd->rate) << 8) / 10;
			colorInfo.hueRemainingTime = 0xFFFF;
			break;
		case COLOR_CTRL_MOVE_DOWN:
			colorInfo.stepHue256 = ((-(s32)cmd->rate) << 8) / 10;
			colorInfo.hueRemainingTime = 0xFFFF;
			break;
		default:
			break;
	}

	light_applyUpdate(&pColor->currentHue, &colorInfo.currentHue256, &colorInfo.stepHue256, &colorInfo.hueRemainingTime,
							ZCL_COLOR_ATTR_HUE_MIN, ZCL_COLOR_ATTR_HUE_MAX, TRUE);

	if(colorInfo.hueRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_stepHueProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_stepHueProcess(zcl_colorCtrlStepHueCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	colorInfo.currentHue256 = (u16)(pColor->currentHue) << 8;

	colorInfo.hueRemainingTime = (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;

	colorInfo.stepHue256 = (((s32)cmd->stepSize) << 8) / colorInfo.hueRemainingTime;

	switch(cmd->stepMode){
		case COLOR_CTRL_STEP_MODE_UP:
			break;
		case COLOR_CTRL_STEP_MODE_DOWN:
			colorInfo.stepHue256 = -colorInfo.stepHue256;
			break;
		default:
			break;
	}

	light_applyUpdate(&pColor->currentHue, &colorInfo.currentHue256, &colorInfo.stepHue256, &colorInfo.hueRemainingTime,
							ZCL_COLOR_ATTR_HUE_MIN, ZCL_COLOR_ATTR_HUE_MAX, TRUE);

	if(colorInfo.hueRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_moveToSaturationProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveToSaturationProcess(zcl_colorCtrlMoveToSaturationCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	colorInfo.currentSaturation256 = (u16)(pColor->currentSaturation) << 8;

	colorInfo.saturationRemainingTime = (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;

	colorInfo.stepSaturation256 = ((s32)(cmd->saturation - pColor->currentSaturation)) << 8;
	colorInfo.stepSaturation256 /= (s32)colorInfo.saturationRemainingTime;

	light_applyUpdate(&pColor->currentSaturation, &colorInfo.currentSaturation256, &colorInfo.stepSaturation256, &colorInfo.saturationRemainingTime,
							ZCL_COLOR_ATTR_SATURATION_MIN, ZCL_COLOR_ATTR_SATURATION_MAX, FALSE);

	if(colorInfo.saturationRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_moveSaturationProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveSaturationProcess(zcl_colorCtrlMoveSaturationCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	colorInfo.currentSaturation256 = (u16)(pColor->currentSaturation) << 8;

	switch(cmd->moveMode){
		case COLOR_CTRL_MOVE_STOP:
			colorInfo.stepSaturation256 = 0;
			colorInfo.saturationRemainingTime = 0;
			break;
		case COLOR_CTRL_MOVE_UP:
			colorInfo.stepSaturation256 = (((s32)cmd->rate) << 8) / 10;
			colorInfo.saturationRemainingTime = 0xFFFF;
			break;
		case COLOR_CTRL_MOVE_DOWN:
			colorInfo.stepSaturation256 = ((-(s32)cmd->rate) << 8) / 10;
			colorInfo.saturationRemainingTime = 0xFFFF;
			break;
		default:
			break;
	}

	light_applyUpdate(&pColor->currentSaturation, &colorInfo.currentSaturation256, &colorInfo.stepSaturation256, &colorInfo.saturationRemainingTime,
							ZCL_COLOR_ATTR_SATURATION_MIN, ZCL_COLOR_ATTR_SATURATION_MAX, FALSE);

	if(colorInfo.saturationRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_stepSaturationProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_stepSaturationProcess(zcl_colorCtrlStepSaturationCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;

	colorInfo.currentSaturation256 = (u16)(pColor->currentSaturation) << 8;

	colorInfo.saturationRemainingTime = (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;

	colorInfo.stepSaturation256 = (((s32)cmd->stepSize) << 8) / colorInfo.saturationRemainingTime;

	switch(cmd->stepMode){
		case COLOR_CTRL_STEP_MODE_UP:
			break;
		case COLOR_CTRL_STEP_MODE_DOWN:
			colorInfo.stepSaturation256 = -colorInfo.stepSaturation256;
			break;
		default:
			break;
	}

	light_applyUpdate(&pColor->currentSaturation, &colorInfo.currentSaturation256, &colorInfo.stepSaturation256, &colorInfo.saturationRemainingTime,
							ZCL_COLOR_ATTR_SATURATION_MIN, ZCL_COLOR_ATTR_SATURATION_MAX, FALSE);

	if(colorInfo.saturationRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_moveToHueAndSaturationProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveToHueAndSaturationProcess(zcl_colorCtrlMoveToHueAndSaturationCmd_t *cmd)
{
	zcl_colorCtrlMoveToHueCmd_t moveToHueCmd;
	zcl_colorCtrlMoveToSaturationCmd_t moveToSaturationCmd;

	moveToHueCmd.hue = cmd->hue;
	moveToHueCmd.direction = COLOR_CTRL_DIRECTION_SHORTEST_DISTANCE;
	moveToHueCmd.transitionTime = cmd->transitionTime;

	moveToSaturationCmd.saturation = cmd->saturation;
	moveToSaturationCmd.transitionTime = cmd->transitionTime;

	ledLight_moveToHueProcess(&moveToHueCmd);
	ledLight_moveToSaturationProcess(&moveToSaturationCmd);
}

/*********************************************************************
 * @fn      ledLight_moveToColorProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveToColorProcess(zcl_colorCtrlMoveToColorCmd_t *cmd)
{
	DEBUG(DEBUG_LEDCOLOR, "moveToColor X=%x, Y=%x",cmd->colorX,cmd->colorY);

	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_X_Y);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_X_Y;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_X_Y;

	colorInfo.currentX			= (u32)(pColor->currentX) << 8;
	colorInfo.currentY			= (u32)(pColor->currentY) << 8;
	colorInfo.XYRemainingTime	= (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;

	colorInfo.stepX				= ((s32)(cmd->colorX - pColor->currentX)) << 8;
	colorInfo.stepX				/= (s32)colorInfo.saturationRemainingTime;
	light_applyUpdate_16(&pColor->currentX, &colorInfo.currentX, &colorInfo.stepX, &colorInfo.XYRemainingTime,
				ZCL_COLOR_ATTR_XY_MIN, ZCL_COLOR_ATTR_XY_MAX, FALSE);

	colorInfo.stepY				= ((s32)(cmd->colorY - pColor->currentY)) << 8;
	colorInfo.stepY				/= (s32)colorInfo.saturationRemainingTime;
	light_applyUpdate_16(&pColor->currentY, &colorInfo.currentY, &colorInfo.stepY, &colorInfo.XYRemainingTime,
				ZCL_COLOR_ATTR_XY_MIN, ZCL_COLOR_ATTR_XY_MAX, FALSE);

	if(colorInfo.XYRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_moveColorProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveColorProcess(zcl_colorCtrlMoveColorCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_X_Y);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_X_Y;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_X_Y;


}

/*********************************************************************
 * @fn      ledLight_stepColorProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_stepColorProcess(zcl_colorCtrlStepColorCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_X_Y);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_X_Y;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_CURRENT_X_Y;


}

/*********************************************************************
 * @fn      ledLight_enhancedMoveToHueProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_enhancedMoveToHueProcess(zcl_colorCtrlEnhancedMoveToHueCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION;

	switch(cmd->direction){
		case COLOR_CTRL_DIRECTION_SHORTEST_DISTANCE:
			break;
		case COLOR_CTRL_DIRECTION_LONGEST_DISTANCE:
			break;
		case COLOR_CTRL_DIRECTION_UP:
			break;
		case COLOR_CTRL_DIRECTION_DOWN:
			break;
		default:
			break;
	}


}

/*********************************************************************
 * @fn      ledLight_enhancedMoveHueProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_enhancedMoveHueProcess(zcl_colorCtrlEnhancedMoveHueCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION;

	switch(cmd->moveMode){
		case COLOR_CTRL_MOVE_STOP:
			break;
		case COLOR_CTRL_MOVE_UP:
			break;
		case COLOR_CTRL_MOVE_DOWN:
			break;
		default:
			break;
	}


}

/*********************************************************************
 * @fn      ledLight_enhancedStepHueProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_enhancedStepHueProcess(zcl_colorCtrlEnhancedStepHueCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_CURRENT_HUE_SATURATION);

	pColor->colorMode = ZCL_COLOR_MODE_CURRENT_HUE_SATURATION;
	pColor->enhancedColorMode = ZCL_ENHANCED_COLOR_MODE_CURRENT_HUE_SATURATION;

	switch(cmd->stepMode){
		case COLOR_CTRL_STEP_MODE_UP:
			break;
		case COLOR_CTRL_STEP_MODE_DOWN:
			break;
		default:
			break;
	}


}

/*********************************************************************
 * @fn      ledLight_enhancedMoveToHueAndSaturationProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_enhancedMoveToHueAndSaturationProcess(zcl_colorCtrlEnhancedMoveToHueAndSaturationCmd_t *cmd)
{
	zcl_colorCtrlEnhancedMoveToHueCmd_t enhancedMoveToHueCmd;
	zcl_colorCtrlMoveToSaturationCmd_t moveToSaturationCmd;

	enhancedMoveToHueCmd.enhancedHue = cmd->enhancedHue;
	enhancedMoveToHueCmd.direction = COLOR_CTRL_DIRECTION_SHORTEST_DISTANCE;
	enhancedMoveToHueCmd.transitionTime = cmd->transitionTime;

	moveToSaturationCmd.saturation = cmd->saturation;
	moveToSaturationCmd.transitionTime = cmd->transitionTime;

	ledLight_enhancedMoveToHueProcess(&enhancedMoveToHueCmd);
	ledLight_moveToSaturationProcess(&moveToSaturationCmd);
}

/*********************************************************************
 * @fn      ledLight_colorLoopSetProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_colorLoopSetProcess(zcl_colorCtrlColorLoopSetCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	if(cmd->updateFlags.bits.direction){
		pColor->colorLoopDirection = cmd->direction;
	}

	if(cmd->updateFlags.bits.time){
		pColor->colorLoopTime = cmd->time;
	}

	if(cmd->updateFlags.bits.startHue){
		pColor->colorLoopStartEnhancedHue = cmd->startHue;
	}

	if(cmd->updateFlags.bits.action){
		switch(cmd->action){
			case COLOR_LOOP_SET_DEACTION:
				// TODO: ??? stopp color loop
				break;
			case COLOR_LOOP_SET_ACTION_FROM_COLOR_LOOP_START_ENHANCED_HUE:
				// TODO: ??? start color loop timer
				break;
			case COLOR_LOOP_SET_ACTION_FROM_ENHANCED_CURRENT_HUE:
				// TODO: ??? start color loop timer
				break;
			default:
				break;
		}
	}

	if(colorInfo.loopRemainingTime) {
		ledLight_colorLoopTimerStop();
		colorLoopTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorLoopTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	} else {
		ledLight_colorLoopTimerStop();
	}
}
#endif

#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)

/*********************************************************************
 * @fn      ledLight_moveToColorTemperatureProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveToColorTemperatureProcess(zcl_colorCtrlMoveToColorTemperatureCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS);	/* set currentColorMode and synchronize */

	colorInfo.colorTempMinMireds = pColor->colorTempPhysicalMinMireds;
	colorInfo.colorTempMaxMireds = pColor->colorTempPhysicalMaxMireds;

	colorInfo.currentColorTemp256 = (u32)(pColor->colorTemperatureMireds) << 8;

	colorInfo.colorTempRemainingTime = (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;

	colorInfo.stepColorTemp256 = ((s32)(cmd->colorTemperature - pColor->colorTemperatureMireds)) << 8;
	colorInfo.stepColorTemp256 /= (s32)colorInfo.colorTempRemainingTime;

	light_applyUpdate_16(&pColor->colorTemperatureMireds, &colorInfo.currentColorTemp256, &colorInfo.stepColorTemp256, &colorInfo.colorTempRemainingTime,
							colorInfo.colorTempMinMireds, colorInfo.colorTempMaxMireds, FALSE);

	if(colorInfo.colorTempRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_moveColorTemperatureProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_moveColorTemperatureProcess(zcl_colorCtrlMoveColorTemperatureCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS);

	pColor->colorMode = ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS;

	if(cmd->colorTempMinMireds){
		colorInfo.colorTempMinMireds = (cmd->colorTempMinMireds < pColor->colorTempPhysicalMinMireds) ? pColor->colorTempPhysicalMinMireds
																									  : cmd->colorTempMinMireds;
	}else{
		colorInfo.colorTempMinMireds = pColor->colorTempPhysicalMinMireds;
	}

	if(cmd->colorTempMaxMireds){
		colorInfo.colorTempMaxMireds = (cmd->colorTempMaxMireds > pColor->colorTempPhysicalMaxMireds) ? pColor->colorTempPhysicalMaxMireds
																									  : cmd->colorTempMaxMireds;
	}else{
		colorInfo.colorTempMaxMireds = pColor->colorTempPhysicalMaxMireds;
	}

	colorInfo.currentColorTemp256 = (u32)(pColor->colorTemperatureMireds) << 8;

	switch(cmd->moveMode){
		case COLOR_CTRL_MOVE_STOP:
			colorInfo.stepColorTemp256 = 0;
			colorInfo.colorTempRemainingTime = 0;
			break;
		case COLOR_CTRL_MOVE_UP:
			colorInfo.stepColorTemp256 = (((s32)cmd->rate) << 8) / 10;
			colorInfo.colorTempRemainingTime = 0xFFFF;
			break;
		case COLOR_CTRL_MOVE_DOWN:
			colorInfo.stepColorTemp256 = ((-(s32)cmd->rate) << 8) / 10;
			colorInfo.colorTempRemainingTime = 0xFFFF;
			break;
		default:
			break;
	}

	light_applyUpdate_16(&pColor->colorTemperatureMireds, &colorInfo.currentColorTemp256, &colorInfo.stepColorTemp256, &colorInfo.colorTempRemainingTime,
							colorInfo.colorTempMinMireds, colorInfo.colorTempMaxMireds, FALSE);

	if(colorInfo.colorTempRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

/*********************************************************************
 * @fn      ledLight_stepColorTemperatureProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_stepColorTemperatureProcess(zcl_colorCtrlStepColorTemperatureCmd_t *cmd)
{
	zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

	ledLight_updateColorMode(ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS);

	pColor->colorMode = ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS;
	pColor->enhancedColorMode = ZCL_COLOR_MODE_COLOR_TEMPERATURE_MIREDS;

	if(cmd->colorTempMinMireds){
		colorInfo.colorTempMinMireds = (cmd->colorTempMinMireds < pColor->colorTempPhysicalMinMireds) ? pColor->colorTempPhysicalMinMireds
																									  : cmd->colorTempMinMireds;
	}else{
		colorInfo.colorTempMinMireds = pColor->colorTempPhysicalMinMireds;
	}

	if(cmd->colorTempMaxMireds){
		colorInfo.colorTempMaxMireds = (cmd->colorTempMaxMireds > pColor->colorTempPhysicalMaxMireds) ? pColor->colorTempPhysicalMaxMireds
																									  : cmd->colorTempMaxMireds;
	}else{
		colorInfo.colorTempMaxMireds = pColor->colorTempPhysicalMaxMireds;
	}

	colorInfo.currentColorTemp256 = (u32)(pColor->colorTemperatureMireds) << 8;

	colorInfo.colorTempRemainingTime = (cmd->transitionTime == 0) ? 1 : cmd->transitionTime;

	colorInfo.stepColorTemp256 = (((s32)cmd->stepSize) << 8) / colorInfo.colorTempRemainingTime;

	switch(cmd->stepMode){
		case COLOR_CTRL_STEP_MODE_UP:
			break;
		case COLOR_CTRL_STEP_MODE_DOWN:
			colorInfo.stepColorTemp256 = -colorInfo.stepColorTemp256;
			break;
		default:
			break;
	}

	light_applyUpdate_16(&pColor->colorTemperatureMireds, &colorInfo.currentColorTemp256, &colorInfo.stepColorTemp256, &colorInfo.colorTempRemainingTime,
							colorInfo.colorTempMinMireds, colorInfo.colorTempMaxMireds, FALSE);

	if(colorInfo.colorTempRemainingTime){
		ledLight_colorTimerStop();
		colorTimerEvt = TL_ZB_TIMER_SCHEDULE(ledLight_colorTimerEvtCb, NULL, ZCL_COLOR_CHANGE_INTERVAL);
	}else{
		ledLight_colorTimerStop();
	}
}

#endif

/*********************************************************************
 * @fn      ledLight_stopMoveStepProcess
 *
 * @brief
 *
 * @param   cmd
 *
 * @return  None
 */
static void ledLight_stopMoveStepProcess(void)
{
	//zcl_lightColorCtrlAttr_t *pColor = zcl_colorAttrGet();

#if (COLOR_RGB_SUPPORT)
	colorInfo.hueRemainingTime = 0;
	colorInfo.saturationRemainingTime = 0;
#endif
#if (COLOR_CCT_SUPPORT)
	colorInfo.colorTempRemainingTime = 0;
#endif

	ledLight_colorTimerStop();
}

/*********************************************************************
 * @fn      ledLight_colorCtrlCb
 *
 * @brief   Handler for ZCL COLOR CONTROL command. This function will set Color Control attribute first.
 *
 * @param   pAddrInfo
 * @param   cmdId - color cluster command id
 * @param   cmdPayload
 *
 * @return  status_t
 */
status_t ledLight_colorCtrlCb(zclIncomingAddrInfo_t *pAddrInfo, u8 cmdId, void *cmdPayload)
{
	if(pAddrInfo->dstEp == LEDLIGHT_ENDPOINT){
		switch(cmdId){
#if (COLOR_RGB_SUPPORT)
			/* Hue/saturation supported */
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_TO_HUE:
				ledLight_moveToHueProcess((zcl_colorCtrlMoveToHueCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_HUE:
				ledLight_moveHueProcess((zcl_colorCtrlMoveHueCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_STEP_HUE:
				ledLight_stepHueProcess((zcl_colorCtrlStepHueCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_TO_SATURATION:
				ledLight_moveToSaturationProcess((zcl_colorCtrlMoveToSaturationCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_SATURATION:
				ledLight_moveSaturationProcess((zcl_colorCtrlMoveSaturationCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_STEP_SATURATION:
				ledLight_stepSaturationProcess((zcl_colorCtrlStepSaturationCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_TO_HUE_AND_SATURATION:
				ledLight_moveToHueAndSaturationProcess((zcl_colorCtrlMoveToHueAndSaturationCmd_t *)cmdPayload);
				break;
			/* X/Y supported */
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_TO_COLOR:
				ledLight_moveToColorProcess((zcl_colorCtrlMoveToColorCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_COLOR:
				ledLight_moveColorProcess((zcl_colorCtrlMoveColorCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_STEP_COLOR:
				ledLight_stepColorProcess((zcl_colorCtrlStepColorCmd_t *)cmdPayload);
				break;
#	if (EXTENDED_COLOR_LIGHT)
			/* Enhanced hue supported */
			case ZCL_CMD_LIGHT_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE:
				ledLight_enhancedMoveToHueProcess((zcl_colorCtrlEnhancedMoveToHueCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_ENHANCED_MOVE_HUE:
				ledLight_enhancedMoveHueProcess((zcl_colorCtrlEnhancedMoveHueCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_ENHANCED_STEP_HUE:
				ledLight_enhancedStepHueProcess((zcl_colorCtrlEnhancedStepHueCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_ENHANCED_MOVE_TO_HUE_AND_SATURATION:
				ledLight_enhancedMoveToHueAndSaturationProcess((zcl_colorCtrlEnhancedMoveToHueAndSaturationCmd_t *)cmdPayload);
				break;
			/* Color loop supported */
			case ZCL_CMD_LIGHT_COLOR_CONTROL_COLOR_LOOP_SET:
				ledLight_colorLoopSetProcess((zcl_colorCtrlColorLoopSetCmd_t *)cmdPayload);
				break;
#	endif
#endif
#if (COLOR_CCT_SUPPORT) || (EXTENDED_COLOR_LIGHT)
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_TO_COLOR_TEMPERATURE:
				ledLight_moveToColorTemperatureProcess((zcl_colorCtrlMoveToColorTemperatureCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_MOVE_COLOR_TEMPERATURE:
				ledLight_moveColorTemperatureProcess((zcl_colorCtrlMoveColorTemperatureCmd_t *)cmdPayload);
				break;
			case ZCL_CMD_LIGHT_COLOR_CONTROL_STEP_COLOR_TEMPERATURE:
				ledLight_stepColorTemperatureProcess((zcl_colorCtrlStepColorTemperatureCmd_t *)cmdPayload);
				break;
#endif
			case ZCL_CMD_LIGHT_COLOR_CONTROL_STOP_MOVE_STEP:
				ledLight_stopMoveStepProcess();
				break;
			default:
				break;
		}
	}

	return ZCL_STA_SUCCESS;
}

#endif	/* ZCL_LIGHT_COLOR_CONTROL */

#endif  /* __PROJECT_TL_DIMMABLE_LIGHT__ */
