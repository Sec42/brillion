#define EXTERN extern
// #include "SDL_mixer.h"
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */
#define SPEED 70 /* Ticks per Frame */

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
	play->a=init_anim();
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
  int userl=0;  /* user moves left */
  int userlr=0; /* user released left */
  int userr=0;  /* user moves right */
  int userrr=0; /* user released right */
  char quit=0;
  int frames; // Number of frames displayed
  int z; // Animation counter...

  Uint32 t_start,t_end,t_gone,t_now;
  Sint32 t_left;
  Uint32 ticks;
  Uint32 q;

  graphic* g=play->g;
  music*   m=play->m;
  field* lvl=play->f;
  a_anim*  a=play->a;

  frames=0; ticks=0;

  while(1){
    frames++;
    t_end=t_start=SDL_GetTicks();

    while( SDL_PollEvent( &event ) ){
      /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
      switch( event.type ){
	case SDL_KEYDOWN:
	  if( event.key.keysym.sym == SDLK_q){
	    quit=1; // Abort game
	  } else if (event.key.keysym.sym == SDLK_RIGHT){
	    userr=2;userrr=0;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userl=2;userlr=0;
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
	  /* If the user releases the key in the same frame he presses
	     it still move one step in the desired direction.  */
	  if (event.key.keysym.sym == SDLK_RIGHT){
	    if(userr==1)
	      userr=0;
	    else
	      userrr=1;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    if(userl==1)
	      userl=0;
	    else
	      userlr=1;
	  };
	  break;

	case SDL_QUIT: /* SDL_QUIT event (window close) */
	  quit = 1;
	  break;

	default:
	  break;
      };
    }

    if(userr==2) userr=1;
    if(userl==2) userl=1;

    move_step(g, m, lvl, userr-userl);

    if(userrr==1) userr=0;
    if(userlr==1) userl=0;

    if(frames%2==0 && lvl->time>0)
      lvl->time--;
    update_scoreboard(play);

//    printf("Already lost %d ticks\n",t_start-SDL_GetTicks());
    for(z=1;z<=AFRAMES;z++){

      animate(g,a,z); // Move what is to move.
//    printf("%d rects\n",g->numrects);
      DISPLAY;

      if(z < AFRAMES){
	t_end+=(SPEED/AFRAMES)-1;
	t_now=SDL_GetTicks();
	t_gone=t_now-t_start;
	t_left=t_end-t_now;

//	printf("anim_sleep: %d\n",t_left);
	q=0;
	if(t_left>0){
	  ticks-=t_left;
	  DELAY(t_left,t_end);
	};
//	printf("sleep: wanted:%d actually:%d bsy=%d\n",a_left,SDL_GetTicks()-a_now,q);
      };
    };

//    printf("Speed: ran for %d ticks at %d/anim\n",t_gone,SPEED/AFRAMES);

    t_end=t_start+SPEED;
    t_now=SDL_GetTicks();
    t_gone=t_now-t_start;
    t_left=t_end-t_now;

    ticks+=t_gone;
    if(t_left>0){
      DELAY(t_left,t_end);
    }else{
      printf("CPU to slow by %d ticks/round\n",-t_left);
    };

    if(quit || lvl->blocks<=0){
      fprintf(stderr,"%f Ticks/frame\n",(float)ticks/frames);
	  if(quit)
		  return(2);
	  if(lvl->blocks==0)
		  return(1);
      return(0);
    };

  }; /* while(1) */
};
