/*	testing color calculation
**	***
*/

/*	includes
*/
#include <stdio.h>
#include <math.h>

#include "color.h"

#define __D50_MATRIX__				1
#define __D65_MATRIX__				2
#define __WIDEGAMUT_MATRIX__		3
#ifndef COLOR_MATRIX
#	define COLOR_MATRIX 			__WIDEGAMUT_MATRIX__
#endif

void LEDLIGHT_RGB2XYZ (u8 RGB_R, u8 RGB_G, u8 RGB_B, s32 *XYZ_X, s32 *XYZ_Y, s32 *XYZ_Z)
{
	if (XYZ_X == NULL || XYZ_Y == NULL || XYZ_Z == NULL) {
		return;		// parameter error
	}

	// defined white point, shortcut
	if (RGB_R == RGB_G && RGB_R == RGB_B) {		// same three values means gray scale
		*XYZ_X	= 0xF6D3;
		*XYZ_Y	= (0xFFFF * RGB_R / 0xFF);	// brightness =RGB_R,RGB_G,RGB_B
		*XYZ_Z	= 0xD33B;
		return;
	}

#if (COLOR_MATRIX == __WIDEGAMUT_MATRIX__)
	*XYZ_X	= 0  +  0x0B750 * RGB_R / 0xFF  +  0x019D6 * RGB_G / 0xFF  +  0x025AC * RGB_B / 0xFF;
	*XYZ_Y	= 0  +  0x04217 * RGB_R / 0xFF  +  0x0B995 * RGB_G / 0xFF  +  0x00451 * RGB_B / 0xFF;
	*XYZ_Z	= 0  +  0x00000 * RGB_R / 0xFF  +  0x00D41 * RGB_G / 0xFF  +  0x0C5F9 * RGB_B / 0xFF;
#elif (COLOR_MATRIX == __D50_MATRIX__)
	*XYZ_X	= 0  +  0x07CA4 * RGB_R / 0xFF  +  0x04E69 * RGB_G / 0xFF  +  0x02BC8 * RGB_B / 0xFF;
	*XYZ_Y	= 0  +  0x02CB6 * RGB_R / 0xFF  +  0x0D322 * RGB_G / 0xFF  +  0x00026 * RGB_B / 0xFF;
	*XYZ_Z	= 0  -  0x00053 * RGB_R / 0xFF  +  0x00458 * RGB_G / 0xFF  +  0x0CF39 * RGB_B / 0xFF;
#elif (COLOR_MATRIX == __D65_MATRIX__)
#endif

	if (*XYZ_X + *XYZ_Y + *XYZ_Z == 0) {		// should never, it's simple black
		// set to reference white point
		*XYZ_X	= 0xF6D3;
		*XYZ_Z	= 0xD33B;
		return;
	} //else
	if (*XYZ_X > 0xFFFF) {
		*XYZ_X	= 0xFFFF;
	} //else
	if (*XYZ_Y > 0xFFFF) {
		*XYZ_Y	= 0xFFFF;
	} //else
	if (*XYZ_Z > 0xFFFF) {
		*XYZ_Z	= 0xFFFF;
	}
	return;
}

