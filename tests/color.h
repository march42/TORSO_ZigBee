
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
