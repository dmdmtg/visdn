#include <stdio.h>

#define	MAXMENUITEMS	30
#define	MAXITEMLEN	80

pr_menu( menu )
char menu[ MAXMENUITEMS ][ MAXITEMLEN ];
{
	register i;

	fprintf( stderr, "\n" );

	for ( i = 0 ; *menu[ i ] ; i++ )
	{
		fprintf( stderr, "%2d) %s\n", i, menu[ i ] );
	}

	fprintf( stderr, "\n" );

	return( i );
}
