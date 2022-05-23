#include "visdn.h"
#include <termio.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef NO_POLL
#include <stropts.h>
#include <poll.h>
#endif

#define DEFTYPE "7506"

int  debug = 0;
extern void	LogInfo(), FeatureStatus();
extern void	ReadData(), HelpTOC(), HelpTopic(), InfoFace();
extern int	InitLog(), InitDebug();
extern int	InfoLookup();
extern char	*getenv();

char *TermProg = NULL;
char *IconPath = NULL;
char *LocalIconPath = NULL;

static char RcvBuf[BUFSIZ];
static char *DataFile = ".visdnrc";
static char *SpeedCallFile = NULL;

static char *HostVersion = "4.0\n";

static char *LogFile = NULL;
static char *DebugFile = NULL;

extern	int	ProcessLogOptions();
static	char *FeatureFile = NULL;

#ifdef	NO_POLL
int	pipepid, ttypid;
#endif	/* NO_POLL */

#define RCVMSG(cp, c, fd) \
		for (cp=RcvBuf; read(fd, &c, 1)==1 && c != EOM; cp++) { \
			*cp = c; \
		} \
		*cp = '\0'

extern void MailNotify();
extern int open_pipe();
void SendCmd();
static void SpeedCallSave();

main(argc, argv)
int argc;
char *argv[];
{
	struct	termio tbuf;
#if defined(vax)
	int	c;
#else
	char	c;
#endif /* vax */
	int	opt;
	char	*cp;
	char	*mailaddr = NULL;
	char	*type = DEFTYPE;
	int	callapps, cols, rows, portnum, cacheit, popit, audix,
		leaveit, openit;
	char	features_monitored[200];   /* 200 should be enough space */
	char	*input_file;
	int	fd;
#ifdef	NO_POLL
	int	n, p[2];
	char	buf[100];
#else
	struct pollfd	fds[2];		/* read from 730 and pipe */
	size_t	num_fds;
#endif

	extern	char *optarg;
	static void SetRaw();
	static void DoExit();

	features_monitored[0] = '\0';
	callapps = cols = rows = cacheit = popit = audix = leaveit = openit = 0;
	portnum = 1;

	while ((opt = getopt(argc, argv, "Aa:Cc:D:EF:f:I:i:L:l:n:OPp:r:s:T:t:V")) != EOF)
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
			/* Arguments starting with a digit are debug levels,
			 * otherwise it's the file we write to.
			*/
			if (*optarg >= '0' && *optarg <= '9')
				debug = atoi(optarg);
			else
				DebugFile = optarg;
			break;
		case 'E':
			leaveit = 1;
			break;
		case 'F':
			/* Arguments starting with a digit are feature numbers,
			 * otherwise it's the name of the file we write to.
			*/
			if (*optarg >= '0' && *optarg <= '9') {
				(void) strcat(features_monitored, " -F ");
				(void) strcat(features_monitored, optarg);
			} else
				FeatureFile = optarg;
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
		case 'L':
			(void) ProcessLogOptions(optarg);
			break;
		case 'l':
			LogFile = optarg;
			break;
		case 'n':
			mailaddr = optarg;
			break;
		case 'O':
			openit = 1;
			break;
		case 'p':
			portnum = atoi(optarg);
			break;
		case 'P':
			popit = 1;
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
		case 'T':
			TermProg = optarg;
			break;
		case 'V':
			(void) printf("VISDN Host Version %s", HostVersion);
			exit(0);
			break;
		default:
			(void) fprintf(stderr, "Usage: %s [-ACEOPV] [-D debug#] [-a callaps]\n", argv[0]);
			(void) fprintf(stderr, "\t[-c cols] [-f startfile ] [-I iconpath] [-i localicon]\n");
			(void) fprintf(stderr, "\t[-l logfile] [-L logoptions] [-n mailaddress] [-p port]\n");
			(void) fprintf(stderr, "\t[-r rows] [-s speedfile] [-t type] [-T pro|fp3]\n");
			(void) fprintf(stderr, "\t[-F feature [...]]\n");
			exit(1);
		}
	}

	if (TermProg==NULL)
	{
		(void) fprintf(stderr, "Must specify -Tpro or -Tfp3\n");
		exit(1);
	}

	if ( ! InitLog(LogFile))
	{
		exit(2);	/* error message printed in InitLog() */
	}

	if ( ! InitDebug(DebugFile))
	{
		exit(2);	/* error message printed in InitDebug() */
	}

	if ( ! InitFeature(FeatureFile))
	{
		exit(2);	/* error message printed in InitFeature() */
	}

	if (IconPath==NULL)
	{
		if ((cp = getenv("TOOLS")) != NULL)
		{
			IconPath = Malloc((unsigned)(strlen(cp)+strlen(DEFICONPATH)+1));
			(void) strcpy(IconPath, cp);
			(void) strcat(IconPath, DEFICONPATH);
		}
	}

	if (LocalIconPath==NULL)
	{
		if ((cp = getenv("HOME")) != NULL)
		{
			LocalIconPath = Malloc((unsigned)(strlen(cp)+strlen(DEFICONPATH)+1));
			(void) strcpy(LocalIconPath, cp);
			(void) strcat(LocalIconPath, DEFICONPATH);
		}
	}

	(void) ioctl(0, TCGETA, &tbuf);

	SetRaw();
	if (BootTerm(callapps, cols, rows, portnum, type, cacheit,
	    debug, popit, audix, features_monitored, leaveit, openit)==FALSE)
		DoExit(1);

