
#ifndef __COLOR_H__
#	define __COLOR_H__

/*	test specifications */
#ifndef EXTENDED_COLOR_LIGHT
#	define EXTENDED_COLOR_LIGHT			0
#endif
#ifndef COLOR_RGB_SUPPORT
#	define COLOR_RGB_SUPPORT			0
#endif
#ifndef COLOR_CCT_SUPPORT
#	define COLOR_CCT_SUPPORT			0
#endif
#ifndef SINGLE_WHITE_SUPPORT
#	define SINGLE_WHITE_SUPPORT			0
#endif
#if (!COLOR_RGB_SUPPORT) && (!COLOR_CCT_SUPPORT) && (!SINGLE_WHITE_SUPPORT)
#	error	You must define COLOR_RGB_SUPPORT and/or COLOR_CCT_SUPPORT and/or SINGLE_WHITE_SUPPORT !!!
#endif /*  */

/* type declarations */
typedef   signed char	s8;
typedef unsigned char	u8;
typedef   signed short	s16;
typedef unsigned short	u16;
typedef   signed int	s32;
typedef unsigned int	u32;

/*	constants for PWM handling */
#	ifndef CLOCK_SYS_CLOCK_HZ
#		define CLOCK_SYS_CLOCK_HZ			48000000
#	endif
#	ifndef PWM_FREQUENCY
#		define PWM_FREQUENCY				6000
#	endif
#	ifndef PWM_MAX_TICK
#		define PWM_MAX_TICK					(CLOCK_SYS_CLOCK_HZ / PWM_FREQUENCY)
#	endif /* PWM_MAX_TICK */
/*	constants for ZigBee attributes */
#	define ZCL_LEVEL_ATTR_MAX_LEVEL			0xFE
#	define ZCL_COLOR_ATTR_HUE_MAX			0xFE
#	define ZCL_COLOR_ATTR_SATURATION_MAX	0xFE
#	define ZCL_COLOR_ATTR_ENHANCED_HUE_MAX	0xFFFF
#	define ZCL_COLOR_ATTR_XY_MAX			0xFEFF
/*	our own */
#	define COLORCHANNEL_MAX 				PWM_MAX_TICK
#	define COLORONOFF_MAX					100


const unsigned char _GAMMA_CORRECTION_LEVEL   [] = {	/* float lvl = powf(((float)pct / 100), 2.4) * 254 + 1; */
	/*  1*/ 0x01,   /*  2*/ 0x01,   /*  3*/ 0x01,   /*  4*/ 0x01,   /*  5*/ 0x01,   /*  6*/ 0x01,   /*  7*/ 0x01,   /*  8*/ 0x01,   /*  9*/ 0x01,   /* 10*/ 0x02,
	/* 11*/ 0x02,   /* 12*/ 0x02,   /* 13*/ 0x02,   /* 14*/ 0x03,   /* 15*/ 0x03,   /* 16*/ 0x04,   /* 17*/ 0x04,   /* 18*/ 0x05,   /* 19*/ 0x05,   /* 20*/ 0x06,
	/* 21*/ 0x07,   /* 22*/ 0x07,   /* 23*/ 0x08,   /* 24*/ 0x09,   /* 25*/ 0x0A,   /* 26*/ 0x0B,   /* 27*/ 0x0B,   /* 28*/ 0x0C,   /* 29*/ 0x0E,   /* 30*/ 0x0F,
	/* 31*/ 0x10,   /* 32*/ 0x11,   /* 33*/ 0x12,   /* 34*/ 0x14,   /* 35*/ 0x15,   /* 36*/ 0x16,   /* 37*/ 0x18,   /* 38*/ 0x19,   /* 39*/ 0x1B,   /* 40*/ 0x1D,
	/* 41*/ 0x1E,   /* 42*/ 0x20,   /* 43*/ 0x22,   /* 44*/ 0x24,   /* 45*/ 0x26,   /* 46*/ 0x28,   /* 47*/ 0x2A,   /* 48*/ 0x2C,   /* 49*/ 0x2E,   /* 50*/ 0x31,
	/* 51*/ 0x33,   /* 52*/ 0x35,   /* 53*/ 0x38,   /* 54*/ 0x3A,   /* 55*/ 0x3D,   /* 56*/ 0x40,   /* 57*/ 0x42,   /* 58*/ 0x45,   /* 59*/ 0x48,   /* 60*/ 0x4B,
	/* 61*/ 0x4E,   /* 62*/ 0x51,   /* 63*/ 0x54,   /* 64*/ 0x58,   /* 65*/ 0x5B,   /* 66*/ 0x5E,   /* 67*/ 0x62,   /* 68*/ 0x65,   /* 69*/ 0x69,   /* 70*/ 0x6C,
	/* 71*/ 0x70,   /* 72*/ 0x74,   /* 73*/ 0x78,   /* 74*/ 0x7C,   /* 75*/ 0x80,   /* 76*/ 0x84,   /* 77*/ 0x88,   /* 78*/ 0x8C,   /* 79*/ 0x91,   /* 80*/ 0x95,
	/* 81*/ 0x9A,   /* 82*/ 0x9E,   /* 83*/ 0xA3,   /* 84*/ 0xA8,   /* 85*/ 0xAC,   /* 86*/ 0xB1,   /* 87*/ 0xB6,   /* 88*/ 0xBB,   /* 89*/ 0xC1,   /* 90*/ 0xC6,
	/* 91*/ 0xCB,   /* 92*/ 0xD0,   /* 93*/ 0xD6,   /* 94*/ 0xDB,   /* 95*/ 0xE1,   /* 96*/ 0xE7,   /* 97*/ 0xED,   /* 98*/ 0xF2,   /* 99*/ 0xF8,   /*100*/ 0xFF,
};

