/* Display the title screen, and handle the main menu...
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: title.c,v 1.5 2003/12/05 00:58:03 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>

#define MENU_ENTRIES 3

char menus[][80] = { "PLAY", "LEVEL", "QUIT" };
int  menux[]     = {100,100,100};
int  menuy[]     = {360,400,440};

void title_main(SDL_Surface *title){
    SDL_Event event;
    SDL_Rect r;
    signed int menu=1,omenu=0;
    graphic * g=play->g;
    int lvl=1;
char lvlnum[50];

    r.x=0;r.y=0;r.w=16;r.h=16; /* XXX: QUAD/2? :) */

    while(1){
	while( SDL_PollEvent( &event ) ){
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
			    if(menu==3)exit(0);
			    if(menu==2){
				SDL_Rect rr;
				rr.x=220;rr.w=100;
				rr.y=400;rr.h=50;
				lvl+=4;
				if(lvl>21)lvl=1;
				sprintf(lvlnum,"%2d",lvl);

	    SDL_BlitSurface(title,&rr,play->g->display,&rr);
				render_font(220,400,lvlnum);
				SDL_Flip(g->display);
				break;
			    };
			    play->level=lvl-1;
			    return;
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
	}

	while (menu<1) menu+=MENU_ENTRIES;
	while (menu>MENU_ENTRIES) menu-=MENU_ENTRIES;

	if(omenu!=menu){
	    SDL_BlitSurface(title,&r,play->g->display,&r);
	    UPDATE(r);
	    r.x=menux[menu-1]-20;r.y=menuy[menu-1]+6;
	    SDL_BlitSurface(play->g->ball[BLUE],NULL,play->g->display,&r);
	    UPDATE(r);
	    omenu=menu;
	}else{
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

    }; /* while(1) */
}

void display_title(int oldscore){
    SDL_Surface *title,*black,*s,*bkg;
    int x;

    s=play->g->display;

    title=IMG_Load("title.png");
    assert(title!=NULL);

    SDL_BlitSurface(title,NULL,s,NULL);
    SDL_Flip(s);

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
	render_font(menux[x],menuy[x],menus[x]);
    };

    if(oldscore >0){
	char s[100];
	sprintf(s,"Your Score: %6d",oldscore);
	render_font(300,10,s);
    };
    SDL_Flip(s);
    title_main(bkg);
    SDL_FreeSurface(bkg);
}
