strsub( s, c1, c2 )
char *s;
char c1;
char c2;
{
	for ( ; *s ; s++ )
		if ( *s == c1 )
			*s = c2;
}
