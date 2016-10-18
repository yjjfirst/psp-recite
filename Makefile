TARGET = recite
OBJS = src/main.o \
	   src/display.o \
	   src/fat.o \
	   src/ctrl.o \
	   src/charsets.o \
	   src/mp3.o \
	   src/mainwin.o \
	   src/autowin.o \
	   src/wordlist.o \
	   src/stack.o \
	   src/wordlib.o \
	   src/wordlib-gre.o \
	   src/wordlib-cet4.o \
	   src/window.o \
	   src/ttf.o \
	   src/gbk2uni.o \
	   src/libsel-win.o \
	   src/wordlib-cet6.o \
	   src/recite-timer.o \
	   src/config.o \
	   src/symfont.o \
	   common/log.o \
	   common/utils.o


INCDIR = ./include/freetype2
CFLAGS = -O2 -G0 -Wall -D_DEBUG
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= ./lib/libfreetype.a ./lib/unzip.a ./lib/libmad.a \
	  ./lib/libid3tag.a ./lib/libz.a -lpspgum -lpspgu -lm -lpspaudio
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = PSP Recite
PSP_EBOOT_ICON = ./images/ICON0.png


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
