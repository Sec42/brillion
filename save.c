/* Save and load user actions
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: save.c,v 1.7 2003/03/26 18:26:39 sec Exp $
 */
#include "brillion.h"

a_save* init_save(){
    a_save*s;
    const char *g;
    int a;

    s=calloc(1,sizeof(a_save));
    s->len=1000;
    s->game=malloc(1001);
#ifdef SAVE
    s->what=R_RECORD;
#else
    s->what=R_NONE;
#endif
    if(0){
	s->len=strlen(g);
	s->what=R_PLAY;
	for(a=0;a<=s->len;a++){
	    if(g[a]=='<') s->game[a]=0;
	    if(g[a]=='.') s->game[a]=1;
	    if(g[a]=='>') s->game[a]=2;
	};
    };
    s->pos=0;

    return(s);
}

signed int handle_save(signed int dir){
    a_save *s=play->s;

    if(s->what==R_NONE)
	return(dir);

    if(s->what==R_RECORD){
	if(s->pos < s->len){
	    s->game[s->pos++]=dir+1;
	}else{
	    /* Whoops, overflow %) */
	};
	return(dir);
    };

    if(s->what==R_PLAY){
	if(s->pos < s->len){
	    return((signed int)s->game[s->pos++]-1);
	} else {
	    printf("FATAL: Ran off end of savegame\n");
	    exit(-1);
	};
    };

    printf("Save type %d not supported\n",s->what);
    exit(-1);
}

void print_save(a_save *s){
    int a;
    FILE *f;

    if(s->what!=R_RECORD)
	return;

    f=fopen(".save","a");
    fprintf(f,"%d:",play->level+1);
    for(a=0;a<s->pos;a++){
	putc("<.>"[(int)s->game[a]],f);
    };
    putc('\n',f);
    s->pos=0; /* XXX */
    fclose(f);
}
