h16776
s 00067/00000/00000
d D 1.1 92/07/23 13:58:32 trl 1 0
c date and time created 92/07/23 13:58:32 by trl
e
u
U
t
T
I 1
#include "visdn.h"

TimerEvent(rt)
unsigned long rt;
{
	register TimerItem *tp;

	/*
	 * this loses timer intervals while mouse buttons lock out the main
	 * main loop, but it gives better response overall
	 */
	LastTime = rt;

	for (tp=TimerList; tp; tp=tp->next)
	{
		TPRINTF("TimerEvent call func\n");
		if ((*tp->func)(tp->arg))
		{
			TimerRemove(tp);
		}
	}
}

TimerItem *TimerAdd(func, arg)
int (*func)();
char *arg;
{
	register TimerItem *tp = new(TimerItem);
	unsigned long realtime();

	if (tp)
	{
		tp->func = func;
		tp->arg = arg;

		if ((tp->next = TimerList)==(TimerItem *)0)
			LastTime = realtime();

		TimerList = tp;
	}

	return tp;
}

TimerRemove(tp)
register TimerItem *tp;
{
	register TimerItem *otp;

	if (tp && TimerList)
	{
		if (TimerList==tp)
		{
			TimerList = tp->next;
			free(tp);
		} else
		{
			for (otp=TimerList; otp->next; otp=otp->next)
				if (otp->next==tp)
				{
					otp->next = tp->next;
					free(tp);
					break;
				}
		}
	}
}
E 1
