/* Simple Timer helper function, only ONE timer at a time
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: timer.c,v 1.4 2003/12/12 02:30:44 sec Exp $
 */
#include "brillion.h"

void init_timer(void){
    int x;
    x=SDL_Init(SDL_INIT_TIMER);
    if(x){
	fprintf(stderr,"WARNING: Initialising the SDL_TIMER subsystem failed\n");
	fprintf(stderr,"WARNING: Some automatic actions may fail\n");
    };
}

Uint32 MyTimerCallback(Uint32 interval, void *param){
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = *(int*)param;

    SDL_PushEvent(&event);

    *(int*)param=SDL_USER_NOTHING;
    return 0;
    /* further activations optional */
}

void time_event(unsigned int centiseconds,int eventtype){
    static SDL_TimerID no=NULL;
    static int type=SDL_USER_NOTHING;

    if(no){
	SDL_RemoveTimer(no);
	type=SDL_USER_NOTHING;
    };

    assert(type==SDL_USER_NOTHING);
    type=eventtype;

    no=SDL_AddTimer(centiseconds*100, MyTimerCallback, &type);

    if(no==NULL){
	fprintf(stderr,"WARNING: time_event failed. Go figure\n");
	type=SDL_USER_NOTHING;
    };
}
