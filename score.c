/* The highscore file reader/writer
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: score.c,v 1.21 2005/12/05 00:20:58 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>
#include <time.h>

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
#define MAXWORDS 8
    char* word[MAXWORDS+1];
    char*	trgt;
    unsigned int space;

    word[0]=(char*)malloc(LINELEN+1);

    f=fopen("Scores","r");

    if(!f){
	fprintf(stderr,"open Scores:");perror("");
	scores=calloc(1,sizeof(the_scores));
	scores->maxscore=-1;
	return(scores);
    };

    if( fscanf(f,"%10s %d ",word[0],&num) < 2 ||
	    strncmp(word[0],"Bscores",7) || (num != 42)){
	fprintf(stderr,"Scores: Corrupt file format\n");
	return(NULL);
    };

    scores=calloc(1,sizeof(the_scores));

    while(curscore<MAX_SCORES && !error && (fgets(word[0],LINELEN,f)!=NULL)){
	if((num=strlen(word[0]))==LINELEN){
	    fprintf(stderr,"Scores: Line too long\n");
	    error++;
	}else{
	    x=0;space=0;
	    for(trgt=word[0];x<MAXWORDS && trgt<word[0]+num;trgt++) {
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
	    }OR_PARSE("score",4){
		score=&scores->scores[curscore];
		strncpy(score->name,word[1],SCORENAMELEN);
		score->name[SCORENAMELEN-1]=0;
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

    if(error){
	fprintf(stderr,"Scores parse error at '%s'\n",word[0]);
	free(scores);
	free(word[0]);
	rename("Scores","Scores.broken");
	return(NULL);
    };

    scores->maxscore=curscore-1;
    free(word[0]);
    return scores;
}

void write_scores(the_scores* scores){
    int x;
    FILE	*f;

    if(scores==NULL){
	fprintf(stderr,"Error: no scores\n"); /* XXX: fixme; */
	return;
    };

    rename("Scores","Scores.bak");
    f=fopen("Scores","w");

    fprintf(f,"Bscores 42\n");

    printf("Writing %d scores\n",scores->maxscore);
    for(x=0;x<=scores->maxscore;x++){
	fprintf(f,"score %s %d %d %d\n",
		scores->scores[x].name,
		scores->scores[x].score,
		(int)scores->scores[x].when,
		scores->scores[x].howlong
	      );
    };
#ifndef __WIN32__
    fsync(fileno(f));
#endif
    fclose(f);
    unlink("Scores.bak");
    printf("Done.\n");
    return;
}

void add_score(the_scores* scores, int points){
    int x,y;

    if(scores==NULL){
	fprintf(stderr,"Error: no scores\n"); /* XXX: fixme; */
	return;
    };

    for(x=0;x<=scores->maxscore;x++){
	if(scores->scores[x].score<points)
	    break;
    };
    if(x<MAX_SCORES){ /* respectable rank */
	printf("New score is No. %d of %d\n",x,scores->maxscore);
	for(y=scores->maxscore;y>=x;y--){
	    if(y<MAX_SCORES-1)
		memcpy(&scores->scores[y+1],&scores->scores[y],sizeof(a_score));
	};
	if(scores->maxscore < MAX_SCORES-1)
	    scores->maxscore++;

	scores->scores[x].name[0]=1;
	scores->scores[x].name[1]=0;
	scores->scores[x].score=points;
	scores->scores[x].when=time(NULL);
	scores->scores[x].howlong=difftime(time(NULL),play->starttime);
    };
}

#ifndef TESTME

/* Plan:
// Display background
// List scores (possibly current one highlighted)
// Scrollable?
// Quit-able.
 */
