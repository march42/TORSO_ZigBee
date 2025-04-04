/*	testing color calculation
**	***
*/

/*	includes
*/
#include <stdio.h>

#include "color.h"


/*	set from HSV (ZigBee values)
**	ZigBee Hue				is fractions of 254 (ZCL_COLOR_ATTR_HUE_MAX)
**		Hue = CurrentHue x 360 / 254
**	ZigBee Saturation		is fractions of 254 (ZCL_COLOR_ATTR_SATURATION_MAX)
**		Saturation = CurrentSaturation / 254
**	ZigBee Value,LEVEL		is fractions of 254 (ZCL_LEVEL_ATTR_MAX_LEVEL)
*/
void LEDLIGHT_setHSV (u8 hue, u8 saturation, u8 value)
{
	s32 HSV_V = (value>=ZCL_LEVEL_ATTR_MAX_LEVEL) ?COLORCHANNEL_MAX :(value * COLORCHANNEL_MAX / ZCL_LEVEL_ATTR_MAX_LEVEL);	// value 0...COLORCHANNEL_MAX ==100%
	if (saturation == 0) {		// short-cut for optimization
		// no saturation means achromatic, ergo white
#if (COLOR_RGB_SUPPORT)
		__LED_RED_SETVALUE   (HSV_V         ,COLORCHANNEL_MAX);
		__LED_RED_SETONOFF   (COLORONOFF_MAX,  COLORONOFF_MAX);
		__LED_GREEN_SETVALUE (HSV_V         ,COLORCHANNEL_MAX);
		__LED_GREEN_SETONOFF (COLORONOFF_MAX,  COLORONOFF_MAX);
		__LED_BLUE_SETVALUE  (HSV_V         ,COLORCHANNEL_MAX);
		__LED_BLUE_SETONOFF  (COLORONOFF_MAX,  COLORONOFF_MAX);
#endif
#if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
		__LED_COLD_SETVALUE  (HSV_V         ,COLORCHANNEL_MAX);
		__LED_COLD_SETONOFF  (COLORONOFF_MAX,  COLORONOFF_MAX);
#	if (COLOR_CCT_SUPPORT)
		__LED_WARM_SETVALUE  (HSV_V         ,COLORCHANNEL_MAX);
		__LED_WARM_SETONOFF  (COLORONOFF_MAX,  COLORONOFF_MAX);
#	endif
#endif
		return;
	}

	s32 HSV_S	= (saturation>=ZCL_COLOR_ATTR_SATURATION_MAX) ?COLORCHANNEL_MAX :(saturation * COLORCHANNEL_MAX / ZCL_COLOR_ATTR_SATURATION_MAX);	// value 0...COLORCHANNEL_MAX ==100%
	s32 _P		= (COLORCHANNEL_MAX - HSV_S                         ) * HSV_V / COLORCHANNEL_MAX;

#if (COLOR_RGB_SUPPORT)
	s16 HSV_H	= (hue>=ZCL_COLOR_ATTR_HUE_MAX) ?360 :(hue * 360 / ZCL_COLOR_ATTR_HUE_MAX);	// value 0...360 degrees

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

	s32 _Q		= (COLORCHANNEL_MAX - HSV_S * (      fraction  / 60)) * HSV_V / COLORCHANNEL_MAX;
	s32 _T		= (COLORCHANNEL_MAX - HSV_S * ((60 - fraction) / 60)) * HSV_V / COLORCHANNEL_MAX;

	switch (sector) {
		case 0:
			__LED_RED_SETVALUE   (HSV_V,COLORCHANNEL_MAX);
			__LED_GREEN_SETVALUE (_T   ,COLORCHANNEL_MAX);
			__LED_BLUE_SETVALUE  (_P   ,COLORCHANNEL_MAX);
			break;
		case 1:
			__LED_RED_SETVALUE   (_Q   ,COLORCHANNEL_MAX);
			__LED_GREEN_SETVALUE (HSV_V,COLORCHANNEL_MAX);
			__LED_BLUE_SETVALUE  (_P   ,COLORCHANNEL_MAX);
			break;
		case 2:
			__LED_RED_SETVALUE   (_P   ,COLORCHANNEL_MAX);
			__LED_GREEN_SETVALUE (HSV_V,COLORCHANNEL_MAX);
			__LED_BLUE_SETVALUE  (_T   ,COLORCHANNEL_MAX);
			break;
		case 3:
			__LED_RED_SETVALUE   (_P   ,COLORCHANNEL_MAX);
			__LED_GREEN_SETVALUE (_Q   ,COLORCHANNEL_MAX);
			__LED_BLUE_SETVALUE  (HSV_V,COLORCHANNEL_MAX);
			break;
		case 4:
			__LED_RED_SETVALUE   (_T   ,COLORCHANNEL_MAX);
			__LED_GREEN_SETVALUE (_P   ,COLORCHANNEL_MAX);
			__LED_BLUE_SETVALUE  (HSV_V,COLORCHANNEL_MAX);
			break;
		case 5:
			__LED_RED_SETVALUE   (HSV_V,COLORCHANNEL_MAX);
			__LED_GREEN_SETVALUE (_P   ,COLORCHANNEL_MAX);
			__LED_BLUE_SETVALUE  (_Q   ,COLORCHANNEL_MAX);
			break;
	}

	__LED_RED_SETONOFF   (COLORONOFF_MAX,COLORONOFF_MAX);
	__LED_GREEN_SETONOFF (COLORONOFF_MAX,COLORONOFF_MAX);
	__LED_BLUE_SETONOFF  (COLORONOFF_MAX,COLORONOFF_MAX);
#endif

#if (SINGLE_WHITE_SUPPORT) || (COLOR_CCT_SUPPORT)
	__LED_COLD_SETVALUE  (_P            ,COLORCHANNEL_MAX);
	__LED_COLD_SETONOFF  (COLORONOFF_MAX,  COLORONOFF_MAX);
#	if (COLOR_CCT_SUPPORT)
	__LED_WARM_SETVALUE  (_P            ,COLORCHANNEL_MAX);
	__LED_WARM_SETONOFF  (COLORONOFF_MAX,  COLORONOFF_MAX);
#	endif
#endif

	return;
}

