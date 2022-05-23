h22655
s 00003/00003/00465
d D 1.9 93/09/07 15:48:57 trl 9 8
c sun fix, null ptr in audix msg
e
s 00002/00002/00466
d D 1.8 93/01/04 12:58:39 trl 8 7
c restructure notify mail subjectline for mailsys
e
s 00007/00004/00461
d D 1.7 92/12/03 12:26:44 trl 7 6
c tile after boot option -T
e
s 00002/00003/00463
d D 1.6 92/12/03 10:44:17 trl 6 5
c openit is zero by default, must specify -O to enable
e
s 00037/00005/00429
d D 1.5 92/12/03 10:32:04 trl 5 4
c fix -O option boot
e
s 00084/00004/00350
d D 1.4 92/08/28 15:11:49 trl 4 3
c 
e
s 00007/00004/00347
d D 1.3 92/08/19 16:11:48 trl 3 2
c leave top
e
s 00024/00000/00327
d D 1.2 92/07/24 15:48:24 trl 2 1
c remove trailing spaces from str2 in MailNotify
e
s 00327/00000/00000
d D 1.1 92/07/23 13:56:37 trl 1 0
c date and time created 92/07/23 13:56:37 by trl
e
u
U
t
T
I 1
#include "visdn.h"
I 4
#include <sys/types.h>
#include <sys/stat.h>
E 4
#include <termio.h>

I 4
#include <signal.h>
extern int errno;
extern char *sys_errlist[];

E 4
#define DEFTYPE "7506"

char *TermProg = "visdn.m";
char RcvBuf[BUFSIZ];
char *LogFile = NULL;
char *DataFile = ".visdnrc";
char *IconPath = NULL;
char *LocalIconPath = NULL;
char *SpeedCallFile = NULL;

char *HostVersion = "2.7\n";

I 4
int pipepid, ttypid;
E 4
FILE *LogFd = NULL;

#define RCVMSG(cp, c) \
		for (cp=RcvBuf; read(0, &c, 1)==1 && c != '\n'; cp++) { \
			*cp = c; \
		} \
		*cp = '\0'

