#define EXTERN extern
// #include "SDL_mixer.h"
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */
#define SPEED 60 /* Ticks per Frame */

#define MX 640
#define MY 480
#define MZ 4

void game(config* c){
  SDL_Event event;
  signed int userx=0;
  char quit=0;
  int q; // Number of frames displayed
  Uint32 now;
  Uint32 round;
  Sint32 round_d;
  Uint32 speed=0;

  graphic* g;
  music* m;
  field* lvl;

  if(c->onelevel)
	  lvl=read_level(c->level);
  else
	  lvl=read_level("levels/o-02.lvl");
  if(!lvl){
    fprintf(stderr, "Whoops, kein level?\n");
    exit(1);
  };

  g=init_graphic();
  m=init_music();
  paint_level(g, lvl);

  fade (g->display, 1000, 1);

  now = SDL_GetTicks();
  q=0;
  while(1){
    q++;
    round=SDL_GetTicks();

    move_step(g, m, lvl, userx);
    update_scoreboard(g, lvl);

    SDL_Flip(g->display); 

    while( SDL_PollEvent( &event ) ){
      /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
      switch( event.type ){
	case SDL_KEYDOWN:
	  if( event.key.keysym.sym == SDLK_q){
	    quit=1;
	  } else if (event.key.keysym.sym == SDLK_b){
	    printf("blocks=%d\n",lvl->blocks);
	  } else if (event.key.keysym.sym == SDLK_d){
	    dump_level(lvl);
	  } else if (event.key.keysym.sym == SDLK_p){
	    snapshot(g);
	  } else if (event.key.keysym.sym == SDLK_RIGHT){
	    userx++;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userx--;
	  };
	  break;

	case SDL_KEYUP:
	  if (event.key.keysym.sym == SDLK_RIGHT){
	    userx--;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userx++;
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
      printf("CPU to slow by %d ticks/round\n",round_d);
    else
      SDL_Delay(round_d);

    if(lvl->blocks==0){quit=1;};
    if(quit){
      fprintf(stderr,"%f Ticks/frame\n",(float)speed/q);
      fade (g->display, 1000, 0);
      SDL_Quit();
      return;
    }
  };
};
