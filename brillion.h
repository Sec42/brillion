/* crillion.h, Sec <sec@42.org>
 * vim:set cin sm ts=8 sw=8:
 * $Id: brillion.h,v 1.21 2003/03/12 14:05:28 sec Exp $
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

#define die(x) do{fprintf(stderr,"DIEd: \"%s\" in %s line %d\n",x,__FILE__,__LINE__);exit(-1);}while(0)


#define BLACK          0
#define RED            1
#define CYAN           2
#define MAGENTA        3
#define GREEN          4
#define BLUE           5
#define YELLOW         6
#define GAME_COLORS    7

#define WHITE          7
#define BG             8
#define FG             9
#define NONE          10
#define MAX_COLORS    11


#define SPACE	0
#define BLOCK	1
#define DISK	2
#define	STAR	3
#define WALL	4
#define DEATH	5
#define OUTER_WALL 6
#define MAX_PIECE 7

enum animations {
	A_NONE,
	A_BALL,
	A_DISK,
	A_EXPLODE,
	A_DIE,
	A_TWINKLE
};

typedef struct {
	int x, y;
} coord;

#define MAX_ANIM 9

typedef struct {
	enum animations	type;
	int  color;
	int  duration;

	coord	block[2];
	coord	pixel[2];
} a_anim;

typedef struct {
	int version;    /* Version number. Always 42 */
	int w,h;        /* Width and height of the level */
	char desc[100]; /* Level name and author */

	int *pieces;	/* The physical level */
	int *colors;    /* The colors of different pieces */
#define COLOR(x,y) lvl->colors[(x)*lvl->h+(y)]
#define PIECE(x,y) lvl->pieces[(x)*lvl->h+(y)]

	signed int blocks;	/* Blocks remaining */
	int time; 	/* Time remaining */
	int x,y;	/* Ball position */
	signed int dir; /* Ball direction */
	int color;	/* Ball color */
	int ppb;	/* Points/Block */
} field;

typedef struct {
	SDL_Surface * display;

	/* UpdateRects list */
	SDL_Rect * rects;
	int numrects;
#define MAXRECTS 256 /* 164 or so for startup */
#define UPDATE(rect) do{assert(b->p->g->numrects<MAXRECTS);memcpy(b->p->g->rects+(b->p->g->numrects++),&rect,sizeof(SDL_Rect));}while(0)

#define DISPLAY do{SDL_UpdateRects(b->p->g->display,b->p->g->numrects,b->p->g->rects);b->p->g->numrects=0;}while(0)
// For development:
//#define DISPLAY do{SDL_Flip(b->p->g->display);b->p->g->numrects=0;}while(0)

	Uint32 colors[MAX_COLORS];

	/* For background */
//	int xoff, yoff;	/* Offset of the level area in main window */
	SDL_Rect level; /* Level Area */
	SDL_Surface * border;

	/* block graphics */
	SDL_Surface * wall;
	SDL_Surface * back;
	SDL_Surface * death;
	SDL_Surface * ball[GAME_COLORS];
	SDL_Surface * ballx[GAME_COLORS];
	SDL_Surface * disk[GAME_COLORS];
	SDL_Surface * block[GAME_COLORS];
	SDL_Surface * blockx[GAME_COLORS];
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
	char*  name;             /* Background graphic */
	coord  pts;            
	coord  bonus;
	coord  blocks;
	coord  lives;
	coord  lname;
	coord  field;
//	coord  fieldsz; // Just assume it matches :)
} a_layout;

typedef struct {
	a_layout* layout;	/* Background graphic */
	char*  	  name;		/* Filename of the level file */
	// block, sound, music currently fixed per game.
} a_level;

enum rec_type {
	R_NONE,
	R_PLAY,
	R_RECORD
};

typedef struct {
	enum rec_type what;
	char*	 game;
	int	 len;
	int	 pos;
} a_save;

typedef enum {
	S_PLAY,
	S_DIE,
	S_QUIT,
	S_FINISH
} status_t;

typedef struct {
	graphic* g;
	music*   m;
	field*   f;
	a_anim*	 a;
	int      anims;
	int	 lives;
	int	 points;
	status_t status;
	a_save*  s;
} a_play;

typedef struct {
	a_level** levels;
	int       maxlevel;
	int	  lives;
} a_game;

typedef struct {
	int	verbose;	// Debugging level
	char*	prog;		// Binary name
	char*	dir;		// Data directory

	// We can argue if they should be here...
	a_game*	game;		// Selected Game
	a_play*	p;		// Things needed for play
} the_status;

#ifndef EXTERN
#define EXTERN extern
#endif

/* ein (kleines) global */
EXTERN the_status* b;  // Das Spiel

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
void update_scoreboard(a_play* p);

/* play.c */
void play_game(a_game* game);

#ifdef SOUND
/* music.c */
music* init_music();
void play_touch(music* m,int piece);
#else
#define init_music() NULL
#define play_touch(a,b)
#endif

/* game.c */
a_game* read_game(char* file);

/* graphics.c (should be anim.c?) */
#define AFRAMES 4
a_anim* init_anim();
void animate(graphic*g, a_anim*a, int step);
void create_moveanim(enum animations type, int color, int ox, int oy, int nx, int ny);
void create_staticanim(enum animations type, int color, int x, int y);

/* These depend on you scheduler 20/10 (msec) seems a sane default */
#define SLEEP_MIN 20
#define SLEEP_GRAN 10

#define DELAY(left,end) do{ \
    if(left>SLEEP_MIN) \
	SDL_Delay(left-SLEEP_GRAN); \
    while(SDL_GetTicks()<end){ q++; }; \
}while(0)

/* effects.c */
typedef enum {
	HORIZ_IN,
	HORIZ_OUT,
	VERT_IN,
	VERT_OUT
} split_t;

void fade (SDL_Surface* s, Uint32 ticks, int fadein);
void split (SDL_Surface* s, SDL_Rect* r_in, Uint32 ticks, split_t type);

/* save.c */

a_save* init_save();
signed int handle_save(a_save* s,signed int dir);
void print_save(a_save *s);
