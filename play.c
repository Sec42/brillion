#define EXTERN extern
// #include "SDL_mixer.h"
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */
#define SPEED 60 /* Ticks per Frame */

#define MX 640
#define MY 480
#define MZ 4

void game(){
  SDL_Event event;
  signed int userx=0;
  char quit=0;
  int q; // Number of frames displayed
  Uint32 now;
  Uint32 round;
  Sint32 round_d;

  graphic * g;
  field * lvl;

  lvl=read_level("levels/o-02.lvl");
  if(!lvl){
    fprintf(stderr, "Whoops, kein level?\n");
    exit(1);
  };

  g=init_graphic();
  paint_level(g, lvl);

  now = SDL_GetTicks();
  q=0;
  while(1){
    q++;
    round=SDL_GetTicks();

    move_step(g, lvl, userx);

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
	    /*
	    char name[40];
	    FILE * pic;
	    unsigned char * x;
	    sprintf(name,"snap_%d",q);
	    pic=fopen(name,"w");
	    fprintf(pic,"P6\n%d %d\n%d\n",MX,MY,255);
	    for (x=disp->pixels;x<(unsigned char*)(disp->pixels+MX*MY*MZ);x+=4)
	      fprintf(pic,"%c%c%c",*(x+2),*(x+1),*(x+0));
	    fclose(pic);
	    */
	  } else if (event.key.keysym.sym == SDLK_RIGHT){
	    userx=1;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userx=-1;
	  };
	  break;

	case SDL_KEYUP:
	  if (event.key.keysym.sym == SDLK_RIGHT){
	    userx=0;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userx=-0;
	  };
	  break;
	case SDL_QUIT: /* SDL_QUIT event (window close) */
	  quit = 1;
	  break;

	default:
	  break;
      };
    }
    round_d=SPEED-(SDL_GetTicks()-round);
    if(round_d<0)
      printf("CPU to slow by %d ticks/round\n",round_d);
    else
      SDL_Delay(round_d);

    if(lvl->blocks==0){quit=1;};
    if(quit){
      fprintf(stderr,"%f frames / sec\n",(float)q/(SDL_GetTicks()-now)*1000);
      SDL_Quit();
      return;
    }
  };
};
