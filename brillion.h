/* crillion.h, Sec <sec@42.org>
 * vim:set cin sm ts=4 sw=4:
 * $Id: brillion.h,v 1.2 2002/10/14 18:23:39 sec Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <SDL.h>

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/* zwei kleine globals */
EXTERN char *prog;    // Programmname
EXTERN char verbose;  // Debugging-Level

#define GAME_COLORS    7

#define BLACK          0
#define BLUE           1
#define MAGENTA        2
#define RED            3
#define GREEN          4
#define YELLOW         5
#define CYAN           6
#define WHITE          7
#define NONE           8
#define MAX_COLORS     9


#define SPACE	0
#define BLOCK	1
#define DISK	2
#define	STAR	3
#define WALL	4
#define DEATH	5

typedef struct {
	int version;    /* Version number. Always 42 */
	int w,h;        /* Width and height of the level */
	char desc[100]; /* Level name and author */

	int *pieces;	/* The physical level */
	int *colors;    /* The colors of different pieces */
#define COLOR(x,y) lvl->colors[(x)*lvl->w+(y)]
#define PIECE(x,y) lvl->pieces[(x)*lvl->w+(y)]

	int blocks;		/* Blocks remaining */
	int time;	 	/* Time remaining */
	int x,y;		/* Ball position */
	signed int dir; /* Ball direction */
	int color;		/* Ball color */
} field;

typedef struct {
	SDL_Surface * display;
	Uint32 colors[MAX_COLORS];

	/* block graphics */
	SDL_Surface * wall;
	SDL_Surface * back;
	SDL_Surface * death;
	SDL_Surface * ball[GAME_COLORS];
	SDL_Surface * disk[GAME_COLORS];
	SDL_Surface * block[GAME_COLORS];
	SDL_Surface * star[GAME_COLORS];
} graphic;

/* physics.c */
void move_step(graphic* g, field* lvl, signed int input);
int move_touch(graphic* g, field* lvl, int x,int y,signed int dx,signed int dy);

/* level.c */
field* read_level(char * file);
void dump_level(field * lvl);

/* graphics.c */
graphic* init_graphic();
void paint_level(graphic* g, field* lvl);
void paint_block(graphic* g, field* lvl, int x, int y);
void paint_ball(graphic* g, field* lvl);
void snapshot(graphic* g);

/* play.c */
void game();