void LEDLIGHT_setXYZ (u16 XYZ_X, u16 XYZ_Y, u16 XYZ_Z)
{
	// X/Y/Z is fraction of 0x10000
	/*	matrix values converted to integer 1.0==0xFFFF
	**	white reference point	X=95/100, Y=1, Z=109/100
	*/
	s32 RGB_R	= (0  +  0x1767C * XYZ_X / 0xFFFF  -  0x02F1E * XYZ_Y / 0xFFFF  -  0x0463C * XYZ_Z / 0xFFFF);
	s32 RGB_G	= (0  -  0x08593 * XYZ_X / 0xFFFF  +  0x1727B * XYZ_Y / 0xFFFF  +  0x01156 * XYZ_Z / 0xFFFF);
	s32 RGB_B	= (0  +  0x008F1 * XYZ_X / 0xFFFF  -  0x018CE * XYZ_Y / 0xFFFF  +  0x149DB * XYZ_Z / 0xFFFF);

	// range limitations
	if (RGB_R < 0) {
		RGB_R	= 0;
	}
	if (RGB_G < 0) {
		RGB_G	= 0;
	}
	if (RGB_B < 0) {
		RGB_B	= 0;
	}
	if (RGB_R > RGB_B && RGB_R > RGB_G && RGB_R > 0xFFFF) {
		RGB_G = RGB_G / RGB_R;
		RGB_B = RGB_B / RGB_R;
		RGB_R = 0xFFFF;
	} else if (RGB_G > RGB_B && RGB_G > RGB_R && RGB_G > 0xFFFF) {
		RGB_R = RGB_R / RGB_G;
		RGB_B = RGB_B / RGB_G;
		RGB_G = 0xFFFF;
	} else if (RGB_B > RGB_R && RGB_B > RGB_G && RGB_B > 0xFFFF) {
		RGB_R = RGB_R / RGB_B;
		RGB_G = RGB_G / RGB_B;
		RGB_B = 0xFFFF;
	}

#if (COLOR_RGB_SUPPORT)
	__LED_RED_SETVALUE   (RGB_R, 0xFFFF);
	__LED_GREEN_SETVALUE (RGB_G, 0xFFFF);
	__LED_BLUE_SETVALUE  (RGB_B, 0xFFFF);
#	endif

#	if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	s32	RGB_max		= (((RGB_R > RGB_G) && (RGB_R > RGB_B)) ?RGB_R :(((RGB_G > RGB_R) && (RGB_G > RGB_B)) ?RGB_G :RGB_B));
	s32	RGB_min		= (((RGB_R < RGB_G) && (RGB_R < RGB_B)) ?RGB_R :(((RGB_G < RGB_R) && (RGB_G < RGB_B)) ?RGB_G :RGB_B));
	s32	RGB_delta	= RGB_max - RGB_min;
	printf ("\tmax=%d,min=%d,delta=%d",RGB_max,RGB_min,RGB_delta);
	s32 saturation	= COLORCHANNEL_MAX * RGB_delta / RGB_max;
	printf (", S=%d",saturation);
	__LED_COLD_SETVALUE (COLORCHANNEL_MAX - saturation,COLORCHANNEL_MAX);
#	if (COLOR_CCT_SUPPORT)
	__LED_WARM_SETVALUE (COLORCHANNEL_MAX - saturation,COLORCHANNEL_MAX);
#	endif
#	endif

/*	xyY --> kelvin
		n = (this.x - 0.332) / (0.1858 - this.y);
		kelvin = Math.abs(437 * n ** 3 + 3601 * n ** 2 + 6861 * n + 5517);
*/

	return;
}

