#include "brillion.h"
/* vim:set cin sw=4 ts=8 sm: */

a_save* init_save(){
    a_save*s;
    char *g=".........>>>>>>>>>>>>>>>>>>>>>>>>>>>..........................<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<..>>>>>>>>>>>>>>>>>>>>>>>>>...<<<........................<<<<<<....>>>..................<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<.>>>>>>>>>>>>>>>>>>>>>>>>>>>...>>>>>>......................<............<<<<<<<<<<<<<<<<<<<<<<<<<<<<.."; // Example for o-02.lvl
    int a;

    s=calloc(1,sizeof(a_save));
    s->len=1000;
    s->game=malloc(1001);
    s->what=R_RECORD;
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
};

signed int handle_save(a_save* s,signed int dir){
    if(s->what==R_NONE)
	return(dir);

    if(s->what==R_RECORD){
	if(s->pos < s->len){
	    s->game[s->pos++]=dir+1;
	    return(dir);
	};
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
};

void print_save(a_save *s){
    int a;

    return;

    for(a=0;a<s->pos;a++){
	putchar("<.>"[(int)s->game[a]]);
	if(a%70==69)
	    putchar('\n');
    };
    putchar('\n');
    s->pos=0; //XXX
};
