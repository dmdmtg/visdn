/* This file is shared between the 730 and X versions.
 * Hence, you'll see some "#ifdef DMD630" lines.
 * Eric Claeys, 1/95
*/

#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#ifdef	DMD630
#include "visdn.h"
#else
#include "xisdn.h"
#endif	/* DMD630 */

static	FILE *openFeatureFile();
void	MailNotify();

static	FILE *LogFP;
static	FILE *DebugFP = stderr;		/* default to stderr */
static	FILE *FeatureFP;
static	char *FeatureFile = NULL;

static	int	truncate_feature_log = 0; /* Truncate log before each write? */
static	int	old_log = 0;		/* use old format logging? */
static	char	*Copen_type = "a";	/* default: open for append */
static	int	Iopen_type = O_APPEND;	/* open() version of Copen_type */

int
InitDebug(debugfile)
char *debugfile;
{
	static	int	called = 0;

	/* Make sure we're only called once. */
	if (called++ > 0) return(1);	/* simulate success */

	if (debugfile && *debugfile) {
		/* Truncate every time */
		if ((DebugFP = fopen(debugfile, "w")) == NULL) {
			(void) fprintf(stderr, "Warning: Cannot open %s.\n", debugfile);
			perror("");
			return(0);	/* failure */
		}
		(void) setbuf(DebugFP, NULL);	/* unbuffer output */
	}
	return(1);	/* success, or no debug file */
}

/* We need this routine since we can't have macros (e.g., PRINTF(a, b)) with
 * varying number of arguments.
*/
#include <varargs.h>

debug_printf(va_alist)
va_dcl
{
	va_list	args;
	char	*fmt;

	va_start(args);
	fmt = va_arg(args, char *);
	(void) vfprintf(DebugFP, fmt, args);
	va_end(args);
}

int
InitLog(logfile)
char *logfile;
{
	if (logfile && *logfile) {
		if ((LogFP = fopen(logfile, "a")) == NULL) {
			(void) fprintf(stderr, "Warning: Cannot open %s.\n", logfile);
			perror("");
			return(0);	/* failure */
		}
	}
	return(1);	/* success, or no log file */
}

/* The Feature file might be a pipe, and hence the open may fail
 * if there is no reader.
 * That's ok, as we'll try to open it later.
 * However, if the open fails due to some other problem, then exit.
*/
int
InitFeature(featurefile)
char *featurefile;
{
	if (featurefile && *featurefile) {
		if ((FeatureFP = openFeatureFile(featurefile)) == NULL) {
			if (errno != ENXIO) {
				(void) fprintf(stderr, "Warning: Cannot open %s.\n", featurefile);
				perror("");
				return(0);	/* failure */
			}
		}
	}
	return(1);	/* success, or no feature file */
}


/*==================================================================*/
static void
afl()	/* 'a'ppend to 'f'eature 'l'og */
{
	/* Since we're appending to it, we're not truncating it */
	truncate_feature_log = 0;
	Copen_type = "a";
	Iopen_type = O_APPEND;
}
static void
tfl()	/* 't'runcate the 'f'eature 'l'og before writing */
{
	truncate_feature_log = 1;
	Copen_type = "w";
	Iopen_type = 0;
}
static void
o()	/* use the 'o'ld log file format */
{
	/* SHOULD THE OLD FORMAT STILL BE SUPPORTED???
	 * The format of the two log files is different,
	 * so there are some potential compatibility issues.
	*/
	old_log = 1;
}

/* Process log file options. */

struct	log_options {
	char	*option;
	void	(*func)();
};
static struct	log_options options[] = {
	{ "old", o},
	{ "append_feature_log", afl},
	{ "truncate_feature_log", tfl},

	{ NULL, NULL }		/* end of list */
};

int
ProcessLogOptions(option)
char	*option;
{
	register	struct	log_options	*o;

/* XXXX Should allow multiple entries per line */

	for (o=options; o->option != NULL; o++) {
		if (strcmp(option, o->option) == 0) {
			(*(o->func))();
			return(0);
		}
	}

	(void) fprintf(stderr, "Ignoring unknown log option: %s\n", option);
	(void) fprintf(stderr, "Possible options are:\n");
	for (o=options; o->option != NULL; o++)
		(void) fprintf(stderr, "\t%s\n", o->option);
	return(1);
}

