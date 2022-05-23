#include <stdio.h>

#ifdef __cplusplus
#include <string.h>
#endif

/* return a pointer into s, where s2 first begins
 */

char *
strinstr( s, s2 )
char *s, *s2;
{
	register len;

	len = strlen( s2 );

	if ( len > strlen( s ) )
		return( NULL );

	for ( ; *s ; s++ )
		if ( ! strncmp( s, s2, len ) )
			return( s );

	return( NULL );
}
