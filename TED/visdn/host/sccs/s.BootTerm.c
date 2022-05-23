h46665
s 00007/00005/00066
d D 1.4 92/12/03 12:26:41 trl 4 3
c tile after boot option -T
e
s 00008/00006/00063
d D 1.3 92/12/03 10:32:01 trl 3 2
c fix -O option boot
e
s 00010/00006/00059
d D 1.2 92/08/19 16:11:47 trl 2 1
c leave top
e
s 00065/00000/00000
d D 1.1 92/07/23 13:56:35 trl 1 0
c date and time created 92/07/23 13:56:35 by trl
e
u
U
t
T
I 1
#include "visdn.h"

#define	TERM5620	'7'
#define	TERM630		'8'

extern char *TermProg;

D 2
Boolean	BootTerm(callapps, cols, rows, portnum, type, cachef, debug, popit, audix)
int callapps, cols, rows, portnum, cachef, debug, popit, audix;
E 2
I 2
D 3
Boolean	BootTerm(callapps, cols, rows, portnum, type, cachef, debug, popit, leaveit, audix)
int callapps, cols, rows, portnum, cachef, debug, popit, leaveit, audix;
E 3
I 3
D 4
Boolean	BootTerm(callapps, cols, rows, portnum, type, cachef, debug, popit, leaveit, audix, openit)
int callapps, cols, rows, portnum, cachef, debug, popit, leaveit, audix, openit;
E 4
I 4
Boolean	BootTerm(callapps, cols, rows, portnum, type, cachef, debug, popit, leaveit, audix, openit, tileonboot)
int callapps, cols, rows, portnum, cachef, debug, popit, leaveit, audix, openit, tileonboot;
E 4
E 3
E 2
char *type;
{
	char cmd[80];
	char *dldcmd, *term;

	if (ioctl(0, JMPX, 0) < 0)
	{
		(void) fprintf(stderr, "not running layers -- aborting\n");
		return(FALSE);
	}

	dldcmd = "dmdld";
	term = "630";

	if (*TermProg=='/' || getenv("JPATH"))
	{
		/* let JPATH variable find viewmail.m */
D 2
		(void) sprintf(cmd, "%s %s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s 2>/dev/null",
E 2
I 2
D 3
		(void) sprintf(cmd, "%s %s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s %s 2>/dev/null",
E 3
I 3
D 4
		(void) sprintf(cmd, "%s %s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s %s %s 2>/dev/null",
E 4
I 4
		(void) sprintf(cmd, "%s %s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s %s %s %s 2>/dev/null",
E 4
E 3
E 2
			dldcmd, TermProg, callapps, cols, rows,
			portnum, type, debug, cachef ? "-C" : "",
D 2
			popit ? "-P" : "", audix ? "-A" : "" );
E 2
I 2
			popit ? "-P" : "",
			leaveit ? "-L" : "",
D 3
			audix ? "-A" : "" );
E 3
I 3
D 4
			audix ? "-A" : "" ,
E 4
I 4
			audix ? "-A" : "",
			tileonboot ? "-T" : "",
E 4
			openit ? "-O" : "");
E 3
E 2
	} else if (getenv("TOOLS"))
	{
		/* else use TOOLS variable to find viewmail.m */
D 2
		(void) sprintf(cmd, "%s %s/lib/%s/%s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s 2>/dev/null",
E 2
I 2
D 3
		(void) sprintf(cmd, "%s %s/lib/%s/%s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s %s 2>/dev/null",
E 3
I 3
D 4
		(void) sprintf(cmd, "%s %s/lib/%s/%s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s %s %s 2>/dev/null",
E 4
I 4
		(void) sprintf(cmd, "%s %s/lib/%s/%s -a%d -c%d -r%d -p%d -t%s -D%d %s %s %s %s %s %s 2>/dev/null",
E 4
E 3
E 2
			dldcmd, getenv("TOOLS"), term, TermProg,
			callapps, cols, rows, portnum, type, debug,
			cachef ? "-C" : "",
D 2
			popit ? "-P" : "", audix ? "-A" : "" );
E 2
I 2
			popit ? "-P" : "",
			leaveit ? "-L" : "",
D 3
			audix ? "-A" : "" );
E 3
I 3
			audix ? "-A" : "",
I 4
			tileonboot ? "-T" : "",
E 4
			openit ? "-O" : "" );
E 3
E 2
	} else
	{
		(void) fprintf(stderr, "cannot find %s (set JPATH or TOOLS) -- aborting\n", TermProg);
		return FALSE;
	}

	if (system(cmd))
	{
		(void) fprintf(stderr, "cannot download %s -- check JPATH or TOOLS", TermProg);
		return(FALSE);
	}

	return (TRUE);
}


char TermType()
{
	char buf[10];

	buf[5] = '\0';

	write(1, "\033[c", 3);
	(void) read(0, buf, 9);

	return(buf[5]);
}
E 1
