/* itoa() - from k&r page 60 */

#ifdef __cplusplus
#include "libtrl.h"
#endif

itoa( n, s )	/* convert n to characters in s */
int n;
char s[];
{
	register int i, sign;

	if ( ( sign = n ) < 0 )
		n = -n;

	i = 0;

	do	/* generate digits in reverse order */
	{
		s[ i++ ] = n % 10 + '0';	/* get next digit */
	}
	while ( ( n /= 10 ) > 0 );	/* delete it */

	if ( sign < 0 )
		s[ i++ ] = '-';

	s[ i ] = '\0';

	reverse( s );
}
