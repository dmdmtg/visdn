#include "visdn.h"
#ifndef	USE730WINDOWS
#include <sys/jioctl.h>
#endif

extern char *TermProg;

Boolean	BootTerm(callapps, cols, rows, portnum, type, cachef, debug, popit, audix, features_monitored, leaveit, openit)
int callapps, cols, rows, portnum, cachef, debug, popit, audix, leaveit, openit;
char *type, *features_monitored;
{
	char cmd[500], download[300];
	char *dldcmd, *term;
	extern char *getenv();

#ifdef	USE730WINDOWS
	if (inlayers()==0)
#else
	if (ioctl(0, JMPX, 0) < 0)
#endif
	{
		(void) fprintf(stderr, "Not running layers -- aborting\r\n");
		return(FALSE);
	}

	dldcmd = "dmdld";
	term = "630";

	if (strcmp(TermProg, "fp3")==0 || strcmp(TermProg, "FP3")==0)
	{
		TermProg = "visdn3.m";
	} else if (strcmp(TermProg, "pro")==0 || strcmp(TermProg, "PRO")==0)
	{
		TermProg = "visdn.m";
	}

	/* Allow the user to specify a full pathname of the download
	 * file in TermProg.
	*/
	if (*TermProg=='/' || (getenv("JPATH") && *getenv("JPATH")))
	{
		/* let JPATH variable find download file */
		(void) strcpy(download, TermProg);
	} else if (getenv("TOOLS"))
	{
		/* else use TOOLS variable to find download file */
		(void) sprintf(download, "%s/lib/%s/%s", getenv("TOOLS"), term, TermProg);
	} else
	{
		(void) fprintf(stderr, "Cannot find %s (set JPATH or TOOLS) -- aborting\r\n", TermProg);
		return FALSE;
	}

	/* There MUST be a space after each argument and their options.
	 * The 730 program expects them.
	*/
	(void) sprintf(cmd, "%s %s -a %d -c %d -r %d -p %d -t %s -D %d %s %s %s %s %s %s 2>/dev/null",
		dldcmd, download, callapps, cols, rows,
		portnum, type, debug,
		features_monitored,
		cachef ? "-C" : "",
		popit ? "-P" : "",
		audix ? "-A" : "" ,
		leaveit ? "-E" : "",
		openit ? "-O" : "");
	if (system(cmd))
	{
		(void) fprintf(stderr, "Cannot download %s -- check JPATH or TOOLS\r\n", TermProg);
		(void) fprintf(stderr, "command line that failed was: %s\n", cmd);
		return(FALSE);
	}

	return (TRUE);
}
