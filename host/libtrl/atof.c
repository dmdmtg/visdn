/* atof() - convert string s to double
 *
 * Written by:	K&R page 69 - 861220
 */

	float
atof( s )
char *s;
{
	double val, power;
	int i, sign;

	for ( i = 0 ; s[ i ] == ' ' || s[ i ] == '\n' || s[ i ] == '\t' ; i++ )
		;	/* skip white space */

	sign = 1;

	if ( s[ i ] == '+' || s[ i ] == '-' )	/* sign */
		sign = ( s[ i++ ] == '+' ) ? 1 : -1;

	for ( val = 0 ; s[ i ] >= '0' && s[ i ] <= '9' ; i++ )
		val = 10 * val + s[ i ] - '0';

	if ( s[ i ] == '.' )
		i++;

	for ( power = 1 ; s[ i ] >= '0' && s[ i ] <= '9' ; i++ )
	{
		val = 10 * val + s[ i ] - '0';
		power *= 10;
	}

	return( (float) (sign * val / power ) );
}
