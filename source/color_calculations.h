/********************************************************************************************************
 * @file    color_calculations.h
 *
 * @brief   header file for calculation formula and tables for color conversion
 *
 * @author	Marc Hefter
 * @date	2024-2025
 * @par     Copyright (C) 2025, Marc Hefter (https://github.com/march42)
 *			All rights reserved.
 *
 *          Licensed under the GNU GENERAL PUBLIC LICENSE, Version 3
 *          You may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              https://www.gnu.org/licenses/gpl-3.0.txt
 *
 *******************************************************************************************************/

#ifndef _COLOR_CALCULATIONS_H_
#define _COLOR_CALCULATIONS_H_


/*	color temperature calculation
**	mired = 1,000,000 / kelvin
**	Mired equals 1 million over Temperature in Kelvin
**	kelvin = 1000000 / mired
*/
#	define COLOR_TEMPERATURE_CONVERT(kelvin_mired)	(1000000 / kelvin_mired)
/*	some useful values */
#	define COLOR_TEMPERATURE_12000K			0x0053
#	define COLOR_TEMPERATURE_10000K			0x0064
#	define COLOR_TEMPERATURE_7500K			0x0085
#	define COLOR_TEMPERATURE_6500K			0x0099	// blue sky daylight
#	define COLOR_TEMPERATURE_6000K			0x00A6
#	define COLOR_TEMPERATURE_5500K			0x00B5	// cold white
#	define COLOR_TEMPERATURE_5000K			0x00C8	// noon sun
#	define COLOR_TEMPERATURE_4000K			0x00FA	// morning sun
#	define COLOR_TEMPERATURE_3800K			0x0107	// neutral (mapped to R=1.0, G=0.8, B=0.6)
#	define COLOR_TEMPERATURE_3000K			0x014D	// warm white
#	define COLOR_TEMPERATURE_2700K			0x0172	// soft white
#	define COLOR_TEMPERATURE_2200K			0x01C6	// light bulb
#	define COLOR_TEMPERATURE_2000K			0x01F4
#	define COLOR_TEMPERATURE_1700K			0x024C	// candle
#	define COLOR_TEMPERATURE_1000K			0x03E8
/*	define default values */
#	define COLOR_TEMPERATURE_NEUTRAL		COLOR_TEMPERATURE_3800K
#if (EXTENDED_COLOR_LIGHT)
#	define COLOR_TEMPERATURE_PHYSICAL_MIN	COLOR_TEMPERATURE_12000K
#	define COLOR_TEMPERATURE_PHYSICAL_MAX	COLOR_TEMPERATURE_1000K
#elif (COLOR_RGB_SUPPORT) || (COLOR_CCT_SUPPORT)
#	if (COLD_LIGHT_TEMPERATURE)
#		define COLOR_TEMPERATURE_PHYSICAL_MIN	COLD_LIGHT_TEMPERATURE
#	else
#		define COLOR_TEMPERATURE_PHYSICAL_MIN	COLOR_TEMPERATURE_6500K
#	endif
#	if (WARM_LIGHT_TEMPERATURE)
#		define COLOR_TEMPERATURE_PHYSICAL_MAX	WARM_LIGHT_TEMPERATURE
#	else
#		define COLOR_TEMPERATURE_PHYSICAL_MAX	COLOR_TEMPERATURE_1700K
#	endif
#endif


/*	color lighting channels and technical definitions
**	***
**	These are not standard, but only what I happend to be confronted with.
**	Check your LEDs technical datasheet
**	***
**	cold white	6500K
**	warm white	2200K
**	red			630nm, 1000mcd
**	green		525nm, 1500mcd
**	blue		465nm, 800mcd
*/
typedef struct {
	u16	wavelength;		// color wavelength in nano-meter
	u16 intensity;		// color brightness in milli-candela
}	ts_LED_Color_TechData;


/*	LED channel levels
**	used to convert and store the desired lighting
*/
typedef struct {
	u16		Value;		// current value in 16 bit precision
	s8		OnOff;		// state On/Off (-1==undefined, 0==off, 1...100==percentage)
}	ts_LED_Channel;

/*	TODO("find the best practice for maximum value, reflecting fractions of 1")
**	maybe it would be best, to use PMW_MAX_TICK; this will ensure direct reflection of the PWM clock tick values
**	maybe try with 0xFEFF, like used in ZigBee
**	using 0xFF should be sufficient; 8bit channel * 8bit level = 16bit PWM granularity
*/
#define COLORCHANNEL_MAX				PWM_MAX_TICK
#define COLORONOFF_MAX					100

