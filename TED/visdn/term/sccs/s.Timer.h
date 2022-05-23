h19496
s 00007/00000/00000
d D 1.1 92/07/23 13:58:57 trl 1 0
c date and time created 92/07/23 13:58:57 by trl
e
u
U
t
T
I 1
typedef struct TimerItem {
	struct TimerItem *next;
	int	(*func)();
	char	*arg;
} TimerItem;

#define TIMER_INT	5		/* timer interval is 1/12 second */
E 1
