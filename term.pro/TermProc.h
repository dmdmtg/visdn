/*
 * TermProc module definition
 */

#define UnixEvent	(TermProcs[PROC_UNIX].event)
#define PhoneEvent	(TermProcs[PROC_PHONE].event)
#define HelpEvent	(TermProcs[PROC_HELP].event)
#define InfoEvent	(TermProcs[PROC_INFO].event)

#define UnixThere	(TermProcs[PROC_UNIX].alive)
#define PhoneThere	(TermProcs[PROC_PHONE].alive)
#define HelpThere	(TermProcs[PROC_HELP].alive)
#define InfoThere	(TermProcs[PROC_INFO].alive)

/* must be big enough to hold face bitmap */
#define HOSTBUFSIZE	512
#define	SNDBUFSIZE	HOSTBUFSIZE/2	/* debugging lines can be large */
#define	RCVBUFSIZE	HOSTBUFSIZE

typedef struct TermProc {
	short	event;			/* event bit flags */
	short	alive;			/* heartbeat  counter */
	char	sndbuf[SNDBUFSIZE];	/* term->host comm buffer */
	char	rcvbuf[RCVBUFSIZE];	/* host->term comm buffer */
} TermProc;

/* TermProc event bits */
#define EVENT_SENDBUF		000001
#define	EVENT_RCVBUF		000002
#define EVENT_MAKECURRENT	000004
#define EVENT_QUIT		000010
#define EVENT_HELPLINE		000020
#define EVENT_NEWCALL		000040

/* bits in GlobalEvent */
#define GLOBAL_QUIT		000001
