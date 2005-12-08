/* Display the title screen, and handle the main menu...
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: title.c,v 1.14 2005/12/08 11:11:18 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>

#define MENU_LINE 40
#define MENU_ENTRIES 5 /* XXXXXXXXXXXXXXXXXXXXXXXX!!!! */

SDL_Surface * create_title(menuentry * entries){
    SDL_Surface *title,*black,*s,*bkg;
    int x;
    int xk,yk;

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

    xk=100;yk=260; /* XXX: Main_menu only */
    for(x=0;entries[x].name;x++){
	render_font_to(xk,yk,entries[x].name,bkg);
	yk+=MENU_LINE;
    };

    /* Display it, for good measure */
    SDL_BlitSurface(bkg,NULL,s,NULL);
    SDL_Flip(s);

    return bkg; /* Caller must SDL_FreeSurface(bkg) eventually. */
}

void cb_sound(SDL_Surface *title,int dwim){
    SDL_Rect sndr;
    graphic * g=play->g;

    if(dwim){
		if(play->m){
		    uninit_music();
		}else{
		    play->m=init_music();
		};
    }

    sndr.x=240;sndr.w=100; sndr.y=300;sndr.h=40; /* XXX: Why there? */
    SDL_BlitSurface(title,&sndr,play->g->display,&sndr);
#ifdef SOUND
	if(play->m){
	    render_font(sndr.x,sndr.y,"On");
	}else{
	    render_font(sndr.x,sndr.y,"Off");
	};
#else
	render_font(sndr.x,sndr.y,"n/a");
#endif
	UPDATE(sndr);
}

void cb_lvl(SDL_Surface *title,int dwim){
    SDL_Rect lvlr;
    graphic * g=play->g;
    char lvlnum[50];

    if(dwim){
	play->level+=4;
	if(play->level>20)play->level=0;
    }

    lvlr.x=240;lvlr.w=100; lvlr.y=340;lvlr.h=50; /* XXX: Why there? */
    sprintf(lvlnum,"%2d",play->level+1);

    SDL_BlitSurface(title,&lvlr,play->g->display,&lvlr);
    render_font(lvlr.x,lvlr.y,lvlnum);
    UPDATE(lvlr);
}

signed int do_menu(menuentry *entries){
    SDL_Event event;
    SDL_Rect r;
    signed int menu,omenu=1,done=0;
    graphic * g=play->g;
    SDL_Surface *title;

    r.x=0;r.y=0;r.w=16;r.h=16; /* XXX: QUAD/2? :) */
    title=create_title(entries);

    for(menu=0;entries[menu].name;menu++){
	if (entries[menu].callback)
	    (entries[menu].callback)(title,0);
    };
    menu=0;

    while(!done){
	while( !done && SDL_PollEvent( &event ) ){
	    /* We are only interested in SDL_KEYDOWN and SDL_KEYUP events */
	    switch( event.type ){
		case SDL_KEYDOWN:
		    switch(event.key.keysym.sym){
			case SDLK_ESCAPE:
			case SDLK_q:
			    omenu=menu=-1;		/* Quit menu */
			    done=1;
			    break;
			case SDLK_UP:
			    if (--menu<0) menu+=MENU_ENTRIES;
			    break;
			case SDLK_DOWN:
			    if (++menu>=MENU_ENTRIES) menu-=MENU_ENTRIES;
			    break;
			case SDLK_RETURN:
			case SDLK_SPACE:
			    if (entries[menu].callback){
				entries[menu].callback (title,1);
			    }else
				done=1;
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
	};

	if(omenu!=menu){
	    SDL_BlitSurface(title,&r,play->g->display,&r);
	    UPDATE(r);
	    r.x=100-30;r.y=260+MENU_LINE*menu+6; /* XXX! */
	    SDL_BlitSurface(play->g->ball[RED],NULL,play->g->display,&r);
	    UPDATE(r);
	    omenu=menu;
	};

	if(!done){
	    /* While No events are pending, we can do funky idle things */
	    SDL_Delay(100);
	    SDL_BlitSurface(title,&r,play->g->display,&r);
	    UPDATE(r);
	    r.x=100-30;r.y=260+MENU_LINE*menu+6; /* XXX! */
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
	    SDL_BlitSurface(play->g->ball[RED],NULL,play->g->display,&r);
	    UPDATE(r);
	};
	DISPLAY;

    }; /* while(!done) */
    SDL_FreeSurface(title);
    return(menu);
}

signed int title_main(){
    menuentry me[MENU_ENTRIES+1] = {
	{"PLAY", NULL},
	{"SOUND", &cb_sound},
	{"LEVEL", &cb_lvl},
	{"SCORES", NULL},
	{"QUIT", NULL},
	{NULL, NULL}
    };

    return do_menu(me);
}
