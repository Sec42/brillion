/* Game physics/mechanics - i.e. How do things move inside the game
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: physics.c,v 1.14 2003/03/21 01:03:19 sec Exp $
 */
#include "brillion.h"


void move_step(signed int input){
  int x,y;
  int xn,yn;
  int ballx,bally;
  field *lvl=play->f;

  ballx=lvl->x;bally=lvl->y;

  x=lvl->x/2; y=lvl->y/2; 

  lvl->y+=lvl->dir; lvl->x+=input;
  xn=lvl->x/2; yn=lvl->y/2; 

  /* NOTE: In the original level 4 the following happens:
   * Frame 1: Ball inside disk (24,20), heading downward
   * Frame 2: Ball displaced into block, block is exploding
   * Frame 3: Ball bounced back into disk,
   * Frame 4: Ball bounces back into (now) free sqaure.
   *
   * Because this behaviour is hard to implement, and i doesn't matter for
   * gameplay, I moved the starting point of the ball into the block, and
   * 'hit' the block without moving simultaneously with the first frame.
   */
  if(PIECE(x,y)) /* Are we inside something? Can happen at start of level */
	move_touch(x,y,0,0);

  /* Where are we moving into? */
  if(xn==x){
    if(yn==y){
      /* inside of own square */;
    }else{
      if (move_touch(xn,yn,0,lvl->dir)){
	lvl->dir=-lvl->dir;
	lvl->y+=2*lvl->dir;
      }
    };
  }else{
    if(yn==y){
      if(move_touch(xn,yn,input,0)){
	input=-input;
	lvl->x+=2*input;
      };
    }else{ /* Tricky case: leaving square diagonally */
      if(PIECE(x,yn) && PIECE(xn,y)){
	/* XXX: What happens if both are STAR?
	 * XXX: both disks: This does only move one disk in the original.
	 */
	move_touch(x,yn,0,lvl->dir);
	move_touch(xn,y,input,0);
	input=-input;
	lvl->x+=2*input;
	lvl->dir=-lvl->dir;
	lvl->y+=2*lvl->dir;
      }else if(PIECE(x,yn)){
	if(move_touch(x,yn,0,lvl->dir)){
	  lvl->dir=-lvl->dir;
	  lvl->y+=2*lvl->dir;
	};
      }else if(PIECE(xn,y)){
	if(move_touch(xn,y,input,0)){
	  input=-input;
	  lvl->x+=2*input;
	};
      }else{
	if(move_touch(xn,yn,0,0)){
	  input=-input;
	  lvl->x+=2*input;
	  lvl->dir=-lvl->dir;
	  lvl->y+=2*lvl->dir;
	};
      };
    };
  };

  if(play->status == S_DIE){ /* Don't move if you die */
    lvl->x=ballx;
    lvl->y=bally;
  }else{
    create_moveanim(A_BALL,lvl->color,ballx,bally,lvl->x,lvl->y);
  };
  return;
}

int move_touch(int x, int y,signed int dx,signed int dy){
  int bounce=1;
  field *lvl=play->f;

  switch(PIECE(x,y)){
    case SPACE:
      bounce=0;
      break;
    case WALL:
    case OUTER_WALL:
      play_touch(PIECE(x,y));
      break;
    case DEATH:
      play_touch(DEATH);
      printf("You die, how embarrassing!\n");
      play->status=S_DIE; /* XXX: layering */
      break;
    case BLOCK:
      if(lvl->color==COLOR(x,y)){
	play_touch(BLOCK);
	PIECE(x,y)=SPACE;
	create_staticanim(A_EXPLODE,COLOR(x,y),x,y);
	if(--lvl->blocks ==0)
	  play->status=S_FINISH;

	play->points+=lvl->ppb;
      };
      break;
    case DISK:
      if(lvl->color==COLOR(x,y)){
	if(!PIECE(x+dx,y+dy)){
	  play_touch(DISK);
	  PIECE(x+dx,y+dy)=PIECE(x,y);
	  COLOR(x+dx,y+dy)=COLOR(x,y);
	  PIECE(x,y)=SPACE;
	  create_moveanim(A_DISK, COLOR(x,y), x, y, x+dx, y+dy);
	}
      };
      break;
    case STAR:
      play_touch(STAR);
      lvl->color=COLOR(x,y);
      break;
    default:
      printf("Whoops?\n");
  };
  return bounce;
}
