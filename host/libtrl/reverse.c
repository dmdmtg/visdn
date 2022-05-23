/* reverse() - from k&r page 59 */

#ifdef __cplusplus
#include <string.h>
#endif

reverse( s )	/* reverse string s in place */
char s[];
{
	register int c, i, j;

	for ( i = 0, j = strlen( s ) - 1 ; i < j ; i++, j-- )
	{
		c = s[ i ];
		s[ i ] = s[ j ];
		s[ j ] = c;
	}
}
