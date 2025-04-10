/********************************************************************************************************
 * @file    color_calculations.c
 *
 * @brief   calculation formula and tables for color conversion
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

#if (__PROJECT_TL_DIMMABLE_LIGHT__)

#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "ledLight.h"
#include "ledLightCtrl.h"

#include "color_calculations.h"
#if (_COLOR_CALCULATIONS__USE_FLOAT_)
#	include <math.h>
#endif

/*	global variables
**	for channel handling
*/
#	if (COLOR_CCT_SUPPORT) || (SINGLE_WHITE_SUPPORT)
	ts_LED_Channel		g_ledChannel_COLD		= { .Value=COLORCHANNEL_MAX, .OnOff=COLORONOFF_MAX, };
#	endif
#	if (COLOR_CCT_SUPPORT)
	ts_LED_Channel		g_ledChannel_WARM		= { .Value=COLORCHANNEL_MAX, .OnOff=COLORONOFF_MAX, };
#	endif
#	if (COLOR_RGB_SUPPORT)
	ts_LED_Channel		g_ledChannel_RED		= { .Value=COLORCHANNEL_MAX, .OnOff=COLORONOFF_MAX, };
	ts_LED_Channel		g_ledChannel_GREEN		= { .Value=COLORCHANNEL_MAX, .OnOff=COLORONOFF_MAX, };
	ts_LED_Channel		g_ledChannel_BLUE		= { .Value=COLORCHANNEL_MAX, .OnOff=COLORONOFF_MAX, };
#	endif

void LEDLIGHT_setOnOff_fromValue (void)
{
#	if (COLOR_CCT_SUPPORT) || (SINGLE_WHITE_SUPPORT)
	g_ledChannel_COLD.OnOff		= (g_ledChannel_COLD.Value  > 0 ?COLORONOFF_MAX :0);
#	endif
#	if (COLOR_CCT_SUPPORT)
	g_ledChannel_WARM.OnOff		= (g_ledChannel_WARM.Value  > 0 ?COLORONOFF_MAX :0);
#	endif
#	if (COLOR_RGB_SUPPORT)
	g_ledChannel_RED.OnOff		= (g_ledChannel_RED.Value   > 0 ?COLORONOFF_MAX :0);
	g_ledChannel_GREEN.OnOff	= (g_ledChannel_GREEN.Value > 0 ?COLORONOFF_MAX :0);
	g_ledChannel_BLUE.OnOff		= (g_ledChannel_BLUE.Value  > 0 ?COLORONOFF_MAX :0);
#	endif
}

/*	set from HSV (ZigBee values)
**	ZigBee Hue				is fractions of 254 (ZCL_COLOR_ATTR_HUE_MAX)
**		Hue = CurrentHue x 360 / 254
**	ZigBee Saturation		is fractions of 254 (ZCL_COLOR_ATTR_SATURATION_MAX)
**		Saturation = CurrentSaturation / 254
**	ZigBee Value,LEVEL		is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)
*/
void LEDLIGHT_setHSV (u8 hue, u8 saturation, u8 value)
{
	s32 HSV_V = value * COLORCHANNEL_MAX / ZCL_LEVEL_ATTR_MAX_LEVEL;				// value 0...COLORCHANNEL_MAX ==100%
	if (saturation == 0) {		// short-cut for optimization
		// no saturation means achromatic, ergo white
		g_ledChannel_RED.Value = g_ledChannel_GREEN.Value = g_ledChannel_BLUE.Value		= HSV_V;
		return;
	}
	s16 HSV_H	= hue * 360 / ZCL_COLOR_ATTR_HUE_MAX;								// value 0...360 degrees
	s32 HSV_S	= saturation * COLORCHANNEL_MAX / ZCL_COLOR_ATTR_SATURATION_MAX;	// value 0...COLORCHANNEL_MAX ==100%

	u8  sector;							// sector of full circle
	u8  fraction;						// fraction of sector
	if (HSV_H < 360)
	{
		sector		= HSV_H / 60;		// 0 <= H < 360
		fraction	= HSV_H % 60;		// 0..59
	} else {
		sector		= 0;				// H==360 =0
		fraction	= 0;
	}

	s32 _P		= (COLORCHANNEL_MAX - HSV_S                         ) * HSV_V / COLORCHANNEL_MAX;
	s32 _Q		= (COLORCHANNEL_MAX - HSV_S * (      fraction  / 60)) * HSV_V / COLORCHANNEL_MAX;
	s32 _T		= (COLORCHANNEL_MAX - HSV_S * ((60 - fraction) / 60)) * HSV_V / COLORCHANNEL_MAX;

	switch (sector) {
		case 0:
			g_ledChannel_RED.Value		= HSV_V;
			g_ledChannel_GREEN.Value	= _T;
			g_ledChannel_BLUE.Value		= _P;
			break;
		case 1:
			g_ledChannel_RED.Value		= _Q;
			g_ledChannel_GREEN.Value	= HSV_V;
			g_ledChannel_BLUE.Value		= _P;
			break;
		case 2:
			g_ledChannel_RED.Value		= _P;
			g_ledChannel_GREEN.Value	= HSV_V;
			g_ledChannel_BLUE.Value		= _T;
			break;
		case 3:
			g_ledChannel_RED.Value		= _P;
			g_ledChannel_GREEN.Value	= _Q;
			g_ledChannel_BLUE.Value		= HSV_V;
			break;
		case 4:
			g_ledChannel_RED.Value		= _T;
			g_ledChannel_GREEN.Value	= _P;
			g_ledChannel_BLUE.Value		= HSV_V;
			break;
		case 5:
			g_ledChannel_RED.Value		= HSV_V;
			g_ledChannel_GREEN.Value	= _P;
			g_ledChannel_BLUE.Value		= _Q;
			break;
	}

#	if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	TODO("use saturation for white channel")
	__LED_COLD_SETVALUE(0,COLORCHANNEL_MAX);
#	endif
#	if (COLOR_CCT_SUPPORT)
	__LED_WARM_SETVALUE(0,COLORCHANNEL_MAX);
#	endif

	LEDLIGHT_setOnOff_fromValue();
	return;
}

