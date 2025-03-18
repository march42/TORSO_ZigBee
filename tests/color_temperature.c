/*	testing color temperature calculation
**	***
*/

/*	includes
*/
#include <stdio.h>

/*	test specifications
*/
#define EXTENDED_COLOR_LIGHT		1
#define COLOR_RGB_SUPPORT			1
#define COLOR_CCT_SUPPORT			0
#define SINGLE_WHITE_SUPPORT		0

/* type declerations
*/
typedef   signed char	s8;
typedef unsigned char	u8;
typedef   signed short	s16;
typedef unsigned short	u16;
typedef   signed int	s32;
typedef unsigned int	u32;

typedef struct {
	u16		Value;		// current value in 16 bit precision
	u8		OnOff;		// state On/Off (-1==undefined, 0==off, 1...100==percentage)
}	ts_LED_Channel;

/*	inline functions
*/
#	define COLOR_TEMPERATURE_CONVERT(kelvin_mired)	(1000000 / kelvin_mired)
#	define COLOR_TEMPERATURE_NEUTRAL	COLOR_TEMPERATURE_CONVERT(3800)
#	define WHITE_CT						COLOR_TEMPERATURE_NEUTRAL
#	define COLD_LIGHT_TEMPERATURE		COLOR_TEMPERATURE_CONVERT(6500)
#	define WARM_LIGHT_TEMPERATURE		COLOR_TEMPERATURE_CONVERT(2200)

/*	definitions
*/
#define COLORCHANNEL_MAX				(48000000 / 6000)
#define COLORONOFF_MAX					100

#if (SINGLE_WHITE_SUPPORT)
	ts_LED_Channel		g_ledChannel_WHITE;
#	define __LED_WHITE_SETVALUE(v,f)			do { g_ledChannel_WHITE.Value	= COLORCHANNEL_MAX * (v) / (f); } while (0);
#	define __LED_WHITE_SETONOFF(v,f)			do { g_ledChannel_WHITE.OnOff	=   COLORONOFF_MAX * (v) / (f); } while (0);
#elif (COLOR_CCT_SUPPORT)
	ts_LED_Channel		g_ledChannel_COLD;
	ts_LED_Channel		g_ledChannel_WARM;
#	define __LED_COLD_SETVALUE(v,f)				do { g_ledChannel_COLD.Value	= COLORCHANNEL_MAX * (v) / (f); } while (0);
#	define __LED_WARM_SETVALUE(v,f)				do { g_ledChannel_WARM.Value	= COLORCHANNEL_MAX * (v) / (f); } while (0);
#	define __LED_COLD_SETONOFF(v,f)				do { g_ledChannel_COLD.OnOff	=   COLORONOFF_MAX * (v) / (f); } while (0);
#	define __LED_WARM_SETONOFF(v,f)				do { g_ledChannel_WARM.OnOff	=   COLORONOFF_MAX * (v) / (f); } while (0);
#endif
#if (COLOR_RGB_SUPPORT)
	ts_LED_Channel		g_ledChannel_RED;
	ts_LED_Channel		g_ledChannel_GREEN;
	ts_LED_Channel		g_ledChannel_BLUE;
#	define __LED_RED_SETVALUE(v,f)				do { g_ledChannel_RED.Value		= COLORCHANNEL_MAX * (v) / (f); } while (0);
#	define __LED_GREEN_SETVALUE(v,f)			do { g_ledChannel_GREEN.Value	= COLORCHANNEL_MAX * (v) / (f); } while (0);
#	define __LED_BLUE_SETVALUE(v,f)				do { g_ledChannel_BLUE.Value	= COLORCHANNEL_MAX * (v) / (f); } while (0);
#	define __LED_RED_SETONOFF(v,f)				do { g_ledChannel_RED.OnOff		=   COLORONOFF_MAX * (v) / (f); } while (0);
#	define __LED_GREEN_SETONOFF(v,f)			do { g_ledChannel_GREEN.OnOff	=   COLORONOFF_MAX * (v) / (f); } while (0);
#	define __LED_BLUE_SETONOFF(v,f)				do { g_ledChannel_BLUE.OnOff	=   COLORONOFF_MAX * (v) / (f); } while (0);
#endif