#ifdef	NO_POLL
	/* Some older machine don't have the poll() call,
	 * so we have to create a couple child processes to be able to
	 * read from a couple files at once.
	*/
	if( pipe( p ) == -1 )
	{
		(void) fprintf( stderr, "%s: no pipes\n", argv[ 0 ] );
		perror("");
		DoExit( 1 );
	}

	/* Parent writes to stdout (terminal), reads from two children on a pipe.
	 * Child1 reads from stdin (terminal), writes to parent on a pipe.
	 * Child2 reads from .visdnpipe, writes to parent on a pipe.
	*/

	if( ( ttypid = fork() ) == 0 )
	{
		/* child1: fd 0 is real stdin, fd 1 is pipe to parent */

		(void) close( p[ 0 ] );
		(void) close( 1 );
		(void) dup( p[ 1 ] );
		(void) close( p[ 1 ] );
		while( ( n = read( 0, buf, sizeof( buf ) ) ) > 0 )
		{
			(void) write( 1, buf, n );
		}
		(void) putchar( QUITCMD );
		exit( 1 );
	}

	if( ( pipepid = fork() ) == 0 )
	{
		int pipefd;

		/* child2: fd is .vidsnpipe, fd 1 is pipe to parent */

		(void) close( p[ 0 ] );

		pipefd = open_pipe(1);	/* will get fd 0 */
		if (pipefd == -1)
			exit(1);

		(void) close( 1 );
		(void) dup( p[ 1 ] );
		(void) close( p[ 1 ] );
		while( ( n = read( pipefd, buf, sizeof( buf ) ) ) > 0 )
		{
			(void) write( 1, buf, n );
		}

		(void) putchar( QUITCMD );
		exit( 1 );
	}

	(void) close( p[ 1 ] );
	(void) close( 0 );
	(void) dup( p[ 0 ] );
	(void) close( p[ 0 ] );

	/* parent: fd 0 is input pipe from two children, fd 1 is real stdout */

	fd = 0;			/* fd for parent to read from */
	input_file = "730";	/* all input will always come from 730 */

#else /* DO have poll() */

	fds[0].fd = 0;			/* from 730 */
	fds[0].events = POLLIN;
	fds[1].fd = open_pipe(0);	/* from pipe (don't hang on open) */
	if (fds[1].fd >= 0) {
		num_fds = 2;
		fds[1].events = POLLIN;
	} else
		num_fds = 1;