/*	set from HSV enhanced (ZigBee values)
**	EnhancedCurrentHue attribute represents non-equidistant steps along the CIE 1931 color triangle, and it provides 16-bits precision.
**		The upper 8 bits of this attribute SHALL be used as an index in the implementation specific XY lookup table
**			to provide the non-equidistance steps (see the ZLL test specification for an example).
**		The lower 8 bits SHALL be used to interpolate between these steps in a linear way in order to provide color zoom for the user.
**	To provide compatibility with standard ZCL, the CurrentHue attribute SHALL contain a hue value in the range 0 to 254, calculated from the EnhancedCurrentHue attribute.
**	***
**	ZigBee Hue				is fractions of 254 (ZCL_COLOR_ATTR_HUE_MAX)
**		Hue = CurrentHue x 360 / 254
**	ZigBee Saturation		is fractions of 254 (ZCL_COLOR_ATTR_SATURATION_MAX)
**		Saturation = CurrentSaturation / 254
**	ZigBee Value,LEVEL		is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)
*/
void LEDLIGHT_setEnhancedHSV (u16 enhancedHue, u8 saturation, u8 level, u8 *derrivedHue)
{
	u8 hue	= (enhancedHue >> 8) & 0xFF;		// simple strip the upper 8 bits
	TODO("consult ZLL test specification and/or something else to get an example");
	if (hue > ZCL_COLOR_ATTR_HUE_MAX) {
		hue	= 0;	// 360° equivalent
	}
	if (derrivedHue != NULL) {
		*derrivedHue = hue;
	}
	LEDLIGHT_setHSV (hue, saturation, level);	// for now just use the stripped value
}

/*	set from XYZ (fraction of 0xFFFF values)
**	X/Y/Z	0 ... 0xFFFF==1.0
**	Y=0xFFFF setting full on brightness
*/
void LEDLIGHT_setXYZ (u16 XYZ_X, u16 XYZ_Y, u16 XYZ_Z)
{
	/*	calculate with floating point
	**	RGB_R	= ( 3.2404542 * XYZ_X - 1.5371385 * XYZ_Y - 0.4985314 * XYZ_Z);
	**	RGB_G	= (-0.969266  * XYZ_X + 1.8760108 * XYZ_Y + 0.041556  * XYZ_Z);
	**	RGB_B	= ( 0.0556434 * XYZ_X - 0.2040259 * XYZ_Y + 1.0572252 * XYZ_Z);
	**	converted to integer 1.0==0xFFFF
	*/
	s32 RGB_R	= 0x33D8B * XYZ_X  -  0x18980 * XYZ_Y  -   0x7F9F * XYZ_Z;
	s32 RGB_G	=  0xF820 * XYZ_X  +  0x1E040 * XYZ_Y  +   0x0AA3 * XYZ_Z;
	s32 RGB_B	=  0x0E3E * XYZ_X  -   0x343A * XYZ_Y  +  0x10EA5 * XYZ_Z;

	g_ledChannel_RED.Value		= ((RGB_R >= 0xFFFF) ?(COLORCHANNEL_MAX) :(RGB_R * COLORCHANNEL_MAX / 0xFFFF));
	g_ledChannel_GREEN.Value	= ((RGB_G >= 0xFFFF) ?(COLORCHANNEL_MAX) :(RGB_G * COLORCHANNEL_MAX / 0xFFFF));
	g_ledChannel_BLUE.Value		= ((RGB_B >= 0xFFFF) ?(COLORCHANNEL_MAX) :(RGB_B * COLORCHANNEL_MAX / 0xFFFF));

#	if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	TODO("use saturation for white channel")
	__LED_COLD_SETVALUE(0,COLORCHANNEL_MAX);
#	endif
#	if (COLOR_CCT_SUPPORT)
	__LED_WARM_SETVALUE(0,COLORCHANNEL_MAX);
#	endif

	LEDLIGHT_setOnOff_fromValue();
	return;
}

/*********************************************************************
 * @fn      LEDLIGHT_setXY
 *
 * @brief	set RGB from XY (ZigBee values)
 * 
 * @param   [in]ZigBee_X	- X value, fraction of 65279 (ZCL_COLOR_ATTR_XY_MAX)
 * 			[in]ZigBee_Y	- Y value, fraction of 65279 (ZCL_COLOR_ATTR_XY_MAX)
 * 			[in]level		- level attribute value (0 - ZCL_LEVEL_ATTR_MAX_LEVEL)
 *
 * @return  None
 */