void LEDLIGHT_setKelvin (u16 kelvin, u16 *derrivedKelvin)
{
#if (COLOR_RGB_SUPPORT)
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
	if (derrivedKelvin) {
		*derrivedKelvin	= kelvin;
	}

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

	g_ledChannel_RED.Value		= ((RGB_R >= 0xFF) ?(COLORCHANNEL_MAX) :(RGB_R * COLORCHANNEL_MAX / 0xFF));
	g_ledChannel_GREEN.Value	= ((RGB_G >= 0xFF) ?(COLORCHANNEL_MAX) :(RGB_G * COLORCHANNEL_MAX / 0xFF));
	g_ledChannel_BLUE.Value		= ((RGB_B >= 0xFF) ?(COLORCHANNEL_MAX) :(RGB_B * COLORCHANNEL_MAX / 0xFF));
	//LEDLIGHT_setOnOff_fromValue();
	return;
#endif
}

void LEDLIGHT_setMireds_RGBW (u16 ZigBee_Mireds, u16 whiteMireds)
{
#if (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)
	// fix range limits
	if (ZigBee_Mireds < COLOR_TEMPERATURE_CONVERT(12000)) {				// LEDLIGHT_setMired minimum
		ZigBee_Mireds = COLOR_TEMPERATURE_CONVERT(12000);
	} else if (ZigBee_Mireds > COLOR_TEMPERATURE_CONVERT(1000)) {			// LEDLIGHT_setMired maximum
		ZigBee_Mireds = COLOR_TEMPERATURE_CONVERT(1000);
	}
	u16 RGB_CT = ZigBee_Mireds;
	if (ZigBee_Mireds <= (whiteMireds / 2)) {			// CT < (WHITE / 2) results in RGB_CT < 0
		RGB_CT	= COLOR_TEMPERATURE_CONVERT(12000);
	} else
	if (ZigBee_Mireds < whiteMireds) {					// colder WHITE
		RGB_CT -= (whiteMireds - ZigBee_Mireds);		// double the distance
		if (RGB_CT < COLOR_TEMPERATURE_CONVERT(12000)) {	// LEDLIGHT_setMired minimum
			RGB_CT = COLOR_TEMPERATURE_CONVERT(12000);
		}
	} else if (ZigBee_Mireds > whiteMireds) {			// warmer WHITE
		RGB_CT += (ZigBee_Mireds - whiteMireds);		// double the distance
		if (RGB_CT > COLOR_TEMPERATURE_CONVERT(1000)) {		// LEDLIGHT_setMired maximum
			RGB_CT = COLOR_TEMPERATURE_CONVERT(1000);
		}
	}
	__LED_WHITE_SETVALUE (255,255);
	u16 kelvin;
	LEDLIGHT_setKelvin (COLOR_TEMPERATURE_CONVERT(RGB_CT), &kelvin);
	printf ("CT=%d, RGB_CT=%d, %dK", ZigBee_Mireds, RGB_CT, kelvin);

	s32 TMP;
	if (ZigBee_Mireds == whiteMireds) {				// SAME
		TMP = COLORONOFF_MAX / 2;
		printf ("\tWHITE=%d", TMP);
	} else if (ZigBee_Mireds < whiteMireds) {			// colder WHITE
		// WARM = (mireds - min) / (max - min)
		TMP = (((ZigBee_Mireds - RGB_CT) * COLORONOFF_MAX) / (whiteMireds - RGB_CT));
		printf ("\tWARM=%d", TMP);
	} else if (ZigBee_Mireds > whiteMireds) {			// warmer WHITE
		TMP = (((RGB_CT - ZigBee_Mireds) * COLORONOFF_MAX) / (RGB_CT - whiteMireds));
		printf ("\tCOLD=%d", TMP);
	}
	__LED_WHITE_SETONOFF (TMP,COLORONOFF_MAX);
	__LED_RED_SETONOFF   (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
	__LED_GREEN_SETONOFF (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
	__LED_BLUE_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
	//LEDLIGHT_setOnOff_fromValue();
#endif
}

void LEDLIGHT_setMireds_RGBCCT (u16 ZigBee_Mireds, const u16 coldMireds, const u16 warmMireds)
{
#if (COLOR_CCT_SUPPORT) && (COLOR_RGB_SUPPORT)
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
	u16 kelvin;
	LEDLIGHT_setKelvin (COLOR_TEMPERATURE_CONVERT(RGB_CT), &kelvin);
	printf ("RGB_CT=%4d/%5dK", RGB_CT, kelvin);
#endif
}

void LEDLIGHT_setMireds_CCT (u16 ZigBee_Mireds, const u16 coldMireds, const u16 warmMireds)
{
#if (COLOR_CCT_SUPPORT) && (!COLOR_RGB_SUPPORT)
	if (ZigBee_Mireds < coldMireds) {				// LEDLIGHT_setMired minimum
		ZigBee_Mireds = coldMireds;
	} else if (ZigBee_Mireds > warmMireds) {			// LEDLIGHT_setMired maximum
		ZigBee_Mireds = warmMireds;
	}
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
	/*if (ZigBee_Mireds == coldMireds) {			// on the edge of cold
		TMP = COLORONOFF_MAX;
		printf ("COLD =%d\t", TMP);
		__LED_COLD_SETVALUE  (255,255);
		__LED_COLD_SETONOFF  (TMP,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (  0,255);
		__LED_WARM_SETONOFF  (  0,COLORONOFF_MAX);
	}
	else if (ZigBee_Mireds == warmMireds) {			// on the edge of warm
		TMP = COLORONOFF_MAX;
		printf ("WARM =%d\t", TMP);
		__LED_COLD_SETVALUE  (  0,255);
		__LED_COLD_SETONOFF  (  0,COLORONOFF_MAX);
		__LED_WARM_SETVALUE  (255,255);
		__LED_WARM_SETONOFF  (TMP,COLORONOFF_MAX);
	}
	else*/
	/*if (ZigBee_Mireds < warmMireds && ZigBee_Mireds > coldMireds)*/ {
		// in between
		TMP = (((ZigBee_Mireds - coldMireds) * COLORONOFF_MAX) / (warmMireds - coldMireds));
		printf ("in-between =%d\t", TMP);
		__LED_WARM_SETVALUE  (255,255);
		__LED_WARM_SETONOFF  (TMP,COLORONOFF_MAX);
		__LED_COLD_SETVALUE  (255,255);
		__LED_COLD_SETONOFF  (COLORONOFF_MAX - TMP,COLORONOFF_MAX);
	}
#endif
}


void TEST_KELVIN (void)
{
	u16  kelvin;
	for (kelvin=900; kelvin<=12100; kelvin+=100) {

#if (COLOR_RGB_SUPPORT) && (!SINGLE_WHITE_SUPPORT) && (!COLOR_CCT_SUPPORT)
		LEDLIGHT_setKelvin (kelvin, NULL);
		printf ("\t%5d Kelvin\tR=%4X\tG=%4X\tB=%4X\r\n", kelvin, g_ledChannel_RED.Value, g_ledChannel_GREEN.Value, g_ledChannel_BLUE.Value);

#elif (COLOR_CCT_SUPPORT) && (!COLOR_RGB_SUPPORT)
		LEDLIGHT_setMireds_CCT (COLOR_TEMPERATURE_CONVERT(kelvin), COLD_LIGHT_TEMPERATURE, WARM_LIGHT_TEMPERATURE);
		printf ("\t%5d Kelvin\tC=%4X/%2X\tW=%4X/%2X\r\n", kelvin
			, g_ledChannel_COLD.Value,g_ledChannel_COLD.OnOff
			, g_ledChannel_WARM.Value,g_ledChannel_WARM.OnOff );

#elif (COLOR_RGB_SUPPORT) && (SINGLE_WHITE_SUPPORT)
		LEDLIGHT_setMireds_RGBW (COLOR_TEMPERATURE_CONVERT(kelvin), WHITE_CT);
		printf ("\t%5d Kelvin\tR=%4X/%2X\tG=%4X/%2X\tB=%4X/%2X\tC=%4X/%2X\r\n", kelvin
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff
			,g_ledChannel_WHITE.Value,g_ledChannel_WHITE.OnOff );

#elif (COLOR_CCT_SUPPORT) && (COLOR_RGB_SUPPORT)
		LEDLIGHT_setMireds_RGBCCT (COLOR_TEMPERATURE_CONVERT(kelvin), COLD_LIGHT_TEMPERATURE, WARM_LIGHT_TEMPERATURE);
		printf ("\t%5d Kelvin\tR=%4X/%2X\tG=%4X/%2X\tB=%4X/%2X\tC=%4X/%2X\tW=%4X/%2X\r\n", kelvin
			,  g_ledChannel_RED.Value,  g_ledChannel_RED.OnOff
			,g_ledChannel_GREEN.Value,g_ledChannel_GREEN.OnOff
			, g_ledChannel_BLUE.Value, g_ledChannel_BLUE.OnOff
			, g_ledChannel_COLD.Value, g_ledChannel_COLD.OnOff
			, g_ledChannel_WARM.Value, g_ledChannel_WARM.OnOff );
#endif

	}
}


int main(void){

	TEST_KELVIN();

	return 0;
}
