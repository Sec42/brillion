# $Id: Makefile,v 1.33 2004/06/21 12:26:14 sec Exp $
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
OBJ=brillion.o graphics.o level.o physics.o play.o game.o effects.o \
	save.o font.o title.o score.o timer.o portab.o

.ifdef OPTIMIZE
CFLAGS=-O3 -ffast-math -fforce-addr -fomit-frame-pointer -pipe -DNDEBUG
.endif

# misses -pedantic (warns too much about system headers)
.ifdef PEDANTIC
#CFLAGS+= -D_POSIX_SOURCE -D_POSIX_C_SOURCE=2
CFLAGS+= -D_POSIX_C_SOURCE=199506L -pedantic
CFLAGS+= -ansi -W -Wall -Wcast-align \
	-Wbad-function-cast -Wcast-qual -Wchar-subscripts -Winline \
	-Wmissing-prototypes -Wnested-externs -Wpointer-arith \
	-Wredundant-decls -Wshadow -Wstrict-prototypes -Wwrite-strings
.endif

CFLAGS+=-Wall `${SDL_CONFIG} --cflags`
LDFLAGS+=`${SDL_CONFIG} --libs` -lSDL_image

.ifdef SOUND
CFLAGS+=-DSOUND
LDFLAGS+=-lSDL_mixer
OBJ+=music.o
.endif

.ifdef WINDOWS
OBJ+=res.o
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
	rm -f $(PRG) $(OBJ) *core Original/*core

tags: brillion.c brillion.h graphics.c level.c physics.c play.c
	-ctags *.[ch]

.ifdef WINDOWS
res.o: res.rc brillion.ico
	windres -i $< -o $@

# Requires the nullsoft installer: http://sourceforge.net/projects/nsis/
installer: $(PRG)
	strip brillion.exe
	perl -p -e s/%VER%/`date +%Y-%m-%d`/ brillion.nsi>now.nsi
	/cygdrive/c/Programme/NSIS/makensis now.nsi
	rm -f now.nsi

put:
	ncftpput upload.sourceforge.net incoming Brillion-Setup-`date +%Y-%m-%d`.exe
.else
install: $(PRG)
	strip $(PRG)
	cp $(PRG) /usr/X11R6/bin
	-mkdir /usr/X11R6/share/brillion
	cp -r Original /usr/X11R6/share/brillion
.endif

.depend: brillion.c brillion.h graphics.c level.c physics.c play.c game.c
	-$(CC) $(CFLAGS) -MM *.c>.depend

devel:	gnu tags .depend
depend: .depend

config: .config
.config: # Makefile
	./config.sh

gnu: GNUmakefile
GNUmakefile: Makefile GNUify
	./GNUify

.if exists(.depend)
.include ".depend"
.endif

