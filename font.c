/* A very simple font blitter - uses SFont/BFont style proportional fonts.
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: font.c,v 1.3 2003/03/21 01:03:19 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>

a_font* init_font(const char *file){
    int x,y;
    SDL_PixelFormat pix;
    SDL_Surface *in,*tmp;
    a_font *f;
    Uint32 *p,ep,op; /* pixel, endpixel, oldpixel */

    f=calloc(1,sizeof(a_font));

    in=IMG_Load(file);
    assert(in!=NULL);

    pix.palette=NULL;
    pix.BitsPerPixel=32;
    pix.Rmask=0x000000ff;
    pix.Gmask=0x0000ff00;
    pix.Bmask=0x00ff0000;
    pix.Amask=0xff000000;

    tmp=SDL_ConvertSurface(in, &pix, SDL_SWSURFACE);
    assert(tmp!=NULL);
    free(in);

    p=tmp->pixels;op=ep=p[0];y=0;
    for(x=0;x<tmp->w;x++){
	if(p[x]!=op){
	    op=p[x];
	    if(op==ep){
		f->wid[y]=x-f->beg[y];
		y++;
	    }else{
		f->beg[y]=x;
	    };
	};
	assert(y<FONTLEN);
    };
    f->h=tmp->h;
    f->len=y;
    f->space=f->wid['-'-FONTBEG]; /* What is typographically correct here? */

    f->font=SDL_DisplayFormat(tmp);
    assert(f->font!=NULL);
    free(tmp);

    return(f);
}

void render_font(int x, int y,char *txt){
    a_font *f=play->g->tfont;
    SDL_Rect rs,rd;
    char *c;

    rd.x=x;rd.y=y;

    rs.y=0; rs.h=f->h-1;

    for(c=txt;*c!=0;c++){
	if(*c<FONTBEG){
	    rd.x+=f->space;;
	    continue;
	};
/*	printf("Rendering: %c (%d) ",*c,*c-FONTBEG); */
	assert((*c-FONTBEG)<f->len);
	rs.x=f->beg[*c-FONTBEG];
	rs.w=f->wid[*c-FONTBEG];
/*	printf("[%d/%d -> %d/%d]\n",rs.x,rs.y,rs.w,rs.h); */
	SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
	rd.x+=rs.w;
    };
}

