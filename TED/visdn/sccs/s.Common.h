h58656
s 00001/00000/00070
d D 1.4 92/09/10 16:56:25 trl 4 3
c 
e
s 00003/00000/00067
d D 1.3 92/09/10 16:51:03 trl 3 2
c rmtactions
e
s 00002/00000/00065
d D 1.2 92/08/28 15:10:13 trl 2 1
c Add post call stuff
e
s 00065/00000/00000
d D 1.1 92/07/23 13:54:56 trl 1 0
c date and time created 92/07/23 13:54:56 by trl
e
u
U
t
T
I 1
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
#define PROCSHIFT	5	/* shift right 5 for process # */
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
I 2
#define DIALNUM		PROCMSG(PROC_HOST, 11)	/* number to post */
I 4
#define RMTACT		PROCMSG(PROC_HOST, 12)	/* do remote action */
E 4
E 2

#define ERASECHAR	PROCMSG(PROC_PHONE, 1)	/* user defined erase char */
#define KILLCHAR	PROCMSG(PROC_PHONE, 2)	/* user defined line kill */
#define HOSTVERSION	PROCMSG(PROC_PHONE, 3)	/* host side version */
#define FBCREAT		PROCMSG(PROC_PHONE, 4)	/* button spec */
#define FBLABEL		PROCMSG(PROC_PHONE, 5)	/* button label */
#define SPDCALL		PROCMSG(PROC_PHONE, 6)	/* speedcall item */
#define KEYPADPOS	PROCMSG(PROC_PHONE, 7)	/* origin coords for keypad */
#define DIALINFO	PROCMSG(PROC_PHONE, 8)	/* area code, exch, etc */
I 2
#define DOPOST		PROCMSG(PROC_PHONE, 9)	/* number to dial NOW */
I 3
#define RMTACT0		PROCMSG(PROC_PHONE, 10)	/* remote action 0 */
#define RMTACT1		PROCMSG(PROC_PHONE, 11)	/* remote action 1 */
#define RMTACT2		PROCMSG(PROC_PHONE, 12)	/* remote action 2 */
E 3
E 2

#define USERINFO	PROCMSG(PROC_INFO, 1)	/* user info from pq(1) */
#define FACEDATA	PROCMSG(PROC_INFO, 2)	/* face icon bitmap data */

/* shared icon information */

#define ICONSIZE	48
#define ICONBYTES	(ICONSIZE * (ICONSIZE / 8))

#define ICONKEYSIZE	20	/* chars allowed for icon key */

/* must be < HOSTBUFSIZE */
#define ICONMSGSIZE	(ICONKEYSIZE+ICONBYTES)
E 1
