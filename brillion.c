/* Written by Sec <sec@42.org>
 * vim:set cin sm ts=4 sw=4:
 * $Id: brillion.c,v 1.4 2003/02/25 16:53:04 sec Exp $
 */

#define EXTERN /**/
#include "brillion.h"

int main(int argc,char **argv){
	/* Deklarationen */
//	config* cfg;
	char c;
	struct stat sb;

	/* die loudly if malloc runs out of ram */
#ifdef __FreeBSD__
#if __FreeBSD__ > 4
	_malloc_options = "X";
#else
	extern char *malloc_options;
	malloc_options = "X";
#endif
#endif

	/* init section */
	b=calloc(1,sizeof(the_status)); 	// This is global

	b->prog=argv[0];
	if(!b->prog)b->prog="brillion";
	if(strrchr(b->prog,'/')){
		b->prog=strrchr(argv[0],'/');
		b->prog++;
	}

	// XXX more sane checks please :)
	if(stat("/usr/X11R6/share/brillion",&sb))
		chdir("/usr/X11R6/share/brillion");

	b->verbose=0;

	/* The getopt loop */
	while ((c = getopt(argc, argv, "g:l:vh")) != EOF)
		switch (c)
		{
			case 'v':
				b->verbose++;
				break;
			case 'g':
				b->game=read_game(optarg);
				if(!b->game)
					die("Broken -g switch");
				break;
			case 'l': // Ugly^99
				b->game=read_game("Original");
				b->game->maxlevel=1;
				b->game->levels[0]->name=optarg;
				break;
			case 'h':
			default:
				fprintf(stderr, "%s: Eine Crillion-Implementierung von Sec <sec@42.org> 10/2002\n\n", b->prog);
				exit(255);
		}

		if(!b->game)
			b->game=read_game("Original");

		if(!b->game)
			die("no game");

//	argc -= optind; argv += optind; /* we want more args */
//
//	/* Check for argument errors */
//	if ( argc ){
//		fprintf(stderr,"%s: Error: too many parameters\n",prog);
//		exit(-1);
//	}

	play_game(b->game);

	fprintf(stderr,"%s: finished\n",b->prog);
	exit(0); // Juhuu!
}
