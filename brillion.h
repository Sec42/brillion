/* The all-in-one Header file
 * vim:set cin sm ts=8 sw=8 sts=4: Sec <sec@42.org>
 * $Id: brillion.h,v 1.35 2003/12/12 02:29:42 sec Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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

/* Colors */
#define BLACK	     0
#define RED	     1
#define CYAN	     2
#define MAGENTA	     3
#define GREEN        4
#define BLUE         5
#define YELLOW       6
#define GAME_COLORS  7

#define WHITE        7
#define BG           8
#define FG           9
#define NONE        10
#define MAX_COLORS  11

/* Block Types */
#define SPACE	     0
#define BLOCK	     1
#define DISK	     2
#define	STAR	     3
#define WALL	     4
#define DEATH	     5
#define OUTER_WALL   6
#define MAX_PIECE    7

typedef enum {
	A_NONE,
	A_BALL,
	A_DISK,
	A_EXPLODE,
	A_DIE,
	A_TWINKLE
} anim_t;

typedef struct {
	int x, y;
} coord;

#define MAX_ANIM 9

typedef struct {
	anim_t	    type;
	int	    color;
	int	    duration;

	coord	    block[2];
	coord	    pixel[2];
} a_anim;

typedef struct {
	int	    version;    /* Version number. Always 42 */
	int	    w,h;        /* Width and height of the level */
	char	    desc[100];	/* Level name and author */

	int	    *pieces;	/* The physical level */
	int	    *colors;    /* The colors of different pieces */
#define COLOR(x,y) lvl->colors[(x)*lvl->h+(y)]
#define PIECE(x,y) lvl->pieces[(x)*lvl->h+(y)]

	int	    blocks;	/* Blocks remaining */
	int	    time; 	/* Time remaining */
	int	    x,y;	/* Ball position */
	signed int  dir;	/* Ball direction */
	int	    color;	/* Ball color */
	int	    ppb;	/* Points/Block */
} field;

#define FONTBEG 33
#define FONTLEN (255-FONTBEG)

typedef struct {
    int beg[FONTLEN+1];
    int wid[FONTLEN+1];
    int h;
    int len;
    int space;
    int lineh;
    SDL_Surface *font;
} a_font;

typedef struct {
	SDL_Surface *display;

	/* UpdateRects list */
	SDL_Rect    *rects;
	int	    numrects;

	Uint32	colors[MAX_COLORS];

	/* For background */
	/*int	    xoff, yoff;*//* Offset of the level area in main window */
	SDL_Rect    level; /* Level Area */
	SDL_Surface *border;

	/* block graphics */
	SDL_Surface *wall;
	SDL_Surface *back;
	SDL_Surface *death;
	SDL_Surface *ball[GAME_COLORS];
	SDL_Surface *ballx[GAME_COLORS];
	SDL_Surface *disk[GAME_COLORS];
	SDL_Surface *block[GAME_COLORS];
	SDL_Surface *blockx[GAME_COLORS];
	SDL_Surface *star[GAME_COLORS];
	SDL_Color   *palette;
	int	    ncolors;

	/* font */
	SDL_Surface *font; /* numbers */
	a_font	    *tfont; /* text */
} graphic;

#define MAXRECTS 256    /* 164 or so for startup */
#define UPDATE(rect)	do{ \
	assert(g->numrects<MAXRECTS); \
	memcpy(g->rects+(g->numrects++),&rect,sizeof(SDL_Rect)); \
}while(0)

#define DISPLAY do{ \
	SDL_UpdateRects(g->display,g->numrects,g->rects); \
	g->numrects=0; \
}while(0)

/* For development: */
/*#define DISPLAY do{SDL_Flip(g->display);g->numrects=0;}while(0) */


typedef struct {
#ifdef SOUND
	Mix_Music   *bg;

	/* touch sounds */
	Mix_Chunk   *wall;
	Mix_Chunk   *death;
	Mix_Chunk   *disk;
	Mix_Chunk   *block;
	Mix_Chunk    *star;
#else
	int	    nothing;	/* To silence compiler warning */
#endif
} music;