void display_scores(void){
    SDL_Surface *img,*bkg,*s;
    SDL_Rect r,inp;
    int do_inp=-1;
    the_scores *scores;
    a_font *font;
    int start=0;
    int x;
    int w;

    scores=b->game->scores;

    if(scores==NULL){
	printf("WARNING: No scores to display\n");
	return;
    };

    s=play->g->display;
    font=play->g->tfont;

    img=IMG_Load("scores.png");
    assert(img!=NULL);
    
    /* Convert to native format. Is this really necessary/sensible? */
    bkg=SDL_ConvertSurface(img, s->format, SDL_SWSURFACE);
    SDL_FreeSurface(img);

    SDL_BlitSurface(bkg,NULL,s,NULL);
    SDL_Flip(s);

#define BORDER 75
#define SSZ ((480-BORDER)/(font->lineh)) /* Wieviele Eintraege/Bildschirm */

    r.y=BORDER;
    w=numwidth(font);
    for(x=0;x<=SSZ;x++){
	char buf[50];
	if(x+start>scores->maxscore)
	    break;

	r.x=10;r.w=0;
	if(scores->scores[x+start].name[0]==1){
	    /* Add Name here? */
	    inp.x=r.x;inp.y=r.y;inp.w=200;do_inp=x+start;
	}else{
	    nice_render_text(&r,scores->scores[x+start].name,font,0); 
	};

	render_num(340,r.y,w-2,scores->scores[x+start].score,font);
/*	render_text(250,50+x*font->lineh,"4h30m",font,0); */
	if(abs(difftime(time(NULL),scores->scores[x+start].when))>60*60*12){
	    strftime(buf,49,"%d.%m.",localtime(&scores->scores[x+start].when));
	}else{
	    strftime(buf,49,"(%H:%M)",localtime(&scores->scores[x+start].when));
	};
	render_fix(380,r.y,w-2,buf,font); 

	if(scores->scores[x+start].howlong > 60*60){
	    snprintf(buf,49,"%dh%d'",scores->scores[x+start].howlong/60/60,scores->scores[x+start].howlong/60%60);
	}else if (scores->scores[x+start].howlong > 60){
	    snprintf(buf,49,"%d'%d\"",scores->scores[x+start].howlong/60,scores->scores[x+start].howlong%60);
	}else{
	    snprintf(buf,49,"%d\"",scores->scores[x+start].howlong);
	};
	buf[49]=0;
	render_text(510,r.y,buf,font,0); 

	r.y+=font->lineh;
    }
    SDL_Flip(s);
    if(do_inp>-1){
	/* XXX: Windows doesn't do strlcpy alert
	strlcpy(scores->scores[do_inp].name,input_text(&inp,font),8); */
	strncpy(scores->scores[do_inp].name,input_text(&inp,font),SCORENAMELEN);
	if(scores->scores[do_inp].name[0]==0)
	    strncpy(scores->scores[do_inp].name,getuser(),SCORENAMELEN);
	scores->scores[do_inp].name[SCORENAMELEN-1]=0;
	write_scores(scores);
    };


    time_event(100,SDL_USER_ENDOFSCORES);

    while(1){
	SDL_Event event;
	while( SDL_PollEvent( &event ) ){
	    switch( event.type ){
		case SDL_KEYDOWN:
		    switch(event.key.keysym.sym){
			case SDLK_ESCAPE:
			case SDLK_q:
			    return;
			default:
			    break;
		    };
		    break;
		case SDL_USEREVENT:
		    switch(event.user.code){
			case SDL_USER_ENDOFSCORES:
			    return;
			default:
			    break;
		    };
		    break;
		case SDL_QUIT: /* SDL_QUIT event (window close) */
		    exit(-1); /* XXX */
		    break;
		default:
		    break;
	    }
	}
    }

}
#endif

#ifdef TESTME
void dump_scores(the_scores* scores){
    int x;
    int w;
    char buf[50];

    if(scores==NULL){
	printf("WARNING: No scores to display\n");
	return;
    };

    printf("High Scores\n");

    for(x=0;x<20;x++){
	if(x>scores->maxscore)
	    break;

	if(scores->scores[x].name[0]==1){
	    /* Huh? */
	    printf("Noname?\n");
	}else{
	    printf("Name: %s, ", scores->scores[x].name);
	};

	printf("Pts: %d, ", scores->scores[x].score);
	strftime(buf,49,"%d.%m. %H:%M",localtime(&scores->scores[x].when));
	printf("@: %s\n",buf);
    }
}

int main(int argc,char **argv){


    char inp[100];
    int run=1;
    the_scores* sc;

    do {
	printf("0) exit\n");
	printf("1) read\n");
	printf("2) write\n");
	printf("3) display\n");
	printf("4) add\n");
	gets(inp);
	switch(inp[0]){
	    case '1':
		sc=read_scores();
		break;
	    case '2':
		write_scores(sc);
		break;
	    case '3':
		dump_scores(sc);
		break;
	    case '4':
		add_score(sc,2222);
		break;
	    default: 
		run=0;
	};
    }while(run);

    printf("Danke, sie haben ein einfaches Testscript sehr gluecklich gemacht...\n\n");
    return(0);
};

#endif


