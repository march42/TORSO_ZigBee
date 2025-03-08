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

/*	set from HSV (ZigBee values)
**	ZigBee Hue				is fractions of 254 (ZCL_COLOR_ATTR_HUE_MAX)
**		Hue = CurrentHue x 360 / 254
**	ZigBee Saturation		is fractions of 254 (ZCL_COLOR_ATTR_SATURATION_MAX)
**		Saturation = CurrentSaturation / 254
**	ZigBee Value,LEVEL		is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)
*/
void LEDLIGHT_setHSV (u8 hue, u8 saturation, u8 value)
{
	float HSV_H, HSV_S, HSV_V;
	HSV_V = ((float)value / ZCL_LEVEL_ATTR_MAX_LEVEL);
	if (saturation == 0) {		// short-cut for optimization
		// no saturation means achromatic, ergo white
		LEDLIGHT_RED->Value		= LEDLIGHT_GREEN->Value	= LEDLIGHT_BLUE->Value	= HSV_V;
		return;
	}
	HSV_H = ((float)hue * 360 / ZCL_COLOR_ATTR_HUE_MAX);
	HSV_S = ((float)saturation / ZCL_COLOR_ATTR_SATURATION_MAX);

	u16   sector   = hue / (ZCL_COLOR_ATTR_HUE_MAX / 6);		// circle sector
	float fraction = hue % (ZCL_COLOR_ATTR_HUE_MAX / 6);		// fractional part

	float p = (float)(HSV_V * (1.0 - HSV_S));
	float q = (float)(HSV_V * (1.0 - HSV_S * fraction / 60));
	float t = (float)(HSV_V * (1.0 - HSV_S * (1 - fraction / 60)));

	switch (sector) {
	case 0:
		LEDLIGHT_RED->Value		= HSV_V;
		LEDLIGHT_GREEN->Value	= t;
		LEDLIGHT_BLUE->Value	= p;
		break;
	case 1:
		LEDLIGHT_RED->Value		= q;
		LEDLIGHT_GREEN->Value	= HSV_V;
		LEDLIGHT_BLUE->Value	= p;
		break;
	case 2:
		LEDLIGHT_RED->Value		= p;
		LEDLIGHT_GREEN->Value	= HSV_V;
		LEDLIGHT_BLUE->Value	= t;
		break;
	case 3:
		LEDLIGHT_RED->Value		= p;
		LEDLIGHT_GREEN->Value	= q;
		LEDLIGHT_BLUE->Value	= HSV_V;
		break;
	case 4:
		LEDLIGHT_RED->Value		= t;
		LEDLIGHT_GREEN->Value	= p;
		LEDLIGHT_BLUE->Value	= HSV_V;
		break;
	case 5:
	default:
		LEDLIGHT_RED->Value		= HSV_V;
		LEDLIGHT_GREEN->Value	= p;
		LEDLIGHT_BLUE->Value	= q;
		break;
	}

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

/*	set from XYZ (fraction of 1 values)
**	Y=1 setting full on brightness
*/
void LEDLIGHT_setXYZ (float XYZ_X, float XYZ_Y, float XYZ_Z)
{
	LEDLIGHT_RED->Value		= (float)( 3.2404542 * XYZ_X - 1.5371385 * XYZ_Y - 0.4985314 * XYZ_Z);
	LEDLIGHT_GREEN->Value	= (float)(-0.969266  * XYZ_X + 1.8760108 * XYZ_Y + 0.041556  * XYZ_Z);
	LEDLIGHT_BLUE->Value	= (float)( 0.0556434 * XYZ_X - 0.2040259 * XYZ_Y + 1.0572252 * XYZ_Z);
	if (LEDLIGHT_RED->Value > 1) { 
		LEDLIGHT_RED->Value = 1; 
	}
	if (LEDLIGHT_GREEN->Value > 1) { 
		LEDLIGHT_GREEN->Value = 1; 
	}
	if (LEDLIGHT_BLUE->Value > 1) { 
		LEDLIGHT_BLUE->Value = 1; 
	}
	return;
}

/*	set from XY (ZigBee values)
**	ZigBee x	is fraction of 65279 (ZCL_COLOR_ATTR_XY_MAX)
**	ZigBee y	is fraction of 65279 (ZCL_COLOR_ATTR_XY_MAX)
**	this is xyY with Y=1 full on brightness
*/
void LEDLIGHT_setXY (u16 ZigBee_X, u16 ZigBee_Y)
{
	float xyY_X		= ZigBee_X / ZCL_COLOR_ATTR_XY_MAX;
	float xyY_Y	= ZigBee_Y / ZCL_COLOR_ATTR_XY_MAX;
	
	float XYZ_X	= xyY_X / xyY_Y;
	float XYZ_Z = (1 - xyY_X - xyY_Y) / xyY_Y;
	LEDLIGHT_setXYZ (XYZ_X, 1, XYZ_Z);
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
 * @param   [in]kelvin		- color temperature value in Kelvin
 *
 * @return  None
 */
void LEDLIGHT_setKelvin (u16 kelvin)
{
	u8	RGB_R, RGB_G, RGB_B;

	// range limitation
#if (1) // (EXTENDED_COLOR_LIGHT)
	if (kelvin < 1000) {
		kelvin = 1000;
	} else if (kelvin > 12000) {
		kelvin = 12000;
	}
#else
	if (kelvin < 1700) {
		kelvin = 1700;
	} else if (kelvin > 7500) {
		kelvin = 7500;
	}
#endif

	switch (kelvin/100) {	// use 100 Kelvin steps
#if (1) // (EXTENDED_COLOR_LIGHT)
		case 10:	RGB_R=255;	RGB_G=56;	RGB_B=0;	break;
		case 11:	RGB_R=255;	RGB_G=71;	RGB_B=0;	break;
		case 12:	RGB_R=255;	RGB_G=83;	RGB_B=0;	break;
		case 13:	RGB_R=255;	RGB_G=93;	RGB_B=0;	break;
		case 14:	RGB_R=255;	RGB_G=101;	RGB_B=0;	break;
		case 15:	RGB_R=255;	RGB_G=109;	RGB_B=0;	break;
		case 16:	RGB_R=255;	RGB_G=115;	RGB_B=0;	break;
#endif
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
		default:
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
#if (1) // (EXTENDED_COLOR_LIGHT)
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
#endif
	}

	LEDLIGHT_RED->Value		= (float)(RGB_R / 255);
	LEDLIGHT_GREEN->Value	= (float)(RGB_G / 255);
	LEDLIGHT_BLUE->Value	= (float)(RGB_B / 255);

	return;
}

/*	set from Color Temperature Mireds (ZigBee values)
**	1...65279
**	ColorTemperatureMireds = 0x0000 indicates an undefined value
**	ColorTemperatureMireds = 0xffff indicates an invalid value
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
	LEDLIGHT_setKelvin (colorTemperature);
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
