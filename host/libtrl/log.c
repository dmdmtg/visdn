#include <stdio.h>
#include <varargs.h>
#ifdef __cplusplus
#include <stdlib.h>
#endif

extern FILE *ptr_log_file;

log( va_alist )
va_dcl
{
	char nav0[ 16 ];

	va_list vp;
	char *fmt;

	if ( ptr_log_file == NULL )
		return( 1 );

	va_start( vp );

	fmt = va_arg( vp, char * );

       vfprintf( ptr_log_file, fmt, vp );
	fprintf( ptr_log_file, "\n" );

	va_end( vp );

	return( 0 );
}
