#define EXTERN extern
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "crillion.h"
/* vim:set cin sw=2 ts=8 sm: */

#define QUAD  32 /* Size of Standard Block, must be divisible by 2 */
#define SPEED 60 /* Ticks per Frame */

#define MX 640
#define MY 480
#define MZ 4

void rplot(SDL_Surface * disp, field * lvl, ball * b, graphic_pieces * gp, int user);
void paint_block(SDL_Surface * disp, field * lvl, graphic_pieces * gp, int x, int y);
void sdl(field lvl, ball b);
void draw_level(SDL_Surface * disp, field * lvl,graphic_pieces * gp);

void dwim(){
  
  /* 
Clvl 42
desc "A Hard days work" <sec@42.org>
xy 20 15
start 10 10 up
time 999
*/

#define COLOR(xx,yy) lvl.colors[(xx)*lvl.x+yy]
#define PIECE(xx,yy) lvl.pieces[(xx)*lvl.x+yy]

  FILE * level;
  char word[50];
  int num;
  field lvl;
  ball b;
  int error=0;
  int x,y;


  /* Schoenwetterparser.  Assumes no errors in file format */
  level=fopen("levels/o-02.lvl","r");

  fscanf(level,"%10s %d ",word,&num);
  if(strncmp(word,"Clvl",4) || (num != 42)){
    fprintf(stderr,"Corrupt level format\n");
    exit(1);
  };

  while((num=fscanf(level,"%10s",word))>0){
    char color,type;
    printf("Read: %d=%s\n",num,word);
    switch(word[0]){
      case 'd':
	if(strncmp(word,"desc",4))
	  error++;
	else
	  fgets(lvl.desc,79,level);
	  num=strlen(lvl.desc);
	  while(num-->0 && (lvl.desc[num] == '\r' || lvl.desc[num] == '\n'))
	    lvl.desc[num]=0;
	break;
      case 't':
	if(strncmp(word,"time",4))
	  error++;
	else
	  fscanf(level,"%d",&lvl.time);
	break;
      case 's':
	if(strncmp(word,"start",5))
	  error++;
	else{
	  fscanf(level,"%d %d %d %d",&b.x,&b.y,&b.dir,&b.color);
	  b.x++;b.y++;
	};
	break;
      case 'l':
	if(strncmp(word,"level",5))
	  error++;
	else{
	  lvl.blocks=0;
	  fscanf(level,"%d %d ",&lvl.x,&lvl.y);
	  lvl.x++;lvl.y++;
	  lvl.pieces=malloc((lvl.x+1)*(lvl.y+1)*sizeof(int));
	  lvl.colors=malloc((lvl.x+1)*(lvl.y+1)*sizeof(int));

	  for (x=0;x<=lvl.x;x++){
		PIECE(x,0)=WALL;
		PIECE(x,lvl.y)=WALL;
		PIECE(0,x)=WALL;
		PIECE(lvl.x,x)=WALL;
	  };

	  for (y=1;y<lvl.y;y++){
	    fgets(word,50,level);
	    for(x=1;x<lvl.x;x++){
	      switch(word[x-1]){
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8':
		  color=word[x-1]-'1'+1;
		  type=STAR;
		  break;
		case 'a': case 'b': case 'c': case 'd':
		case 'e': case 'f': case 'g': case 'h':
		  color=word[x-1]-'a'+1;
		  type=BLOCK;
		  lvl.blocks++;
		  break;
		case 'A': case 'B': case 'C': case 'D':
		case 'E': case 'F': case 'G': case 'H':
		  color=word[x-1]-'A'+1;
		  type=DISK;
		  break;
		case '#':
		  color=0;type=WALL;
		  break;
		case '*':
		  color=0;type=DEATH;
		  break;
		case '.':
		  color=0;type=SPACE;
		  break;
		case 0 :
		default:
		  printf("URGH! '%c'\n",word[x-1]);exit(1);break;
	      }
	      PIECE(x,y)=type;
	      COLOR(x,y)=color;
	    }
	    if(word[lvl.x]!='\n' && word[lvl.x]!='\r' && word[lvl.x]!=0){
		    printf("level line %d too long!\n",y);exit(1);
	    };
	  };
	};
	break;
      default:
	error++;
    };
    if(error){
      fprintf(stderr,"Error parsing level at '%s'\n",word);
      exit(-1);
    };

  };

  /* What does our level look like? */
  printf("Level: %s\n",lvl.desc);
  printf("Level: x=%d y=%d time=%d\n",lvl.x,lvl.y,lvl.time);
  printf("Ball: x=%d y=%d direction=%d color=%d\n",b.x,b.y,b.dir,b.color);
  for(y=0;y<=lvl.y;y++){
    for(x=0;x<=lvl.x;x++){
      if(COLOR(x,y))
	putchar('0'+COLOR(x,y));
      else
	putchar(' ');

      if(PIECE(x,y)!=SPACE)
	putchar(PIECE(x,y)+'@');
      else
	putchar(' ');
    };
    putchar('\n');
  };

  sdl(lvl,b);

};

  Mix_Chunk *music;
  Mix_Chunk *music2;
