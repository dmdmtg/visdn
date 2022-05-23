/* get_logdir - search password file for user's login directory
 *
 * Written by:	TR Lowe 861114
 */

#include <stdio.h>
#include <pwd.h>
#ifdef __cplusplus
#include <string.h>
#endif

get_logdir( login, login_dir, uid )
char *login,*login_dir;
int *uid;
{
	struct passwd *entry;

	if ( ( entry = getpwnam( login ) ) == NULL )
		return( 1 );

	strcpy( login_dir, entry->pw_dir );
	*uid = entry->pw_uid;

	return( 0 );
}
