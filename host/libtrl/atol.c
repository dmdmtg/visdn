/* atol - convert ASCII character strings to positive longs
 *
 * Written by:	TR Lowe 861012
 */

	long
atol( s )
char *s;
{
	long l = 0L;

	for ( ; *s >= '0' && *s <= '9' ; s++ )
		l = 10L * l + (long) ( *s - '0' );

	return( l );
}