#	if (COLORCHANNEL_MAX==PWM_MAX_TICK)
#	else
#	endif
#	if (COLOR_CCT_SUPPORT) || (SINGLE_WHITE_SUPPORT)
		extern	ts_LED_Channel		g_ledChannel_COLD;
#		define __LED_COLD_SETONOFF(v,f)				do { g_ledChannel_COLD.OnOff	= (f==  COLORONOFF_MAX ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_COLD_SETVALUE(v,f)				do { g_ledChannel_COLD.Value	= (f==COLORCHANNEL_MAX ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
#		if (COLORCHANNEL_MAX==PWM_MAX_TICK)
#			define __LED_COLD_PWMCOUNT				((s32)(g_ledChannel_COLD.Value * g_ledChannel_COLD.OnOff *                                 / COLORONOFF_MAX))
#		else
#			define __LED_COLD_PWMCOUNT				((s32)(g_ledChannel_COLD.Value * g_ledChannel_COLD.OnOff * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#		endif
#		if (!SINGLE_WHITE_SUPPORT)
			extern	ts_LED_Channel		g_ledChannel_WARM;
#			define __LED_WARM_SETONOFF(v,f)			do { g_ledChannel_WARM.OnOff	= (f==  COLORONOFF_MAX ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#			define __LED_WARM_SETVALUE(v,f)			do { g_ledChannel_WARM.Value	= (f==COLORCHANNEL_MAX ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
#			if (COLORCHANNEL_MAX==PWM_MAX_TICK)
#				define __LED_WARM_PWMCOUNT			((s32)(g_ledChannel_WARM.Value * g_ledChannel_WARM.OnOff *                                 / COLORONOFF_MAX))
#			else
#				define __LED_WARM_PWMCOUNT			((s32)(g_ledChannel_WARM.Value * g_ledChannel_WARM.OnOff * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#			endif
#		endif
#	endif
#	if (COLOR_RGB_SUPPORT)
		extern	ts_LED_Channel		g_ledChannel_RED;
#		define __LED_RED_SETONOFF(v,f)				do { g_ledChannel_RED.OnOff   = (f==  COLORONOFF_MAX ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_RED_SETVALUE(v,f)				do { g_ledChannel_RED.Value   = (f==COLORCHANNEL_MAX ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
		extern	ts_LED_Channel		g_ledChannel_GREEN;
#		define __LED_GREEN_SETONOFF(v,f)			do { g_ledChannel_GREEN.OnOff = (f==  COLORONOFF_MAX ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_GREEN_SETVALUE(v,f)			do { g_ledChannel_GREEN.Value = (f==COLORCHANNEL_MAX ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
		extern	ts_LED_Channel		g_ledChannel_BLUE;
#		define __LED_BLUE_SETONOFF(v,f)				do { g_ledChannel_BLUE.OnOff  = (f==  COLORONOFF_MAX ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_BLUE_SETVALUE(v,f)				do { g_ledChannel_BLUE.Value  = (f==COLORCHANNEL_MAX ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
#		if (COLORCHANNEL_MAX==PWM_MAX_TICK)
#			define __LED_RED_PWMCOUNT				((s32)(g_ledChannel_RED.Value   * g_ledChannel_RED.OnOff                                     / COLORONOFF_MAX))
#			define __LED_GREEN_PWMCOUNT				((s32)(g_ledChannel_GREEN.Value * g_ledChannel_GREEN.OnOff                                   / COLORONOFF_MAX))
#			define __LED_BLUE_PWMCOUNT				((s32)(g_ledChannel_BLUE.Value  * g_ledChannel_BLUE.OnOff                                    / COLORONOFF_MAX))
#		else
#			define __LED_RED_PWMCOUNT				((s32)(g_ledChannel_RED.Value   * g_ledChannel_RED.OnOff   * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#			define __LED_GREEN_PWMCOUNT				((s32)(g_ledChannel_GREEN.Value * g_ledChannel_GREEN.OnOff * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#			define __LED_BLUE_PWMCOUNT				((s32)(g_ledChannel_BLUE.Value  * g_ledChannel_BLUE.OnOff  * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#		endif
#	endif


void LEDLIGHT_setOnOff_fromValue (void);

/*********************************************************************
 * @fn      LEDLIGHT_setHSV
 *
 * @brief	set RGB from HSV (ZigBee values)
 * 
 * @param   [in]hue			-	hue attribute value (0 - ZCL_COLOR_ATTR_HUE_MAX)
 * 			[in]saturation	-	saturation attribute value (0 - ZCL_COLOR_ATTR_SATURATION_MAX)
 * 			[in]value		-	level attribute value (0 - ZCL_LEVEL_ATTR_MAX_LEVEL)
 *
 * @return  None
 */
void LEDLIGHT_setHSV (u8 hue, u8 saturation, u8 value);

