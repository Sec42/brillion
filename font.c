/* A very simple font blitter - uses SFont/BFont style proportional fonts.
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: font.c,v 1.9 2003/12/12 02:29:42 sec Exp $
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

SDL_Rect *render_text(int x, int y,const char *txt,a_font *f,int maxw){
    static SDL_Rect rv;
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

int numwidth(a_font *f){
    const char *txt = "0123456789";
    const char *c;
    int w;

    assert(f !=NULL);

    w=0;
    for(c=txt;*c!=0;c++){
	if(*c<FONTBEG){
	    if(f->space>w)
		w=f->space;
	    continue;
	};
	assert((*c-FONTBEG)<f->len);
	if(f->wid[*c-FONTBEG]>w)
	    w=f->wid[*c-FONTBEG];
    };
    return w;
}

void render_num(int x, int y, int w,int num,a_font *f){
    SDL_Rect rs,rd;
    int c;
    signed int dx=x;

    assert(f !=NULL);

    rd.y=y;

    rs.y=0; 
    rs.h=f->h-1;

    for(c=num;c>0||dx==x;c/=10){
	dx-=w;
	rs.x=f->beg[(c%10)+'0'-FONTBEG];
	rs.w=f->wid[(c%10)+'0'-FONTBEG];
	rd.x=dx+(w-rs.w)/2;
	
	SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
    };
}

void render_fix(int x, int y, int w,const char *txt,a_font *f){
    SDL_Rect rs,rd;
    const char *c;
    signed int dx=x;

    assert(f !=NULL);

    rd.y=y;

    rs.y=0; 
    rs.h=f->h-1;

    for(c=txt;*c!=0;c++){
	if(*c>='0' && *c<='9'){
	    rs.x=f->beg[*c-FONTBEG];
	    rs.w=f->wid[*c-FONTBEG];
	    rd.x=dx+(w-rs.w)/2;

	    SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
	    dx+=w;
	}else if (*c == ' '){
	    dx+=f->space;
	}else{
	    rs.x=f->beg[*c-FONTBEG];
	    rs.w=f->wid[*c-FONTBEG];
	    rd.x=dx;

	    SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
	    dx+=rs.w;
	};
    };
}

#define ALIGN_LEFT   0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT  2
#define ALIGN_MASK   3

#define LIMIT_NONE   0
#define LIMIT_SIZE   4

void nice_render_text(SDL_Rect *rv,const char *txt,a_font *f,int flags){
    SDL_Rect rs,rd;
    const char *c;
    int maxw=rv->w;

    assert(f !=NULL);

    if(flags & ALIGN_MASK){
	size_text(rv,txt,f);
	if(flags & LIMIT_SIZE && rv->w > maxw)
	    flags|= ~ALIGN_MASK;
	if((flags & ALIGN_MASK) == ALIGN_RIGHT)
	    rv->x+= maxw-rv->w;
	else if ((flags & ALIGN_MASK) == ALIGN_CENTER)
	    rv->x+=(maxw-rv->w)/2;
    };

    rd.x=rv->x;
    rd.y=rv->y;

    rv->w=0;
    rs.y=0; rv->h=rs.h=f->h-1;

    for(c=txt;*c!=0;c++){
	if(*c<FONTBEG){
	    rd.x+=f->space;
	    rv->w+=f->space;
	    continue;
	};
	assert((*c-FONTBEG)<f->len);
	rs.x=f->beg[*c-FONTBEG];
	rs.w=f->wid[*c-FONTBEG];
	rv->w+=rs.w;
	if(maxw && (rv->w>maxw)){
	    break;
	};
	SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
	rd.x+=rs.w;
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

/* Not reentrant, but who cares? :) */
char* input_text(SDL_Rect *in, a_font *f){
    static char str[99];
    SDL_Rect rd,rs;
    SDL_Surface *copy,*s;
    SDL_Event ev;
    graphic* g=play->g;
    int num=0;
    int blink=0;
    char ch;

    s=play->g->display;

    /* XXX: Perhaps only copy needed area?
       I think, copying might be faster in the current form */
    copy=SDL_CreateRGBSurface(SDL_HWSURFACE,
	    s->w, s->h, s->format->BitsPerPixel,
	    s->format->Rmask, s->format->Gmask,
	    s->format->Bmask, s->format->Amask);
    SDL_BlitSurface(s,NULL,copy,NULL);

    rd.x=in->x;
    rd.y=in->y;
    rs.y=0; rd.h=rs.h=f->h-1;

    time_event(5,SDL_USER_BLINK);
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

#define CLEARBLINK do{rd.w=f->wid['_'-FONTBEG];SDL_BlitSurface(copy, &rd, play->g->display, &rd);UPDATE(rd);}while(0)
#define SETBLINK do{rs.x=f->beg['_'-FONTBEG];rs.w=f->wid['_'-FONTBEG];SDL_BlitSurface(f->font, &rs, play->g->display, &rd);UPDATE(rd);}while(0)
#define BEEP do{putc('\a',stdout);fflush(stdout);}while(0)

    while(SDL_WaitEvent(&ev)){
	if(ev.type==SDL_KEYDOWN){
	    if ( (ev.key.keysym.unicode & 0xFF80) == 0 ){
		ch = ev.key.keysym.unicode & 0x7F;

		if(blink) CLEARBLINK;
		if(ch>=FONTBEG && (ch-FONTBEG)<f->len){
		    rs.x=f->beg[ch-FONTBEG];
		    rs.w=f->wid[ch-FONTBEG];

		    printf("%c is %d\n",ch,rs.w);
		    if(in->w > 0 && rd.x+rs.w > in->x+in->w){
			BEEP;
		    }else{
			SDL_BlitSurface(f->font, &rs, play->g->display, &rd);
			UPDATE(rd);
			rd.x+=rs.w;
			str[num++]=ch;
		    };
		}else if(ch==' '){
		    rd.x+=f->space;;
		    str[num++]=ch;
		}else if(ch == 8 && num>0){
		    ch=str[--num];
		    if(ch==' ')
			rd.w=f->space;
		    else
			rd.w=f->wid[ch-FONTBEG];
		    rd.x-=rd.w;
		    SDL_BlitSurface(copy, &rd, play->g->display, &rd);
		    UPDATE(rd);
		}else if(ch==SDLK_ESCAPE||ch==SDLK_RETURN){
		    str[num]=0;
		    break;
		}else if (ch == 0){
		    ; /* Non-character key, like SHIFT */
		}else{
		    BEEP;
		};

		if(blink) SETBLINK;
	    }else{
		BEEP;
	    };


	}else if (ev.type == SDL_USEREVENT && ev.user.code==SDL_USER_BLINK){
		blink=1-blink;
		if(blink)
		    SETBLINK;
		else
		    CLEARBLINK;

		time_event(5,SDL_USER_BLINK);
	}else if (ev.type == SDL_QUIT){
	    exit(-1); /* XXX */
	};
	DISPLAY;
    }

    DISPLAY;
    SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0,0);
    SDL_FreeSurface(copy);
    printf("Input is: %s\n",str);
    return str;
}
