/* A very simple font blitter - uses SFont/BFont style proportional fonts.
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: font.c,v 1.8 2003/12/11 02:46:15 sec Exp $
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
    f->lineh=f->h+(f->h/6);
    f->len=y;
    f->space=f->wid['-'-FONTBEG]; /* What is typographically correct here? */

    SDL_SetColorKey(tmp, SDL_SRCCOLORKEY, p[1]);
    f->font=SDL_DisplayFormat(tmp);
    assert(f->font!=NULL);
    free(tmp);

    return(f);
}

void render_font(int x, int y,const char *txt){
    a_font *f=play->g->tfont;
    SDL_Rect rs,rd;
    const char *c;

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

SDL_Rect    rv; /* Returnvalue. Yes, it's ugly - but that way, the caller can
		   safely ignore it. */
SDL_Rect *render_text(int x, int y,const char *txt,a_font *f,int maxw){
    SDL_Rect rs,rd;
    const char *c;

    if(f == NULL)
	f=play->g->tfont;
    assert(f !=NULL);

    rv.x=rd.x=x;
    rv.y=rd.y=y;

    rv.w=0;
    rs.y=0; rv.h=rs.h=f->h-1;

    for(c=txt;*c!=0;c++){
	if(*c<FONTBEG){
	    rd.x+=f->space;
	    rv.w+=f->space;
	    continue;
	};
/*	printf("Rendering: %c (%d) ",*c,*c-FONTBEG);  */
	assert((*c-FONTBEG)<f->len);
	rs.x=f->beg[*c-FONTBEG];
	rs.w=f->wid[*c-FONTBEG];
	rv.w+=rs.w;
	if(maxw && (rv.w>maxw)){
/*	    printf("Break!\n"); */
	    break;
	};
/*	printf("[%d/%d -> %d/%d]\n",rs.x,rs.y,rs.w,rs.h);  */
	SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
	rd.x+=rs.w;
    };
    return &rv;
}

void neu_render_text(SDL_Rect *rd,const char *txt,a_font *f,int maxw){
    SDL_Rect rs;
    const char *c;

    assert(f !=NULL);

    rs.y=0; rd->h=rs.h=f->h-1;

    for(c=txt;*c!=0;c++){
	if(*c<FONTBEG){
	    rd->x+=f->space;
	    continue;
	};
/*	printf("Rendering: %c (%d) ",*c,*c-FONTBEG);  */
	assert((*c-FONTBEG)<f->len);
	rs.x=f->beg[*c-FONTBEG];
	rs.w=f->wid[*c-FONTBEG];
/*	if(maxw && (rd->w>maxw)){
	    break;
	}; */
/*	printf("[%d/%d -> %d/%d]\n",rs.x,rs.y,rs.w,rs.h);  */
	SDL_BlitSurface(f->font, &rs, play->g->display, rd);
	rd->x+=rs.w;
    };
}

void size_text(SDL_Rect *sr,const char *txt,a_font *f){
    const char *c;

    assert(f !=NULL);

    sr->w=0;
    sr->h=f->h-1;

    for(c=txt;*c!=0;c++){
	if(*c<FONTBEG){
	    sr->w+=f->space;
	    continue;
	};
	assert((*c-FONTBEG)<f->len);
	sr->w+=f->wid[*c-FONTBEG];
    };
}

void input_text(SDL_Rect *in, a_font *f){
    SDL_Rect rd,rs;
    char str[99];
    int num=0;
    char ch;
    SDL_Surface *copy,*s;
    int blink=0;

    s=play->g->display;

    copy=SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);

    SDL_BlitSurface(s,NULL,copy,NULL);


    rd.x=in->x;
    rd.y=in->y;
    rs.y=0; rd.h=rs.h=f->h-1;

    time_event(1,SDL_USER_BLINK);
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    while(1){
	SDL_Event event;
	while( SDL_PollEvent( &event ) ){
	    switch( event.type ){
		case SDL_KEYDOWN:
		    switch(event.key.keysym.sym){
			case SDLK_ESCAPE:
			case SDLK_q:
			    str[num]=0;
			    printf("Inputed: %s\n",str);
			    return;
			default:

if ( (event.key.keysym.unicode & 0xFF80) == 0 ) {
    ch = event.key.keysym.unicode & 0x7F;

    if(blink){
	rd.w=f->wid['_'-FONTBEG];
	SDL_BlitSurface(copy, &rd, play->g->display, &rd);
	SDL_Flip(s);
    };

    if(ch==' '){
	rd.x+=f->space;;
	str[num++]=ch;
    }else if(ch>=FONTBEG){
	assert((ch-FONTBEG)<f->len);
	rs.x=f->beg[ch-FONTBEG];
	rs.w=f->wid[ch-FONTBEG];
	SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
	rd.x+=rs.w;
	str[num++]=ch;
    }else if(ch == 8 && num>0){
	ch=str[--num];
	if(ch==' ')
	    rd.w=f->space;
	else
	    rd.w=f->wid[ch-FONTBEG];
	rd.x-=rd.w;
	SDL_BlitSurface(copy, &rd, play->g->display, &rd);
    };
    if(blink){
	rs.x=f->beg['_'-FONTBEG];
	rs.w=f->wid['_'-FONTBEG];
	SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
    };
    SDL_Flip(s);
};
break;

		    };
		    break;
		case SDL_USEREVENT:
		    if(event.user.code==SDL_USER_BLINK){
			blink=1-blink;
			if(blink){
			    rs.x=f->beg['_'-FONTBEG];
			    rs.w=f->wid['_'-FONTBEG];
			    SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
			    SDL_Flip(s);
			}else{
			    rd.w=f->wid['_'-FONTBEG];
			    SDL_BlitSurface(copy, &rd, play->g->display, &rd);
			    SDL_Flip(s);
			};
			
			time_event(1,SDL_USER_BLINK);
		    };
		    break;
		case SDL_QUIT: /* SDL_QUIT event (window close) */
		    exit(-1); /* XXX */
		    break;
		default:
		    break;
	    }
	}
    }
    SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0,0);
};


