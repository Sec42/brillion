PRG=brillion
OBJ=brillion.o graphics.o level.o physics.o play.o

#Config this:
SDL_CONFIG?=sdl-config
CFLAGS=-g -O -pipe
#CFLAGS=-O3 -ffast-math -fforce-addr -fomit-frame-pointer -pipe

# Don't touch this.
CFLAGS+=-Wall `${SDL_CONFIG} --cflags`
LDFLAGS+=`${SDL_CONFIG} --libs` -lSDL_image # -lSDL_mixer

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

