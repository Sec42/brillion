/* Display the game background & field. Do animations, too.
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: graphics.c,v 1.37 2003/03/25 15:52:45 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */

#define MX 640
#define MY 480

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} myc;

/* hue: color tone
 * sat -> 0 = white (other colors increase)
 * val -> 0 = black (primary color decreases)
 * relative brightness: Red: 88, Green: 127, Blue: 40 :: Sum should be == 255
 */

#define BRED 88
#define BGREEN 127
#define BBLUE 40

myc mk_color(int color, int br, int reality){
    myc c;
    int t,sat,val;
    switch(color){
	case RED:     t=BRED             +20;break;
	case CYAN:    t=     BGREEN+BBLUE+ 5;break;
	case MAGENTA: t=BRED       +BBLUE+10;break;
	case GREEN:   t=     BGREEN      - 5;break;
	case BLUE:    t=            BBLUE+10;break;
	case YELLOW:  t=BRED+BGREEN      ;break;
	default:      t=BRED+BGREEN+BBLUE;
    };
    if(!reality)
	t=255;

    /* Saturation is actually 255-saturation here */
    if(br>t){
	sat=(br-t)*256/(256-t);
	val=255;
    }else{
	sat=0;
	val=br*255/t;
    };

    switch(color){
	case RED:     c.r=val; c.g=sat; c.b=sat; c.a=0; break;
	case CYAN:    c.r=sat; c.g=val; c.b=val; c.a=0; break;
	case MAGENTA: c.r=val; c.g=sat; c.b=val; c.a=0; break;
	case GREEN:   c.r=sat; c.g=val; c.b=sat; c.a=0; break;
	case BLUE:    c.r=sat; c.g=sat; c.b=val; c.a=0; break;
	case YELLOW:  c.r=val; c.g=val; c.b=sat; c.a=0; break;
	default:      c.r=sat; c.g=sat; c.b=sat; c.a=0; break;
    };

    return(c);
}

/* Seems like an ugly hack, but I know no sane way */
SDL_Surface* color_graphic(SDL_Surface* in, int color, int alpha){
    graphic   *g=play->g;
    SDL_Surface* out;
    SDL_Surface* tmp;
    SDL_PixelFormat f;
    myc* p4;

    f.palette=NULL;
    f.BitsPerPixel=32;
#if SDL_BYTEORDER == 1234
    f.Rmask=0x000000ff;
    f.Gmask=0x0000ff00;
    f.Bmask=0x00ff0000;
    f.Amask=0xff000000;
#else
    f.Rmask=0xff000000;
    f.Gmask=0x00ff0000;
    f.Bmask=0x0000ff00;
    f.Amask=0x000000ff;
#endif

    tmp=SDL_ConvertSurface(in, &f, SDL_SWSURFACE);
    assert(tmp!=NULL);

    /* perhaps We should check the 'pitch' ? */
    for(p4=tmp->pixels;p4<(myc*)tmp->pixels+(tmp->w*tmp->h);p4++){
	if(alpha!=3){
	    if(p4->r == p4->g && p4->r == 0 ){
		*p4=mk_color(color,p4->b,0);
	    };
	}else{
	    if(p4->r == p4->g && p4->b == p4->g){
		*p4=mk_color(color,p4->b,1);
	    };
	};
    };

    out=SDL_DisplayFormat(tmp);
    assert(out!=NULL);

    if(alpha)
	SDL_SetColorKey(out, SDL_SRCCOLORKEY, g->colors[BG]);

    return(out);
}