main(argc, argv)
int argc;
char *argv[];
{
	struct termio tbuf;
#if defined(vax)
	int c;
#else
	char c;
#endif /* vax || sun */
D 4
	int opt;
E 4
I 4
	int opt, p[2];
E 4
	char *cp;
I 4
	char buf[ 100 ];
	int n;
E 4
	char *mailaddr = NULL;
	char *type = DEFTYPE;
D 3
	int callapps, cols, rows, portnum, cacheit, debug, popit, audix;
E 3
I 3
D 5
	int callapps, cols, rows, portnum, cacheit, debug, popit, leaveit, audix;
E 5
I 5
D 7
	int callapps, cols, rows, portnum, cacheit, debug, popit, leaveit, audix, openit;
E 7
I 7
	int callapps, cols, rows, portnum, cacheit, debug, popit, leaveit, audix, openit, tileonboot;
E 7
E 5
E 3
	extern char *optarg;
	extern void SetRaw();
	extern void Exit();
	extern char *getenv();

D 3
	callapps = cols = rows = cacheit = debug = 0;
E 3
I 3
D 6
	callapps = cols = rows = cacheit = debug = popit = leaveit = 0;
I 5
	openit = 1;
E 6
I 6
D 7
	callapps = cols = rows = cacheit = debug = popit = leaveit = openit = 0;
E 7
I 7
	callapps = cols = rows = cacheit = debug = popit = leaveit = tileonboot = openit = 0;
E 7
E 6
E 5
E 3
	portnum = 1;

D 3
	while ((opt = getopt(argc, argv, "Aa:Cc:D:f:I:i:l:n:Pp:r:s:t:V")) != EOF)
E 3
I 3
D 5
	while ((opt = getopt(argc, argv, "Aa:Cc:D:f:I:i:l:Ln:Pp:r:s:t:V")) != EOF)
E 5
I 5
D 7
	while ((opt = getopt(argc, argv, "Aa:Cc:D:f:I:i:l:Ln:OPp:r:s:t:V")) != EOF)
E 7
I 7
	while ((opt = getopt(argc, argv, "Aa:Cc:D:f:I:i:l:Ln:OPp:r:s:Tt:V")) != EOF)
E 7
E 5
E 3
	{
		switch (opt)
		{
		case 'A':
			audix = 1;
			break;
		case 'a':	/* call appearances */
			callapps = atoi(optarg);
			break;
		case 'C':
			cacheit = 1;
			break;
		case 'c':
			cols = atoi(optarg);
			break;
		case 'D':
			debug = strtol(optarg, NULL, 0);
			break;
		case 'f':
			DataFile = optarg;
			break;
		case 'i':
			LocalIconPath = optarg;
			break;
		case 'I':
			IconPath = optarg;
			break;
		case 'l':
			LogFile = optarg;
			break;
I 3
		case 'L':
			leaveit = 1;
			break;
E 3
		case 'n':
			mailaddr = optarg;
			break;
		case 'p':
			portnum = atoi(optarg);
			break;
		case 'P':
			popit = 1;
			break;
I 7
		case 'T':
			tileonboot = 1;
			break;
E 7
I 5
		case 'O':
D 6
			openit = 0;
E 6
I 6
			openit = 1;
E 6
			break;
E 5
		case 'r':
			rows = atoi(optarg);
			break;
		case 's':
			SpeedCallFile = optarg;
			break;
		case 't':
			type = optarg;
			break;
		case 'V':
			printf("VISDN Host Version %s", HostVersion);
			exit(0);
			break;
		default:
			(void) fprintf(stderr, "usage: %s [-CPV] [-a callaps] [-c cols] [-f startfile ] [-I iconpath]\n\t[-i localicon] [-l logfile] [-n mailaddress] [-p port] [-r rows]\n\t[-s speedfile] [-t type]\n", argv[0]);
			exit(1);
		}
	}

	if (LogFile != NULL)
	{
		if ((LogFd = fopen(LogFile, "a"))==NULL)
		{
			(void) fprintf(stderr, "Cannot open %s\n", LogFile);
			exit(2);
		}
	}

	if (IconPath==NULL)
	{
		if ((cp = getenv("TOOLS")) != NULL)
		{
			IconPath = Malloc(strlen(cp)+strlen(DEFICONPATH)+1);
			strcpy(IconPath, cp);
			strcat(IconPath, DEFICONPATH);
		}
	}

	if (LocalIconPath==NULL)
	{
		if ((cp = getenv("HOME")) != NULL)
		{
			LocalIconPath = Malloc(strlen(cp)+strlen(DEFICONPATH)+1);
			strcpy(LocalIconPath, cp);
			strcat(LocalIconPath, DEFICONPATH);
		}
	}

	(void) ioctl(0, TCGETA, &tbuf);

	SetRaw();
	if (BootTerm(callapps, cols, rows, portnum, type, cacheit,
D 3
	    debug, popit, audix)==FALSE)
E 3
I 3
D 5
	    debug, popit, leaveit, audix)==FALSE)
E 5
I 5
D 7
	    debug, popit, leaveit, audix, openit)==FALSE)
E 7
I 7
	    debug, popit, leaveit, audix, openit,tileonboot)==FALSE)
E 7
E 5
E 3
		Exit(1);

I 4
	if( pipe( p ) == -1 )
	{
		fprintf( stderr, "%s: no pipes\n", argv[ 0 ] );
		Exit( 1 );
	}

I 5
/* parent writes to stdout (terminal), reads from two children on a pipe
 * child1 reads from stdin (terminal), writes to parent on a pipe
 * child2 reads from .visdnpipe, writes to parent on a pipe
 */

