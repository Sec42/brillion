#define EXTERN extern
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */


void move_step(graphic* gp,music* m,field* lvl, signed int input){
  int x,y;
  int xn,yn;
  int ballx,bally;

  ballx=lvl->x;bally=lvl->y;

  x=lvl->x/2; y=lvl->y/2; 

//  paint_block(gp,lvl,x,y); /* Restore background */

  lvl->y+=lvl->dir; lvl->x+=input;
  xn=lvl->x/2; yn=lvl->y/2; 

  /* Are we inside something? Can happen at start of level */
  if(PIECE(x,y))
	move_touch(gp,m,lvl,x,y,0,0);

  /* Where are we moving into? */
  if(xn==x){
    if(yn==y){
      /* inside of own square */;
    }else{
      if (move_touch(gp,m,lvl,xn,yn,0,lvl->dir)){
	lvl->dir=-lvl->dir;
	lvl->y+=2*lvl->dir;
      }
    };
  }else{
    if(yn==y){
      if(move_touch(gp,m,lvl,xn,yn,input,0)){
	input=-input;
	lvl->x+=2*input;
      };
    }else{ /* Tricky case: leaving square diagonally */
      if(PIECE(x,yn) && PIECE(xn,y)){
	// XXX: What happens if both are STAR?
	// XXX: This does only move one disk in the original.
	move_touch(gp,m,lvl,x,yn,0,lvl->dir);
	move_touch(gp,m,lvl,xn,y,input,0);
	input=-input;
	lvl->x+=2*input;
	lvl->dir=-lvl->dir;
	lvl->y+=2*lvl->dir;
      }else if(PIECE(x,yn)){
	if(move_touch(gp,m,lvl,x,yn,0,lvl->dir)){
	  lvl->dir=-lvl->dir;
	  lvl->y+=2*lvl->dir;
	};
      }else if(PIECE(xn,y)){
	if(move_touch(gp,m,lvl,xn,y,input,0)){
	  input=-input;
	  lvl->x+=2*input;
	};
      }else{
	if(move_touch(gp,m,lvl,xn,yn,0,0)){
	  input=-input;
	  lvl->x+=2*input;
	  lvl->dir=-lvl->dir;
	  lvl->y+=2*lvl->dir;
	};
      };
    };
  };

  create_moveanim(A_BALL,lvl->color,ballx,bally,lvl->x,lvl->y);
  return;
};

int move_touch(graphic* gp, music* m, field* lvl, int x, int y,signed int dx,signed int dy){
  int bounce=1;

  switch(PIECE(x,y)){
    case SPACE:
      bounce=0;
      break;
    case WALL:
    case OUTER_WALL:
      play_touch(m, PIECE(x,y));
      break;
    case DEATH:
      play_touch(m, DEATH);
      printf("You die, how embarrassing!\n");
      lvl->blocks=-1; // XXX
      break;
    case BLOCK:
      if(lvl->color==COLOR(x,y)){
	play_touch(m, BLOCK);
	PIECE(x,y)=SPACE;
	paint_block(gp,lvl,x,y);
	lvl->blocks--;
	b->p->points+=lvl->ppb; // XXX: Layering
      };
      break;
    case DISK:
      if(lvl->color==COLOR(x,y)){
	if(!PIECE(x+dx,y+dy)){
	  play_touch(m, DISK);
	  PIECE(x+dx,y+dy)=PIECE(x,y);
	  COLOR(x+dx,y+dy)=COLOR(x,y);
	  PIECE(x,y)=SPACE;

	  create_moveanim(A_DISK, COLOR(x,y), x, y, x+dx, y+dy);
//	  paint_block(gp,lvl,x+dx,y+dy);
//	  paint_block(gp,lvl,x,y);
	}
      };
      break;
    case STAR:
      play_touch(m, STAR);
      lvl->color=COLOR(x,y);
      break;
    default:
      printf("Whoops?\n");
  };
  return bounce;
};
