#define EXTERN extern
// #include "SDL_mixer.h"
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */
#define SPEED 60 /* Ticks per Frame */

#define MX 640
#define MY 480

int play_level(a_play* play);

void play_game(a_game* game){
	int		cur_lvl=0;
	int		old_lvl=999; // Just != cur_lvl
	a_play*	play;

	play=calloc(1,sizeof(a_play));
	b->p=play;

	// Graphic, Music&Sound static per game for now
	play->g=init_graphic();
	play->m=init_music();
	play->points=0;

	play->lives=game->lives;

	do{

		// Setup Level
		if(play->f){
			free(play->f->pieces); // XXX: should be abstract.
			free(play->f->colors);
			free(play->f);
		};
		play->f=read_level(game->levels[cur_lvl]->name);
		if(!play->f){
			die("Whoops, kein level?");
		};

		// Display for Startup
		SDL_BlitSurface(play->g->border, NULL, play->g->display, NULL);
		paint_level(play->g, play->f);
		if(cur_lvl!=old_lvl) // Fade in...
			fade (play->g->display, 1000, 1);
		old_lvl=cur_lvl;

		// Clear Animation list
		bzero(play->a,sizeof(play->a));

		switch (play_level(play)){
			case 0:
				play->lives--;
				break;
			case 1:
				play->points+=10*(play->f->time+1);
				cur_lvl++;
				break;
			case 2: /* Aborted */
				play->lives=0;
				break;
		};

		if(cur_lvl!=old_lvl){
			fade (play->g->display, 1000, 0); // Fade out...

		};

	}while(play->lives>0 && cur_lvl < game->maxlevel);

	if(play->lives>0)
		printf("Congrats, you made it through all levels alive!\n");

	printf("You accumulated %d points\n",play->points);

	// XXX: uninit_graphic/_music ?
	free(play);
	SDL_Quit();
};

int play_level(a_play* play){
  SDL_Event event;
  int userl=0;
  int userr=0;
  char quit=0;
  int q; // Number of frames displayed
  Uint32 now;
  Uint32 round;
  Sint32 round_d;
  Uint32 speed=0;

  graphic* g=play->g;
  music* m=play->m;
  field* lvl=play->f;
//  anim*	a=play->a;

  now = SDL_GetTicks();
  q=0;
  while(1){
    q++;
    round=SDL_GetTicks();

    move_step(g, m, lvl, userr-userl);
    if(q%2==0 && lvl->time>0)
      lvl->time--;
    update_scoreboard(play);

    DISPLAY;

    while( SDL_PollEvent( &event ) ){
      /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
      switch( event.type ){
	case SDL_KEYDOWN:
	  if( event.key.keysym.sym == SDLK_q){
	    quit=1; // Abort game
	  } else if (event.key.keysym.sym == SDLK_RIGHT){
	    userr=1;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userl=1;
	  } else if (event.key.keysym.sym == SDLK_ESCAPE){
	    lvl->blocks=-1; // Kill this life.
	  } else if (event.key.keysym.sym == SDLK_d){
	    dump_level(lvl);
	  } else if (event.key.keysym.sym == SDLK_s){
	    lvl->blocks=0; // XXX: Cheat ;-)
	  } else if (event.key.keysym.sym == SDLK_p){
	    snapshot(g);
	  };
	  break;

	case SDL_KEYUP:
	  if (event.key.keysym.sym == SDLK_RIGHT){
	    userr=0;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userl=0;
	  };
	  break;

	case SDL_QUIT: /* SDL_QUIT event (window close) */
	  quit = 1;
	  break;

	default:
	  break;
      };
    }

    speed+=SDL_GetTicks()-round;
    round_d=SPEED-(SDL_GetTicks()-round);
    if(round_d<0)
      printf("CPU to slow by %d ticks/round\n",-round_d);
    else
      SDL_Delay(round_d);

    if(quit || lvl->blocks<=0){
      fprintf(stderr,"%f Ticks/frame\n",(float)speed/q);
	  if(quit)
		  return(2);
	  if(lvl->blocks==0)
		  return(1);
      return(0);
    };
  };
};
