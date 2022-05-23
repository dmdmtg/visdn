h63636
s 00034/00000/00000
d D 1.1 92/07/23 13:57:28 trl 1 0
c date and time created 92/07/23 13:57:28 by trl
e
u
U
t
T
I 1
/*
 * DEBUG definitions
 */

/*
 * bits in Debug flag
 */
#define DEBUG_HOSTCOMM	(1<<0)
#define DEBUG_PHONE	(1<<1)
#define DEBUG_HELP	(1<<2)
#define DEBUG_INFO	(1<<3)
#define DEBUG_LAMP	(1<<4)
#define DEBUG_TIMER	(1<<5)
#define DEBUG_MOUSE	(1<<6)

#define NDEBUGFLAGS	7

#ifdef DEBUG
#define DPRINTF if (Debug & DEBUG_PHONE) lprintf
#define HPRINTF if (Debug & DEBUG_HELP) lprintf
#define IPRINTF	if (Debug & DEBUG_INFO) lprintf
#define LPRINTF if (Debug & DEBUG_LAMP) lprintf
#define MPRINTF if (Debug & DEBUG_MOUSE) lprintf
#define TPRINTF if (Debug & DEBUG_TIMER) lprintf
#define CPRINTF if (Debug & DEBUG_HOSTCOMM) lprintf
#else
#define DPRINTF
#define HPRINTF
#define IPRINTF
#define LPRINTF
#define MPRINTF
#define TPRINTF
#define CPRINTF
#endif /* DEBUG */
E 1
