/* The game settings file parser
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: game.c,v 1.7 2003/03/21 01:03:19 sec Exp $
 */
#include "brillion.h"

/* Game file format:
   first line has to be "Brillion <version>"
     only <version>==42 is currently supported
once:
   "lives \d+" (number of lives)
   
many:
   "bg \w+"    (background image filename)
   "level \w+" (level setup filename)
 */

a_game* read_game(const char * file){
    FILE	 * f;
    a_game * game;
    a_level level;
    int	  levelnum=0;

    int num;
    int error=0;
    int x;

    /* for parsing */
    char* word[9];
#define LINELEN 80
    char*	trgt;
    unsigned int space;

    assert(file);

    if(chdir(file)){
	fprintf(stderr,"Game '%s': ",file);perror("");
	return(NULL);
    };

    f=fopen("Settings","r");

    if(!f){
	fprintf(stderr,"Game '%s': Settings: ",file);perror("");
	return(NULL);
    };

    word[0]=(char*)malloc(LINELEN+1);

    fscanf(f,"%10s %d ",word[0],&num);
    if(strncmp(word[0],"Brillion",8) || (num != 42)){
	fprintf(stderr,"Game '%s': Corrupt file format\n",file);
	free(word[0]);
	return(NULL);
    };

    game=calloc(1,sizeof(a_game));
    game->layout=calloc(1,sizeof(a_layout));

    while(!error && (fgets(word[0],LINELEN,f)!=NULL)){
	if((num=strlen(word[0]))==LINELEN){
	    fprintf(stderr,"Game '%s': Line too long\n",file);
	    error++;
	}else{
	    x=0;space=0;
	    for(trgt=word[0];x<9 && trgt<word[0]+num;trgt++) {
		if( *trgt == ' ' || *trgt == '\t' || *trgt == '\r' || *trgt == '\n'){
		    *trgt=0;
		    space=1;
		}else{
		    if(space){
			space=0;
			word[++x]=trgt;
		    };
		};
	    };
	    word[++x]=NULL;

	    if(word[0][0] == '#'){ /* Handle comments */
		word[0][0]=0; x=1;
	    }

/*      for(num=0;num<x;num++) fprintf(stderr,"%d: '%s'\n",num,word[num]); */

#define BEG_PARSE(s,z) if(!strncmp(word[0],s,strlen(s)+1)){if((z+1)!=x){fprintf(stderr,"Directive %s requires %d words\n",s,(z+1));error++;}else
#define OR_PARSE(s,z) }else if(!strncmp(word[0],s,strlen(s)+1)){if((z+1)!=x){fprintf(stderr,"Directive %s requires %d words\n",s,(z+1));error++;}else
#define END_PARSE }else

	    BEG_PARSE("",0){
		/* printf("empty line!\n"); */
	    }OR_PARSE("cpts",2){
		game->layout->pts.x=atoi(word[1]);
		game->layout->pts.y=atoi(word[2]);
	    }OR_PARSE("ctime",2){
		game->layout->time.x=atoi(word[1]);
		game->layout->time.y=atoi(word[2]);
	    }OR_PARSE("cblocks",2){
		game->layout->blocks.x=atoi(word[1]);
		game->layout->blocks.y=atoi(word[2]);
	    }OR_PARSE("clives",2){
		game->layout->lives.x=atoi(word[1]);
		game->layout->lives.y=atoi(word[2]);
	    }OR_PARSE("clevel",2){
		game->layout->level.x=atoi(word[1]);
		game->layout->level.y=atoi(word[2]);
	    }OR_PARSE("lives",1){
		game->lives=atoi(word[1]);
	    }OR_PARSE("bg",1){
		printf("bg not impl.\n");
	    }OR_PARSE("level",1){
		if(levelnum>=game->maxlevel){
		    game->maxlevel+=21;
		    game->levels=realloc(game->levels,
			    game->maxlevel*(sizeof(a_level*)));
		};

		level.name=(char*)malloc(strlen(word[1])+1);
		strcpy(level.name,word[1]);

		game->levels[levelnum]=(a_level*)malloc(sizeof(a_level));
		memcpy(game->levels[levelnum],&level,sizeof(a_level));
		levelnum++;
	    }END_PARSE{
		fprintf(stderr,"'%s' is not a valid command\n",word[0]);
		error++;
	    };

	}; /* end if line !too long */
    }; /* end while */

    game->maxlevel=levelnum;

    if(game->lives == 0){
	fprintf(stderr,"Defaulting to 3 lives/game\n");
	game->lives=3;
    };

    if(error){
	fprintf(stderr,"Game '%s': Parse error at '%s'\n",file,word[0]);
	free(game);
	free(word[0]);
	return(NULL);
    };
    free(word[0]);
    return game;
}
