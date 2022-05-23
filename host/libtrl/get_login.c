#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>

char *
get_login()
{
	char *strdup();
	struct passwd *pw, *getpwuid();
	uid_t uid, getuid();

	uid = getuid();

	setpwent();
	pw = getpwuid( uid );
	endpwent();

	if ( ! pw )
		return( NULL );
	else
		return( strdup( pw->pw_name ) );
}