/*	color temperature handling */
#	define COLOR_TEMPERATURE_CONVERT(kelvin_mired)	(1000000 / kelvin_mired)
#	define MIRED(kelvin)				(1000000 / kelvin)
#	define COLOR_TEMPERATURE_NEUTRAL	COLOR_TEMPERATURE_CONVERT(3800)
#	define WHITE_CT						COLOR_TEMPERATURE_NEUTRAL
#	define COLD_LIGHT_TEMPERATURE		COLOR_TEMPERATURE_CONVERT(6500)
#	define WARM_LIGHT_TEMPERATURE		COLOR_TEMPERATURE_CONVERT(2200)

/*	LED channel handling */
typedef struct {
	u16		Value;		// current value in 16 bit precision
	s8		OnOff;		// state On/Off (-1==undefined, 0==off, 1...100==percentage)
}	ts_LED_Channel;

/*	LED channel variables and inline functions */
#	if (COLOR_CCT_SUPPORT) || (SINGLE_WHITE_SUPPORT)
		ts_LED_Channel		g_ledChannel_COLD;
#		define __LED_COLD_SETVALUE(v,f)				do { g_ledChannel_COLD.Value	= ((v==0 || f==COLORCHANNEL_MAX) ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
#		define __LED_COLD_SETONOFF(v,f)				do { g_ledChannel_COLD.OnOff	= ((v==0 || f==  COLORONOFF_MAX) ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		if (COLORCHANNEL_MAX==PWM_MAX_TICK)
#			define __LED_COLD_PWMCOUNT				((s32)(g_ledChannel_COLD.Value * g_ledChannel_COLD.OnOff                                   / COLORONOFF_MAX))
#		else
#			define __LED_COLD_PWMCOUNT				((s32)(g_ledChannel_COLD.Value * g_ledChannel_COLD.OnOff * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#		endif
#		if (!SINGLE_WHITE_SUPPORT)
			ts_LED_Channel		g_ledChannel_WARM;
#			define __LED_WARM_SETONOFF(v,f)			do { g_ledChannel_WARM.OnOff	= ((v==0 || f==  COLORONOFF_MAX) ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#			define __LED_WARM_SETVALUE(v,f)			do { g_ledChannel_WARM.Value	= ((v==0 || f==COLORCHANNEL_MAX) ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
#			if (COLORCHANNEL_MAX==PWM_MAX_TICK)
#				define __LED_WARM_PWMCOUNT			((s32)(g_ledChannel_WARM.Value * g_ledChannel_WARM.OnOff *                                 / COLORONOFF_MAX))
#			else
#				define __LED_WARM_PWMCOUNT			((s32)(g_ledChannel_WARM.Value * g_ledChannel_WARM.OnOff * PWM_MAX_TICK / COLORCHANNEL_MAX / COLORONOFF_MAX))
#			endif
#		endif
#	endif
#	if (COLOR_RGB_SUPPORT)
		ts_LED_Channel		g_ledChannel_RED;
#		define __LED_RED_SETONOFF(v,f)				do { g_ledChannel_RED.OnOff   = ((v==0 || f==  COLORONOFF_MAX) ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_RED_SETVALUE(v,f)				do { g_ledChannel_RED.Value   = ((v==0 || f==COLORCHANNEL_MAX) ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
		ts_LED_Channel		g_ledChannel_GREEN;
#		define __LED_GREEN_SETONOFF(v,f)			do { g_ledChannel_GREEN.OnOff = ((v==0 || f==  COLORONOFF_MAX) ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_GREEN_SETVALUE(v,f)			do { g_ledChannel_GREEN.Value = ((v==0 || f==COLORCHANNEL_MAX) ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
		ts_LED_Channel		g_ledChannel_BLUE;
#		define __LED_BLUE_SETONOFF(v,f)				do { g_ledChannel_BLUE.OnOff  = ((v==0 || f==  COLORONOFF_MAX) ?v :(  COLORONOFF_MAX * v / f)); } while (0);
#		define __LED_BLUE_SETVALUE(v,f)				do { g_ledChannel_BLUE.Value  = ((v==0 || f==COLORCHANNEL_MAX) ?v :(COLORCHANNEL_MAX * v / f)); } while (0);
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

#	define RGB_LIGHTNESS(R,G,B)			(((R << 1) + R + (G << 2) + B) >> 3)

#endif /* __COLOR_H__ */
