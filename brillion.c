/* Written by Sec <sec@42.org>
 * vim:set cin sm ts=4 sw=4:
 * $Id: brillion.c,v 1.1 2002/10/14 17:59:09 sec Exp $
 */

#define EXTERN /**/
#include "brillion.h"

int main(int argc,char **argv){
	/* Deklarationen */
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

	/* The getopt loop */
	while ((c = getopt(argc, argv, "z:vmh")) != EOF)
		switch (c)
		{
			case 'v':
				verbose++;
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

	game();

	fprintf(stderr,"%s: finished\n",prog);
	exit(0); // Juhuu!
}