void sdl(field lvl, ball b){
  unsigned int x,y;
  unsigned int q;

  char quit=0;
  SDL_Surface * disp;
  SDL_Surface * pad;
  SDL_Event event;
  Uint32 now;
  Uint32 round;
  Sint32 round_d;
  graphic_pieces * gp;
  Uint32 sdl_colors[MAX_COLORS];
  SDL_Rect rect;
  signed int userx=0;
  Uint32 * sptr;
  Mix_Music *title;

  gp=malloc(sizeof(graphic_pieces));

  x=SDL_Init(SDL_INIT_VIDEO);
  SDL_WM_SetCaption(prog, prog);

  if(x==-1) {
    fprintf(stderr,"Could not initialize SDL: %s.\n", SDL_GetError());
    exit(-1);
  }
  if(!(disp=SDL_SetVideoMode(MX, MY, 24, SDL_ANYFORMAT|SDL_DOUBLEBUF))){
    printf("Could not SetVideoMode SDL: %s.\n", SDL_GetError());
    exit(-1);
  };
  assert(SDL_MUSTLOCK(disp)==0);
  assert(disp->format->BytesPerPixel==MZ);

  sdl_colors[RED]=     SDL_MapRGB(disp->format, 0xff, 0x00, 0x00);
  sdl_colors[GREEN]=   SDL_MapRGB(disp->format, 0x00, 0xff, 0x00);
  sdl_colors[BLUE]=    SDL_MapRGB(disp->format, 0x00, 0x00, 0xff);
  sdl_colors[YELLOW]=  SDL_MapRGB(disp->format, 0xff, 0xff, 0x00);
  sdl_colors[CYAN]=    SDL_MapRGB(disp->format, 0x00, 0xff, 0xff);
  sdl_colors[MAGENTA]= SDL_MapRGB(disp->format, 0xff, 0x00, 0xff);
  sdl_colors[WHITE]=   SDL_MapRGB(disp->format, 0xff, 0xff, 0xff);
  sdl_colors[NONE]=    SDL_MapRGBA(disp->format,0,0,0,0);

  pad=IMG_Load("graphics/ball_blue.gif");
  for(x=1;x<GAME_COLORS;x++){
    gp->ball[x]=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
    assert(gp->ball[x]->format->BytesPerPixel==4);
    assert(gp->ball[x]!=NULL);
    for(sptr=gp->ball[x]->pixels;sptr<(Uint32*)(gp->ball[x]->pixels+(gp->ball[x]->w*gp->ball[x]->h*4));sptr++){
      if(*sptr == sdl_colors[BLUE])
	*sptr=sdl_colors[x];
      if(*sptr == sdl_colors[WHITE])
	*sptr=sdl_colors[NONE];
    };
  };

  rect.w=rect.h=QUAD/2;
  rect.x=rect.y=QUAD/4;

  pad=IMG_Load("graphics/block_blue.gif");
  for(x=1;x<GAME_COLORS;x++){
    gp->block[x]=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
    assert(gp->block[x]->format->BytesPerPixel==4);
    assert(gp->block[x]!=NULL);
    for(sptr=gp->block[x]->pixels;sptr<(Uint32*)(gp->block[x]->pixels+(gp->block[x]->w*gp->block[x]->h*4));sptr++){
      if(*sptr == sdl_colors[BLUE])
	*sptr=sdl_colors[x];
    };
  };
  pad=IMG_Load("graphics/star_blue.gif");
  for(x=1;x<GAME_COLORS;x++){
    gp->star[x]=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
    assert(gp->star[x]->format->BytesPerPixel==4);
    assert(gp->star[x]!=NULL);
    for(sptr=gp->star[x]->pixels;sptr<(Uint32*)(gp->star[x]->pixels+(gp->star[x]->w*gp->star[x]->h*4));sptr++){
      if(*sptr == sdl_colors[BLUE])
	*sptr=sdl_colors[x];
    };
  };
  pad=IMG_Load("graphics/disk_blue.gif");
  for(x=1;x<GAME_COLORS;x++){
    gp->disk[x]=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
    assert(gp->disk[x]->format->BytesPerPixel==4);
    assert(gp->disk[x]!=NULL);
    for(sptr=gp->disk[x]->pixels;sptr<(Uint32*)(gp->disk[x]->pixels+(gp->disk[x]->w*gp->disk[x]->h*4));sptr++){
      if(*sptr == sdl_colors[BLUE])
	*sptr=sdl_colors[x];
    };
  };

  pad=IMG_Load("graphics/space.gif");
  gp->back=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
  assert(gp->back!=NULL);

  pad=IMG_Load("graphics/wall.gif");
  gp->wall=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
  assert(gp->wall!=NULL);

  pad=IMG_Load("graphics/death.gif");
  gp->death=SDL_ConvertSurface(pad, disp->format, SDL_HWSURFACE);
  assert(gp->death!=NULL);

  SDL_FreeSurface(pad);

  Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,1,1024);
  Mix_AllocateChannels(4);
  music=Mix_LoadWAV("sounds/hit_death.wav");
  assert(music);
  music2=Mix_LoadWAV("sounds/hit_star.wav");
  assert(music2);
  title=Mix_LoadMUS("sounds/Cymbals.mp3");
  Mix_PlayMusic(title, 1);
  assert(title);

  draw_level(disp,&lvl,gp);

  now = SDL_GetTicks();
  q=0;
  while(1){
    q++;
    round=SDL_GetTicks();
    rplot(disp, &lvl,&b, gp, userx);
    SDL_Flip(disp); 

    while( SDL_PollEvent( &event ) ){
      /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
      switch( event.type ){
	case SDL_KEYDOWN:
	  if( event.key.keysym.sym == SDLK_q){
	    quit=1;
	  } else if (event.key.keysym.sym == SDLK_b){
	    printf("blocks=%d\n",lvl.blocks);
	  } else if (event.key.keysym.sym == SDLK_d){
  /* What does our level look like? */
  printf("Level: %s\n",lvl.desc);
  printf("Level: x=%d y=%d time=%d\n",lvl.x,lvl.y,lvl.time);
  printf("Ball: x=%d y=%d direction=%d color=%d\n",b.x,b.y,b.dir,b.color);
  for(y=0;y<=lvl.y;y++){
    for(x=0;x<=lvl.x;x++){
      if(COLOR(x,y))
	putchar('0'+COLOR(x,y));
      else
	putchar(' ');

      if(PIECE(x,y)!=SPACE)
	putchar(PIECE(x,y)+'@');
      else
	putchar(' ');
    };
    putchar('\n');
  };
	  } else if (event.key.keysym.sym == SDLK_p){
	    char name[40];
	    FILE * pic;
	    unsigned char * x;
	    sprintf(name,"snap_%d",q);
	    pic=fopen(name,"w");
	    fprintf(pic,"P6\n%d %d\n%d\n",MX,MY,255);
	    for (x=disp->pixels;x<(unsigned char*)(disp->pixels+MX*MY*MZ);x+=4)
	      fprintf(pic,"%c%c%c",*(x+2),*(x+1),*(x+0));
	    fclose(pic);
	  } else if (event.key.keysym.sym == SDLK_RIGHT){
	    userx=1;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userx=-1;
	  };
	  break;

	case SDL_KEYUP:
	  if (event.key.keysym.sym == SDLK_RIGHT){
	    userx=0;
	  } else if (event.key.keysym.sym == SDLK_LEFT){
	    userx=-0;
	  };
	  break;
	case SDL_QUIT: /* SDL_QUIT event (window close) */
	  quit = 1;
	  break;

	default:
	  break;
      };
    }
    round_d=SPEED-(SDL_GetTicks()-round);
    if(round_d<0)
      printf("CPU to slow by %d ticks/round\n",round_d);
    else
      SDL_Delay(round_d);

    if(lvl.blocks==0){quit=1;};
    if(quit){
      fprintf(stderr,"%f frames / sec\n",(float)q/(SDL_GetTicks()-now)*1000);
      SDL_Quit();
      return;
    }
  };
};

