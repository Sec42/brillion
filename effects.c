/* timed graphic effects
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: effects.c,v 1.8 2003/03/26 17:16:13 sec Exp $
 */
#include "brillion.h"

void split_simple (SDL_Surface* s, Uint32 ticks, int splitvert, int splitin){
    SDL_Surface *black, *copy;
    Uint32 old_time, curr_time;
    int dist=0;
    int target;
    SDL_Rect rectl,rectr,drectl,drectr;

    if(splitvert){
	target=s->h/2;

	rectl.w=rectr.w=s->w;
	rectl.x=rectr.x=0;
	drectl.x=drectr.x=0;
	drectl.y=0; rectr.y=target;
    }else{
	target=s->w/2;

	rectl.h=rectr.h=s->h;
	rectl.y=rectr.y=0;
	drectl.y=drectr.y=0;
	drectl.x=0; rectr.x=target;
    };

    black=SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);
    copy= SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);

    if(!black || !copy){
	fprintf (stderr, "split_simple: failure creating surface\n");
	return;
    }

    SDL_FillRect(black, NULL, SDL_MapRGB(s->format,0,0,0));
    SDL_BlitSurface(s, NULL, copy, NULL);

    curr_time=SDL_GetTicks();

    while (dist < target){
	if(splitin){
	    if(splitvert){
		rectl.h=rectr.h=dist;
		rectl.y=target-dist;
		drectr.y=2*target-dist;
	    }else{
		rectl.w=rectr.w=dist;
		rectl.x=target-dist;
		drectr.x=2*target-dist;
	    }
	}else{
	    if(splitvert){
		rectl.h=rectr.h=target-dist;
		rectl.y=dist;
		drectr.y=target+dist;
	    }else{
		rectl.w=rectr.w=target-dist;
		rectl.x=dist;
		drectr.x=target+dist;
	    }
	};

	SDL_BlitSurface(black, NULL, s, NULL);
	SDL_BlitSurface(copy, &rectl, s, &drectl);
	SDL_BlitSurface(copy, &rectr, s, &drectr);

	old_time=curr_time;
	curr_time=SDL_GetTicks();

	SDL_Flip (s);

	dist += s->w/2 * ((float) (curr_time - old_time) / ticks);

    };

    if(splitin)
	SDL_BlitSurface(copy, NULL, s, NULL);
    else
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format,0,0,0));

    SDL_Flip(s);

    SDL_FreeSurface(black);
    SDL_FreeSurface(copy);
}

/* Fading by palette modification */
void fade_p (SDL_Surface* s, Uint32 ticks, int fadein){
    SDL_Color *colors;
    Uint32 old_time, curr_time;
    float alpha=0;
    graphic *g=play->g;
    int q;

    colors=calloc(g->ncolors,sizeof(SDL_Color));

    if(!fadein)
	memcpy(colors,g->palette,256*sizeof(SDL_Color));

    SDL_SetPalette(s,SDL_PHYSPAL, colors, 0, g->ncolors); 

    if(fadein)
	SDL_Flip(s);

    curr_time=SDL_GetTicks();
    while (alpha < 255.0){
	for(q=0;q<g->ncolors;q++){
	    colors[q].r=g->palette[q].r*((fadein?alpha:255-alpha)/255);
	    colors[q].g=g->palette[q].g*((fadein?alpha:255-alpha)/255);
	    colors[q].b=g->palette[q].b*((fadein?alpha:255-alpha)/255);
	};

	old_time=curr_time;
	curr_time=SDL_GetTicks();

	SDL_SetPalette(s,SDL_PHYSPAL,colors, 0,g->ncolors); 

	alpha += 255 * ((float) (curr_time - old_time) / ticks);
    };

    if(!fadein){
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format,0,0,0));
	SDL_Flip(s);
    };

/*    SDL_SetPalette(s,SDL_PHYSPAL,g->palette, 0,g->ncolors); */
    free(colors);
}

/* fading by alpha blending */
void fade (SDL_Surface* s, Uint32 ticks, int fadein){
    SDL_Surface* black, *copy;
    Uint32 old_time, curr_time;
    float alpha=0;

    if(s->format->BitsPerPixel == 8){
	/* Palette fading */
	fade_p(s,ticks,fadein);
	return;
    };

    black=SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, 
	    s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, 
	    s->format->Bmask, s->format->Amask);
    copy= SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, 
	    s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, 
	    s->format->Bmask, s->format->Amask);

    if(!black || !copy){
	fprintf (stderr, "fade: failure creating surface\n");
	return;
    }

    SDL_FillRect(black, NULL, SDL_MapRGB(s->format,0,0,0));
    SDL_BlitSurface(s, NULL, copy, NULL);

    curr_time=SDL_GetTicks();

    while (alpha < 255.0){
	SDL_BlitSurface(copy, NULL, s, NULL);
	SDL_SetAlpha(black, SDL_SRCALPHA, (Uint8)(fadein?255-alpha:alpha));
	SDL_BlitSurface(black, NULL, s, NULL);

	old_time=curr_time;
	curr_time=SDL_GetTicks();

	SDL_Flip (s);

	alpha += 255 * ((float) (curr_time - old_time) / ticks);
    };

    if(fadein)
	SDL_BlitSurface(copy, NULL, s, NULL);
    else
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format,0,0,0));

    SDL_Flip (s);

    SDL_FreeSurface(black);
    SDL_FreeSurface(copy);
}

