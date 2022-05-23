h56982
s 00088/00000/00000
d D 1.1 92/07/23 13:56:36 trl 1 0
c date and time created 92/07/23 13:56:36 by trl
e
u
U
t
T
I 1
#include <time.h>
#include "visdn.h"

extern FILE *LogFd;

LogInfo(addr, buf)
char *addr, *buf;
{
	register char *cp;
	CallInfo info;
	long tmptime;
	char	time_buf[27];

	for (cp=buf, info.status=buf; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.callapp=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.calleddn=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.callingdn=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	if (*info.calleddn=='\0' && *info.callingdn=='\0')
		return;
	for (info.callednm=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.callingnm=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.cos=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.inid=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.miscinfo=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.fulldisp=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';
	for (info.datetime=cp; *cp && *cp != ';'; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	if (LogFd)
	{
		(void) time(&tmptime);
		(void) strcpy( time_buf, ctime(&tmptime) );
		time_buf[24] = '\0';	/* get rid of \n */
		(void) fprintf(LogFd, "%s    %s\t%s\t%s\t%s\n",
			time_buf,
			info.status,
			info.calleddn,
			info.callingdn,
			info.callingnm);
		(void) fflush(LogFd);
	}

	if (*info.status=='u' && *info.callingdn)
		MailNotify(addr, "called", info.callingdn, info.callingnm);
}
E 1