void load_graphics(void){
    SDL_Surface* pad;
    int x;
    graphic *g=play->g;

    g->colors[RED]=     SDL_MapRGB (g->display->format, 0xff, 0x00, 0x00);
    g->colors[CYAN]=    SDL_MapRGB (g->display->format, 0x00, 0xff, 0xff);
    g->colors[MAGENTA]= SDL_MapRGB (g->display->format, 0xff, 0x00, 0xff);
    g->colors[GREEN]=   SDL_MapRGB (g->display->format, 0x00, 0xff, 0x00);
    g->colors[BLUE]=    SDL_MapRGB (g->display->format, 0x00, 0x00, 0xff);
    g->colors[YELLOW]=  SDL_MapRGB (g->display->format, 0xff, 0xff, 0x00);
    g->colors[WHITE]=   SDL_MapRGB (g->display->format, 0xff, 0xff, 0xff);
    g->colors[BG]=      SDL_MapRGB (g->display->format, 0xff, 0x7f, 0x00);
    g->colors[FG]=      SDL_MapRGB (g->display->format, 0xff, 0x7f, 0x7f);
    g->colors[NONE]=    SDL_MapRGBA(g->display->format,0,0,0,0);

    /* Load blocks, and create all colors */
    pad=IMG_Load("ball_grey.gif");
    for(x=1;x<GAME_COLORS;x++)
	g->ball[x]=color_graphic(pad,x,3);
    SDL_FreeSurface(pad);

    pad=IMG_Load("ballX.gif");
    for(x=1;x<GAME_COLORS;x++)
	g->ballx[x]=color_graphic(pad,x,2);
    SDL_FreeSurface(pad);

    pad=IMG_Load("block_blue.gif");
    for(x=1;x<GAME_COLORS;x++)
	g->block[x]=color_graphic(pad,x,0);
    SDL_FreeSurface(pad);

    pad=IMG_Load("blockX.gif");
    for(x=1;x<GAME_COLORS;x++)
	g->blockx[x]=color_graphic(pad,x,2);
    SDL_FreeSurface(pad);

    pad=IMG_Load("star_blue.gif");
    for(x=1;x<GAME_COLORS;x++)
	g->star[x]=color_graphic(pad,x,0);
    SDL_FreeSurface(pad);

    pad=IMG_Load("disk_blue.gif");
    for(x=1;x<GAME_COLORS;x++)
	g->disk[x]=color_graphic(pad,x,0);
    SDL_FreeSurface(pad);

    pad=IMG_Load("space.gif");
    g->back=SDL_ConvertSurface(pad, g->display->format, SDL_HWSURFACE);
    assert(g->back!=NULL);
    SDL_FreeSurface(pad);

    pad=IMG_Load("wall.gif");
    g->wall=SDL_ConvertSurface(pad, g->display->format, SDL_HWSURFACE);
    assert(g->wall!=NULL);
    SDL_FreeSurface(pad);

    pad=IMG_Load("death.gif");
    g->death=SDL_ConvertSurface(pad, g->display->format, SDL_HWSURFACE);
    assert(g->death!=NULL);
    SDL_FreeSurface(pad);

    /* XXX: do that somewhere else... */
    g->border=IMG_Load("bkg.png");
    assert(g->border!=NULL);
    g->level.x=20;g->level.y=(g->display->h - (QUAD*11))/2; /* XXX read from somewhere... */
/*  play->g->level.w=(play->f->w-1)*QUAD; play->g->level.h=(play->f->h-1)*QUAD; */
  g->level.w=(15)*QUAD; g->level.h=(11)*QUAD; /* XXX^2 */

  g->font=IMG_Load("numbers.png");
  g->tfont=init_font("font.png");
}

