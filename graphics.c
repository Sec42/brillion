#define EXTERN extern
#include <SDL_image.h>
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */

#define MX 640
#define MY 480

/* Seems like an ugly hack, but I know no sane way */
SDL_Surface* color_graphic(graphic* g, SDL_Surface* in, int color, int alpha){
  SDL_Surface* out;
  Uint32* p4;
  Uint16* p2;
  Uint8* p1;

  out=SDL_ConvertSurface(in, g->display->format, SDL_HWSURFACE);
  assert(out!=NULL);

  switch(out->format->BytesPerPixel){
    case 4:
      for(p4=out->pixels;p4<(Uint32*)out->pixels+(out->w*out->h);p4++){
	if(*p4 == g->colors[BLUE])
	  *p4=g->colors[color];
	if(alpha==1)
	  if(*p4 == g->colors[WHITE])
	    *p4=g->colors[NONE];
      };
      break;
    case 2:
      for(p2=out->pixels;p2<(Uint16*)out->pixels+(out->w*out->h);p2++){
	if(*p2 == g->colors[BLUE])
	  *p2=g->colors[color];
	if(alpha==1)
	  if(*p2 == g->colors[WHITE])
	    *p2=g->colors[NONE];
      };
      break;
    case 1:
      for(p1=out->pixels;p1<(Uint8*)out->pixels+(out->w*out->h);p1++){
	if(*p1 == g->colors[BLUE])
	  *p1=g->colors[color];
	if(alpha==1)
	  if(*p1 == g->colors[WHITE])
	    *p1=g->colors[NONE];
      };
      break;
    default:
      fprintf(stderr,"This is a %d byte/pixel display, which is not supported\n",out->format->BytesPerPixel);
      fprintf(stderr,"You can try removing SDL_ANYFORMAT in graphics.c\n");

      exit(-1);
  };
  if(alpha==2)
    SDL_SetColorKey(out, SDL_SRCCOLORKEY, g->colors[BG]);

  return(out);
};

