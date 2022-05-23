#include <stdio.h>
#include <varargs.h>
#ifdef __cplusplus
#include <stdlib.h>
#endif

extern int verbose;

run_sys_cmd( va_alist )
va_dcl
{
	char sys_cmd[ 256 ];

	va_list vp;
	char *fmt;

	va_start( vp );

	fmt = va_arg( vp, char * );

       vsprintf( sys_cmd, fmt, vp );

	va_end( vp );

	if ( verbose )
		inform( "EXECUTING: %s", sys_cmd );

	return( system( sys_cmd ) );
}
