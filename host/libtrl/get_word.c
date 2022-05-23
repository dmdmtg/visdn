#include <stdio.h>

/* get_word - get the next white space separated word from a line 
 *
 * Written by:	TR Lowe 870713
 */

/* test routine:

main()
{
	char line[ 128 ], word[ 12], *ptr_line, *get_word();
	int wordnum;

	setbuf( stdout, 0 );

	printf("enter a line: ");
	getline(line,128);

	ptr_line = line;

	for( wordnum = 0, ptr_line = line ;
		( ptr_line = get_word( ptr_line, word ) ) != NULL ;
			wordnum++ )
	{
		printf("wordnum=%d,word=%s\n",wordnum,word);
	}
}
*/

char *
get_word( s, w )
char *s, *w;
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