/* Try to open the feature file.
 * It may be a pipe, so be careful.
*/
static FILE *
openFeatureFile(file)
char	*file;
{
	int	fd;
	FILE	*fp;

	FeatureFile = file;	/* save it in case we need it again */

/* I don't remember why I don't use O_NDELAY in both cases ... */
#ifdef	O_NONBLOCK
#define	_BLOCK_	O_NONBLOCK
#else
#define	_BLOCK_	O_NDELAY
#endif
	if ((fd = open(file, O_WRONLY|O_CREAT|Iopen_type|_BLOCK_)) == -1) {
		return(NULL);
	}

	if ((fp = fdopen(fd, Copen_type)) == NULL) {
		return(NULL);
	}
/* XXXX really should only do this if it's a pipe */
	(void) signal(SIGPIPE, SIG_IGN);	/* only needed for pipes */

	return(fp);
}

void
FeatureStatus(buf)
char *buf;
{
	long	tmptime;
	char	time_buf[27];

	if (LogFP != NULL || FeatureFile != NULL)
	{
		(void) time(&tmptime);
		(void) strcpy( time_buf, ctime(&tmptime) );
		time_buf[24] = '\0';	/* get rid of \n */

		if (LogFP != NULL)
		{
			(void) fprintf(LogFP, "%s\t%s\n", time_buf, buf);
			(void) fflush(LogFP);
		}
		if (FeatureFile != NULL)
		{
			/* May need to try and open it, especially if it's a
			 * pipe and there never was a reader.
			*/
			if (FeatureFP == NULL)
				FeatureFP = openFeatureFile(FeatureFile);
			if (FeatureFP != NULL) {
				extern	char	*getenv();
				static	char	*logname = NULL;

				if (logname == NULL)
					logname = getenv("LOGNAME");

				/* If we're supposed to truncate the file before
				 * each write (i.e., to ensure there's at most
				 * 1 line in the file at all times),
				 * do that now.
				*/
				if (truncate_feature_log == 1) {
					if (ftruncate(fileno(FeatureFP), 0) == 0) {
						rewind(FeatureFP);
					} else {
						perror("ftruncate");
						(void) fclose(FeatureFP);
						FeatureFP = openFeatureFile(FeatureFile);
					}
				}

				/* Ignore the error since it's probably due to
				 * the reader on the pipe having gone away.
				*/
				/* Include the logname since the reader on
				 * the pipe won't know where the data came from.
				*/
				if (fprintf(FeatureFP, "%s\t%s\t%s\n", time_buf, buf, logname) > 0)
					(void) fflush(FeatureFP);
				/* If there was a reader on the pipe,
				 * but it went away,
				 * the fprintf() above will fail (which
				 * is ok).  However, all future ones will
				 * fail even after a new reader starts
				 * unless we clear the error bit.
				*/
				clearerr(FeatureFP);
			}
		}
	}
}