void LEDLIGHT_setXY (u16 ZigBee_X, u16 ZigBee_Y, u8 ZigBee_Level)
{
#if (_COLOR_CALCULATIONS__USE_FLOAT_)
	float xyY_x	= 1.0 / 65535 * ZigBee_X;							// change fraction
	float xyY_y	= 1.0 / 65535 * ZigBee_Y;							// change fraction
	float xyY_Y	= 1.0 / ZCL_LEVEL_ATTR_MAX_LEVEL * ZigBee_Level;	// change fraction
	DEBUG(DEBUG_LEDCOLOR, "x=%f, y=%f, Y=%f\r", xyY_x,xyY_y,xyY_Y);

	/*	xyY to XYZ */
	float XYZ_X	=        xyY_x          * xyY_Y / xyY_y;
	float XYZ_Y =                         xyY_Y;
	float XYZ_Z	= (1.0 - xyY_x - xyY_y) * xyY_Y / xyY_y;
	DEBUG(DEBUG_LEDCOLOR, "X=%f, Y=%f, Z=%f\r", XYZ_X,XYZ_Y,XYZ_Z);

	/*	XYZ to RGB conversion using wide gamut matrix, with D50 reference white */
	float RGB_r	=  1.4628067 * XYZ_X - 0.1840623 * XYZ_Y - 0.2743606 * XYZ_Z;
	float RGB_g	= -0.5217933 * XYZ_X + 1.4472381 * XYZ_Y + 0.0677227 * XYZ_Z;
	float RGB_b	=  0.0349342 * XYZ_X - 0.0968930 * XYZ_Y + 1.2884099 * XYZ_Z;

	/*	reverse gamma correction */
	RGB_r	= RGB_r <= 0.0031308f ? 12.92f * RGB_r : (1.0f + 0.055f) * powf(RGB_r, (1.0f / 2.4f)) - 0.055f;
	RGB_g	= RGB_g <= 0.0031308f ? 12.92f * RGB_g : (1.0f + 0.055f) * powf(RGB_g, (1.0f / 2.4f)) - 0.055f;
	RGB_b	= RGB_b <= 0.0031308f ? 12.92f * RGB_b : (1.0f + 0.055f) * powf(RGB_b, (1.0f / 2.4f)) - 0.055f;

	/*	change scaling to PWM counter */
	RGB_r	*= COLORCHANNEL_MAX;
	RGB_g	*= COLORCHANNEL_MAX;
	RGB_b	*= COLORCHANNEL_MAX;
	/*	set LED channels */
	__LED_RED_SETVALUE   (RGB_r, COLORCHANNEL_MAX);
	__LED_GREEN_SETVALUE (RGB_g, COLORCHANNEL_MAX);
	__LED_BLUE_SETVALUE  (RGB_b, COLORCHANNEL_MAX);
	/*	set OnOff to full on, level is computed into RGB channel values */
	__LED_RED_SETONOFF   (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
	__LED_GREEN_SETONOFF (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
	__LED_BLUE_SETONOFF  (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);

#else
	// ensure x+y=1
	if (ZigBee_X + ZigBee_Y > ZCL_COLOR_ATTR_XY_MAX) {
		ZigBee_X	= ZigBee_X / (ZigBee_X + ZigBee_Y);
		ZigBee_Y	= ZCL_COLOR_ATTR_XY_MAX - ZigBee_X;
	}
	// project into XYZ
	s32 XYZ_X	=                          ZigBee_X             * ZCL_COLOR_ATTR_XY_MAX / ZigBee_Y;
	s32 XYZ_Y	=                                                 ZCL_COLOR_ATTR_XY_MAX           ;		// full on brightness
	s32 XYZ_Z	= (ZCL_COLOR_ATTR_XY_MAX - ZigBee_X - ZigBee_Y) * ZCL_COLOR_ATTR_XY_MAX / ZigBee_Y;
	// now X/Y/Z is fraction of ZCL_COLOR_ATTR_XY_MAX
	DEBUG(DEBUG_LEDCOLOR, "X=%x, Y=%x, Z=%x\r", XYZ_X,XYZ_Y,XYZ_Z);

	//	converted to integer 1.0==0xFFFF
	s32 RGB_R	= 0x33D8B * XYZ_X / 0xFFFF  -  0x18980 * XYZ_Y / 0xFFFF  -   0x7F9F * XYZ_Z / 0xFFFF;
	s32 RGB_G	=  0xF820 * XYZ_X / 0xFFFF  +  0x1E040 * XYZ_Y / 0xFFFF  +   0x0AA3 * XYZ_Z / 0xFFFF;
	s32 RGB_B	=  0x0E3E * XYZ_X / 0xFFFF  -   0x343A * XYZ_Y / 0xFFFF  +  0x10EA5 * XYZ_Z / 0xFFFF;
	if (RGB_R < 0) {
		RGB_R	= 0;
	} else if (RGB_R > 0xFFFF) {
		RGB_R	= 0xFFFF;
	}
	if (RGB_G < 0) {
		RGB_G	= 0;
	} else if (RGB_G > 0xFFFF) {
		RGB_G	= 0xFFFF;
	}
	if (RGB_B < 0) {
		RGB_B	= 0;
	} else if (RGB_B > 0xFFFF) {
		RGB_B	= 0xFFFF;
	}
	DEBUG(DEBUG_LEDCOLOR, "R=%x, G=%x, B=%x\r", RGB_R,RGB_G,RGB_B);

	__LED_RED_SETVALUE   (RGB_R, 0xFFFF);
	__LED_RED_SETONOFF   (ZigBee_Level, ZCL_LEVEL_ATTR_MAX_LEVEL);
	__LED_GREEN_SETVALUE (RGB_G, 0xFFFF);
	__LED_GREEN_SETONOFF (ZigBee_Level, ZCL_LEVEL_ATTR_MAX_LEVEL);
	__LED_BLUE_SETVALUE  (RGB_B, 0xFFFF);
	__LED_BLUE_SETONOFF  (ZigBee_Level, ZCL_LEVEL_ATTR_MAX_LEVEL);
#endif

#	if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	s32 RGB_lightness	= __LED_RGB_LIGHTNESS;
	__LED_COLD_SETVALUE (RGB_lightness,COLORCHANNEL_MAX);
	__LED_COLD_SETONOFF (g_ledChannel_RED.OnOff,COLORONOFF_MAX);
#	if (COLOR_CCT_SUPPORT)
	__LED_WARM_SETVALUE (RGB_lightness,COLORCHANNEL_MAX);
	__LED_WARM_SETONOFF (g_ledChannel_RED.OnOff,COLORONOFF_MAX);
#	endif
#	endif

	return;
}

/*********************************************************************
 * @fn      LEDLIGHT_setKelvin
 *
 * @brief	set RGB from color temperature in kelvin
 * 
 * @details	Kelvin to RGB conversion table derrived from Andreas Siess work.
 * 				see https://andi-siess.de/rgb-to-color-temperature/
 * 				A. Siess, "Kelvin (color temperature) to RGB conversion table". Zenodo, Nov. 27, 2024. doi: 10.5281/zenodo.14230959
 * 
 * @param   [in]kelvin			- color temperature value in Kelvin
 * 			[out]derrivedKelvin	- actual color temperature value in Kelvin, WITHIN limited range
 *
 * @return  None
 */
void LEDLIGHT_setKelvin (u16 kelvin, u16 *derrivedKelvin)
{
	u8	RGB_R, RGB_G, RGB_B;

	u16 colorTemp = kelvin / 100;
	/* range limitation should not be necessary with default in switch case structure
	if (colorTemp < 10) {
		colorTemp = 10;
	} else if (colorTemp > 120) {
		colorTemp = 120;
	} */

	switch (colorTemp) {	// use 100 Kelvin steps
		case 10:	RGB_R=255;	RGB_G=56;	RGB_B=0;	break;
		case 11:	RGB_R=255;	RGB_G=71;	RGB_B=0;	break;
		case 12:	RGB_R=255;	RGB_G=83;	RGB_B=0;	break;
		case 13:	RGB_R=255;	RGB_G=93;	RGB_B=0;	break;
		case 14:	RGB_R=255;	RGB_G=101;	RGB_B=0;	break;
		case 15:	RGB_R=255;	RGB_G=109;	RGB_B=0;	break;
		case 16:	RGB_R=255;	RGB_G=115;	RGB_B=0;	break;

		case 17:	RGB_R=255;	RGB_G=121;	RGB_B=0;	break;
		case 18:	RGB_R=255;	RGB_G=126;	RGB_B=0;	break;
		case 19:	RGB_R=255;	RGB_G=131;	RGB_B=0;	break;
		case 20:	RGB_R=255;	RGB_G=138;	RGB_B=18;	break;
		case 21:	RGB_R=255;	RGB_G=142;	RGB_B=33;	break;
		case 22:	RGB_R=255;	RGB_G=147;	RGB_B=44;	break;
		case 23:	RGB_R=255;	RGB_G=152;	RGB_B=54;	break;
		case 24:	RGB_R=255;	RGB_G=157;	RGB_B=63;	break;
		case 25:	RGB_R=255;	RGB_G=161;	RGB_B=72;	break;
		case 26:	RGB_R=255;	RGB_G=165;	RGB_B=79;	break;
		case 27:	RGB_R=255;	RGB_G=169;	RGB_B=87;	break;
		case 28:	RGB_R=255;	RGB_G=173;	RGB_B=94;	break;
		case 29:	RGB_R=255;	RGB_G=177;	RGB_B=101;	break;
		case 30:	RGB_R=255;	RGB_G=180;	RGB_B=107;	break;
		case 31:	RGB_R=255;	RGB_G=184;	RGB_B=114;	break;
		case 32:	RGB_R=255;	RGB_G=187;	RGB_B=120;	break;
		case 33:	RGB_R=255;	RGB_G=190;	RGB_B=126;	break;
		case 34:	RGB_R=255;	RGB_G=193;	RGB_B=132;	break;
		case 35:	RGB_R=255;	RGB_G=196;	RGB_B=137;	break;
		case 36:	RGB_R=255;	RGB_G=199;	RGB_B=143;	break;
		case 37:	RGB_R=255;	RGB_G=201;	RGB_B=148;	break;

		default:	colorTemp = 38;			// neutral white
		case 38:	RGB_R=255;	RGB_G=204;	RGB_B=153;	break;

		case 39:	RGB_R=255;	RGB_G=206;	RGB_B=159;	break;
		case 40:	RGB_R=255;	RGB_G=209;	RGB_B=163;	break;
		case 41:	RGB_R=255;	RGB_G=211;	RGB_B=168;	break;
		case 42:	RGB_R=255;	RGB_G=213;	RGB_B=173;	break;
		case 43:	RGB_R=255;	RGB_G=215;	RGB_B=177;	break;
		case 44:	RGB_R=255;	RGB_G=217;	RGB_B=182;	break;
		case 45:	RGB_R=255;	RGB_G=219;	RGB_B=186;	break;
		case 46:	RGB_R=255;	RGB_G=221;	RGB_B=190;	break;
		case 47:	RGB_R=255;	RGB_G=223;	RGB_B=194;	break;
		case 48:	RGB_R=255;	RGB_G=225;	RGB_B=198;	break;
		case 49:	RGB_R=255;	RGB_G=227;	RGB_B=202;	break;
		case 50:	RGB_R=255;	RGB_G=228;	RGB_B=206;	break;
		case 51:	RGB_R=255;	RGB_G=230;	RGB_B=210;	break;
		case 52:	RGB_R=255;	RGB_G=232;	RGB_B=213;	break;
		case 53:	RGB_R=255;	RGB_G=233;	RGB_B=217;	break;
		case 54:	RGB_R=255;	RGB_G=235;	RGB_B=220;	break;
		case 55:	RGB_R=255;	RGB_G=236;	RGB_B=224;	break;
		case 56:	RGB_R=255;	RGB_G=238;	RGB_B=227;	break;
		case 57:	RGB_R=255;	RGB_G=239;	RGB_B=230;	break;
		case 58:	RGB_R=255;	RGB_G=240;	RGB_B=233;	break;
		case 59:	RGB_R=255;	RGB_G=242;	RGB_B=236;	break;
		case 60:	RGB_R=255;	RGB_G=243;	RGB_B=239;	break;
		case 61:	RGB_R=255;	RGB_G=244;	RGB_B=242;	break;
		case 62:	RGB_R=255;	RGB_G=245;	RGB_B=245;	break;
		case 63:	RGB_R=255;	RGB_G=246;	RGB_B=247;	break;
		case 64:	RGB_R=255;	RGB_G=248;	RGB_B=251;	break;
		case 65:	RGB_R=255;	RGB_G=249;	RGB_B=253;	break;
		case 66:	RGB_R=254;	RGB_G=249;	RGB_B=255;	break;
		case 67:	RGB_R=252;	RGB_G=247;	RGB_B=255;	break;
		case 68:	RGB_R=249;	RGB_G=246;	RGB_B=255;	break;
		case 69:	RGB_R=247;	RGB_G=245;	RGB_B=255;	break;
		case 70:	RGB_R=245;	RGB_G=243;	RGB_B=255;	break;
		case 71:	RGB_R=243;	RGB_G=242;	RGB_B=255;	break;
		case 72:	RGB_R=240;	RGB_G=241;	RGB_B=255;	break;
		case 73:	RGB_R=239;	RGB_G=240;	RGB_B=255;	break;
		case 74:	RGB_R=237;	RGB_G=239;	RGB_B=255;	break;
		case 75:	RGB_R=235;	RGB_G=238;	RGB_B=255;	break;

		case 76:	RGB_R=233;	RGB_G=237;	RGB_B=255;	break;
		case 77:	RGB_R=231;	RGB_G=236;	RGB_B=255;	break;
		case 78:	RGB_R=230;	RGB_G=235;	RGB_B=255;	break;
		case 79:	RGB_R=228;	RGB_G=234;	RGB_B=255;	break;
		case 80:	RGB_R=227;	RGB_G=233;	RGB_B=255;	break;
		case 81:	RGB_R=225;	RGB_G=232;	RGB_B=255;	break;
		case 82:	RGB_R=224;	RGB_G=231;	RGB_B=255;	break;
		case 83:	RGB_R=222;	RGB_G=230;	RGB_B=255;	break;
		case 84:	RGB_R=221;	RGB_G=230;	RGB_B=255;	break;
		case 85:	RGB_R=220;	RGB_G=229;	RGB_B=255;	break;
		case 86:	RGB_R=218;	RGB_G=229;	RGB_B=255;	break;
		case 87:	RGB_R=217;	RGB_G=227;	RGB_B=255;	break;
		case 88:	RGB_R=216;	RGB_G=227;	RGB_B=255;	break;
		case 89:	RGB_R=215;	RGB_G=226;	RGB_B=255;	break;
		case 90:	RGB_R=214;	RGB_G=225;	RGB_B=255;	break;
		case 91:	RGB_R=212;	RGB_G=225;	RGB_B=255;	break;
		case 92:	RGB_R=211;	RGB_G=224;	RGB_B=255;	break;
		case 93:	RGB_R=210;	RGB_G=223;	RGB_B=255;	break;
		case 94:	RGB_R=209;	RGB_G=223;	RGB_B=255;	break;
		case 95:	RGB_R=208;	RGB_G=222;	RGB_B=255;	break;
		case 96:	RGB_R=207;	RGB_G=221;	RGB_B=255;	break;
		case 97:	RGB_R=207;	RGB_G=221;	RGB_B=255;	break;
		case 98:	RGB_R=206;	RGB_G=220;	RGB_B=255;	break;
		case 99:	RGB_R=205;	RGB_G=220;	RGB_B=255;	break;
		case 100:	RGB_R=207;	RGB_G=218;	RGB_B=255;	break;
		case 101:	RGB_R=207;	RGB_G=218;	RGB_B=255;	break;
		case 102:	RGB_R=206;	RGB_G=217;	RGB_B=255;	break;
		case 103:	RGB_R=205;	RGB_G=217;	RGB_B=255;	break;
		case 104:	RGB_R=204;	RGB_G=216;	RGB_B=255;	break;
		case 105:	RGB_R=204;	RGB_G=216;	RGB_B=255;	break;
		case 106:	RGB_R=203;	RGB_G=215;	RGB_B=255;	break;
		case 107:	RGB_R=202;	RGB_G=215;	RGB_B=255;	break;
		case 108:	RGB_R=202;	RGB_G=214;	RGB_B=255;	break;
		case 109:	RGB_R=201;	RGB_G=214;	RGB_B=255;	break;
		case 110:	RGB_R=200;	RGB_G=213;	RGB_B=255;	break;
		case 111:	RGB_R=200;	RGB_G=213;	RGB_B=255;	break;
		case 112:	RGB_R=199;	RGB_G=212;	RGB_B=255;	break;
		case 113:	RGB_R=198;	RGB_G=212;	RGB_B=255;	break;
		case 114:	RGB_R=198;	RGB_G=212;	RGB_B=255;	break;
		case 115:	RGB_R=197;	RGB_G=211;	RGB_B=255;	break;
		case 116:	RGB_R=197;	RGB_G=211;	RGB_B=255;	break;
		case 117:	RGB_R=197;	RGB_G=210;	RGB_B=255;	break;
		case 118:	RGB_R=196;	RGB_G=210;	RGB_B=255;	break;
		case 119:	RGB_R=195;	RGB_G=210;	RGB_B=255;	break;
		case 120:	RGB_R=195;	RGB_G=209;	RGB_B=255;	break;
	}

	if (derrivedKelvin != NULL) {
		*derrivedKelvin = colorTemp * 100;		// set after loop, to catch changed default
	}

	g_ledChannel_RED.Value		= ((RGB_R >= 0xFF) ?(COLORCHANNEL_MAX) :(RGB_R * COLORCHANNEL_MAX / 0xFF));
	g_ledChannel_GREEN.Value	= ((RGB_G >= 0xFF) ?(COLORCHANNEL_MAX) :(RGB_G * COLORCHANNEL_MAX / 0xFF));
	g_ledChannel_BLUE.Value		= ((RGB_B >= 0xFF) ?(COLORCHANNEL_MAX) :(RGB_B * COLORCHANNEL_MAX / 0xFF));
	LEDLIGHT_setOnOff_fromValue();
	return;
}

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
void LEDLIGHT_setMired_RGBW (u16 ZigBee_Mireds, const u16 whiteMireds)
{
#if (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)

	// fix range limits
	if (ZigBee_Mireds < COLOR_TEMPERATURE_CONVERT(12000)) {				// LEDLIGHT_setMired minimum
		ZigBee_Mireds = COLOR_TEMPERATURE_CONVERT(12000);
	} else if (ZigBee_Mireds > COLOR_TEMPERATURE_CONVERT(1000)) {		// LEDLIGHT_setMired maximum
		ZigBee_Mireds = COLOR_TEMPERATURE_CONVERT(1000);
	}
	u16 RGB_CT = ZigBee_Mireds;
	if (ZigBee_Mireds <= (whiteMireds / 2)) {							// CT < (WHITE / 2) results in RGB_CT < 0
		RGB_CT	= COLOR_TEMPERATURE_CONVERT(12000);
	} else
	if (ZigBee_Mireds < whiteMireds) {									// colder WHITE
		RGB_CT -= (whiteMireds - ZigBee_Mireds);						// double the distance
		if (RGB_CT < COLOR_TEMPERATURE_CONVERT(12000)) {				// LEDLIGHT_setMired minimum
			RGB_CT = COLOR_TEMPERATURE_CONVERT(12000);
		}
	} else if (ZigBee_Mireds > whiteMireds) {							// warmer WHITE
		RGB_CT += (ZigBee_Mireds - whiteMireds);						// double the distance
		if (RGB_CT > COLOR_TEMPERATURE_CONVERT(1000)) {					// LEDLIGHT_setMired maximum
			RGB_CT = COLOR_TEMPERATURE_CONVERT(1000);
		}
	}
	__LED_COLD_SETVALUE (255,255);
	LEDLIGHT_setKelvin (COLOR_TEMPERATURE_CONVERT(RGB_CT), NULL);

	s32 TMP;
	if (ZigBee_Mireds < whiteMireds) {							// colder WHITE
		// WARM = (mireds - min) / (max - min)
		TMP = (((ZigBee_Mireds - RGB_CT) * COLORONOFF_MAX) / (whiteMireds - RGB_CT));
		printf ("\tWARM=%d", TMP);
	} else if (ZigBee_Mireds > whiteMireds) {							// warmer WHITE
		TMP = (((RGB_CT - ZigBee_Mireds) * COLORONOFF_MAX) / (RGB_CT - whiteMireds));
		printf ("\tCOLD=%d", TMP);
	} else
	/* if neither lesser nor greater it should be equal
	if (ZigBee_Mireds == whiteMireds) */
	{
		TMP = COLORONOFF_MAX / 2;
		printf ("\tWHITE=%d", TMP);
	}
	__LED_COLD_SETONOFF  (TMP,COLORONOFF_MAX);
	__LED_RED_SETONOFF   (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
	__LED_GREEN_SETONOFF (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
	__LED_BLUE_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
#endif /* (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT) */
}

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
void LEDLIGHT_setMired_RGBCCT (u16 ZigBee_Mireds, const u16 coldMireds, const u16 warmMireds)
{
#if (COLOR_RGB_SUPPORT) && (COLOR_CCT_SUPPORT)
	u16 RGB_minCT	= COLOR_TEMPERATURE_CONVERT(12000);
	u16 RGB_maxCT	= COLOR_TEMPERATURE_CONVERT(1000);
	// fix range limits
	if (ZigBee_Mireds < RGB_minCT) {				// LEDLIGHT_setMired minimum
		ZigBee_Mireds = RGB_minCT;
	} else if (ZigBee_Mireds > RGB_maxCT) {			// LEDLIGHT_setMired maximum
		ZigBee_Mireds = RGB_maxCT;
	}
	u16 RGB_CT = ZigBee_Mireds;
	s32 TMP = 0;
/*	color temperature range limits
**	***
**	  RGB minimum =   83 mired, 12000 Kelvin (hard limit, RGB conversion table)
**	WHITE minimum =  154 mired,  6500 Kelvin (cold white LED)
**	WHITE single  =  263 mired,  3800 Kelvin (neutral)
**	WHITE maximum =  455 mired,  2200 Kelvin (warm white LED)
**	  RGB maximum = 1000 mired,  1000 Kelvin (hard limit, RGB conversion table)
**	***
**	COLDer = (mireds - min) / (max - min)
**	WARMer = (max - mireds) / (max - min)
*/
	if (ZigBee_Mireds < RGB_minCT || ZigBee_Mireds < (coldMireds / 2)) {
		// conversion table ends at 12,000K so this is hard limit
		// for doubled distance CT < (WHITE / 2) results in RGB_CT < 0
		RGB_CT	= RGB_minCT;										// coldest color supported by RGB table
		//	RGB_CT=RGB_minCT	<= ZigBee_Mireds	<= coldMireds
		TMP		= ((ZigBee_Mireds - coldMireds) * COLORONOFF_MAX / (RGB_CT - coldMireds));	// COLD = 1 - WARM
		printf ("OUT COLD=%d\t", TMP);
		// set COLD level
		__LED_COLD_SETVALUE  (255,255);
		__LED_COLD_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
		// set WARM to 0/4 off
		__LED_WARM_SETVALUE  (  0,255);
		__LED_WARM_SETONOFF  (  0,COLORONOFF_MAX);
		// set RGB to 4/4 full brightness
		__LED_RED_SETONOFF   (TMP,COLORONOFF_MAX);
		__LED_GREEN_SETONOFF (TMP,COLORONOFF_MAX);
		__LED_BLUE_SETONOFF  (TMP,COLORONOFF_MAX);
	}
	else if (ZigBee_Mireds < coldMireds) {							// colder WHITE
		RGB_CT -= (coldMireds - ZigBee_Mireds);						// double the distance
		if (RGB_CT < RGB_minCT) {
			RGB_CT	= RGB_minCT;									// range limit
		}
		TMP = (((ZigBee_Mireds - coldMireds) * COLORONOFF_MAX) / (RGB_CT - coldMireds));
		printf ("colder =%d\t", TMP);
		__LED_COLD_SETVALUE  (255,255);
		__LED_COLD_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (  0,255);
		__LED_WARM_SETONOFF  (  0,COLORONOFF_MAX);
		__LED_RED_SETONOFF   (TMP,COLORONOFF_MAX);
		__LED_GREEN_SETONOFF (TMP,COLORONOFF_MAX);
		__LED_BLUE_SETONOFF  (TMP,COLORONOFF_MAX);
	}
	else if (ZigBee_Mireds == coldMireds) {			// on the edge of cold
		RGB_CT = ZigBee_Mireds;
		TMP = COLORONOFF_MAX / 2;
		printf ("COLD =%d\t", TMP);
		__LED_COLD_SETVALUE  (255,255);
		__LED_COLD_SETONOFF  (TMP,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (  0,255);
		__LED_WARM_SETONOFF  (  0,COLORONOFF_MAX);
		__LED_RED_SETONOFF   (TMP,COLORONOFF_MAX);
		__LED_GREEN_SETONOFF (TMP,COLORONOFF_MAX);
		__LED_BLUE_SETONOFF  (TMP,COLORONOFF_MAX);
	}
	else if (ZigBee_Mireds == warmMireds) {			// on the edge of warm
		RGB_CT = ZigBee_Mireds;
		TMP = COLORONOFF_MAX / 2;
		printf ("WARM =%d\t", TMP);
		__LED_COLD_SETVALUE  (  0,255);
		__LED_COLD_SETONOFF  (  0,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (255,255);
		__LED_WARM_SETONOFF  (TMP,COLORONOFF_MAX);
		__LED_RED_SETONOFF   (TMP,COLORONOFF_MAX);
		__LED_GREEN_SETONOFF (TMP,COLORONOFF_MAX);
		__LED_BLUE_SETONOFF  (TMP,COLORONOFF_MAX);
	}
	else if (warmMireds < ZigBee_Mireds) {							// warmer WHITE
		RGB_CT += (ZigBee_Mireds - warmMireds);						// double the distance
		if (RGB_CT > RGB_maxCT) {
			RGB_CT	= RGB_maxCT;									// range limit
		}
		TMP = (((warmMireds - ZigBee_Mireds) * COLORONOFF_MAX) / (warmMireds - RGB_CT));
		printf ("warmer =%d\t", TMP);
		__LED_COLD_SETVALUE  (  0,255);
		__LED_COLD_SETONOFF  (  0,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (255,255);
		__LED_WARM_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
		__LED_RED_SETONOFF   (TMP,COLORONOFF_MAX);
		__LED_GREEN_SETONOFF (TMP,COLORONOFF_MAX);
		__LED_BLUE_SETONOFF  (TMP,COLORONOFF_MAX);
	}
	else /*if (ZigBee_Mireds < warmMireds && ZigBee_Mireds > coldMireds)*/ {
		// in between
		RGB_CT = ZigBee_Mireds;
		TMP = (((ZigBee_Mireds - coldMireds) * COLORONOFF_MAX) / (warmMireds - coldMireds));
		printf ("in-between =%d\t", TMP);
		__LED_COLD_SETVALUE  (255,255);
		__LED_COLD_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (255,255);
		__LED_WARM_SETONOFF  (TMP,COLORONOFF_MAX);
		__LED_RED_SETONOFF   (4,4);
		__LED_GREEN_SETONOFF (4,4);
		__LED_BLUE_SETONOFF  (4,4);
	}
	LEDLIGHT_setKelvin (COLOR_TEMPERATURE_CONVERT(RGB_CT), NULL);
#endif /* (COLOR_RGB_SUPPORT) && (COLOR_CCT_SUPPORT) */
}

/*	set from Color Temperature Mireds (ZigBee values)
**	1...65279
**	ZigBee_Mireds = 0x0000 indicates an undefined value
**	ZigBee_Mireds = 0xffff indicates an invalid value
*/
void LEDLIGHT_setMired (u16 ZigBee_Mireds)
{
	/*	ZigBee color temperature given in mireds
	**	mired = 1,000,000 / kelvin
	**	0x0000	is undefined value (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_UNDEFINED)
	**	     1	is minimum (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MIN)
	**	0xFEFF	is maximum (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MAX)
	**	0xFFFF	is invalid value (ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_INVALID)
	*/
	if (ZigBee_Mireds == ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_UNDEFINED) {
		return;		// nothing to do, when value undefined
	}
	else
	if (ZigBee_Mireds == ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_INVALID) {
		ZigBee_Mireds = COLOR_TEMPERATURE_NEUTRAL;	// set to neutral
	}
/* MIN/MAX can actually changed via attribute, so simply limiting to predefined compile time values is a bad idea
#if (EXTENDED_COLOR_LIGHT)
#	define COLOR_TEMPERATURE_PHYSICAL_MIN	COLOR_TEMPERATURE_12000K
#	define COLOR_TEMPERATURE_PHYSICAL_MAX	COLOR_TEMPERATURE_1000K
#elif (COLOR_RGB_SUPPORT)
#	define COLOR_TEMPERATURE_PHYSICAL_MIN	COLOR_TEMPERATURE_6500K
#	define COLOR_TEMPERATURE_PHYSICAL_MAX	COLOR_TEMPERATURE_1700K
#elif (COLOR_CCT_SUPPORT)
#	define COLOR_TEMPERATURE_PHYSICAL_MIN	COLOR_TEMPERATURE_6000K
#	define COLOR_TEMPERATURE_PHYSICAL_MAX	COLOR_TEMPERATURE_3000K
#endif
	else
	if (ZigBee_Mireds < ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MIN) {
		ZigBee_Mireds = ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MIN;		// set to minimum
	}
	else
	if (ZigBee_Mireds > ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MAX) {
		ZigBee_Mireds = ZCL_COLOR_ATTR_TEMPERATURE_MIREDS_MAX;		// set to maximum
	}
*/
	u16 colorTemperature	= COLOR_TEMPERATURE_CONVERT(ZigBee_Mireds);
	LEDLIGHT_setKelvin (colorTemperature, NULL);
}



/*	***
**	XYZ from RGB
**	float XYZ_X = red * 0.649926f + green * 0.103455f + blue * 0.197109f;
**	float XYZ_Y = red * 0.234327f + green * 0.743075f + blue * 0.022598f;
**	float XYZ_Z = red * 0.0000000f + green * 0.053077f + blue * 1.035763f;
**	***
**	XYZ from XY
**	float z = 1.0f - x - y;
**	float XYZ_Y = brightness; // The given brightness value
**	float XYZ_X = (XYZ_Y / y) * x;
**	float XYZ_Z = (XYZ_Y / y) * z;
**	***
**	XY from XYZ
**	float x = XYZ_X / (XYZ_X + XYZ_Y + XYZ_Z);
**	float y = XYZ_Y / (XYZ_X + XYZ_Y + XYZ_Z);
**	***
**	XY to RGB
*/


void setHSV_fromRGB (u8 R, u8 G, u8 B, u16 *HSV_H, u8 *HSV_S, u8 *HSV_V)
{
	if (NULL==HSV_H || NULL==HSV_S || NULL==HSV_V) {
		return;		// parameter invalid
	}
	//	convert the fractions
	float R_ = R / 255;
	float G_ = G / 255;
	float B_ = B / 255;

	float Cmax  = ((R>G && R>B) ?R_ :((G>R && G>B) ?G_ :B_));
	float Cmin  = ((R<G && R<B) ?R_ :((G<R && G<B) ?G_ :B_));
	float delta = Cmax - Cmin;

	/* if (delta == 0)	// no chroma, so it`s gray
	{
		*HSV_H = 0;
		*HSV_S = 0;
	}
	else */
	{
		if (Cmax == R_) {
			*HSV_H = ((u16)((G_ - B_) / delta) % 6) * 60;
		}
		else if (Cmax == G_) {
			*HSV_H = ((u16)((B_ - R_) / delta) + 2) * 60;
		}
		else if (Cmax == B_) {
			*HSV_H = ((u16)((R_ - G_) / delta) + 4) * 60;
		}

		if (Cmax == 0) {
			*HSV_S = 0;
		}
		else {
			*HSV_S = (delta / Cmax) * 100;
		}
	}
	*HSV_V = Cmax * 100;
}

#endif /* (__PROJECT_TL_DIMMABLE_LIGHT__) */
