/* The highscore file reader/writer
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: score.c,v 1.1 2003/12/05 01:31:01 sec Exp $
 */
#include "brillion.h"

#define MAX_SCORES 10

typedef struct {
    char    name[8];
    int	    score;
    time_t  when;
    int	    howlong;
/*    char    flags[8];  */
/*    int     csum;      */ 
} a_score;

typedef struct {
    a_score scores[MAX_SCORES];
    int	    maxscore;
} the_scores;

the_scores* read_scores(void){
    FILE	*f;
    the_scores  *scores;
    int curscore=0;
    a_score	*score;

    int num;
    int error=0;
    int x;


    /* for parsing */
#define LINELEN 200
    char* word[9];
    char*	trgt;
    unsigned int space;

    word[0]=(char*)malloc(LINELEN+1);

    f=fopen("Scores","r");

    if(!f){
	fprintf(stderr,"open Scores:");perror("");
	return(NULL);
    };

    fscanf(f,"%10s %d ",word[0],&num);
    if(strncmp(word[0],"Bscores",7) || (num != 42)){
	fprintf(stderr,"Scores: Corrupt file format\n");
	return(NULL);
    };

    scores=calloc(1,sizeof(the_scores));

    while(!error && (fgets(word[0],LINELEN,f)!=NULL)){
	if((num=strlen(word[0]))==LINELEN){
	    fprintf(stderr,"Scores: Line too long\n");
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

//      for(num=0;num<x;num++) fprintf(stderr,"%d: '%s'\n",num,word[num]);

#define BEG_PARSE(s,z) if(!strncmp(word[0],s,strlen(s)+1)){if((z+1)!=x){fprintf(stderr,"Directive %s requires %d words\n",s,(z+1));error++;}else
#define OR_PARSE(s,z) }else if(!strncmp(word[0],s,strlen(s)+1)){if((z+1)!=x){fprintf(stderr,"Directive %s requires %d words\n",s,(z+1));error++;}else
#define END_PARSE }else

	    BEG_PARSE("",0){
		/* printf("empty line!\n"); */
	    }OR_PARSE("score",4){
		score=&scores->scores[curscore];
		strncpy(score->name,word[1],8);
		score->name[7]=0;
		score->score=  atoi(word[2]);
		score->when=   atoi(word[3]);
		score->howlong=atoi(word[4]);
		curscore++;
	    }END_PARSE{
		fprintf(stderr,"'%s' is not a valid command\n",word[0]);
		error++;
	    };

	}; /* end if line !too long */
    }; /* end while */

    scores->maxscore=curscore-1;

    if(error){
	fprintf(stderr,"Scores parse error at '%s'\n",word[0]);
	free(scores);
	free(word[0]);
	return(NULL);
    };
    free(word[0]);
    return scores;
};

void write_scores(the_scores* scores){
    int x;
    for(x=0;x<=scores->maxscore;x++){
	printf("score %s %d %d %d\n",
		scores->scores[x].name,
		scores->scores[x].score,
		scores->scores[x].when,
		scores->scores[x].howlong
	      );
    };
    return;
};

