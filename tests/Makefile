
CC			?= gcc
CFLAGS		+= -Werror
#CPPFLAGS	+= -I$(TC32DIR)/tc32-elf/include -I$(TC32DIR)/include
#LDFLAGS	+= -L$(TC32DIR)/tc32-elf/lib -L$(TC32DIR)/lib
LDLIBS		+= -lc
LDLIBS		+= -lg

#	floating point and math functions
CPPFLAGS		+= -D__USE_FLOATING_POINT=1 -D__PRINTF_FLOAT=1
LDLIBS			+= -lm

#	ZigBee extended color light functionality
EXTENDED_COLOR_LIGHT	?= 0
lighttype	+= -DEXTENDED_COLOR_LIGHT=$(EXTENDED_COLOR_LIGHT)
#	different color lights
COLOR_RGB_SUPPORT		?= 0
COLOR_CCT_SUPPORT		?= 0
SINGLE_WHITE_SUPPORT	?= 0
ifeq ($(COLOR_RGB_SUPPORT)$(COLOR_CCT_SUPPORT)$(SINGLE_WHITE_SUPPORT),000)
	COLOR_RGB_SUPPORT	= 1
	COLOR_CCT_SUPPORT	= 1
endif
lighttype	+= -DCOLOR_RGB_SUPPORT=$(COLOR_RGB_SUPPORT)
lighttype	+= -DCOLOR_CCT_SUPPORT=$(COLOR_CCT_SUPPORT)
lighttype	+= -DSINGLE_WHITE_SUPPORT=$(SINGLE_WHITE_SUPPORT)
#	channel settings
light_RGB		= -DCOLOR_RGB_SUPPORT=1 -DCOLOR_CCT_SUPPORT=0 -DSINGLE_WHITE_SUPPORT=0
light_RGBW		= -DCOLOR_RGB_SUPPORT=1 -DCOLOR_CCT_SUPPORT=0 -DSINGLE_WHITE_SUPPORT=1
light_RGBCCT	= -DCOLOR_RGB_SUPPORT=1 -DCOLOR_CCT_SUPPORT=1 -DSINGLE_WHITE_SUPPORT=0
light_CCT		= -DCOLOR_RGB_SUPPORT=0 -DCOLOR_CCT_SUPPORT=1 -DSINGLE_WHITE_SUPPORT=0

BUILDDIR	?= ../build/tests
SOURCES		+= $(wildcard ./*.c)
HEADERS		+= $(wildcard ./*.h)
PROGRAMS	+= $(subst ./,$(BUILDDIR)/,$(SOURCES:.c=))
EXTRAS		+= $(subst ./,$(BUILDDIR)/,$(SOURCES:.c=-RGB))
EXTRAS		+= $(subst ./,$(BUILDDIR)/,$(SOURCES:.c=-RGBW))
EXTRAS		+= $(subst ./,$(BUILDDIR)/,$(SOURCES:.c=-RGBCCT))
EXTRAS		+= $(subst ./,$(BUILDDIR)/,$(SOURCES:.c=-CCT))

.PHONY: all extras info clean allclean distclean
# doesn't work	.DEFAULT: all
# the first rule will be used if no specified
all: info $(OBJS) $(PROGRAMS)

extras: $(HEADERS) $(EXTRAS)

info:	$(SOURCES)
	$(info build_info)
	$(info SOURCES 	$(SOURCES))
	$(info HEADERS 	$(HEADERS))
	$(info PROGRAMS	$(PROGRAMS))

clean:
	$(info removing built objects)
	rm -f $(OBJS) $(PROGRAMS)

allclean:
	$(info removing built dir)
	rm -fR $(BUILDDIR)

distclean: clean
	$(info removing ALL built objects)
	rm -f $(OBJS) $(PROGRAMS)
	rm -fR $(BUILDDIR)

$(BUILDDIR)/%: %.c $(HEADERS)
	@mkdir -p $(@D)
	$(info building $@)
	$(CC) $(CPPFLAGS) $(lighttype) $(CFLAGS) $< $(LDLIBS) -o $@

$(BUILDDIR)/%-RGB: %.c $(BUILDDIR)/%
	@mkdir -p $(@D)
	$(info building $@)
	$(CC) $(CPPFLAGS) $(light_RGB) $(CFLAGS) $< $(LDLIBS) -o $@

$(BUILDDIR)/%-RGBW: %.c $(BUILDDIR)/%
	@mkdir -p $(@D)
	$(info building $@)
	$(CC) $(CPPFLAGS) $(light_RGBW) $(CFLAGS) $< $(LDLIBS) -o $@

$(BUILDDIR)/%-RGBCCT: %.c $(BUILDDIR)/%
	@mkdir -p $(@D)
	$(info building $@)
	$(CC) $(CPPFLAGS) $(light_RGBCCT) $(CFLAGS) $< $(LDLIBS) -o $@

$(BUILDDIR)/%-CCT: %.c $(BUILDDIR)/%
	@mkdir -p $(@D)
	$(info building $@)
	$(CC) $(CPPFLAGS) $(light_CCT) $(CFLAGS) $< $(LDLIBS) -o $@
