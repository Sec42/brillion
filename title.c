/* Display the title screen, and handle the main menu...
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: title.c,v 1.9 2004/06/16 23:08:17 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>

#define MENU_ENTRIES 5

char menus[][80] = { "PLAY", "SOUND", "LEVEL", "SCORES", "QUIT" };
int  menux[]     = {100,100,100,100,100};
int  menuy[]     = {280,320,360,400,440};

int title_main(){
    SDL_Event event;
    SDL_Rect r,lvlr, sndr;
    signed int menu=1,omenu=0,done=0;
    graphic * g=play->g;
    int lvl=1, olvl=0;
    int snd=play->sound, osnd=2;
    char lvlnum[50];
    SDL_Surface *title;

    title=create_title(0);
    r.x=0;r.y=0;r.w=16;r.h=16; /* XXX: QUAD/2? :) */
    sndr.x=240;sndr.w=100; sndr.y=320;sndr.h=40; /* XXX: Why there? */
    lvlr.x=240;lvlr.w=100; lvlr.y=360;lvlr.h=50; /* XXX: Why there? */

    while(!done){
	while( !done && SDL_PollEvent( &event ) ){
	    /* We are only interested in SDL_KEYDOWN and SDL_KEYUP events */
	    switch( event.type ){
		case SDL_KEYDOWN:
		    switch(event.key.keysym.sym){
			case SDLK_ESCAPE:
			case SDLK_q:
			    exit(-1);		/* Quit game */
			    break;
			case SDLK_UP:
			    menu--;
			    break;
			case SDLK_DOWN:
			    menu++;
			    break;
			case SDLK_RETURN:
			case SDLK_SPACE:
			    switch(menu){
				case 1:            /* Play */
				    play->level=lvl-1;
				    play->sound=snd;
				case 4:            /* scores */
				case MENU_ENTRIES: /* Quit */
				    done=1;
				    break;
				case 2:	           /* snd */
				    snd=1-snd;
				    break;
				case 3:            /* lvl */
				    lvl+=4;
				    if(lvl>21)lvl=1;
				    break;
				default:
				    assert("unhandled menu item"&&NULL);
			    };
			    break;
			default:		/* perhaps beep? */
			    break;
		    };
		    break;

		case SDL_KEYUP:
		    break;

		case SDL_QUIT: /* SDL_QUIT event (window close) */
		    exit(-1); /* XXX */
		    break;

		default:
		    break;
	    };

	    while (menu<1) menu+=MENU_ENTRIES;
	    while (menu>MENU_ENTRIES) menu-=MENU_ENTRIES;
	};

	if(omenu!=menu){
	    SDL_BlitSurface(title,&r,play->g->display,&r);
	    UPDATE(r);
	    r.x=menux[menu-1]-20;r.y=menuy[menu-1]+6;
	    SDL_BlitSurface(play->g->ball[BLUE],NULL,play->g->display,&r);
	    UPDATE(r);
	    omenu=menu;
	};
	if(osnd!=snd){
	    osnd=snd;
	    SDL_BlitSurface(title,&sndr,play->g->display,&sndr);
#ifdef SOUND
	    if(snd){
		render_font(sndr.x,sndr.y,"On");
	    }else{
		render_font(sndr.x,sndr.y,"Off");
	    };
#else
	    render_font(sndr.x,sndr.y,"n/a");
#endif
	    UPDATE(sndr);
	};

	if(olvl!=lvl){
	    olvl=lvl;
	    sprintf(lvlnum,"%2d",lvl);

	    SDL_BlitSurface(title,&lvlr,play->g->display,&lvlr);
	    render_font(lvlr.x,lvlr.y,lvlnum);
	    UPDATE(lvlr);
	};

	if(!done){
	    /* While No events are pending, we can do funky idle things */
	    SDL_Delay(100);
	    SDL_BlitSurface(title,&r,play->g->display,&r);
	    UPDATE(r);
	    r.x=menux[menu-1]-20;r.y=menuy[menu-1]+6;
	    switch(rand()&7){
		case 0:
		    r.x++;r.y++;break;
		case 1:
		    r.x++;      break;
		case 2:
		    r.x++;r.y--;break;
		case 3:
		    r.y++;break;
		case 4:
		    r.y--;break;
		case 5:
		    r.x--;r.y++;break;
		case 6:
		    r.x--;      break;
		case 7:
		    r.x--;r.y--;break;
	    };
	    SDL_BlitSurface(play->g->ball[BLUE],NULL,play->g->display,&r);
	    UPDATE(r);
	};
	DISPLAY;

    }; /* while(!done) */
    SDL_FreeSurface(title);
    return(menu);
}

SDL_Surface * create_title(int oldscore){
    SDL_Surface *title,*black,*s,*bkg;
    int x;

    s=play->g->display;

    title=IMG_Load("title.png");
    assert(title!=NULL);

    /* sleep(1); */ /* I'd like to have a cool intro here ;-) */

    black=NULL;
    /* To make it more readable, perhaps?
    black=SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, 
	    s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, 
	    s->format->Bmask, s->format->Amask);
    SDL_FillRect(black, NULL, SDL_MapRGB(s->format,0,0,0));
    SDL_SetAlpha(black, SDL_SRCALPHA, 64);
    SDL_BlitSurface(black, NULL, title, NULL);
    */

    bkg=SDL_ConvertSurface(title, s->format, SDL_SWSURFACE);
    SDL_FreeSurface(title);
    for(x=0;x<MENU_ENTRIES;x++){
	render_font_to(menux[x],menuy[x],menus[x],bkg);
    };

    if(oldscore >0){
	char sc[100];
	sprintf(sc,"Your Score: %6d",oldscore);
	render_font_to(300,10,sc,bkg);
    };

    /* Display it, for good measure */
    SDL_BlitSurface(bkg,NULL,s,NULL);
    SDL_Flip(s);

    return bkg; /* Caller must SDL_FreeSurface(bkg) eventually. */
}