#define setpix(x,y) \
  if((x<MX) && (y<MY) && (x>0) && (y>0) ) {\
  img[((MY-y)*MX+x)*MZ+0]=cr; \
  img[((MY-y)*MX+x)*MZ+1]=cg; \
  img[((MY-y)*MX+x)*MZ+2]=cb; \
  };

#define col(A) switch(A){ \
  case(BLUE):  cr=0;cg=0;cb=255;break;\
  case(GREEN): cr=0;cg=255;cb=0;break;\
  case(RED):   cr=255;cg=0;cb=0;break;\
  case(BLACK): cr=0;cg=0;cb=0;break;\
  };

#undef COLOR
#undef PIECE
#define COLOR(xx,yy) lvl->colors[(xx)*lvl->x+yy]
#define PIECE(xx,yy) lvl->pieces[(xx)*lvl->x+yy]
void draw_level(SDL_Surface * disp, field * lvl,graphic_pieces * gp){
  int x,y;
  SDL_Rect rect;
  rect.w=rect.h=QUAD;

  for(y=1;y<lvl->y;y++){
    rect.y=QUAD*(y-1);
    for(x=1;x<lvl->x;x++){
      rect.x=QUAD*(x-1);
      switch(PIECE(x,y)){
	case BLOCK:
	  SDL_BlitSurface(gp->block[COLOR(x,y)], NULL, disp, &rect);
	  break;
	case DISK:
	  SDL_BlitSurface(gp->disk[COLOR(x,y)], NULL, disp, &rect);
	  break;
	case STAR:
	  SDL_BlitSurface(gp->star[COLOR(x,y)], NULL, disp, &rect);
	  break;
	case WALL:
	  SDL_BlitSurface(gp->wall, NULL, disp, &rect);
	  break;
	case DEATH:
	  SDL_BlitSurface(gp->death, NULL, disp, &rect);
	  break;
	case SPACE:
	  SDL_BlitSurface(gp->back, NULL, disp, &rect);
	  break;
	default:
	  printf("Cannot draw %d\n",PIECE(x,y));
      };
    };
  };
};

