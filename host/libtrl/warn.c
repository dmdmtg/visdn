#include <stdio.h>
#include <varargs.h>
#ifdef __cplusplus
#include <stdlib.h>
#endif

extern char *av0;

warn( va_alist )
va_dcl
{
	char nav0[ 16 ];

	va_list vp;
	char *fmt;

	basename( av0, nav0 );

	va_start( vp );

	fmt = va_arg( vp, char * );

	fprintf( stderr, "%s: WARNING: ", nav0 );
       vfprintf( stderr, fmt, vp );
	fprintf( stderr, "\n" );

	va_end( vp );
}