int main (void)
{
	// hue (0..360), 6 segments of 60°
	// hue 0==0, 42==60, 0xFE==360
	u8 hue[]		= {
		(  0 * 0xFE / 360),(  1 * 0xFE / 360), ( 15 * 0xFE / 360), ( 30 * 0xFE / 360), ( 45 * 0xFE / 360), ( 59 * 0xFE / 360),
		( 60 * 0xFE / 360),( 61 * 0xFE / 360), ( 75 * 0xFE / 360), ( 90 * 0xFE / 360), (105 * 0xFE / 360), (119 * 0xFE / 360),
		(120 * 0xFE / 360),(121 * 0xFE / 360), (135 * 0xFE / 360), (150 * 0xFE / 360), (165 * 0xFE / 360), (179 * 0xFE / 360),
		(180 * 0xFE / 360),(181 * 0xFE / 360), (195 * 0xFE / 360), (210 * 0xFE / 360), (225 * 0xFE / 360), (239 * 0xFE / 360),
		(240 * 0xFE / 360),(241 * 0xFE / 360), (255 * 0xFE / 360), (270 * 0xFE / 360), (285 * 0xFE / 360), (299 * 0xFE / 360),
		(300 * 0xFE / 360),(301 * 0xFE / 360), (315 * 0xFE / 360), (330 * 0xFE / 360), (345 * 0xFE / 360), (359 * 0xFE / 360),
		(360 * 0xFE / 360)
	};
	u8 saturation[]	= { 0, 127, 254 };
	u8 value[]		= { 0, 127, 254 };

	printf ("COLORCHANNEL_MAX=%4X\r\n",COLORCHANNEL_MAX);
	printf ("  COLORONOFF_MAX=%3d\r\n",  COLORONOFF_MAX);

	u8	_h, _s, _v;
	for (_h=0; _h<sizeof(hue); ++_h) {
		printf ("\r\n");
		for (_s=0; _s<sizeof(saturation); ++_s) {
			for (_v=0; _v<sizeof(value); ++_v) {

				printf ("\tH=%3d, S=%3d, V=%3d", hue[_h], saturation[_s], value[_v]);
				LEDLIGHT_setHSV (hue[_h], saturation[_s], value[_v]);

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