void paint_block(SDL_Surface * disp, field * lvl, graphic_pieces * gp, int x, int y){
  SDL_Rect rect;
  rect.w=rect.h=QUAD;

  rect.y=QUAD*(y-1);
  rect.x=QUAD*(x-1);
  switch(PIECE(x,y)){
    case BLOCK:
      SDL_BlitSurface(gp->block[COLOR(x,y)], NULL, disp, &rect);
      break;
    case DISK:
      SDL_BlitSurface(gp->disk[COLOR(x,y)], NULL, disp, &rect);
      break;
    case STAR:
      SDL_BlitSurface(gp->star[COLOR(x,y)], NULL, disp, &rect);
      break;
    case WALL:
      SDL_BlitSurface(gp->wall, NULL, disp, &rect);
      break;
    case DEATH:
      SDL_BlitSurface(gp->death, NULL, disp, &rect);
      break;
    case SPACE:
      SDL_BlitSurface(gp->back, NULL, disp, &rect);
      break;
    default:
      printf("Cannot draw %d\n",PIECE(x,y));
  };
};

int TOUCH(SDL_Surface * disp, field * lvl, graphic_pieces * gp,ball * b,int x, int y,signed int xx,signed int yy){
  int bounce=1;

  switch(PIECE(x,y)){
    case SPACE:
      bounce=0;
      break;
    case WALL:
      break;
    case DEATH:
      printf("You die, how embarrassing!\n");
      Mix_PlayChannel(1,music, 0);
      break;
    case BLOCK:
      if(b->color==COLOR(x,y)){
	PIECE(x,y)=SPACE;
	paint_block(disp,lvl, gp,x,y);
	lvl->blocks--;
      };
      break;
    case DISK:
      if(b->color==COLOR(x,y)){
	if(!PIECE(x+xx,y+yy)){
	  PIECE(x+xx,y+yy)=PIECE(x,y);
	  COLOR(x+xx,y+yy)=COLOR(x,y);
	  PIECE(x,y)=SPACE;
	  paint_block(disp,lvl, gp,x+xx,y+yy);
	  paint_block(disp,lvl, gp,x,y);
	}
      };
      break;
    case STAR:
      b->color=COLOR(x,y);
      Mix_PlayChannel(-1,music2, 0);
      break;
    default:
      printf("Whoops?\n");
  };
  return bounce;
};

