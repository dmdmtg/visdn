h45136
s 00121/00000/00000
d D 1.1 92/07/23 13:58:31 trl 1 0
c date and time created 92/07/23 13:58:31 by trl
e
u
U
t
T
I 1
/*
 * Phone interface Output Queue routines
 */

#include "visdn.h"

/*
 * initialize initial output queue conditions
 */
OutQueueInit(oqdp)
register OutQueueData *oqdp;
{
	/* nothing in the queue */
	oqdp->queue = (OutQueue *)0;

	/* one item on the free list */
	oqdp->freelist = new(OutQueue);
	oqdp->freelist->next = (OutQueue *)0;
}

/*
 * manufacture a new queue item
 */
OutQueue *OQnew(oqdp)
register OutQueueData *oqdp;
{
	register OutQueue *oqp;

	/* grab one off the free list */
	if (oqdp != (OutQueueData *)0 && (oqp = oqdp->freelist))
		oqdp->freelist = oqp->next;
	else /* or make a new one */
		oqp = new(OutQueue);

	oqp->next = (OutQueue *)0;
	oqp->buf = (char *)0;
	oqp->flags = 0;
	oqp->timeout = 0;

	return oqp;
}

/*
 * append an item to the end of the queue
 */
void OQappend(oqdp, oqp)
register OutQueueData *oqdp;
register OutQueue *oqp;
{
	register OutQueue *tqp;

	if (oqdp==(OutQueueData *)0 || oqp==(OutQueue *)0)
		return;
	oqp->next = (OutQueue *)0;

	if (tqp = oqdp->queue)
	{
		while (tqp->next)
			tqp = tqp->next;
		tqp->next = oqp;
	} else
		oqdp->queue = oqp;
}

/*
 * remove a queue item from the active list and append it to the free list
 */
void OQfree(oqdp, qp)
register OutQueueData *oqdp;
register OutQueue *qp;
{
	register OutQueue *oqp, *tqp;

	/* make sure we have valid pointers */
	if (oqdp==(OutQueueData *)0 || qp==(OutQueue *)0)
		return;

	if ((oqp = oqdp->queue) != (OutQueue *)0)
	{
		/* find item in queue */
		if (qp==oqp)
			oqdp->queue = qp->next; /* first item is easy */
		else
		{
			/* not first item -- follow links */
			for (tqp=oqp->next; tqp; oqp=tqp, tqp=tqp->next)
				if (qp==tqp)
				{
					oqp->next = tqp->next;
					break;
				}
		}
	}

	/* finally, add it to free list */
	free(qp->buf);
	qp->buf = (char *)0;
	qp->next = oqdp->freelist;
	oqdp->freelist = qp;
}

/*
 * append a string to the output queue
 */
int OutEnQueue(oqdp, s)
OutQueueData *oqdp;
char *s;
{
	register OutQueue *oqp;

	if (oqp = OQnew(oqdp))
	{
		if (s[0]=='C' && s[1]=='A')
		{
			oqp->flags |= OQ_NEED_CA;
			oqp->buf = String(&s[2]);
		} else
			oqp->buf = String(s);
		OQappend(oqdp, oqp);
	}
}
E 1
