/* itopostfix() - form a "st", "nd", "rd", or "th" postfix for an integer
 *
 * Written by:	TR Lowe 861004 Copyright 1986 (c)  All rights reserved.
 *
 * Inputs:
 *		i - a character string integer to calculate postfix with
 *		p - a character string pointer to hold postfix
 * Outputs:
 *		p - variable pointed to by p will hold postfix
 * Returns:
 *		0 - successful
 *		1 - failed
 */

#ifdef __cplusplus
#include <string.h>
#include <stdlib.h>
#endif

	int
num_postfix( i, p )
char *i;
char *p;
{
	int j;

	/* handle 11, 12 and 13 specially */

	j = atoi( i ) % 100;

	if ( j == 11 || j == 12 || j == 13 )
	{
		strcpy( p, "th" );

		return( 0 );
	}

	switch( ( atoi( i ) % 10 ) )
	{
	case 1:
		strcpy( p, "st" );
		break;
	case 2:
		strcpy( p, "nd" );
		break;
	case 3:
		strcpy( p, "rd" );
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 0:
		strcpy( p, "th" );
		break;
	default:
		return( 1 );
	}

	return( 0 );
}
