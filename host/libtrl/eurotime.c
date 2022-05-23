/* eurotime() - return the European style of the tim (hhmmss)
 *
 * Written by: TR Lowe 921220
 */

#include <time.h>

#ifdef __cplusplus
#include <stdlib.h>
#endif

eurotime( s )
char *s;
{
	long longtime;
	struct tm *timebuf;

	time( &longtime );

	timebuf = localtime( &longtime );

	sprintf( s, "%02d%02d%02d",
			timebuf->tm_hour,
			timebuf->tm_min,
			timebuf->tm_sec );

	return( 0 );
}
