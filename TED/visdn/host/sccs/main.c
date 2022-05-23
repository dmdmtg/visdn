#include "visdn.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <termio.h>

#include <signal.h>
extern int errno;
extern char *sys_errlist[];

#define DEFTYPE "7506"

char *TermProg = "visdn.m";
char RcvBuf[BUFSIZ];
char *LogFile = NULL;
char *DataFile = ".visdnrc";
char *IconPath = NULL;
char *LocalIconPath = NULL;
char *SpeedCallFile = NULL;

char *HostVersion = "2.7\n";

int pipepid, ttypid;
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
	int opt, p[2];
	char *cp;
	char buf[ 100 ];
	int n;
	char *mailaddr = NULL;
	char *type = DEFTYPE;
	int callapps, cols, rows, portnum, cacheit, debug, popit, leaveit, audix, openit, tileonboot;
	extern char *optarg;
	extern void SetRaw();
	extern void Exit();
	extern char *getenv();

	callapps = cols = rows = cacheit = debug = popit = leaveit = tileonboot = openit = 0;
	portnum = 1;

	while ((opt = getopt(argc, argv, "Aa:Cc:D:f:I:i:l:Ln:OPp:r:s:Tt:V")) != EOF)
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
		case 'L':
			leaveit = 1;
			break;
		case 'n':
			mailaddr = optarg;
			break;
		case 'p':
			portnum = atoi(optarg);
			break;
		case 'P':
			popit = 1;
			break;
		case 'T':
			tileonboot = 1;
			break;
		case 'O':
			openit = 1;
			break;
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
	    debug, popit, leaveit, audix, openit,tileonboot)==FALSE)
		Exit(1);

	if( pipe( p ) == -1 )
	{
		fprintf( stderr, "%s: no pipes\n", argv[ 0 ] );
		Exit( 1 );
	}

/* parent writes to stdout (terminal), reads from two children on a pipe
 * child1 reads from stdin (terminal), writes to parent on a pipe
 * child2 reads from .visdnpipe, writes to parent on a pipe
 */

	if( ( ttypid = fork() ) == 0 )
	{
		/* child1: fd 0 is real stdin, fd 1 is pipe to parent */

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

		/* child2: fd is .vidsnpipe, fd 1 is pipe to parent */

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

	/* parent: fd 0 is input pipe from two children, fd 1 is real stdout */

	while (1)
	{
		if( (n = read(0, &c, 1))==1)
		{
			c &= MSGMASK;
/*
fprintf( stderr, "c=%d\n", c );fflush(stderr);
*/
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
			case DIALNUM: /* octal 0353, decimal 235 */
				RCVMSG(cp, c);
				putchar( DOPOST );
				puts( RcvBuf );
/*
fprintf( stderr, "dialnum=%s\n", RcvBuf );fflush(stderr);
*/
				break;
			case RMTACT: /* octal 0354, decimal 236 */
				RCVMSG(cp, c);
				putchar( RMTACT0 );
				puts( RcvBuf );
fprintf( stderr, "rmtact=%s\n", RcvBuf );fflush(stderr);
/*
*/
				break;
			}
		}
		else if( n == 0 )
		{
			Exit( 1 );
		}
	}
}

void Exit(code)
int code;
{
	extern void SetCooked();

	if( pipepid > 0 )
		kill( pipepid, SIGTERM );
	if( ttypid > 0 )
		kill( ttypid, SIGTERM );
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
	ttybuf.c_cc[VMIN] = 1;
	ttybuf.c_cc[VTIME] = 0;

	(void) ioctl(0, TCSETA, &ttybuf);
}

void SetCooked()
{
	(void) ioctl(1, TCSETAW, &savetty);
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
fprintf( stderr, "sendbuf=%s\n", sendbuf );fflush(stderr);
/*
*/
}

MailNotify(addr, subj, str1, str2)
char *addr, *subj, *str1, *str2;
{
	char sysbuf[BUFSIZ];
	char ch;
	char *p, *q;

	for ( q = NULL, p = str2 ; p && *p ; p++ )
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

	(void) sprintf(sysbuf, "echo 'Subject: %s [%s %s]\\n' | /bin/mail %s\n",
			subj, str2?str2:"", str1?str1:"", addr );
	(void) system(sysbuf);

	if ( q != NULL )
		*q= ch;
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
/*	extern char *malloc();*/

	if ((cp = (char *) malloc(size))==NULL)
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