void split (SDL_Surface* s, SDL_Rect* r_in, Uint32 ticks, split_t type){
    SDL_Surface *black, *copy;
    Uint32 old_time, curr_time;
    int dist=0;
    int delta=0;
    int target;
    SDL_Rect *r;
    SDL_Rect rectl,rectr,drectl,drectr;

    if(r_in == NULL){
	r=malloc(sizeof(SDL_Rect));
	r->x=0;
	r->y=0;
	r->w=s->w;
	r->h=s->h;
    }else{
	r=r_in;
    };

    switch(type){
	case HORIZ_IN:
	    target=r->h/2;

	    rectl.x=rectr.x=0;
	    rectl.w=rectr.w=r->w;

	    rectl.y=target; rectr.y=target;
	    rectl.h=rectr.h=0;

	    drectl.x=drectr.x=r->x;
	    drectl.y=r->y; drectr.y=r->y+2*target;
	    break;

	case HORIZ_OUT:
	    target=r->h/2;

	    rectl.x=rectr.x=0;
	    rectl.w=rectr.w=r->w;

	    rectl.y=0; rectr.y=target;
	    rectl.h=rectr.h=target;

	    drectl.x=drectr.x=r->x;
	    drectl.y=r->y; drectr.y=r->y+target;
	    break;

	case VERT_IN:
	    target=r->w/2;

	    rectl.y=rectr.y=0;
	    rectl.h=rectr.h=r->h;

	    rectl.x=target; rectr.x=target;
	    rectl.w=rectr.w=0;

	    drectl.x=r->x; drectr.x=r->x+2*target;
	    drectl.y=drectr.y=r->y;
	    break;

	case VERT_OUT:
	    target=r->w/2;

	    rectl.x=0; rectr.x=target;
	    rectl.h=rectr.h=r->h;

	    rectl.y=rectr.y=0;
	    rectl.w=rectr.w=target;

	    drectl.x=r->x; drectr.x=r->x+target;
	    drectl.y=drectr.y=r->y;
	    break;

	default:
	    printf("split: unknown type\n");
	    return;
    };

    black=SDL_CreateRGBSurface(SDL_HWSURFACE, r->w, r->h, 
	    s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, 
	    s->format->Bmask, s->format->Amask);
    copy= SDL_CreateRGBSurface(SDL_HWSURFACE, r->w, r->h, 
	    s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, 
	    s->format->Bmask, s->format->Amask);

    if(!black || !copy){
	fprintf (stderr, "split: failure creating surface\n");
	return;
    }

    SDL_FillRect(black, NULL, SDL_MapRGB(s->format,0,0,0));

    /* for palette displays... */
    if(s->format->BitsPerPixel == 8)
	SDL_SetPalette(copy,SDL_LOGPAL,s->format->palette->colors,
		0,s->format->palette->ncolors);
    SDL_BlitSurface(s, r, copy, NULL);

    curr_time=SDL_GetTicks();

    while (dist < target){
	switch(type){
	    case HORIZ_OUT:
		rectl.y+=delta;
		rectl.h-=delta;

		rectr.h-=delta;

		drectr.y+=delta;
		break;

	    case HORIZ_IN:
		rectl.y-=delta;
		rectl.h+=delta;

		rectr.h+=delta;
		drectr.y-=delta;
		break;

	    case VERT_OUT:
		rectl.x+=delta;
		rectl.w-=delta;

		rectr.w-=delta;
		drectr.x+=delta;
		break;

	    case VERT_IN:
		rectl.x-=delta;
		rectl.w+=delta;

		rectr.w+=delta;
		drectr.x-=delta;
		break;
	};

	SDL_BlitSurface(black, NULL, s, r);
	SDL_BlitSurface(copy, &rectl, s, &drectl);
	SDL_BlitSurface(copy, &rectr, s, &drectr);

	old_time=curr_time;
	curr_time=SDL_GetTicks();

	SDL_UpdateRect(s,r->x,r->y,r->w,r->h);

	delta = target * ((float) (curr_time - old_time) / ticks);
	if(delta>target/4){delta=target/4;} /* At least 4 updates */
	dist += delta;

    };

    if(type==HORIZ_IN||type==VERT_IN)
	SDL_BlitSurface(copy, NULL, s, r);
    else
	SDL_FillRect(s, r, SDL_MapRGB(s->format,0,0,0));

    SDL_UpdateRect(s,r->x,r->y,r->w,r->h);

    SDL_FreeSurface(black);
    SDL_FreeSurface(copy);
    if(r_in==NULL)
	free(r);
}
