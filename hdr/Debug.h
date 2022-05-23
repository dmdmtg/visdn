/*
 * DEBUG definitions
 */

/*
 * bits in Debug flag
 */
#define DEBUG_UNIXCOMM		(1<<0)	/*   1 */
#define DEBUG_PHONE		(1<<1)	/*   2 */
#define DEBUG_HELP		(1<<2)	/*   4 */
#define DEBUG_INFO		(1<<3)	/*   8 */
#define DEBUG_LAMP		(1<<4)	/*  16 */
#define DEBUG_TIMER		(1<<5)	/*  32 */
#define DEBUG_MOUSE		(1<<6)	/*  64 */
#define DEBUG_SPEEDLIST		(1<<7)	/* 128 */
#define DEBUG_RAWPHONELINE	(1<<8)	/* 256 */

#define NDEBUGFLAGS		9

#ifdef DEBUG
	/* We need to send debug output for the UNIX port to the 730,
	 * rather than to the UNIX process, since the 730 process that sends
	 * to UNIX also has UPRINTF's in it, and we get into recusion problems.
	*/
#define UPRINTF	if (Debug & DEBUG_UNIXCOMM)	lprintf

#define DPRINTF	if (Debug & DEBUG_PHONE)	debug_printf
#define HPRINTF	if (Debug & DEBUG_HELP)		debug_printf
#define IPRINTF	if (Debug & DEBUG_INFO)		debug_printf
#define LPRINTF	if (Debug & DEBUG_LAMP)		debug_printf
#define TPRINTF	if (Debug & DEBUG_TIMER)	debug_printf
#define MPRINTF	if (Debug & DEBUG_MOUSE)	debug_printf
#define SPRINTF	if (Debug & DEBUG_SPEEDLIST)	debug_printf
#define RPRINTF	if (Debug & DEBUG_RAWPHONELINE)	debug_printf
#else
#define UPRINTF
#define DPRINTF
#define HPRINTF
#define IPRINTF
#define LPRINTF
#define TPRINTF
#define MPRINTF
#define SPRINTF
#define RPRINTF
#endif /* DEBUG */