void load_graphics(graphic * gp){
  SDL_Surface* pad;
  int x;

  gp->colors[RED]=     SDL_MapRGB(gp->display->format, 0xff, 0x00, 0x00);
  gp->colors[CYAN]=    SDL_MapRGB(gp->display->format, 0x00, 0xff, 0xff);
  gp->colors[MAGENTA]= SDL_MapRGB(gp->display->format, 0xff, 0x00, 0xff);
  gp->colors[GREEN]=   SDL_MapRGB(gp->display->format, 0x00, 0xff, 0x00);
  gp->colors[BLUE]=    SDL_MapRGB(gp->display->format, 0x00, 0x00, 0xff);
  gp->colors[YELLOW]=  SDL_MapRGB(gp->display->format, 0xff, 0xff, 0x00);
  gp->colors[WHITE]=   SDL_MapRGB(gp->display->format, 0xff, 0xff, 0xff);
  gp->colors[BG]=      SDL_MapRGB(gp->display->format, 0xff, 0x7f, 0x00);
  gp->colors[FG]=      SDL_MapRGB(gp->display->format, 0xff, 0x7f, 0x7f);
  gp->colors[NONE]=    SDL_MapRGBA(gp->display->format,0,0,0,0);

  /* Load blocks, and create all colors */
  pad=IMG_Load("ball_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->ball[x]=color_graphic(gp,pad,x,2);
  SDL_FreeSurface(pad);

  pad=IMG_Load("ballX.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->ballx[x]=color_graphic(gp,pad,x,2);
  SDL_FreeSurface(pad);

  pad=IMG_Load("block_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->block[x]=color_graphic(gp,pad,x,0);
  SDL_FreeSurface(pad);

  pad=IMG_Load("blockX.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->blockx[x]=color_graphic(gp,pad,x,2);
  SDL_FreeSurface(pad);

  pad=IMG_Load("star_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->star[x]=color_graphic(gp,pad,x,0);
  SDL_FreeSurface(pad);

  pad=IMG_Load("disk_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->disk[x]=color_graphic(gp,pad,x,0);
  SDL_FreeSurface(pad);

  pad=IMG_Load("space.gif");
  gp->back=SDL_ConvertSurface(pad, gp->display->format, SDL_HWSURFACE);
  assert(gp->back!=NULL);
  SDL_FreeSurface(pad);

  pad=IMG_Load("wall.gif");
  gp->wall=SDL_ConvertSurface(pad, gp->display->format, SDL_HWSURFACE);
  assert(gp->wall!=NULL);
  SDL_FreeSurface(pad);

  pad=IMG_Load("death.gif");
  gp->death=SDL_ConvertSurface(pad, gp->display->format, SDL_HWSURFACE);
  assert(gp->death!=NULL);
  SDL_FreeSurface(pad);

  // XXX: do that somewhere else...
  gp->border=IMG_Load("bkg.png");
  assert(gp->border!=NULL);
  gp->level.x=20;gp->level.y=(gp->display->h - (QUAD*11))/2; // XXX read from somewhere...
//  play->g->level.w=(play->f->w-1)*QUAD; play->g->level.h=(play->f->h-1)*QUAD;
  gp->level.w=(15)*QUAD; gp->level.h=(11)*QUAD; // XXX^2

  gp->font=IMG_Load("numbers.png");

  //SDL_FillRect(gp->display,NULL,gp->colors[WHITE]);
};

graphic* init_graphic(){
  SDL_Surface* disp;
  graphic* g;

  if(SDL_Init(SDL_INIT_VIDEO)){
    fprintf(stderr,"Could not initialize SDL: %s.\n", SDL_GetError());
    exit(-1);
  };
  if(!(disp=SDL_SetVideoMode(MX, MY, 24, SDL_ANYFORMAT))){
    printf("Could not set videomode: %s.\n", SDL_GetError());
    exit(-1);
  };
  assert(SDL_MUSTLOCK(disp)==0);
  printf("SDL uses %d bytes/pixel Display format\n",disp->format->BytesPerPixel);

  SDL_WM_SetCaption(b->prog, b->prog);

  g=calloc(1,sizeof(graphic));
  g->display=disp;

  g->rects=calloc(MAXRECTS,sizeof(SDL_Rect));
  g->numrects=0;

  load_graphics(g);

  return g;
}

/* -------------------------------------------------------------------- */
// Where is a Block
#define POSX(z) (QUAD*(z-1)+g->level.x)
#define POSY(z) (QUAD*(z-1)+g->level.y)
// Where is the Ball
#define POS2X(z) (QUAD/2*(z-2)+g->level.x)
#define POS2Y(z) (QUAD/2*(z-2)+g->level.y)


void paint_level(graphic* g, field* lvl){
  int x,y;

  for(y=1;y<lvl->h;y++)
    for(x=1;x<lvl->w;x++)
      paint_block(g,lvl,x,y);
};

void paint_ball(graphic* g, field* lvl){
  /* Paint new Ball */
  SDL_Rect rect;

  rect.w=rect.h=QUAD/2;
  rect.x=POS2X(lvl->x);
  rect.y=POS2Y(lvl->y);
  SDL_BlitSurface(g->ball[lvl->color], NULL, g->display, &rect);
  UPDATE(rect);

  return;
};

void blank_block(graphic* g, int x, int y){
  SDL_Rect rect;

  rect.w=rect.h=QUAD;
  rect.x=POSX(x);
  rect.y=POSY(y);

  SDL_BlitSurface(g->back, NULL, g->display, &rect);
  UPDATE(rect);
};

void paint_block(graphic* g, field* lvl, int x, int y){
  SDL_Rect rect;

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
};

void snapshot(graphic* g){
  static int q=0;
  char name[40];
  FILE* pic;
  unsigned char* x;

  if(g->display->format->BytesPerPixel!=4){
    fprintf(stderr,"Das geht nicht\n");
    return;
  };

  sprintf(name,"snap_%d.pnm",++q);
  pic=fopen(name,"w");
  fprintf(pic,"P6\n%d %d\n%d\n",g->display->w,g->display->h,255);
  for (x=g->display->pixels;
      x<(unsigned char*)(g->display->pixels+
	g->display->w * g->display->h * g->display->format->BytesPerPixel);
      x+=g->display->format->BytesPerPixel)
    fprintf(pic,"%c%c%c",*(x+2),*(x+1),*(x+0));
  fclose(pic);
}

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1

void print_number(graphic *g, char *str, int x, int y, int alignment){
  char *c;
  SDL_Rect dst,r;

  /* Font specifics ... */
  char fudge=5;
  char wid=25;
  char hei=31;

  r.w=dst.w=wid-fudge*2;
  r.h=dst.h=hei;
  r.y=0;

  dst.y=y;
  switch (alignment) {
  case ALIGN_RIGHT:
    dst.x=x-(strlen(str)*dst.w);
    break;


  default:
  case ALIGN_LEFT:
    dst.x=x;
  }

  for(c=str;*c!=0;c++){
    if(*c!=' '){
      int xoff=*c-'0';
      r.x=wid*xoff+fudge;
      SDL_BlitSurface(g->font, &r, g->display, &dst);
    }else{
      SDL_FillRect(g->display, &dst, g->colors[BLACK]);
    };
    UPDATE(dst);
    dst.x+=dst.w;
  }
};


void update_scoreboard(a_play *p){
  char t[10];

  sprintf(t,"%5d",p->points);
  print_number(p->g,t,600,150, ALIGN_RIGHT);

  sprintf(t,"%3d",p->f->blocks);
  print_number(p->g,t,600,200, ALIGN_RIGHT);

  sprintf(t,"%3d",p->f->time);
  print_number(p->g,t,600,250, ALIGN_RIGHT);

  sprintf(t,"%3d",p->lives);
  print_number(p->g,t,600,300, ALIGN_RIGHT);
};

a_anim* init_anim(){
  a_anim *a;
  a=calloc(MAX_ANIM,sizeof(a_anim));
  return a;
};

void create_moveanim(enum animations type, int color, int ox, int oy, int nx, int ny){
  a_anim*  a=b->p->a;
  graphic* g=b->p->g;
  int v;

  if(type!=A_BALL){ // Ball is always index 0.
    a++;
    while(a->type != A_NONE) a++; // Search free anim space.
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
};

void create_staticanim(enum animations type, int color, int x, int y){
  a_anim*  a=b->p->a;
  graphic* g=b->p->g;

  a++; // Index 0 is always the ball move.
  while(a->type != A_NONE) a++; // Search free anim space.
  // XXX: Don't run over the end.

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
};


void animate(graphic*g, a_anim*a, int step){
  SDL_Rect rect,srect;
  int aidx=0;
  int x1,y1;

  for (aidx=0;aidx < MAX_ANIM; aidx++){
    switch(a[aidx].type){
      case A_NONE:
//	printf("No animation here\n");
	break;

      case A_BALL:
	for(x1=a[aidx].block[0].x;x1<=a[aidx].block[1].x;x1++)
	  for(y1=a[aidx].block[0].y;y1<=a[aidx].block[1].y;y1++)
	    blank_block(g,x1,y1);

	// Ball displaying is last, so the ball can fly through another anim
	// (only one relevant is A_EXPLODE for now)
	break;

      case A_DISK:
	blank_block(g,a[aidx].block[0].x,a[aidx].block[0].y);
	blank_block(g,a[aidx].block[1].x,a[aidx].block[1].y);

	rect.w=rect.h=QUAD;
	rect.x=a[aidx].pixel[0].x+(a[aidx].pixel[1].x*(step+AFRAMES*a[aidx].duration)/(2*AFRAMES));
	rect.y=a[aidx].pixel[0].y+(a[aidx].pixel[1].y*(step+AFRAMES*a[aidx].duration)/(2*AFRAMES));
	SDL_BlitSurface(g->disk[a[aidx].color], NULL, g->display, &rect);

	if(step == AFRAMES){ // Disk Animation is be 2 Frames long
	  if(++a[aidx].duration==2){
	    a[aidx].type=A_NONE;
	  };
	};
	break;
      case A_DIE:
	blank_block(g,a[aidx].block[0].x,a[aidx].block[0].y);

	rect.x=a[aidx].pixel[0].x;
	rect.y=a[aidx].pixel[0].y;

	srect.y=0;srect.x=(QUAD/2)*((a[aidx].duration*AFRAMES+step-1)>>1);
	srect.w=srect.h=QUAD/2;

	if(srect.x>g->ballx[a[aidx].color]->w){
	  a[aidx].type=A_NONE;
	}else{
	  SDL_BlitSurface(g->ballx[a[aidx].color], &srect, g->display, &rect);
	};

	if(step == AFRAMES)
	  a[aidx].duration++;

	break;

      case A_EXPLODE:
	blank_block(g,a[aidx].block[0].x,a[aidx].block[0].y);

	rect.x=a[aidx].pixel[0].x;
	rect.y=a[aidx].pixel[0].y;

	srect.y=0;srect.x=(QUAD)*((a[aidx].duration*AFRAMES+step-1)>>1);
	srect.w=srect.h=QUAD;

	if(srect.x>g->blockx[a[aidx].color]->w){
	  a[aidx].type=A_NONE;
	}else{
	  SDL_BlitSurface(g->blockx[a[aidx].color], &srect, g->display, &rect);
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

	if(step == AFRAMES){ // Ball Animation is one Frame long
	    a->type=A_NONE;
	};
  };
};
