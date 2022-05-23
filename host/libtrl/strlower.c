#define	ISUPPER(x)	((x>='A')&&(x<='Z'))

strlower( to, from )
char *to, *from;
{
	for ( ; *from ; to++, from++ )
	{
		if ( ISUPPER( *from ) )
			*to = *from + 32;
		else
			*to = *from;
	}

	*to = '\0';
}
