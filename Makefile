#Config this:
CFLAGS?=-O -pipe
CFLAGS+=-g

.if exists(.config)
.include ".config"
.else
config:
	${MAKE}
.endif

# It shouldn't be necessary to edit anything below this line.
PRG=brillion
OBJ=brillion.o graphics.o level.o physics.o play.o game.o effects.o save.o

INC=-IBFontv1.0.4-1
VPATH=BFontv1.0.4-1

.ifdef OPTIMIZE
CFLAGS=-O3 -ffast-math -fforce-addr -fomit-frame-pointer -pipe
.endif

# misses -pedantic (warns too much about system headers)
# and -ansi (errors out on C++-style comments)
.ifdef PEDANTIC
CFLAGS+= -D_POSIX_SOURCE -W -Wall -Wbad-function-cast -Wcast-align \
	-Wcast-qual -Wchar-subscripts -Winline \
	-Wmissing-prototypes -Wnested-externs -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings
.endif

CFLAGS+=-Wall `${SDL_CONFIG} --cflags` -IBFontv1.0.4-1
LDFLAGS+=`${SDL_CONFIG} --libs` -lSDL_image

.ifdef SOUND
CFLAGS+=-DSOUND
LDFLAGS+=-lSDL_mixer
OBJ+=music.o
.endif

.ifdef DMALLOC_OPTIONS
CFLAGS+=-I/usr/local/include/ -DDMALLOC
LDFLAGS+=-L/usr/local/lib/ -ldmalloc
.endif

.ifdef PROFILE
CFLAGS+=-pg
LDFLAGS+=-pg
LDFLAGS=-pg -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL-1.1_p -lc_r -lSDL_image
LDFLAGS+=-static -L/usr/X11R6/lib -lesd -laa -lncurses -lXext -lvga -lSDL-1.1 -lX11 -lpng -ltiff -lz -ljpeg -lm -lvgl -lusbhid
.endif

.if ${USER} == "sec"
all: tags
.endif

all: $(PRG)

$(PRG): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

clean:
	rm -f $(PRG) $(OBJ) *core

tags: brillion.c brillion.h graphics.c level.c physics.c play.c
	-ctags *.[ch]

install: $(PRG)
	cp $(PRG) /usr/X11R6/bin
	-mkdir /usr/X11R6/share/brillion
	cp -r Original /usr/X11R6/share/brillion

.depend: brillion.c brillion.h graphics.c level.c physics.c play.c game.c
	-$(CC) $(CFLAGS) -MM *.c>.depend

devel:	gnu tags .depend
depend: .depend

config: .config
.config: Makefile
	./config.sh

gnu: GNUmakefile
GNUmakefile: Makefile GNUify
	./GNUify
