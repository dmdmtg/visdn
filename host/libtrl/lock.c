/* kernel level locking
 *
 *	write lock on whole file blocks other processes which wait
 *	fifo until the file is unlocked
 */

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

lock_file( s, p )
char *s;
FILE **p;
{
	struct flock l;

	if ( ( *p = fopen( s, "a+" ) ) == NULL )
	{
		fprintf( stderr, "cannot open lockfile %s, errno=%d\n",
			s, errno );
		return( 1 );
	}

	l.l_type = F_WRLCK;
	l.l_whence = 0;
	l.l_start = l.l_len = 0L;

        if ( fcntl( fileno( *p ), F_SETLKW, &l ) == -1 )
	{
		fprintf( stderr, "cannot lock lockfile %s, errno=%d\n",
			s, errno );
		fclose( *p );
		return( 2 );
	}

	return( 0 );
}
unlock_file( p )
FILE *p;
{
	if ( p != NULL )
		fclose( p );

	return( 0 );
}
