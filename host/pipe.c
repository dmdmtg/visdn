/* This file is shared between the 730 and X versions.
 * Eric Claeys, 1/95
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PIPEFILE	".visdnpipe"
char *PipeFile = PIPEFILE;

extern	char	*Malloc();

/* Open the visdn pipe and return the fd. */
int
open_pipe(hang)
int	hang;	/* 1 == hang on open */
{
	int	pfd;
	char	*h;
	int	open_mode;
	extern	char	*getenv();

	if( ( h = getenv( "HOME" ) ) == NULL || *h == '\0')
	{
		h = ".";
	}

	PipeFile = (char *)Malloc((unsigned)(strlen(h)+1+strlen(PIPEFILE)+1));
	(void) sprintf( PipeFile, "%s/%s", h, PIPEFILE );

	/* Assume if these fail that the pipe is already there.
	 * Normally it won't be there, as it's removed before we exit.
	*/
	(void) mknod( PipeFile, (mode_t) (0600|S_IFIFO), (dev_t)0 );
	(void) chmod( PipeFile, (mode_t) 0600 );

	if (hang)
		open_mode = O_RDONLY;
	else
		open_mode = O_RDONLY | O_NDELAY;
	if( ( pfd = open( PipeFile, open_mode ) ) == -1 )
	{
		perror(PipeFile);
	} else {
		/* Some machines (e.g., Solaris 2.3) return a POLLHUP on
		 * a read of the pipe if the writer goes away.
		 * So, for example, if we "echo xyz > $PIPE", after the echo
		 * subsequent poll()s will return POLLHUP and we can never
		 * read anything from it.
		 * So, to get around this (rather than closing and reopening
		 * the pipe), open it for writing here, and don't do
		 * anything with the file descriptor (i.e., just leave it
		 * open).  This way, even after the "echo" dies, the pipe
		 * will still be open for writing.
		*/
		if (open( PipeFile, O_WRONLY ) == -1) {
			perror(PipeFile);
		}
	}
	return(pfd);

}