graphic* init_graphic(void){
    SDL_Surface* disp;
    graphic* g;

    if(SDL_Init(SDL_INIT_VIDEO)){
	fprintf(stderr,"Could not initialize SDL: %s.\n", SDL_GetError());
	exit(-1);
    };

    /* HWPALETTE, optional FULLSCREEN? check before ANYFORMAT? */
    if(!(disp=SDL_SetVideoMode(MX, MY, 32, SDL_ANYFORMAT|SDL_HWPALETTE))){
	printf("Could not set videomode: %s.\n", SDL_GetError());
	exit(-1);
    };
    assert(SDL_MUSTLOCK(disp)==0);
    printf("SDL uses %d bytes/pixel Display format\n",disp->format->BytesPerPixel);
    SDL_WM_SetCaption(b->prog, b->prog);

    g=calloc(1,sizeof(graphic));
    g->display=disp;

    if(disp->format->BytesPerPixel == 1){
	int q;
	printf("*UGH* this is an Palette display\n");
	printf("X11 uses %d colors\n",disp->format->palette->ncolors);

	g->ncolors=disp->format->palette->ncolors;
	g->palette=calloc(g->ncolors,sizeof(SDL_Color));
	for(q=0;q<g->ncolors;q++){
	    g->palette[q].r=disp->format->palette->colors[q].r;
	    g->palette[q].g=disp->format->palette->colors[q].g;
	    g->palette[q].b=disp->format->palette->colors[q].b;
	};
    };

    g->rects=calloc(MAXRECTS,sizeof(SDL_Rect));
    g->numrects=0;

    play->g=g;
    load_graphics();

    return g;
}

/* -------------------------------------------------------------------- */
/* Where is a Block */
#define POSX(z) (QUAD*(z-1)+g->level.x)
#define POSY(z) (QUAD*(z-1)+g->level.y)
/* Where is the Ball */
#define POS2X(z) (QUAD/2*(z-2)+g->level.x)
#define POS2Y(z) (QUAD/2*(z-2)+g->level.y)


void paint_level(void){
    int x,y;
    field *lvl=play->f;

    for(y=1;y<lvl->h;y++)
	for(x=1;x<lvl->w;x++)
	    paint_block(x,y);
}

void paint_ball(void){
    /* Paint new Ball */
    SDL_Rect rect;
    graphic *g=play->g;
    field *lvl=play->f;

    rect.w=rect.h=QUAD/2;
    rect.x=POS2X(lvl->x);
    rect.y=POS2Y(lvl->y);
    SDL_BlitSurface(g->ball[lvl->color], NULL, g->display, &rect);
    UPDATE(rect);

    return;
}

void blank_block(int x, int y){
    SDL_Rect rect;
    graphic *g=play->g;

    rect.w=rect.h=QUAD;
    rect.x=POSX(x);
    rect.y=POSY(y);

    SDL_BlitSurface(g->back, NULL, g->display, &rect);
    UPDATE(rect);
}

void paint_block(int x, int y){
    SDL_Rect rect;
    graphic *g=play->g;
    field *lvl=play->f;

    rect.w=rect.h=QUAD;
    rect.x=POSX(x);
    rect.y=POSY(y);

    switch(PIECE(x,y)){
	case BLOCK:
	    SDL_BlitSurface(g->block[COLOR(x,y)], NULL, g->display, &rect);
	    break;
	case DISK:
	    SDL_BlitSurface(g->disk[COLOR(x,y)], NULL, g->display, &rect);
	    break;
	case STAR:
	    SDL_BlitSurface(g->star[COLOR(x,y)], NULL, g->display, &rect);
	    break;
	case WALL:
	    SDL_BlitSurface(g->wall, NULL, g->display, &rect);
	    break;
	case DEATH:
	    SDL_BlitSurface(g->death, NULL, g->display, &rect);
	    break;
	case SPACE:
	    SDL_BlitSurface(g->back, NULL, g->display, &rect);
	    break;
	default:
	    fprintf(stderr,"Cannot draw %d at %d/%d\n",PIECE(x,y),x,y);
    };
    UPDATE(rect);
}

void snapshot(void){
    static int q=0;
    char name[40];
    FILE* pic;
    unsigned char* x;
    graphic *g=play->g;

    if(g->display->format->BytesPerPixel!=4){
	fprintf(stderr,"Das geht nicht\n");
	return;
    };

    sprintf(name,"snap_%d.pnm",++q);
    pic=fopen(name,"w");
    fprintf(pic,"P6\n%d %d\n%d\n",g->display->w,g->display->h,255);
    for (x=g->display->pixels; 
	    x<(unsigned char*)g->display->pixels+ g->display->w * 
		g->display->h * g->display->format->BytesPerPixel;
	    x+=g->display->format->BytesPerPixel)
	fprintf(pic,"%c%c%c",*(x+2),*(x+1),*(x+0));
    fclose(pic);
}

