#define EXTERN extern
#include "SDL_image.h"
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
	if(alpha)
	  if(*p4 == g->colors[WHITE])
	    *p4=g->colors[NONE];
      };
      break;
    case 2:
      for(p2=out->pixels;p2<(Uint16*)out->pixels+(out->w*out->h);p2++){
	if(*p2 == g->colors[BLUE])
	  *p2=g->colors[color];
	if(alpha)
	  if(*p2 == g->colors[WHITE])
	    *p2=g->colors[NONE];
      };
      break;
    case 1:
      for(p1=out->pixels;p1<(Uint8*)out->pixels+(out->w*out->h);p1++){
	if(*p1 == g->colors[BLUE])
	  *p1=g->colors[color];
	if(alpha)
	  if(*p1 == g->colors[WHITE])
	    *p1=g->colors[NONE];
      };
      break;
    default:
      fprintf(stderr,"This is a %d byte/pixel display, which is not supported\n",out->format->BytesPerPixel);
      fprintf(stderr,"You can try removing SDL_ANYFORMAT in graphics.c\n");

      exit(-1);
  };

  return(out);
};

void load_graphics(graphic * gp){
  SDL_Surface* pad;
  int x;

  gp->colors[RED]=     SDL_MapRGB(gp->display->format, 0xff, 0x00, 0x00);
  gp->colors[GREEN]=   SDL_MapRGB(gp->display->format, 0x00, 0xff, 0x00);
  gp->colors[BLUE]=    SDL_MapRGB(gp->display->format, 0x00, 0x00, 0xff);
  gp->colors[YELLOW]=  SDL_MapRGB(gp->display->format, 0xff, 0xff, 0x00);
  gp->colors[CYAN]=    SDL_MapRGB(gp->display->format, 0x00, 0xff, 0xff);
  gp->colors[MAGENTA]= SDL_MapRGB(gp->display->format, 0xff, 0x00, 0xff);
  gp->colors[WHITE]=   SDL_MapRGB(gp->display->format, 0xff, 0xff, 0xff);
  gp->colors[NONE]=    SDL_MapRGBA(gp->display->format,0,0,0,0);

  /* Load blocks, and create all colors */
  pad=IMG_Load("graphics/ball_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->ball[x]=color_graphic(gp,pad,x,1);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/block_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->block[x]=color_graphic(gp,pad,x,0);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/star_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->star[x]=color_graphic(gp,pad,x,0);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/disk_blue.gif");
  for(x=1;x<GAME_COLORS;x++)
    gp->disk[x]=color_graphic(gp,pad,x,0);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/space.gif");
  gp->back=SDL_ConvertSurface(pad, gp->display->format, SDL_HWSURFACE);
  assert(gp->back!=NULL);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/wall.gif");
  gp->wall=SDL_ConvertSurface(pad, gp->display->format, SDL_HWSURFACE);
  assert(gp->wall!=NULL);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/death.gif");
  gp->death=SDL_ConvertSurface(pad, gp->display->format, SDL_HWSURFACE);
  assert(gp->death!=NULL);
  SDL_FreeSurface(pad);

  pad=IMG_Load("graphics/bkg.png");
  if(pad){
    gp->xoff=20;gp->yoff=(gp->display->h - (QUAD*11))/2;
    SDL_BlitSurface(pad, NULL, gp->display, NULL);
    SDL_FreeSurface(pad);
  };

  //SDL_FillRect(gp->display,NULL,gp->colors[WHITE]);
};

graphic* init_graphic(){
  SDL_Surface* disp;
  graphic* g;

  if(SDL_Init(SDL_INIT_VIDEO)){
    fprintf(stderr,"Could not initialize SDL: %s.\n", SDL_GetError());
    exit(-1);
  };
  if(!(disp=SDL_SetVideoMode(MX, MY, 32, SDL_ANYFORMAT|SDL_DOUBLEBUF))){
    printf("Could not set videomode: %s.\n", SDL_GetError());
    exit(-1);
  };
  assert(SDL_MUSTLOCK(disp)==0);

  SDL_WM_SetCaption(prog, prog);

  g=calloc(1,sizeof(graphic));
  g->display=disp;

  load_graphics(g);

  return g;
}

/* -------------------------------------------------------------------- */

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
  rect.x=QUAD/2*(lvl->x-2)+g->xoff;
  rect.y=QUAD/2*(lvl->y-2)+g->yoff;
  SDL_BlitSurface(g->ball[lvl->color], NULL, g->display, &rect);

  return;
};

void paint_block(graphic* g, field* lvl, int x, int y){
  SDL_Rect rect;

  rect.w=rect.h=QUAD;
  rect.x=QUAD*(x-1)+g->xoff;
  rect.y=QUAD*(y-1)+g->yoff;

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

void fade (SDL_Surface* s, Uint32 ticks, int fadein){
  SDL_Surface* black, *copy;
  Uint32 old_time, curr_time;
  float alpha=0;

  black=SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);
  copy= SDL_CreateRGBSurface(SDL_HWSURFACE, s->w, s->h, s->format->BitsPerPixel, s->format->Rmask, s->format->Gmask, s->format->Bmask, s->format->Amask);

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

  SDL_FreeSurface(black);
  SDL_FreeSurface(copy);
}