/*********************************************************************
 * @fn      LEDLIGHT_setEnhancedHSV
 *
 * @brief	set RGB from enhanced HSV (ZigBee values)
 * 
 * @note	EnhancedCurrentHue attribute represents non-equidistant steps along the CIE 1931 color triangle, and it provides 16-bits precision.
 *				The upper 8 bits of this attribute SHALL be used as an index in the implementation specific XY lookup table
 *					to provide the non-equidistance steps (see the ZLL test specification for an example).
 *				The lower 8 bits SHALL be used to interpolate between these steps in a linear way in order to provide color zoom for the user.
 * @note	To provide compatibility with standard ZCL, the CurrentHue attribute SHALL contain a hue value in the range 0 to 254, calculated from the EnhancedCurrentHue attribute.
 * 
 * @param   [in]enhancedHue		-	enhanced hue attribute value (16bit precision)
 * 			[in]saturation		-	saturation attribute value (0 - ZCL_COLOR_ATTR_SATURATION_MAX)
 * 			[in]value			-	level attribute value (0 - ZCL_LEVEL_ATTR_MAX_LEVEL)
 *			[out]derrivedHue	-	(derrived) hue attribute value (0 - ZCL_COLOR_ATTR_HUE_MAX)
 *									NULL to disable
 *
 * @return  None
 */
void LEDLIGHT_setEnhancedHSV (u16 enhancedHue, u8 saturation, u8 level, u8 *derrivedHue);

/*********************************************************************
 * @fn      LEDLIGHT_setXYZ
 *
 * @brief	set RGB from XYZ (fraction of 1 values)
 * 
 * @param   [in]XYZ_X		-	X value (0 - 1)
 * 			[in]XYZ_Y		-	Y value (0 - 1)
 * 			[in]XYZ_Z		-	Z value (0 - 1)
 *
 * @return  None
 */
void LEDLIGHT_setXYZ (u16 XYZ_X, u16 XYZ_Y, u16 XYZ_Z);

/*********************************************************************
 * @fn      LEDLIGHT_setXY
 *
 * @brief	set RGB from XY (ZigBee values)
 * 
 * @param   [in]ZigBee_X		- X value, fraction of 65279 (ZCL_COLOR_ATTR_XY_MAX)
 * 			[in]ZigBee_Y		- Y value, fraction of 65279 (ZCL_COLOR_ATTR_XY_MAX)
 * 			[in]ZigBee_Level	- level attribute value (0 - ZCL_LEVEL_ATTR_MAX_LEVEL)
 *
 * @return  None
 */
void LEDLIGHT_setXY (u16 ZigBee_X, u16 ZigBee_Y, u8 ZigBee_Level);

/*********************************************************************
 * @fn      LEDLIGHT_setKelvin
 *
 * @brief	set RGB from color temperature in kelvin
 * 
 * @param   [in]kelvin		- color temperature value in Kelvin
 * 			[out]derrivedKelvin	- actual color temperature value in Kelvin, WITHIN limited range
 *
 * @return  None
 */
void LEDLIGHT_setKelvin (u16 kelvin, u16 *derrivedKelvin);

/*********************************************************************
 * @fn      LEDLIGHT_setMired
 *
 * @brief	set RGB from color temperature in mired (ZigBee values)
 * 
 * @param   [in]ZigBee_Mireds	- color temperature value in mired, 1...65279 (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MIN...ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MAX)
 *
 * @return  None
 */
void LEDLIGHT_setMired (u16 ZigBee_Mireds);

/*********************************************************************
 * @fn      LEDLIGHT_setMired_RGBW
 *
 * @brief	set RGB from color temperature in mired (ZigBee values)
 * 
 * @param   [in]ZigBee_Mireds	- color temperature value in mired, 1...65279 (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MIN...ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MAX)
 *          [in]whiteMireds		- color temperature of the white LED channel
 *
 * @return  None
 */
void LEDLIGHT_setMired_RGBW (u16 ZigBee_Mireds, u16 whiteMireds);

/*********************************************************************
 * @fn      LEDLIGHT_setMired_RGBCCT
 *
 * @brief	set RGB from color temperature in mired (ZigBee values)
 * 
 * @param   [in]ZigBee_Mireds	- color temperature value in mired, 1...65279 (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MIN...ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MAX)
 *          [in]coldMireds		- color temperature of the cold white LED channel
 *          [in]warmMireds		- color temperature of the warm white LED channel
 *
 * @return  None
 */
void LEDLIGHT_setMired_RGBCCT (u16 ZigBee_Mireds, const u16 coldMireds, const u16 warmMireds);


#endif /* _COLOR_CALCULATIONS_H_ */