E 5
	if( ( ttypid = fork() ) == 0 )
	{
I 5
		/* child1: fd 0 is real stdin, fd 1 is pipe to parent */

E 5
		close( p[ 0 ] );
		close( 1 );
		dup( p[ 1 ] );
		close( p[ 1 ] );
		while( ( n = read( 0, buf, sizeof( buf ) ) ) > 0 )
		{
			write( 1, buf, n );
		}
		putchar( QUITCMD );
		exit( 1 );
	}

	if( ( pipepid = fork() ) == 0 )
	{
		int fd;
		char name[ 100 ], *h;

I 5
		/* child2: fd is .vidsnpipe, fd 1 is pipe to parent */

E 5
		close( p[ 0 ] );

		if( ( h = getenv( "HOME" ) ) == NULL )
		{
			h = ".";
		}

		sprintf( name, "%s/.visdnpipe", h );

		mknod( name, (mode_t) (0600|S_IFIFO), (dev_t)0 );
		chmod( name, (mode_t) 0600 );
		if( ( fd = open( name, 2 ) ) == -1 )
		{
			exit( 1 );
		}

		close( 1 );
		dup( p[ 1 ] );
		close( p[ 1 ] );
		while( ( n = read( fd, buf, sizeof( buf ) ) ) > 0 )
		{
			write( 1, buf, n );
		}

		putchar( QUITCMD );
		exit( 1 );
	}

	close( p[ 1 ] );
	close( 0 );
	dup( p[ 0 ] );
	close( p[ 0 ] );

I 5
	/* parent: fd 0 is input pipe from two children, fd 1 is real stdout */

E 5
E 4
	while (1)
	{
D 4
		if (read(0, &c, 1)==1)
E 4
I 4
		if( (n = read(0, &c, 1))==1)
E 4
		{
			c &= MSGMASK;
I 5
/*
fprintf( stderr, "c=%d\n", c );fflush(stderr);
*/
E 5
			switch (c&MSGMASK)
			{
			case QUITCMD:
				Exit(0);
				/* notreached */
			case INITTERM:
				SendCmd(ERASECHAR, &tbuf.c_cc[VERASE], 1);
				SendCmd(KILLCHAR, &tbuf.c_cc[VKILL], 1);
				SendCmd(HOSTVERSION, HostVersion, 0);
				ReadData(DataFile);
				break;
			case AUDIXMSG:
				MailNotify(mailaddr, "AUDIX Message", NULL, NULL);
				break;
			case LOADREQUEST:
				ReadData(DataFile);
				break;
			case CALLINFO:
				RCVMSG(cp, c);
				LogInfo(mailaddr, RcvBuf);
				break;
			case HELPTOC:
				HelpTOC();
				break;
#ifdef POSTQUERY
			case INFOREQ:
				RCVMSG(cp, c);
				InfoLookup(RcvBuf);
				break;
#endif /* POSTQUERY */
			case FACEREQ:
				RCVMSG(cp, c);
				InfoFace(RcvBuf);
				break;
			case HELPTOPIC:
				RCVMSG(cp, c);
				HelpTopic(RcvBuf);
				break;
			case SCALLSAVE:
				RCVMSG(cp, c);
				SpeedCallSave(SpeedCallFile, RcvBuf);
				break;
I 4
D 5
			case DIALNUM:
E 5
I 5
			case DIALNUM: /* octal 0353, decimal 235 */
E 5
				RCVMSG(cp, c);
				putchar( DOPOST );
				puts( RcvBuf );
I 5
/*
fprintf( stderr, "dialnum=%s\n", RcvBuf );fflush(stderr);
*/
E 5
				break;
I 5
			case RMTACT: /* octal 0354, decimal 236 */
				RCVMSG(cp, c);
				putchar( RMTACT0 );
				puts( RcvBuf );
fprintf( stderr, "rmtact=%s\n", RcvBuf );fflush(stderr);
/*
*/
				break;
E 5
E 4
			}
		}
I 4
		else if( n == 0 )
		{
			Exit( 1 );
		}
E 4
	}
}

void Exit(code)
int code;
{
	extern void SetCooked();

I 4
	if( pipepid > 0 )
		kill( pipepid, SIGTERM );
	if( ttypid > 0 )
		kill( ttypid, SIGTERM );
E 4
	SetCooked();
	exit(code);
}

static struct termio savetty;

