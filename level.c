#define EXTERN extern
#include "brillion.h"
/* vim:set cin sw=2 ts=8 sm: */

/* Level format:
   first line has to be "Clvl <version>"
     only <version>==42 is currently supported
   then (desc|start|time) in random order
   lastly "level <x> <y>"
   followed by the level in ascii

   1-6 are STARs in different colors
   a-f are BLOCKs
   A-F are DISKs
   *   is  DEATH
   #   is  WALL
   .   is  EMPTY
 */

field * read_level(char * file){
  FILE  * level;
  field * lvl;

  char word[50];
  int num;
  int error=0;
  int x,y;

  assert(file);
  level=fopen(file,"r");
  if(!level){
    fprintf(stderr,"Level '%s':",file);perror("Open");
    return(NULL);
  };

  fscanf(level,"%10s %d ",word,&num);
  if(strncmp(word,"Clvl",4) || (num != 42)){
    fprintf(stderr,"Level '%s': Corrupt level format\n",file);
    return(NULL);
  };

  lvl=calloc(1,sizeof(field));

  while((num=fscanf(level,"%10s",word))>0){
    char color,type;

    if(!strncmp(word,"desc",4)){
      if(!fgets(lvl->desc,80,level)){
	error++;
      }else{
	if((num=strlen(lvl->desc))==80){
	  fprintf(stderr,"Level '%s': Description too long\n",file);
	  error++;
	};
	while(num-->0 && (lvl->desc[num] == '\r' || lvl->desc[num] == '\n'))
	  lvl->desc[num]=0;
      };
    }else if(!strncmp(word,"time",4)){
      if(fscanf(level,"%d",&lvl->time)<1)
	error++;
    }else if(!strncmp(word,"start",5)){
      if(fscanf(level,"%d %d %d %d",&lvl->x,&lvl->y,&lvl->dir,&lvl->color)<4)
	error++;
      lvl->x++;lvl->y++;
    }else if(!strncmp(word,"level",5)){
      lvl->blocks=0;
      fscanf(level,"%d %d ",&lvl->w,&lvl->h);
      if((lvl->w != 15)||(lvl->h!=11)){ /* XXX: more dynamic ? */
	fprintf(stderr,"Level '%s': Level dimensions illegal\n",file);
	error++;
      }else if(lvl->pieces||lvl->colors){
	fprintf(stderr,"Level '%s': Duplicate level spec\n",file);
	error++;
      }else{
	lvl->w++;lvl->h++;
	lvl->pieces=malloc((lvl->w+1)*(lvl->h+1)*sizeof(int));
	lvl->colors=malloc((lvl->w+1)*(lvl->h+1)*sizeof(int));
	for (x=0;x<=lvl->w;x++){ // Wall around gamefield.
	  PIECE(x,0)=WALL;
	  PIECE(x,lvl->h)=WALL;
	}
	for (y=0;y<=lvl->h;y++){
	  PIECE(0,y)=WALL;
	  PIECE(lvl->w,y)=WALL;
	};

	for (y=1;y<lvl->h;y++){ // For each line.
	  fgets(word,50,level);
	  for(x=1;x<lvl->w;x++){
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
		lvl->blocks++;
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
		fprintf(stderr,"Level '%s': Illegal Character '%c' found at %d/%d\n",file,word[x-1],x,y);
		error++;
	    }
	    PIECE(x,y)=type;
	    COLOR(x,y)=color;
	  };
	  if(word[lvl->w]!='\n' && word[lvl->w]!='\r' && word[lvl->w]!=0){
	    fprintf(stderr,"Level '%s': Line %d too long\n",file,y);
	    error++;
	  };
	};
      };
    }else{ /* No known keyword */
      fprintf(stderr,"Level '%s': Unknown keyword encountered\n",file);
      error++;
    }
    if(error){
      fprintf(stderr,"Level '%s': Parse error at '%s'\n",file,word);
      if(lvl->pieces) free(lvl->pieces);
      if(lvl->colors) free(lvl->colors);
      free(lvl);
      return(NULL);
    };
  }; /* End of reading level */

  if(lvl->w == 0 || lvl->h == 0){
    fprintf(stderr,"Level '%s': Missing 'level' directive\n",file);
    error++;
  }else if(lvl->x<1 || lvl->y<1 || lvl->x/2 >= lvl->w || lvl->y/2 >= lvl->h){
    fprintf(stderr,"Level '%s': Missing 'start' directive, or ball out of Gamefield\n",file);
    error++;
  };

  if(error){
    if(lvl->pieces) free(lvl->pieces);
    if(lvl->colors) free(lvl->colors);
    free(lvl);
    return(NULL);
  };

  return lvl;
};

void dump_level(field * lvl){
  int x,y;

  printf("Level: %s\n",lvl->desc);
  printf("size=%d/%d time=%d\n",lvl->w-1,lvl->h-1,lvl->time);
  printf("ball=%d/%d direction=%d color=%d\n",
      lvl->x-1,lvl->y-1,lvl->dir,lvl->color);

  for(y=0;y<=lvl->h;y++){
    for(x=0;x<=lvl->w;x++){
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
};