void rplot(SDL_Surface * disp, field * lvl, ball * b, graphic_pieces * gp,int user){
  SDL_Rect rect;
  int x,y;
  int xn,yn;

  /* Restore background */
  x=b->x/2; y=b->y/2; 
  paint_block(disp,lvl,gp,x,y);

  b->y+=b->dir; b->x+=user;

  xn=b->x/2; yn=b->y/2; 

  if(PIECE(x,y))
	TOUCH(disp,lvl,gp,b,x,y,0,0);
  if(xn==x){
    if(yn==y){
      /* inside of own square */;
    }else{
      if (TOUCH(disp,lvl,gp,b,xn,yn,0,b->dir)){
	b->dir=-b->dir;
	b->y+=2*b->dir;
      }
    };
  }else{
    if(yn==y){
      if(TOUCH(disp,lvl,gp,b,xn,yn,user,0)){
	user=-user;
	b->x+=2*user;
      };
    }else{
      if(PIECE(x,yn) && PIECE(xn,y)){ /* XXX: Tricky */;
	// does this really touch both? If so, what order
	// (This is relevant e.g. if both are type==STAR
	// If yes, does it really move DISKs? 
	TOUCH(disp,lvl,gp,b,x,yn,0,b->dir);
	TOUCH(disp,lvl,gp,b,xn,y,user,0);
	user=-user;
	b->x+=2*user;
	b->dir=-b->dir;
	b->y+=2*b->dir;
      }else if(PIECE(x,yn) ){
	if(TOUCH(disp,lvl,gp,b,x,yn,0,b->dir)){
	  b->dir=-b->dir;
	  b->y+=2*b->dir;
	};
      }else if(PIECE(xn,y) ){
	if(TOUCH(disp,lvl,gp,b,xn,y,user,0)){
	  user=-user;
	  b->x+=2*user;
	};
      }else{
	if(TOUCH(disp,lvl,gp,b,xn,yn,0,0)){
	  user=-user;
	  b->x+=2*user;
	  b->dir=-b->dir;
	  b->y+=2*b->dir;
	};
      };
    };
  };


  /* Paint new Ball */
  rect.w=rect.h=QUAD/2;
  rect.x=QUAD/2*(b->x-2);
  rect.y=QUAD/2*(b->y-2);
  SDL_BlitSurface(gp->ball[b->color], NULL, disp, &rect);

  return;
};
