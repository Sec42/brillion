PRG=crillion
OBJ=crillion.o board.o

#Config this:
SDL_CONFIG?=sdl-config
CFLAGS=-g -pipe
#CFLAGS=-O3 -ffast-math -fforce-addr -fomit-frame-pointer -pipe

# Don't touch this.
CFLAGS+=-Wall `${SDL_CONFIG} --cflags`
LDFLAGS+=`${SDL_CONFIG} --libs` -lSDL_image -lSDL_mixer

.if(defined(DMALLOC_OPTIONS))
CFLAGS+=-I/usr/local/include/ -DDMALLOC
LDFLAGS+=-L/usr/local/lib/ -ldmalloc
.endif

.if(defined(PROFILE))
CFLAGS+=-pg
LDFLAGS+=-pg
LDFLAGS+=-static -L/usr/X11R6/lib -lesd -laa -lncurses -lXext -lvga -lm -lSDL-1.1 -lX11
.endif

all: $(PRG)

$(PRG): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

clean:
	rm -f $(PRG) $(OBJ) *core

tags:
	ctags *.[ch]

#LDFLAGS=-L/usr/local/lib -Wl,-rpath,/usr/local/lib -lSDL-1.1 -pthread -lm -L/usr/local/lib -lesd -laudiofile -lm -L/usr/X11R6/lib -lX11 -lXext -lXxf86vm -lXxf86dga -lXv -lXinerama -lvga -laa
# DO NOT DELETE

board.o: crillion.h
crillion.o: crillion.h
crillion: Makefile