void LEDLIGHT_setXY (u16 ZigBee_X, u16 ZigBee_Y, u8 ZigBee_Level)
{
	if (ZigBee_Level == 0) {
#if (COLOR_RGB_SUPPORT)
		// set values/levels to OFF
		//__LED_RED_SETVALUE   (0, 0xFFFF);
		__LED_RED_SETONOFF   (ZigBee_Level, ZCL_LEVEL_ATTR_MAX_LEVEL);
		//__LED_GREEN_SETVALUE (0, 0xFFFF);
		__LED_GREEN_SETONOFF (ZigBee_Level, ZCL_LEVEL_ATTR_MAX_LEVEL);
		//__LED_BLUE_SETVALUE  (0, 0xFFFF);
		__LED_BLUE_SETONOFF  (ZigBee_Level, ZCL_LEVEL_ATTR_MAX_LEVEL);
	#	endif
		return;				// skip computations, if brightness is OFF
	}

#if (0)
	// range limitation, for error handling
	if (ZigBee_X > ZCL_COLOR_ATTR_XY_MAX) {	// x = CurrentX / 65536 (CurrentX in the range 0 to 65279 inclusive)
		ZigBee_X		= ZCL_COLOR_ATTR_XY_MAX;
	}
	if (ZigBee_Y > ZCL_COLOR_ATTR_XY_MAX) {	// y = CurrentY / 65536 (CurrentY in the range 0 to 65279 inclusive)
		ZigBee_Y		= ZCL_COLOR_ATTR_XY_MAX;
	}
	if (ZigBee_Level > ZCL_LEVEL_ATTR_MAX_LEVEL) {
		ZigBee_Level	= ZCL_LEVEL_ATTR_MAX_LEVEL;
	}
#endif

	/*	XYZ color space calculation
	**	x = X / (X + Y + Z)
	**	y = Y / (X + Y + Z)
	**	z = Z / (X + Y + Z) = 1 - x - y
	**	white point X=Y=Z =1/3
	*/
	s32 XYZ_X	= 0xFFFF / 3;
	s32 XYZ_Y	= 0xFFFF / 3;
	s32 XYZ_Z	= 0xFFFF - XYZ_X - XYZ_Y;
	if (ZigBee_Level >= ZCL_LEVEL_ATTR_MAX_LEVEL) {
		XYZ_Y	= 0xFFFF;
		XYZ_X	=           ZigBee_X             * XYZ_Y / ZigBee_Y;
		XYZ_Z	= (0xFFFF - ZigBee_X - ZigBee_Y) * XYZ_Y / ZigBee_Y;
	}
	else
	{
		XYZ_Y	= (0xFFFF * ZigBee_Level / ZCL_LEVEL_ATTR_MAX_LEVEL);
		XYZ_X	= (ZigBee_X * XYZ_Y) / (0xFFFF * ZigBee_Y);
		XYZ_Z	= ((0xFFFF - ZigBee_X - ZigBee_Y) * XYZ_Y) / (0xFFFF * ZigBee_Y);
	}
	// now X/Y/Z is fraction of 0xFFFF

#if (COLOR_MATRIX == __WIDEGAMUT_MATRIX__)
	/*	matrix values converted to integer 1.0==0xFFFF
	**	wide gamut RGB, D50 reference white
	*/
	s32 RGB_R	= 0  +  0x1767C * XYZ_X / 0xFFFF  -  0x02F1E * XYZ_Y / 0xFFFF  -  0x0463C * XYZ_Z / 0xFFFF;
	s32 RGB_G	= 0  -  0x08593 * XYZ_X / 0xFFFF  +  0x1727B * XYZ_Y / 0xFFFF  +  0x01156 * XYZ_Z / 0xFFFF;
	s32 RGB_B	= 0  +  0x008F1 * XYZ_X / 0xFFFF  -  0x018CE * XYZ_Y / 0xFFFF  +  0x149DB * XYZ_Z / 0xFFFF;
#elif (COLOR_MATRIX == __D50_MATRIX__)
	/*	matrix values converted to integer 1.0==0xFFFF
	**	white reference point	X=95/100, Y=1, Z=109/100
	*/
	s32 RGB_R	= 0  +  0x25D20 * XYZ_X / 0xFFFF  -  0x0DE1B * XYZ_Y / 0xFFFF  -  0x07FB2 * XYZ_Z / 0xFFFF;
	s32 RGB_G	= 0  -  0x08027 * XYZ_X / 0xFFFF  +  0x1656E * XYZ_Y / 0xFFFF  +  0x01AD1 * XYZ_Z / 0xFFFF;
	s32 RGB_B	= 0  +  0x003A0 * XYZ_X / 0xFFFF  -  0x007D8 * XYZ_Y / 0xFFFF  +  0x13B7C * XYZ_Z / 0xFFFF;
#elif (COLOR_MATRIX == __D65_MATRIX__)
#endif

	// range limitations
	if (RGB_R < 0) {
		RGB_R	= 0;
	}
	if (RGB_G < 0) {
		RGB_G	= 0;
	}
	if (RGB_B < 0) {
		RGB_B	= 0;
	}

	if (RGB_R > 0xFFFF && RGB_R > RGB_B && RGB_R > RGB_G) {
		RGB_G = RGB_G * 0xFFFF / RGB_R;
		RGB_B = RGB_B * 0xFFFF / RGB_R;
		RGB_R = 0xFFFF;
	} else if (RGB_G > 0xFFFF && RGB_G > RGB_B && RGB_G > RGB_R) {
		RGB_R = RGB_R * 0xFFFF / RGB_G;
		RGB_B = RGB_B * 0xFFFF / RGB_G;
		RGB_G = 0xFFFF;
	} else if (RGB_B > 0xFFFF && RGB_B > RGB_R && RGB_B > RGB_G) {
		RGB_R = RGB_R * 0xFFFF / RGB_B;
		RGB_G = RGB_G * 0xFFFF / RGB_B;
		RGB_B = 0xFFFF;
	}
	//printf ("\tR=%4X,G=%4X,B=%4X", RGB_R, RGB_G, RGB_B);

#if (COLOR_RGB_SUPPORT)
	__LED_RED_SETVALUE   (RGB_R, 0xFFFF);
	__LED_RED_SETONOFF   (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
	__LED_GREEN_SETVALUE (RGB_G, 0xFFFF);
	__LED_GREEN_SETONOFF (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
	__LED_BLUE_SETVALUE  (RGB_B, 0xFFFF);
	__LED_BLUE_SETONOFF  (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
#	endif

#	if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	s32 RGB_l		= RGB_LIGHTNESS (RGB_R, RGB_G, RGB_B);
	__LED_COLD_SETVALUE (RGB_l, 0xFFFF);
	__LED_COLD_SETONOFF (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
#	if (COLOR_CCT_SUPPORT)
	__LED_WARM_SETVALUE (RGB_l, 0xFFFF);
	__LED_WARM_SETONOFF (ZCL_LEVEL_ATTR_MAX_LEVEL, ZCL_LEVEL_ATTR_MAX_LEVEL);
#	endif
#	endif

/*	xyY --> kelvin
		n = (this.x - 0.332) / (0.1858 - this.y);
		kelvin = Math.abs(437 * n ** 3 + 3601 * n ** 2 + 6861 * n + 5517);
*/

	return;
}

void LEDLIGHT_CCT2XY (u16 ZigBee_mired, u16 *ZigBee_X, u16 *ZigBee_Y)
{
	/*	conversion of (correlated) color temperature to xy coordinate
	**	based on approximation
	**	see WikiPedia article Planckian locus https://en.wikipedia.org/wiki/Planckian_locus#Approximation
	**	the formulas are based on Kelvin instead of mired
	*/
	float kelvin = ZigBee_mired<=0 ?0.0 :(1.0e6 / ZigBee_mired);	// with float the powf is not needed for ^2 and ^3
	float x,y;
	if (kelvin < 1667)
	{
		// hard limit, because i have no according formula
		kelvin	= 1667;
	}
	else if (kelvin > 25000)
	{
		// hard limit, because i have no according formula
		kelvin	= 25000;
	}

	//else	// check again, to match on corrected out of band value
	if (kelvin <= 4000)
	{	// 1667 K <= color temperature <= 4000 K
		x = - 0.2661239 * 1.0e9 / (kelvin*kelvin*kelvin) - 0.2343589 * 1.0e6 / (kelvin*kelvin) + 0.8776956 * 1.0e3 / kelvin + 0.179910;
		if (kelvin <= 2222)
		{	// 1667 K <= color temperature <= 2222 K
			y = - 1.1063814 * (x*x*x) - 1.34811020 * (x*x) + 2.18555832 * x - 0.20219683;
		}
		else
		//if (kelvin <= 4000)
		{	// 2222 K <= color temperature <= 4000 K
			y = - 0.9549476 * (x*x*x) - 1.37418593 * (x*x) + 2.09137015 * x - 0.16748867;
		}
	}
	else
	//if (kelvin <= 25000)
	{	// 4000 K <= color temperature <= 25000 K
		x = - 3.0258469 * 1.0e9 / (kelvin*kelvin*kelvin) + 2.1070379 * 1.0e6 / (kelvin*kelvin) + 0.2226347 * 1.0e3 / kelvin + 0.240390;
		y = + 3.0817580 * (x*x*x) - 5.87338670 * (x*x) + 3.75112997 * x - 0.37001483;
	}

	printf ("\tCT=%5.0f, x=%f, y=%f", kelvin, x, y);
	*ZigBee_X	= (u16)(x * 65535.0 + 0.5);		// change to fraction of 65535 and cast to 16 bit
	*ZigBee_Y	= (u16)(y * 65535.0 + 0.5);
	return;
}


int main (void)
{
	u8	RGB_list[]		= { 0, 1, 0x10, 0x7F, 0xFE, 0xFF };
	u16	Mired_list[]	= { 0, 40, 39,
		599,	// 1667 K
		555,	// 1800 K
		454,	// 2200 K
		370,	// 2700 K
		333,	// 3000 K
		285,	// 3500 K
		250,	// 4000 K
		200,	// 5000 K
		175,	// 5700 K
		153,	// 6500 K
	};

	printf ("COLORCHANNEL_MAX=%4X\r\n",COLORCHANNEL_MAX);
	printf ("  COLORONOFF_MAX=%3d\r\n",  COLORONOFF_MAX);

	u8	_r, _g, _b;
	u16	_x, _y, _l;
	s32	_X, _Y, _Z;
	s32 XY_x, XY_y, XY_level;

	for (_r=0; _r<sizeof(Mired_list)/sizeof(Mired_list[0]); ++_r) {
		LEDLIGHT_CCT2XY (Mired_list[_r], &_x, &_y);
		printf ("\tCT=%d\tx=%4X,y=%4X\r\n", Mired_list[_r], _x, _y);
		_l = ZCL_LEVEL_ATTR_MAX_LEVEL;
		LEDLIGHT_setXY (_x, _y, _l);

#if (COLOR_RGB_SUPPORT) && (!SINGLE_WHITE_SUPPORT) && (!COLOR_CCT_SUPPORT)
		printf ("\tR=%4X/%3d\tG=%4X/%3d\tB=%4X/%3d\r\n"
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff );
#elif (COLOR_CCT_SUPPORT) && (!COLOR_RGB_SUPPORT)
		printf ("\tC=%4X/%3d\tW=%4X/%3d\r\n"
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff
			, g_ledChannel_WARM.Value, g_ledChannel_WARM.OnOff );
#elif (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)
		printf ("\tR=%4X/%3d\tG=%4X/%3d\tB=%4X/%3d\tC=%4X/%3d\r\n"
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff );
#elif (COLOR_CCT_SUPPORT) && (COLOR_RGB_SUPPORT)
		printf ("\tR=%4X/%3d\tG=%4X/%3d\tB=%4X/%3d\tC=%4X/%3d\tW=%4X/%3d\r\n"
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff
			, g_ledChannel_WARM.Value, g_ledChannel_WARM.OnOff );
#endif
	}

	for (_r=0; _r<sizeof(RGB_list)/sizeof(RGB_list[0]); ++_r) {
		printf ("\r\n");
		for (_g=0; _g<sizeof(RGB_list)/sizeof(RGB_list[0]); ++_g) {
			for (_b=0; _b<sizeof(RGB_list)/sizeof(RGB_list[0]); ++_b) {
				printf ("\tR=%2X,G=%2X,B=%2X", RGB_list[_r],RGB_list[_g],RGB_list[_b]);

				LEDLIGHT_RGB2XYZ (RGB_list[_r],RGB_list[_g],RGB_list[_b], &_X,&_Y,&_Z);
				//printf ("\tX=%4X,Y=%4X,Z=%4X", _X,_Y,_Z);

				XY_level	= _Y * 0xFE / 0xFFFF; //(_Y >> 8) &0xFE;
				XY_x		= (_X * 0xFFFF / (_X + _Y + _Z)) &0xFEFF;
				XY_y		= (_Y * 0xFFFF / (_X + _Y + _Z)) &0xFEFF;
				printf ("\tx=%4X,y=%4X,l=%2X", XY_x, XY_y, XY_level);

				LEDLIGHT_setXY (XY_x, XY_y, XY_level);

#if (COLOR_RGB_SUPPORT) && (!SINGLE_WHITE_SUPPORT) && (!COLOR_CCT_SUPPORT)
		printf ("\tR=%4X/%3d\tG=%4X/%3d\tB=%4X/%3d\r\n"
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff );
#elif (COLOR_CCT_SUPPORT) && (!COLOR_RGB_SUPPORT)
		printf ("\tC=%4X/%3d\tW=%4X/%3d\r\n"
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff
			, g_ledChannel_WARM.Value, g_ledChannel_WARM.OnOff );
#elif (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)
		printf ("\tR=%4X/%3d\tG=%4X/%3d\tB=%4X/%3d\tC=%4X/%3d\r\n"
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff );
#elif (COLOR_CCT_SUPPORT) && (COLOR_RGB_SUPPORT)
		printf ("\tR=%4X/%3d\tG=%4X/%3d\tB=%4X/%3d\tC=%4X/%3d\tW=%4X/%3d\r\n"
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff
			, g_ledChannel_WARM.Value, g_ledChannel_WARM.OnOff );
#endif

			}
		}
	}

    return (0);
}
