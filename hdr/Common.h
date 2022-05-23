/*
 * visdn - visual ISDN
 *
 * Alan Hastings Dec 1989
 */

/*
 * messages are identified by the process they are destined for
 * so the host link can be shared among the various terminal co-processes.
 * A single character encodes the process id and the message number.
 *
 *	bits 7 6 5	- process id
 *	bits 4 3 2 1 0	- message number
 */

#define	MSGMASK		0xFF	/* 8 bits for entire message */
#define PROCMASK	0xE0	/* 3 bits for process # */
#define PROCSHIFT	5	/* shift right to get process #, left to put */

#define PROCMSG(h, n)	(((h<<PROCSHIFT)|n)&MSGMASK)

#define PROC_UNIX	0	/* message destined for UNIX comm process */
#define PROC_PHONE	1	/* message destined for phone process */
#define PROC_HELP	2	/* message destined for help process */
#define	PROC_INFO	3	/* message destined for User Info process */
#define TERM_PROCS	4	/* how many processes live in the terminal */

#define	PROC_HOST	7	/* message destined for UNIX host side */

#define IGNORECMD	PROCMSG(PROC_UNIX, 1)	/* ignore this command */
#define TOCITEM		PROCMSG(PROC_UNIX, 2)	/* table of contents line */
#define HELPLINE	PROCMSG(PROC_UNIX, 3)	/* line of help text */
/* PROCMSG(PROC_UNIX, 12) is illegal (maps to \n which is special) */

#define AUDIXMSG	PROCMSG(PROC_HOST, 1)	/* audix light went on */
#define CALLINFO	PROCMSG(PROC_HOST, 2)	/* call data */
#define INITTERM	PROCMSG(PROC_HOST, 3)	/* terminal side is ready */
#define QUITCMD		PROCMSG(PROC_HOST, 4)	/* it's miller time */
#define LOADREQUEST	PROCMSG(PROC_HOST, 5)	/* send up button defs */
#define HELPTOC		PROCMSG(PROC_HOST, 6)	/* help table of contents */
#define HELPTOPIC	PROCMSG(PROC_HOST, 7)	/* send help on a topic */
#define	INFOREQ		PROCMSG(PROC_HOST, 8)	/* request for post info */
#define	FACEREQ		PROCMSG(PROC_HOST, 9)	/* request face icon */
#define	SCALLSAVE	PROCMSG(PROC_HOST, 10)	/* speed call item to save */
#define	FEATURE_STATUS	PROCMSG(PROC_HOST, 11)	/* a feature changed */
#define	DEBUGLINE	PROCMSG(PROC_HOST, 12)	/* output a debug line */
#define	DEBUG_VALUE	PROCMSG(PROC_HOST, 13)	/* debug value changed */
#define	DIALNUM		PROCMSG(PROC_HOST, 14)	/* number to dial (octal 0356) */
#define	RMTACT		PROCMSG(PROC_HOST, 15)	/* remote action (octal 0357) */

#define ERASECHAR	PROCMSG(PROC_PHONE, 1)	/* user defined erase char */
#define KILLCHAR	PROCMSG(PROC_PHONE, 2)	/* user defined line kill */
#define HOSTVERSION	PROCMSG(PROC_PHONE, 3)	/* host side version */
#define FBCREAT		PROCMSG(PROC_PHONE, 4)	/* button spec */
#define FBLABEL		PROCMSG(PROC_PHONE, 5)	/* button label */
#define SPDCALL		PROCMSG(PROC_PHONE, 6)	/* speedcall item */
#define KEYPADPOS	PROCMSG(PROC_PHONE, 7)	/* origin coords for keypad */
#define DIALINFO	PROCMSG(PROC_PHONE, 8)	/* area code, exch, etc */
#define TUNESTRING	PROCMSG(PROC_PHONE, 9)	/* sending tune string */
#define CAESCAPE	PROCMSG(PROC_PHONE, 10)	/* sending escape code */
#define DIALPARAM	PROCMSG(PROC_PHONE, 11) /* dial parameters */
#define	DIALNUM730	PROCMSG(PROC_PHONE, 12)	/* number to dial */
#define	RMTACT730	PROCMSG(PROC_PHONE, 13)	/* remote action */
#define	ERRORMSG	PROCMSG(PROC_PHONE, 14)	/* display an error message */
#define	UNIXLEAVING	PROCMSG(PROC_PHONE, 15)	/* the UNIX process is leaving */

#define USERINFO	PROCMSG(PROC_INFO, 1)	/* user info from pq(1) */
#define FACEDATA	PROCMSG(PROC_INFO, 2)	/* face icon bitmap data */

/* shared icon information */

#define ICONSIZE	48
#define ICONBYTES	(ICONSIZE * (ICONSIZE / 8))

#define ICONKEYSIZE	20	/* chars allowed for icon key */

/* must be < HOSTBUFSIZE */
#define ICONMSGSIZE	(ICONKEYSIZE+ICONBYTES)

#define MAXCALLAPPS	30	/* # line appearances (Help Desk has lots) */

#ifndef LOG_SEP
#define	LOG_SEP	';'	/* Character that separates log file entries. */
#endif

/* Default End Of Message character when sending from 730 to UNIX. */
#define	EOM		'\n'
/* The eom character when sending debugging lines, which may
 * contain EOM.
 * Try to pick a character that isn't likely to be in a message.
 * Can't use \0 since it's the end of string character.
*/
#define	DEBUG_EOM	'\04'
