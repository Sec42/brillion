/* crillion.h, Sec <sec@42.org>
 * vim:set cin sm ts=4 sw=4:
 * $Id: crillion.h,v 1.1 2002/10/14 13:51:36 sec Exp $
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

void dwim(void);

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
	int version;
	int x,y;
	int time;
	int blocks;
	int *pieces;
	int *colors;
	char desc[100];
} field;

typedef struct {
	int x,y;
	signed int dir;
	int color;
} ball;

typedef struct {
	SDL_Surface * wall;
	SDL_Surface * back;
	SDL_Surface * death;
	SDL_Surface * ball[MAX_COLORS];
	SDL_Surface * disk[MAX_COLORS];
	SDL_Surface * block[MAX_COLORS];
	SDL_Surface * star[MAX_COLORS];
} graphic_pieces;

