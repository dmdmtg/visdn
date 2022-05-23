/* eurodate() - return the European style of a date (yymmdd)
 *
 * Written by: TR Lowe 870916
 */

#include <time.h>

#ifdef __cplusplus
#include <stdlib.h>
#endif

eurodate( date )
char *date;
{
	long longtime;
	struct tm *timebuf;

	time( &longtime );

	timebuf = localtime( &longtime );

	sprintf( date, "%02d%02d%02d",
			timebuf->tm_year,
			timebuf->tm_mon + 1,
			timebuf->tm_mday );

	return( 0 );
}
