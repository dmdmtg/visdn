#include <stdio.h>

/*
 * getword - get the next white space separated word from a line 
 *
 * Written by:	TR Lowe 870713
 * Modified by:	DC Hervey 891002 - added length limit parameter
 */

/* test routine:

main()
{
	char line[ 128 ], word[ 12], *ptr_line, *getword();
	int wordnum;

	setbuf( stdout, 0 );

	printf("enter a line: ");
	getline(line,128);

	ptr_line = line;

	for( wordnum = 0, ptr_line = line ;
		( ptr_line = getword( ptr_line, word, 12 ) ) != NULL ;
			wordnum++ )
	{
		printf("wordnum=%d,word=%s\n",wordnum,word);
	}
}
*/

char *
getword( s, w, lim )
char *s, *w;
int lim;
{
	register word_len;

	if ( *s == '\0' || *s == '\n' )
		return( NULL );

	for ( ; ; s++ )
	{
		switch( *s )
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			continue;
		default:
			break;
		}
		break;
	}

	for ( w[ word_len = 0 ] = '\0' ; ; s++ )
	{
		switch( *s )
		{
		case ' ':
		case '\t':
		case '\0':
		case '\n':
			break;
		default:
			/* check for overflow */
			if ( ( word_len + 1 ) == lim )
			{
				w[ word_len ] = '\0';
				return( s );
			}
			w[ word_len++ ] = *s;
			continue;
		}
		break;
	}

	w[ word_len ] = '\0';

	if ( word_len == 0 )
		s = NULL;

	return( s );
}
