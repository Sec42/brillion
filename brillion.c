/* Written by Sec <sec@42.org>
 * vim:set cin sm ts=4 sw=4:
 * $Id: brillion.c,v 1.2 2002/10/15 11:39:24 sec Exp $
 */

#define EXTERN /**/
#include "brillion.h"

int main(int argc,char **argv){
	/* Deklarationen */
	config* cfg;
	char c;

#ifdef __FreeBSD__
	/* die loudly if malloc runs out of ram */
	extern char *malloc_options;
	malloc_options = "X";
#endif

	/* init section */
	prog=argv[0];
	if(!prog)prog="brillion";
	if(strrchr(prog,'/')){
		prog=strrchr(argv[0],'/');
		prog++;
	}
	verbose=0;
	cfg=calloc(1,sizeof(config));

	/* The getopt loop */
	while ((c = getopt(argc, argv, "l:vh")) != EOF)
		switch (c)
		{
			case 'v':
				verbose++;
				break;
			case 'l':
				cfg->onelevel++;
				strncpy(cfg->level,optarg,80);cfg->level[79]=0;
				break;
			case 'h':
			default:
				fprintf(stderr, "%s: Eine Crillion-Implementierung von Sec <sec@42.org> 10/2002\n\n", prog);
				exit(255);
		}

//	argc -= optind; argv += optind; /* we want more args */
//
//	/* Check for argument errors */
//	if ( argc ){
//		fprintf(stderr,"%s: Error: too many parameters\n",prog);
//		exit(-1);
//	}

	game(cfg);

	fprintf(stderr,"%s: finished\n",prog);
	exit(0); // Juhuu!
}