#endif	/* NO_POLL */

	for (;;)
	{
#ifndef	NO_POLL
		/* This is more tricky since there are 2 fd's to read from,
		 * and we want to make sure we read 1 line at a time from
		 * the current fd (i.e., we don't want to read one char from
		 * the 730 and then 1 from the pipe, etc.).
		*/
		if (poll(fds, num_fds, INFTIM) == -1) {
/* XXX Really should send message to 730 side */
			perror("poll");
			DoExit(2);
		}
		if (fds[0].revents & POLLIN) {
			fd = fds[0].fd;
			input_file = "730";
		} else if (fds[1].revents & POLLIN) {
			fd = fds[1].fd;
			input_file = "pipe";
		} else {
			int	revents;
			char	*where;

			/* Check for error (e.g., EOF) on input. */
			if (fds[0].revents != 0) {
				where = "phone";
				revents = fds[0].revents;
			}
			if (fds[1].revents != 0) {
				where = "command pipe";
				revents = fds[1].revents;
			}
			if (revents) {
/* XXX Really should send message to 730 side */
				/* It's probably not worth trying to figure out what
				 * the problem is and then fixing it, so just exit.
				*/
				char	msg[60];
				(void) sprintf(msg, "Unable to read from %s (revents=0x%x).", where, revents);
				SendCmd(ERRORMSG, msg, 0);
				(void) fprintf(stderr, "%s\n", msg);
				sleep(5);	/* so maybe they'll see the phone LCD */
				DoExit( 1 );
			}

			/* Neither one had anything -- try again. */
			continue;
		}
#endif	/* NO_POLL */

		/* Read a character -- either from the 730 or the pipe. */
		if (read(fd, &c, 1)==1)
		{
			c &= MSGMASK;
			switch (c&MSGMASK)
			{
			case QUITCMD:
				DoExit(0);
				/* NOTREACHED */
			case INITTERM:
				SendCmd(ERASECHAR, (char *)&tbuf.c_cc[VERASE], 1);
				/* We need the IGNORECMDs to get back in sync
				 * with the terminal side.
				 * I'm not sure why this is needed, but it
				 * appears to be from when we send 1 character
				 * in SendCmd(), which causes the terminal
				 * side to eat the first character (after
				 * the command)
				 * of the string sent right after the KILLCHAR.
				 * It's a bug in the termial side.
				*/
				SendCmd(IGNORECMD, "\n", 0);
				SendCmd(KILLCHAR, (char *)&tbuf.c_cc[VKILL], 1);
				SendCmd(IGNORECMD, "\n", 0);
				SendCmd(HOSTVERSION, HostVersion, 0);
				ReadData(DataFile);
				break;
			case AUDIXMSG:
				MailNotify(mailaddr, "AUDIX Message", NULL, NULL, NULL);
				break;
			case LOADREQUEST:
				ReadData(DataFile);
				break;
			case CALLINFO:
				RCVMSG(cp, c, fd);
				LogInfo(mailaddr, RcvBuf);
				break;
			case FEATURE_STATUS:
				RCVMSG(cp, c, fd);
				FeatureStatus(RcvBuf);
				break;
			case DEBUGLINE:
				/* The regular RCVMSG() reads until it finds EOM,
				 * but the message may (legally) contain that character,
				 * so instead read until the DEBUG_EOM is found.
				*/
				for (cp=RcvBuf; read(fd, &c, 1)==1 && c != DEBUG_EOM; cp++) {
					*cp = c;
				}
				*cp = '\0';
				(void) debug_printf(RcvBuf);
				break;
			case HELPTOC:
				HelpTOC();
				break;
#ifdef POSTQUERY
			case INFOREQ:
				RCVMSG(cp, c, fd);
				(void) InfoLookup(RcvBuf);
				break;
#endif /* POSTQUERY */
			case FACEREQ:
				RCVMSG(cp, c, fd);
				InfoFace(RcvBuf);
				break;
			case HELPTOPIC:
				RCVMSG(cp, c, fd);
				HelpTopic(RcvBuf);
				break;
			case SCALLSAVE:
				RCVMSG(cp, c, fd);
				SpeedCallSave(SpeedCallFile, RcvBuf);
				break;
			case DEBUG_VALUE:
				RCVMSG(cp, c, fd);
				debug = atoi(RcvBuf);
				(void) debug_printf("debug set to %d\n", debug);
				break;
			case DIALNUM:		/* octal 0356 */
				RCVMSG(cp, c, fd);
				(void) putchar( DIALNUM730 );
				(void) puts( RcvBuf );
				break;
			case RMTACT:		/* octal 0357 */
				RCVMSG(cp, c, fd);
				(void) putchar( RMTACT730 );
				(void) puts( RcvBuf );
				break;
			case '\n':	/* ignore -- it's for syncronization */
				break;
			default:
				(void) fprintf(stderr, "Unknown command from %s: 0x%x (%c)\n", input_file, (int)c, c);
#ifdef	XXX
/* I'm not sure if this is a good idea or not. */
				/* ignore the rest of the line */
				while (read(fd, &c, 1) == 1 &&
					c != '\r' && c != '\n')
						;
#endif
				break;
			}
		} else {
			/* Got EOF or some error on input.
			 * Since there's not much we can do, simply exit.
			 */
			DoExit( 1 );
		}
	}
	/*NOTREACHED*/