typedef struct {
	char	    *name;	/* Background graphic */
	coord	    pts;            
	coord	    time;
	coord	    blocks;
	coord	    lives;
	coord	    level;
	coord	    lname;
	coord	    field;
	/*coord   fieldsz;	// Just assume it matches :) */
} a_layout;

typedef struct { /* block, sound, music currently fixed per game. */
	char	    *name;	/* Filename of the level file */
} a_level;

typedef enum {
	R_NONE,
	R_PLAY,
	R_RECORD
} rec_t;

typedef struct {
	rec_t	    what;
	char	    *game;
	int	    len;
	int	    pos;
} a_save;

typedef enum {
	S_PLAY,
	S_DIE,
	S_QUIT,
	S_FINISH
} status_t;

typedef struct {
	graphic	    *g;
	music	    *m;
	field	    *f;
	a_anim	    *a;
	int	    anims;
	int	    lives;
	int	    points;
	int	    level;
	status_t    status;
	a_save	    *s;
	a_layout    *layout;
} a_play;

#define MAX_SCORES 10
#define SCORENAMELEN 8

typedef struct {
	char    name[SCORENAMELEN];
	int     score;
	time_t  when;
	int     howlong;
/*	char    flags[8];  */
/*	int     csum;      */
} a_score;

typedef struct {
	a_score scores[MAX_SCORES];
	int     maxscore;
} the_scores;


typedef struct {
	a_level	    **levels;
	int	    maxlevel;
	int	    lives;
	a_layout    *layout;
	the_scores  *scores;
} a_game;

typedef struct {
	int	    verbose;	/* Debugging level */
	const char  *prog;	/* Binary name */
	char	    *dir;	/* Data directory */

	/* We can argue if they should be here... */
	a_game	    *game;	/* (constant) things from selected game */
} the_status;

#ifndef EXTERN
#define EXTERN extern
#endif

/* a (small) global */
EXTERN the_status   *b;  /* The game */
EXTERN a_play	    *play; /* we are currently playing this */


/* Function Prototypes follow here */

/* physics.c */
void move_step(signed int input);
int move_touch(int x,int y,signed int dx,signed int dy);

/* level.c */
field* read_level(char *file);
void free_level(field *lvl);
void dump_level(field *lvl);

/* graphics.c */
graphic* init_graphic(void);
void paint_level(void);
void paint_block(int x, int y);
void paint_ball(void);
void snapshot(void);
void update_scoreboard(void);
void clear_number_cache(void);

/* play.c */
void play_game(a_game* game);

#ifdef SOUND
/* music.c */
music* init_music(void);
void play_touch(int piece);
#else
#define init_music() NULL
#define play_touch(a)
#endif

/* game.c */
a_game* read_game(const char* file);

/* graphics.c (should be anim.c?) */
#define AFRAMES 4
a_anim* init_anim(void);
void animate(int step);
void create_moveanim(anim_t type, int color, int ox, int oy, int nx, int ny);
void create_staticanim(anim_t type, int color, int x, int y);

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
a_save* init_save(void);
signed int handle_save(signed int dir);
void print_save(a_save *s);

/* font.c */
a_font* init_font(const char *file);
void render_font(int x, int y,const char *txt);
SDL_Rect *render_text(int x, int y,const char *txt,a_font *f,int maxw);
void size_text(SDL_Rect *sr,const char *txt,a_font *f);
char *input_text(SDL_Rect *in, a_font *f);

int numwidth(a_font *f);
void render_num(int x, int y, int w,int num,a_font *f);
void render_fix(int x, int y, int w,const char *txt,a_font *f);
void nice_render_text(SDL_Rect *rv,const char *txt,a_font *f,int flags);

/* score.c */
the_scores* read_scores(void);
void write_scores(void);
void display_scores(void);
void add_score(void);

/* title.c */
void display_title(int oldscore);

/* timer.c */
#define SDL_USER_NOTHING     0
#define SDL_USER_ENDOFSCORES 1
#define SDL_USER_BLINK       2
void init_timer(void);
void time_event(unsigned int centiseconds,int eventtype);
