/* Glue code - Things that have to be done differently on certain OSs
 * vim:set cin sm ts=8 sw=4 sts=4: - Sec <sec@42.org>
 * $Id: portab.c,v 1.1 2004/06/21 10:39:35 sec Exp $
 */

#include "brillion.h"

#ifdef __WIN32__
#include <windows.h>
#else
#include <pwd.h>
#endif

/* Get (short) Name of the logged in User */

#ifdef __WIN32__
/* This uses a static buffer - NOT Reentrant */
char * getuser(void){
    static char acUserName[100];
    DWORD nUserName = sizeof(acUserName);
    if (GetUserName(acUserName, &nUserName)) {
	return(acUserName);
    } else {
	fprintf(stderr,"Failed to lookup user name, error code %d.\n", GetLastError());
    }
    return("<fail>");
}
#else
/* I found no doc if I have to free the (struct passwd*) - peeking in FreeBSDs
 * libc makes me think it is static, and thus not to be freed.
 */
char * getuser(void){
    struct passwd* pw;
    pw=getpwuid(getuid());
    if(pw){
	return(pw->pw_name);
    }else{
	fprintf(stderr,"Failed to get login name.\n");
	return("<fail>");
    };
};
#endif
