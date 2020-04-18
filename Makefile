# $Id: Makefile,v 1.36 2005/12/08 12:38:48 sec Exp $
#Config this:
CFLAGS?=-O -pipe
CFLAGS+=-Wall

# And perhaps this
PREFIX?=/usr/X11R6
BINDIR?=${PREFIX}/bin
DATADIR?=${PREFIX}/share/brillion

DATE=`date +%Y-%m-%d`

.if exists(.config)
.include ".config"
.else
config:
	${MAKE}
.endif

.if exists(.version)
.include ".version"
.endif

# It shouldn't be necessary to edit anything below this line.
PRG=brillion
OBJ=brillion.o graphics.o level.o physics.o play.o game.o effects.o \
	save.o font.o title.o score.o timer.o portab.o
SRCS=brillion.c graphics.c level.c physics.c play.c game.c effects.c \
	save.c font.c title.c score.c timer.c portab.c

.ifdef OPTIMIZE
CFLAGS=-O3 -ffast-math -fforce-addr -fomit-frame-pointer -pipe -DNDEBUG
.endif

.ifdef DEVEL
CFLAGS+=-g -DDEVEL
VERS?=CVS
VERS+=(devel)
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

.ifdef SOUND
CFLAGS+=-DSOUND
LDFLAGS+=-lSDL_mixer
OBJ+=music.o
.endif
SRCS+=music.c

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

CFLAGS+=`${SDL_CONFIG} --cflags`
LDFLAGS+=`${SDL_CONFIG} --libs`

CFLAGS+= -DBDATADIR=${DATADIR}
LDFLAGS+=-lSDL_image
.ifdef VERS
CFLAGS+= -DPROG_VERSION="\"${VERS}\""
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
installer: getlibs release clean $(PRG)
	strip brillion.exe
	echo Bscores 42>Original/Scores
	echo score Sec 72241 1559345943 596>>Original/Scores
	perl -p -e s/%VER%/$(DATE)/ brillion.nsi>now.nsi
	makensis now.nsi
	rm -f now.nsi

DLLDIR=/mingw64/bin
getlibs:
	cp ${DLLDIR}/*.dll .

winput:
	ncftpput upload.sourceforge.net incoming Brillion-Setup-$(DATE).exe
put: winput
.else
install: $(PRG)
	strip $(PRG)
	cp $(PRG) ${BINDIR}
	-mkdir ${DATADIR}
	cp -r Original ${DATADIR}
.endif

archive: release
	-rm -rf brillion-$(DATE)
	mkdir brillion-$(DATE)
	cp -pr INSTALL THANKS Makefile $(SRCS) brillion.h brillion.ico brillion.nsi res.rc GNUify .version config.sh GNUmakefile Original/ brillion-$(DATE)
	tar --exclude Original/Scores --exclude CVS -cvzf brillion-$(DATE).tgz brillion-$(DATE)
	rm -rf brillion-$(DATE)

release:
	echo VERS=${DATE} >.version

put:
	ncftpput upload.sourceforge.net incoming brillion-$(DATE).tgz

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