void print_number(int cacheno, int num, coord co){
    char *c,*d;
    SDL_Rect dst,r;
    static char cache[100]="                                                                                                   ";
    graphic *g=play->g;

    /* Font specifics ... */
    char fudge=5;
    char wid=25;
    char hei=31;

    r.w=dst.w=wid-fudge*2;
    r.h=dst.h=hei;
    r.y=0;

    sprintf(cache,"%5d",num);

    dst.y=co.y;
    dst.x=co.x-(5*dst.w);

    assert((cacheno+1)*8<100);
    for(d=(c=cache)+(cacheno*8);*c!=0;c++,d++){
	if(*c != *d){
	    *d=*c;
	    if(*c!=' '){
		int xoff=*c-'0';
		r.x=wid*xoff+fudge;
		SDL_BlitSurface(g->font, &r, g->display, &dst);
	    }else{
		SDL_FillRect(g->display, &dst, g->colors[BLACK]);
	    };
	    UPDATE(dst);
	};
	dst.x+=dst.w;
    }
}

void update_scoreboard(void){
    a_layout *l=play->layout;

    print_number(1, play->level+1,  l->level);
    print_number(2, play->points,   l->pts);
    print_number(3, play->f->blocks,l->blocks);
    print_number(4, play->f->time,  l->time);
    print_number(5, play->lives,    l->lives);
}

a_anim* init_anim(void){
    a_anim *a;
    a=calloc(MAX_ANIM,sizeof(a_anim));
    return a;
}

void create_moveanim(anim_t type, int color, int ox, int oy, int nx, int ny){
    a_anim*  a=play->a;
    graphic* g=play->g;
    int v;

    if(type!=A_BALL){ /* Ball is always index 0. */
	a++;
	while(a->type != A_NONE) a++; /* Search free anim space. */
    };


    a->duration=0;
    a->color=color;
    switch(a->type=type){
	case A_BALL:
	    if(ox>nx) v=1; else v=0;
	    a->block[v].x=ox/2;
	    a->block[1-v].x=nx/2;
	    if(oy>ny) v=1; else v=0;
	    a->block[v].y=oy/2;
	    a->block[1-v].y=ny/2;
	    a->pixel[0].x=POS2X(ox);
	    a->pixel[0].y=POS2Y(oy);
	    a->pixel[1].x=POS2X(nx)-a->pixel[0].x;
	    a->pixel[1].y=POS2Y(ny)-a->pixel[0].y;
	    break;

	case A_DISK:
	    if(ox>nx) v=1; else v=0;
	    a->block[v].x=ox;
	    a->block[1-v].x=nx;
	    if(oy>ny) v=1; else v=0;
	    a->block[v].y=oy;
	    a->block[1-v].y=ny;

	    a->pixel[0].x=POSX(ox);
	    a->pixel[1].x=POSX(nx)-a->pixel[0].x;
	    a->pixel[0].y=POSY(oy);
	    a->pixel[1].y=POSY(ny)-a->pixel[0].y;
	    break;

	default:
	    a->type=A_NONE;
	    printf("Unknown type %d in create_moveanim\n",type);
	    break;
    };
}

void create_staticanim(anim_t type, int color, int x, int y){
    a_anim*  a=play->a;
    graphic* g=play->g;

    a++; /* Index 0 is always the ball move. */
    while(a->type != A_NONE) a++; /* Search free anim space. */
    /* XXX: Don't run over the end. */

    a->duration=0;
    a->color=color;
    switch(a->type=type){
	case A_DIE:
	    a->block[0].x=x/2;
	    a->block[0].y=y/2;
	    a->pixel[0].x=POS2X(x);
	    a->pixel[0].y=POS2Y(y);
	    break;
	case A_EXPLODE:
	    a->block[0].x=x;
	    a->block[0].y=y;
	    a->pixel[0].x=POSX(x);
	    a->pixel[0].y=POSY(y);
	    break;

	default:
	    a->type=A_NONE;
	    printf("Unknown type %d in create_moveanim\n",type);
	    break;
    };
}


