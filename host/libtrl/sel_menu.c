#include <stdio.h>

#ifdef __cplusplus
#include <stdlib.h>
#endif

#define	MAXLINE	80

/* was sel_menu( i ) */
select_menu( i, raw )
int i;
int raw;
{
	char line[ MAXLINE ];
	int ret = -1;

	if ( raw )
		return( raw_select_menu( i ) );

	line[ 0 ] = '\0';

	while ( ret < 0 || ret > i - 1 )
	{
		fprintf( stderr, "Choose item (0-%d): ", i - 1 );
		fgets( line, MAXLINE, stdin );
		ret = atoi( line );
	}

	return( ret );
}
raw_select_menu( i )
int i;
{
	char ch;
	int ret = -1;

	setbuf( stdin, 0 );
	raw_term( 0 );

	while ( ret < 0 || ret > i - 1 )
	{
		fprintf( stderr, "Choose item (0-%d): ", i - 1 );

		read( 0, &ch, 1 );

		fprintf( stderr, "\r\n" );

		if ( ch == '\n' )
			ch = '0';

		ret = ch - '0';
	}

	unraw_term( 0 );

	return( ret );
}