void
LogInfo(addr, buf)
char *addr, *buf;
{
	register char *cp;
	CallInfo info;
	long tmptime;
	char	time_buf[27];
	int	i;

	/* The code that follows parses out ascii string buf sent from
	 * 730 side. This is essentially the data structure LCD.msgs[mode][0-0F]
	 * which is a collection of strings for a given display mode matching
	 * of the varying display types, see DISPLAY in fp3 api manual.
	 * "first field" below refers to first field in buf, not in LCD.msgs.
	 */

	/* first field is status of call */
	for (cp=buf, info.status=buf; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* Second field is blank, corresponds to string at LCD.msgs[mode][0].
	 * This is blank so indexes match display types on page 7-35 of fp3
	 * api manual, section on DISPLAY.
	 */
	if (*cp==LOG_SEP) cp++;

	/* Third field is call id or call appearance number. */
	for (info.callapp=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* fourth field is called dn */
	for (info.calleddn=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* fifth field is calling dn */
	for (info.callingdn=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* sixth field is called party name */
	for (info.callednm=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* seventh field is calling party name */
	for (info.callingnm=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* eighth field is originating permissions (whatever that is) */
	for (info.cos=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* ninth field is isdn call id */
	for (info.inid=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* tenth field is misc call information */
	for (info.miscinfo=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* eleventh field is entire display */
	for (info.fulldisp=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* twelfth field is date and time of day (of call) */
	for (info.datetime=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

	/* Here we could add the fp3 display types 0b through 0f
	 * which correspond to redirecting info.
	 */

	/* Skip unused ones to get to the duration */
	for (i=0; i<7 && *cp && *cp == LOG_SEP ; i++, cp++)
		;
	for (info.duration=cp; *cp && *cp != LOG_SEP; cp++)
		;
	if (*cp=='\0')
		return;
	*cp++ = '\0';

#ifdef notdef	/* for now, log ALL entries - the 730 shouldn't send bad ones */
	if (*info.calleddn=='\0' && *info.callingdn=='\0' && *info.callingnm=='\0')
		return;
#endif

	if (LogFP)
	{
		(void) time(&tmptime);
		(void) strcpy( time_buf, ctime(&tmptime) );
		time_buf[24] = '\0';	/* get rid of \n */
		(void) fseek(LogFP, (long)0, 2);
		if (old_log == 0) {
			char	entry[200];
			char	*dn_sep, *nm_sep;

			/* If there is only incoming OR outgoing information,
			 * use the single number and name (which may be null).
			 * If there is incoming AND outgoing information,
			 * write both, separating with "/", e.g., "out/in".
			 *
			 * Using this technique, we don't really care if
			 * the call is an incoming or outgoing one.
			*/

			if (*info.calleddn != '\0' && *info.callingdn != '\0') {
				char	*p;

				dn_sep = ":";

				/* If the called and calling name are missing,
				 * don't print the nm_sep since it looks dumb.
				*/
				if (*info.callednm == '\0' && *info.callingnm == '\0')
					nm_sep = "";
				else
					nm_sep = dn_sep;

				/* To avoid a bunch of blanks after the called
				 * info (e.g., "EC CLAEYS        :JP JONES",
				 * get rid of them.
				*/
				for (p= &info.calleddn[strlen(info.calleddn)-1]; *p && *p==' '; p--)
					;
				*(p+1) = '\0';
				if (*info.callednm != '\0') {
					for (p= &info.callednm[strlen(info.callednm)-1]; *p && *p==' '; p--)
					;
					*(p+1) = '\0';
				}
			} else {
				dn_sep = nm_sep = "";
			}
			(void) sprintf(entry, "%s\t%s\t%s\t%s\t%s%s%s\t%s%s%s\t%s\n",
				time_buf,
				info.status,
				info.callapp,
				info.inid,
				info.calleddn, dn_sep, info.callingdn,
				info.callednm, nm_sep, info.callingnm,
				info.duration);
			(void) fprintf(LogFP, "%s", entry);
		} else {
			(void) fprintf(LogFP, "%s    %s\t%s\t%s\t%s\n",
				time_buf,
				info.status,
				info.calleddn,
				info.callingdn,
				info.callingnm);
		}
		(void) fflush(LogFP);
	}

	if (*info.status=='u' && *info.callingdn)
		MailNotify(addr, "Phone call missed", info.callingdn, info.callingnm, info.datetime);
}

void
MailNotify(addr, subj, str1, str2, str3)
char *addr, *subj, *str1, *str2, *str3;
{
	char sysbuf[BUFSIZ];
	char nstr2[ 100 ], *t = NULL;

	if (addr == NULL || *addr == '\0')
		return;		/* no mail address specified */

	(void) strncpy( nstr2, str2 ? str2 : "", sizeof(nstr2)-1 );
	for( t = &nstr2[strlen(nstr2)-1]; t && *t == ' '; --t )
		continue;

	if( t )
		*++t = '\0';
	
	(void) sprintf(sysbuf, "(echo 'Subject: %s [%s] %s';echo '%s') | /bin/mail %s\n",
			nstr2, str1?str1:"", subj, str3?str3:"", addr );
	(void) system(sysbuf);
}