void animate(int step){
    SDL_Rect rect,srect;
    int aidx=0;
    int x1,y1;
    graphic   *g=play->g;
    a_anim    *a=play->a;

    for (aidx=0;aidx < MAX_ANIM; aidx++){
	switch(a[aidx].type){
	    case A_NONE:
/*	printf("No animation here\n"); */
		break;

	    case A_BALL:
		for(x1=a[aidx].block[0].x;x1<=a[aidx].block[1].x;x1++)
		    for(y1=a[aidx].block[0].y;y1<=a[aidx].block[1].y;y1++)
			blank_block(x1,y1);

		/* Ball displaying is last, so the ball can fly through another
		 * anim (only one relevant is A_EXPLODE for now)
		 */
		break;

	    case A_DISK:
		blank_block(a[aidx].block[0].x,a[aidx].block[0].y);
		blank_block(a[aidx].block[1].x,a[aidx].block[1].y);

		rect.w=rect.h=QUAD;
		rect.x=a[aidx].pixel[0].x+(a[aidx].pixel[1].x*
			(step+AFRAMES*a[aidx].duration)/(2*AFRAMES));
		rect.y=a[aidx].pixel[0].y+(a[aidx].pixel[1].y*
			(step+AFRAMES*a[aidx].duration)/(2*AFRAMES));
		SDL_BlitSurface(g->disk[a[aidx].color], NULL, g->display, &rect);

		if(step == AFRAMES){ /* Disk Animation is be 2 Frames long */
		    if(++a[aidx].duration==2){
			a[aidx].type=A_NONE;
		    };
		};
		break;
	    case A_DIE:
		blank_block(a[aidx].block[0].x,a[aidx].block[0].y);

		rect.x=a[aidx].pixel[0].x;
		rect.y=a[aidx].pixel[0].y;

		srect.y=0;srect.x=(QUAD/2)*((a[aidx].duration*AFRAMES+step-1)>>1);
		srect.w=srect.h=QUAD/2;

		if(srect.x>g->ballx[a[aidx].color]->w){
		    a[aidx].type=A_NONE;
		}else{
		    SDL_BlitSurface(g->ballx[a[aidx].color], &srect,
			    g->display, &rect);
		};

		if(step == AFRAMES)
		    a[aidx].duration++;

		break;

	    case A_EXPLODE:
		blank_block(a[aidx].block[0].x,a[aidx].block[0].y);

		rect.x=a[aidx].pixel[0].x;
		rect.y=a[aidx].pixel[0].y;

		srect.y=0;srect.x=(QUAD)*((a[aidx].duration*AFRAMES+step-1)>>1);
		srect.w=srect.h=QUAD;

		if(srect.x>g->blockx[a[aidx].color]->w){
		    a[aidx].type=A_NONE;
		}else{
		    SDL_BlitSurface(g->blockx[a[aidx].color], &srect,
			    g->display, &rect);
		};

		if(step == AFRAMES)
		    a[aidx].duration++;

		break;

	    default:
		printf("Step: %d, idx: %d, ",step,aidx);
		printf("Unknown animate %d\n",a[aidx].type);
		break;
	};
    };

    if(a->type == A_BALL){
	rect.w=rect.h=QUAD/2;
	rect.x=a->pixel[0].x+(a->pixel[1].x*step/AFRAMES);
	rect.y=a->pixel[0].y+(a->pixel[1].y*step/AFRAMES);
	SDL_BlitSurface(g->ball[a->color], NULL, g->display, &rect);

	if(step == AFRAMES){ /* Ball Animation is one Frame long */
	    a->type=A_NONE;
	};
    };
}
