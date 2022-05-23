/* index - return the index into line of key, -1 if none
 *
 * written by:	TR Lowe 860327
 */

index( line, key )
char line[], key[];
{
	register int i, j, k;

	for ( i = 0; line[ i ] != 0; i++ )
	{
		for ( j = i, k = 0 ;
			key[ k ] != 0 && line[ j ] == key[ k ] ;
				j++, k++ )
			;

		if ( key[ k ] == 0 )
			return( i );
	}

	return( -1 );
}
