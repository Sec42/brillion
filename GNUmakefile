#
#This file is autogenerated, do not edit!
#

#Config this:
SDL_CONFIG?=sdl-config
CFLAGS?=-g -O -pipe

# It shouldn't be necessary to edit anything below this line.
PRG=brillion
OBJ=brillion.o graphics.o level.o physics.o play.o

ifdef OPTIMIZE
CFLAGS=-O3 -ffast-math -fforce-addr -fomit-frame-pointer -pipe
endif

CFLAGS+=-Wall `${SDL_CONFIG} --cflags`
LDFLAGS+=`${SDL_CONFIG} --libs` -lSDL_image # -lSDL_mixer

ifdef DMALLOC_OPTIONS
CFLAGS+=-I/usr/local/include/ -DDMALLOC
LDFLAGS+=-L/usr/local/lib/ -ldmalloc
endif

ifdef PROFILE
CFLAGS+=-pg
LDFLAGS+=-pg
LDFLAGS+=-static -L/usr/X11R6/lib -lesd -laa -lncurses -lXext -lvga -lSDL-1.1 -lX11 -lpng -ltiff -lz -ljpeg -lm
endif

all: gnu $(PRG)

$(PRG): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

clean:
	rm -f $(PRG) $(OBJ) *core

tags:
	ctags *.[ch]

install:
	@echo You wish.

gnu: GNUmakefile

GNUmakefile: Makefile
	@(echo "#";\
	echo '#This file is autogenerated, do not edit!';\
	echo "#";echo;sed 's/^\.//' <Makefile) >$@
