/* main(). Paramter parsing and other setup.Written by Sec <sec@42.org>
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: brillion.c,v 1.15 2005/11/26 18:23:57 sec Exp $
 */

#define EXTERN /* Global variable(s) here... */
#include "brillion.h"

int main(int argc,char **argv){
#ifndef __WIN32__
    signed char c;
#endif

    /* die loudly if malloc runs out of ram */
#ifndef _POSIX_C_SOURCE
#ifdef __FreeBSD__
#if __FreeBSD__ > 4
    _malloc_options = "X";
#else
    extern char *malloc_options;
    malloc_options = "X";
#endif
#endif
#endif

    /* init section */
    b=calloc(1,sizeof(the_status)); 	/* This is global */
    assert(b!=NULL);

    b->prog=(char *)argv[0];
    if(!b->prog)b->prog="brillion";
    if(strrchr(b->prog,'/')){
	b->prog=strrchr(argv[0],'/');
	b->prog++;
    }

    /* XXX more sane checks please :)
    if(stat("/usr/X11R6/share/brillion",&sb)) */
    (void) chdir("/usr/X11R6/share/brillion");

    b->verbose=0;

#ifndef __WIN32__
    /* The getopt loop */
    while ((c = getopt(argc, argv, "g:l:vh")) != EOF)
	switch (c) {
	    case 'v':
		b->verbose++;
		break;
	    case 'g':
		b->game=read_game(optarg);
		if(!b->game)
		    die("Broken -g switch");
		break;
#ifdef DEVEL
	    case 'l': /* Ugly^99 */
		b->game=read_game("Original");
		b->game->maxlevel=1;
		b->game->levels[0]->name=optarg;
		break;
#endif
	    case 'h':
	    default:
		fprintf(stderr, "%s: Eine Crillion-Implementierung von Sec <sec@42.org> 10/2002\n\n", b->prog);
		exit(255);
	}
#endif

    if(!b->game)
	b->game=read_game("Original");

    if(!b->game)
	die("no game");

/*  argc -= optind; argv += optind; // we want more args

    // Check for argument errors
    if ( argc ){
	fprintf(stderr,"%s: Error: too many parameters\n",prog);
	exit(EXIT_FAILURE);
    } */

    run_game(b->game);

    fprintf(stderr,"%s: finished\n",b->prog);
    exit(0); /* Juhuu! */
}