#ifdef lint
	return(0);	/* Keeps lint happy */
#endif
}


static void
DoExit(code)
int code;
{
	static void SetCooked();

#ifdef	NO_POLL
	/* Kill any children we may have. */
	if( pipepid > 0 )
		kill( pipepid, SIGTERM );
	if( ttypid > 0 )
		kill( ttypid, SIGTERM );
#else
	extern	char	*PipeFile;
	(void) unlink(PipeFile);
#endif	/* NO_POLL */

	SetCooked();
	exit(code);
}

static struct termio savetty;

static void
SetRaw()
{
	struct termio ttybuf;

	(void) ioctl(0, TCGETA, &ttybuf);
	savetty = ttybuf;

	ttybuf.c_iflag = IGNBRK | IGNPAR;
	ttybuf.c_oflag = 0;
	ttybuf.c_lflag = 0;
	ttybuf.c_cflag = ttybuf.c_cflag & (CBAUD | CLOCAL) | CREAD | CS8;
	/* We used to wait 3 seconds for a character.
	 * That's inefficient, so wait forever for a character (when we didn't
	 * get one before we just looped anyway).
	*/
	ttybuf.c_cc[VMIN] = 1;		/* used to be 0 */
	ttybuf.c_cc[VTIME] = 0;		/* used to be 30 */

	(void) ioctl(0, TCSETA, &ttybuf);
}

static void
SetCooked()
{
	(void) ioctl(0, TCSETAW, &savetty);
}

void
SendCmd(cmd, s, cnt)
char cmd;
register char *s;
int cnt;
{
	char sendbuf[BUFSIZ];
	register unsigned int n;
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

	/* Should probably check this write; if it fails,
	 * we should (?) assume the 730 side died.
	*/
	(void) write(1, sendbuf, n);
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
	cp = Malloc((unsigned)(len+1));
	if (s)
		(void) strcpy(cp, s);
	else
		*cp = '\0';

	return cp;
}

static void
SpeedCallSave(f, s)
char *f, *s;
{
	char *list, *name, *value;
	FILE *fp;

	if (f)
	{
		if ((fp = fopen(f, "a")) != NULL)
		{
			list = strtok(s, ";");
			name = strtok(NULL, ";");
			value = strtok(NULL, ";");
			if (list && name && value)
				(void) fprintf(fp, "speedlist=%s\tname=%s\tvalue=%s\n",
					list, name, value);
			(void) fclose(fp);
		}
	}
}
