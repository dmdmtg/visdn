/* getline - get a line of input from stdin, return its length
 *
 * taken from: K&R
 * put in sccs:	TR Lowe 851116
 */

#ifdef __cplusplus
#include <stdlib.h>
#endif

#ifndef EOF
#define EOF	-1
#endif

	int
getline( s, lim )
char s[];
register int lim;
{
	register int c, i;

	i = 0;

	while ( --lim > 0 && ( c = getchar() ) != EOF && c != '\n' )
		s [ i++ ] = c;

	if ( c == '\n' )
		s[ i++ ] = c;

	s[ i ] = '\0';

	return( i );
}
