/* Display the title screen, and handle the main menu...
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: title.c,v 1.1 2003/03/25 15:51:19 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>

void display_title(void){
    SDL_Surface *title,*black,*s;

    s=play->g->display;

    title=IMG_Load("title.png");
    assert(title!=NULL);

    SDL_BlitSurface(title,NULL,s,NULL);
    SDL_Flip(s);

    sleep(2);
    black=SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, 
	    s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, 
	    s->format->Bmask, s->format->Amask);
    SDL_FillRect(black, NULL, SDL_MapRGB(s->format,0,0,0));
    SDL_SetAlpha(black, SDL_SRCALPHA, 64);
    SDL_BlitSurface(black, NULL, s, NULL);
    SDL_Flip(s);
    sleep(3);
}
