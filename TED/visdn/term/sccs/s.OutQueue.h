h28845
s 00040/00000/00000
d D 1.1 92/07/23 13:58:56 trl 1 0
c date and time created 92/07/23 13:58:56 by trl
e
u
U
t
T
I 1
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

extern OutQueue *OQnew();
extern void OQfree();
extern unsigned long realtime();

/* some OutQueue operations */
#define OQEMPTY(oqp)	((oqp)->queue==(OutQueue *)0)
#define OQMSGACK(oqp)	((oqp)->flags&OQ_MSG_ACK)
#define OQTOP(oqp)	((oqp)->queue)
#define	OQTIMEOUT(oqdp, oqp) \
			((oqp)->timeout > 0 && \
			((oqdp)->start_time+(oqp)->timeout < realtime()))

#define OQackmsg(oqp)	(oqp)->flags |= OQ_MSG_ACK; \
			alarm(0)
#define OQstartmsg(oqp)	(oqp)->flags &= ~OQ_MSG_ACK; \
			(oqp)->start_time = realtime(); \
			alarm(2*60)
E 1
