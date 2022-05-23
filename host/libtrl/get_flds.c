#ifndef NULL_CH
#define	NULL_CH		'\0'
#endif

#ifndef NEWLINE_CH
#define	NEWLINE_CH	'\n'
#endif

#ifndef MAX_LINE
#define	MAX_LINE	256
#endif

#ifndef MAX_FLDS
#define	MAX_FLDS	64
#endif

void
get_flds( line, flds, fld_sep )
char line[ MAX_LINE ];
char flds[ MAX_FLDS ][ MAX_LINE ];
char fld_sep;
{
	register char *l;
	register fld_num, fld_len;

	for ( fld_num = 0 ; fld_num < MAX_FLDS ; fld_num++ )
		flds[ fld_num ][ 0 ] = NULL_CH;

	for ( l = line, fld_num = 0, fld_len = 0 ; *l != NULL_CH && *l != NEWLINE_CH ; l++ )
	{
		if ( *l == fld_sep )
		{
			flds[ fld_num++ ][ fld_len ] = NULL_CH;
			if ( fld_num >= MAX_FLDS )
				return;
			fld_len = 0;
		}
		else
		{
			flds[ fld_num ][ fld_len++ ] = *l;
		}
	}

	flds[ fld_num ][ fld_len ] = NULL_CH;
}
