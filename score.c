/* The highscore file reader/writer
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: score.c,v 1.5 2003/12/11 01:21:27 sec Exp $
 */
#include "brillion.h"
#include <SDL_image.h>
#include <pwd.h>
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

/*      for(num=0;num<x;num++) fprintf(stderr,"%d: '%s'\n",num,word[num]); */

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
}

void write_scores(void){
    int x;
    the_scores* scores;
    FILE	*f;

    scores=b->game->scores;
    assert(scores!=NULL);

    rename("Scores","Scores.bak");
    f=fopen("Scores","w");

    fprintf(f,"Bscores 42\n");

    for(x=0;x<=scores->maxscore;x++){
	fprintf(f,"score %s %d %d %d\n",
		scores->scores[x].name,
		scores->scores[x].score,
		(int)scores->scores[x].when,
		scores->scores[x].howlong
	      );
    };
    fsync(fileno(f));
    fclose(f);
    unlink("Scores.bak");
    return;
}

void add_score(void){
    the_scores * scores;
    int x,y;
    struct passwd* pw;

    scores=b->game->scores;
    if(scores==NULL){
	scores=calloc(1,sizeof(the_scores));
	b->game->scores=scores;
	x=0;
    }else{
	if(scores->maxscore ==MAX_SCORES)
	    scores->maxscore--;

	for(x=0;x<=scores->maxscore;x++){
	    if(scores->scores[x].score<play->points)
		break;
	};
	for(y=scores->maxscore;y>=x;y--){
	    bcopy(&scores->scores[y],&scores->scores[y+1],sizeof(a_score));
	};
	scores->maxscore++;
    };

    pw=getpwuid(getuid());
    strlcpy(scores->scores[x].name,pw->pw_name,SCORENAMELEN);
    scores->scores[x].score=play->points;
    scores->scores[x].when=time(NULL);
    scores->scores[x].howlong=0;
}


/* Plan:
// Display background
// List scores (possibly current one highlighted)
// Scrollable?
// Quit-able.
 */
void display_scores(void){
    SDL_Surface *img,*bkg,*s;
    the_scores *scores;
    a_font *font;
    int start=0;
    int x;

    scores=b->game->scores;

    s=play->g->display;
    font=play->g->tfont;

    img=IMG_Load("scores.png");
    assert(img!=NULL);
    
    /* Convert to native format. Is this really necessary/sensible? */
    bkg=SDL_ConvertSurface(img, s->format, SDL_SWSURFACE);
    SDL_FreeSurface(img);

    SDL_BlitSurface(bkg,NULL,s,NULL);
    render_font(100,10,"High Scores");
    SDL_Flip(s);
    if(scores==NULL)
	return;

#define SSZ 10 /* Wieviele Eintraege/Bildschirm (640-border)/(font.lineh) */
    for(x=0;x<SSZ;x++){
	char buf[50];
	SDL_Rect r;
	if(x+start>scores->maxscore)
	    break;
	render_text(10,50+x*font->lineh,scores->scores[x+start].name,font,0);
	sprintf(buf,"%6d",scores->scores[x+start].score);
	size_text(&r,buf,font);
	render_text(250-r.w,50+x*font->lineh,buf,font,0);
/*	render_text(250,50+x*font->lineh,"4h30m",font,0); */
	strftime(buf,49,"%d.%m. %H:%M",localtime(&scores->scores[x+start].when));
	render_text(400,50+x*font->lineh,buf,font,0);
    }
    SDL_Flip(s);

    time_event(3,SDL_USER_ENDOFSCORES);

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