void SetRaw()
{
	struct termio ttybuf;

	(void) ioctl(0, TCGETA, &ttybuf);
	savetty = ttybuf;

	ttybuf.c_iflag = IGNBRK | IGNPAR;
	ttybuf.c_oflag = 0;
	ttybuf.c_lflag = 0;
	ttybuf.c_cflag = ttybuf.c_cflag & (CBAUD | CLOCAL) | CREAD | CS8;
D 4
	ttybuf.c_cc[VMIN] = 0;
	ttybuf.c_cc[VTIME] = 30;
E 4
I 4
	ttybuf.c_cc[VMIN] = 1;
	ttybuf.c_cc[VTIME] = 0;
E 4

	(void) ioctl(0, TCSETA, &ttybuf);
}

void SetCooked()
{
D 5
	(void) ioctl(0, TCSETAW, &savetty);
E 5
I 5
	(void) ioctl(1, TCSETAW, &savetty);
E 5
}

SendCmd(cmd, s, cnt)
char cmd;
register char *s;
int cnt;
{
	char sendbuf[BUFSIZ];
	register int n;
	register char *cp;

	cp = sendbuf;
	*cp++ = '\n';	/* for synchronization */
	*cp++ = cmd;
	if (cnt==0)
	{
		for (n=2; s && *s; n++)
			*cp++ = *s++;
	} else
	{
		if (s==NULL)
			return;
		for (n=2; cnt--; n++)
			*cp++ = *s++;
	}

	write(1, sendbuf, n);
I 5
fprintf( stderr, "sendbuf=%s\n", sendbuf );fflush(stderr);
/*
*/
E 5
}

MailNotify(addr, subj, str1, str2)
char *addr, *subj, *str1, *str2;
{
	char sysbuf[BUFSIZ];
I 2
	char ch;
	char *p, *q;
E 2

I 2
D 9
	for ( q = NULL, p = str2 ; *p ; p++ )
E 9
I 9
	for ( q = NULL, p = str2 ; p && *p ; p++ )
E 9
	{
		if ( *p == ' ' )
		{
			if ( q == NULL )
				q = p;
		}
		else
		{
			q = NULL;
		}
	}

	if ( q != NULL )
	{
		ch = *q;
		*q = '\0';
	}

E 2
D 8
	(void) sprintf(sysbuf, "echo 'Subject: %s [%s] %s\\n' | /bin/mail %s\n",
			str2?str2:"", str1?str1:"", subj, addr );
E 8
I 8
	(void) sprintf(sysbuf, "echo 'Subject: %s [%s %s]\\n' | /bin/mail %s\n",
			subj, str2?str2:"", str1?str1:"", addr );
E 8
	(void) system(sysbuf);
I 2

	if ( q != NULL )
		*q= ch;
E 2
}

char *String(s)
register char *s;
{
	register char *cp;
	register int len;

	if (s==NULL)
		len = 0;
	else
		len = strlen(s);
	cp = Malloc(len+1);
	if (s)
		(void) strcpy(cp, s);
	else
		*cp = '\0';

	return cp;
}

#include <malloc.h>

char *Malloc(size)
unsigned size;
{
	register char *cp;
D 9
	extern char *malloc();
E 9
I 9
/*	extern char *malloc();*/
E 9

D 9
	if ((cp = malloc(size))==NULL)
E 9
I 9
	if ((cp = (char *) malloc(size))==NULL)
E 9
	{
		(void) fprintf(stderr, "malloc failed for size %d\n", size);
		exit(3);
	}

	return cp;
}

Free(s)
char *s;
{
	if (s)
	    free(s);
}

SpeedCallSave(f, s)
char *f, *s;
{
	char *list, *name, *value;
	FILE *fd;
	extern char *strtok();

	if (f)
	{
		if ((fd = fopen(f, "a")) != NULL)
		{
			list = strtok(s, ";");
			name = strtok(NULL, ";");
			value = strtok(NULL, ";");
			if (list && name && value)
				(void) fprintf(fd, "speedlist=%s\tname=%s\tvalue=%s\n",
					list, name, value);
			fclose(fd);
		}
	}
}
E 1
