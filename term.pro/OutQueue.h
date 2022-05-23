/* The X and DMD versions of this file are the same.
 * As such, you'll see some "#ifdef DMD630" code.
 * Eric Claeys, 1/95
*/

#ifdef	DMD630
#define	TICKS_PER_SECOND	60	/* resolution of alarm() */
#define	do_alarm	alarm		/* use alarm() call */
#else
#define	TICKS_PER_SECOND	 1	/* UNIX does alarm() by full second */
	/* UNIX uses do_alarm() function */
#endif	/* DMD630 */

typedef struct OutQueue OutQueue;
struct OutQueue {
	OutQueue *	next;
	char *		buf;
	short		flags;
	short		timeout;
};

typedef struct OutQueueData OutQueueData;
struct OutQueueData {
	OutQueue *	queue;
	OutQueue *	freelist;
	unsigned long	start_time;
	unsigned long	flags;
};

/* flags bits */
#define	OQ_MSG_ACK		0001		/* message acknowledged */
#define	OQ_NEED_CA		0002		/* need a new call appearance */
#define	OQ_WAIT_FOR_TONE	0004		/* wait for dial tone */
#define	OQ_PAUSE		0010		/* wait for timeout */
#define	OQ_WAIT_FOR_CONN	0020		/* wait for connect */
#define	OQ_WAIT_FOR_RETRY	0040		/* wait for retry */

extern OutQueue *OQnew();
extern void OQfree();

/* some OutQueue operations */
#define OQEMPTY(oqp)	((oqp)->queue==(OutQueue *)0)
#define OQMSGACK(oqp)	((oqp)->flags&OQ_MSG_ACK)
#define OQTOP(oqp)	((oqp)->queue)
#define	OQTIMEOUT(oqdp, oqp) \
			((oqp)->timeout > 0 && \
			((oqdp)->start_time+(oqp)->timeout < realtime()))

#define OQackmsg(oqp)	(oqp)->flags |= OQ_MSG_ACK; \
			do_alarm(0)
#define OQstartmsg(oqp)	(oqp)->flags &= ~OQ_MSG_ACK; \
			(oqp)->start_time = realtime(); \
			do_alarm(2*TICKS_PER_SECOND)
