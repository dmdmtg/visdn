typedef struct TimerItem {
	struct TimerItem *next;
	int	(*func)();
	char	*arg;
} TimerItem;

#define TIMER_INT	5		/* timer interval is 1/12 second */
