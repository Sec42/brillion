/* crillion.h, Sec <sec@42.org>
 * vim:set cin sm ts=8 sw=8:
 * $Id: brillion.h,v 1.7 2002/10/25 00:38:45 sec Exp $
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

#ifdef SOUND
#include <SDL_mixer.h>
#endif

#ifdef DMALLOC
#include <dmalloc.h>
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
#define OUTER_WALL 6
#define MAX_PIECE 7

typedef struct {
	char level[100];
	int onelevel;
} config;

typedef struct {
	int version;    /* Version number. Always 42 */
	int w,h;        /* Width and height of the level */
	char desc[100]; /* Level name and author */

	int *pieces;	/* The physical level */
	int *colors;    /* The colors of different pieces */
#define COLOR(x,y) lvl->colors[(x)*lvl->h+(y)]
#define PIECE(x,y) lvl->pieces[(x)*lvl->h+(y)]

	int blocks;	/* Blocks remaining */
	int time; 	/* Time remaining */
	int x,y;	/* Ball position */
	signed int dir; /* Ball direction */
	int color;	/* Ball color */
} field;

typedef struct {
	SDL_Surface * display;
	Uint32 colors[MAX_COLORS];
	int xoff, yoff;	/* Offset of the level area in main window */

	/* block graphics */
	SDL_Surface * wall;
	SDL_Surface * back;
	SDL_Surface * death;
	SDL_Surface * ball[GAME_COLORS];
	SDL_Surface * disk[GAME_COLORS];
	SDL_Surface * block[GAME_COLORS];
	SDL_Surface * star[GAME_COLORS];

	/* font */
	SDL_Surface * font;
} graphic;

typedef struct {
#ifdef SOUND
	Mix_Music* bg;

	/* touch sounds */
	Mix_Chunk* wall;
	Mix_Chunk* death;
	Mix_Chunk* disk;
	Mix_Chunk* block;
	Mix_Chunk* star;
#else
#endif
} music;

typedef struct {
	int x, y;
} coord;

typedef struct {
	char*  gfx[MAX_PIECE]; /* Block Graphics */
	char*  sfx[MAX_PIECE]; /* Soundbites */
	char*  bg;             /* Background graphic */
	char*  txtfont;        /* Font for level name */
	char*  numfont;        /* Font for digits */
	char*  bgsound;        /* Music during title */
	char*  level;          /* Filename of the level file */
	coord  pts;            
	coord  bonus;
	coord  blocks;
	coord  lives;
	coord  name;
	coord  field;
//	coord  fieldsz; // Just assume it matches :)
} desc;

typedef struct {
	graphic* g;
	music*   m;
	field*   f;
} play;

typedef struct {
	desc** levels;
	int    maxlevel;
} gameing;


/* physics.c */
void move_step(graphic* g, music* m,field* lvl, signed int input);
int move_touch(graphic* g, music* m,field* lvl, int x,int y,signed int dx,signed int dy);

/* level.c */
field* read_level(char * file);
void dump_level(field * lvl);

/* graphics.c */
graphic* init_graphic();
void paint_level(graphic* g, field* lvl);
void paint_block(graphic* g, field* lvl, int x, int y);
void paint_ball(graphic* g, field* lvl);
void snapshot(graphic* g);
void fade (SDL_Surface* s, Uint32 ticks, int fadein);
void update_scoreboard(graphic* g, field* lvl);

/* play.c */
void game(config* cfg);

#ifdef SOUND
/* music.c */
music* init_music();
void play_touch(music* m,int piece);
#else
#define init_music() NULL
#define play_touch(a,b)
#endif
