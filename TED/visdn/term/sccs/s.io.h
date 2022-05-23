h21886
s 00315/00000/00000
d D 1.1 92/07/23 13:58:57 trl 1 0
c date and time created 92/07/23 13:58:57 by trl
e
u
U
t
T
I 1
/*
 * io.h - data structures dealing with 5630's I/O stream architecture
 * 
 *	The stream IO architecture is based on IO blocks which are self_contained
 *	but interconnected. IO blocks communicate with each other through stream 
 *	message. The architecture supports three types of messages:
 *
 *		. common data sent downstream
 *		. common data sent upstream 
 *		. control data sent downstream
 *
 *	Note that control data sent upstream is handled differently (see doctl() in
 *	demux.c for an example). 
 *
 *	Common data going downstream or upstream are processed sequentially by the
 *	IO blocks in the stream. For example the Xt block chops the sent data into
 *	packets (see psend.c), and combines received packets into data (see precv.c).
 *
 *	Control data, inside an IO block, can be:
 *
 *		. ignored, if the IO block does not understand it, and the message
 *		  is sent downstream for subsequent IO blocks,
 *		. partially processed, and the message is sent downstream
 *		  for further processing,
 *		. fully processed, the message does not have to go any further.
*/

/* Generic IO block structure:
 *	
 *	Each IO block structure must have the generic IO structure defined first.
 *	Fields that pertain to individual blocks follow. For an example, see
 *	the definition of the logical (Logio) and physical (Pysio) IO block in this file.
*/
typedef struct Ioblock {
	int state;			/* state of io block */
	int id;				/* who am i? */
	struct Ioblock *upstrm;		/* previous io block */
	struct Ioblock *dwnstrm;	/* next io block */
	int (*puts)();			/* upstream block calls it for output */
	int (*gets)();			/* downstream block calls it for input */
	int (*sctl)();			/* upstream calls it for IO control */
	long xpand;			/* for future expansion */
} Ioblock;


/* Known IO block (used in the ID field):
*/	
#define LOGIO	1	/* logical block */
#define PYSIO	2	/* physical block */
#define XT	3	/* xt block */
#define LAN	4	/* LAN encoding block */
#define AULOG	5	/* auto-login block */
#define UXLAN	6	/* networking block */


/* Generic IO message:
 *
 *	Not all the fields need to be used. A certain IO block may request 
 *	certain fields of its input message to be initialized, therefore the
 *	previous adjacent block must output a message with those fields set up.
*/	
typedef struct Iomsg {
	char *data;		/* pointer to data buffer */
	short size;		/* size of buffer */
	short type;		/* type of message */
	Ioblock *blk;		/* next io block in the stream (up or down) */
	long misc;		/* miscellaneous */
} Iomsg;





/* LOGICAL IO BLOCK:
 *
 *	The logical IO block is the start of the stream nearest to the user's
 *	processes (which unfortunately are not IO blocks). It processes information
 *	concerning the logical environment of a given installed physical device.
 *	For example, an EIA device can be in the MUX or NONMUX environment.
*/ 
#define	NPROC		8	/* repeated from dmdproc.h */


typedef struct Logio {
	/* common to all I/O block */

	int state;		/* AVAILABLE or environment state */
	int id;			/* LOGIO */
	Ioblock *upstrm;	/* not used */
	Ioblock *dwnstrm;	/* pysio block (nonmux), or xt block (mux) */
	int (*puts)();		/* not used */
	int (*gets)();		/* "logiogets()" */
	int (*sctl)();		/* not used */
	long xpand;		/* not used */

	/* common to all Logical I/O block */
	/* interface with user processes */
	int (*nsend)();		/* "hostputs()" */
	int (*nrecv)();		/* "hostgets()" */
	int (*sysctl)();	/* "hostctl()" */

	struct Usrenv *up;	/* user-interface environment */
	struct Pysio *pysio;	/* shortcut to phys. block w/o going to stream */
	short numprocs;		/* number of available host connections */
	short countprocs;	/* number of processes created */
	int (*defproc)();	/* default process */
	struct Proc *chanproc[NPROC];	/* connections table */
	long pad1[8];		/* padding, in case chanproc or Logio itself
				** need expansion
				*/
	struct Proc *kproc;	/* per-host process queue */
	long pad2[7];		/* more padding */
} Logio;



	

/* Possible Logical IO block states: */
#define AVAILABLE	0
#define NONMUX		1	/* resident environment */
#define MUX		2	/* xt environment */
#define HOSTLOCAL	3	/* local environment */
#define MCC		4	/* Multi Cluster Control environment */
#define MUXDEAD		5	/* dead environment (host connectivity lost */
#define XENV1		6
#define XENV2		7
#define XENV3		8
#define XENV4		9
#define XENV5		10

/* Logical I/O block status */
#define L_HOST_ACTIVE	0x1	/* mounted as active host port (shown on New submenu) */
#define L_HOST_SLEEP	0x2	/* mounted as non-active host (not shown on New submenu) */
#define L_PRNT_ACTIVE	0x4	/* mounted as printer port (not shown on New submenu) */


#define NLOGIOS		4
#ifdef INIT_C
struct Logio logio[NLOGIOS];
#else
extern struct Logio logio[NLOGIOS];
#endif



/* PHYSICAL IO BLOCK
 *
 *	This block is the last block in the stream. It is nearest to
 *	the hardware.
*/
typedef struct Iomapping {
	int capability;		/* Or'ing of PRINTER, HOST */
	int setting;		/* PRINTER or HOST */
} Iomapping;

typedef struct Pysio {
	/* common to all I/O blocks */

	int state;		/* AVAILABLE, EIA, SSI: device name if in use */
	int id;			/* PYSIO */
	Ioblock *upstrm;
	Ioblock *dwnstrm;	/* null since this is end of stream */
	int (*puts)();		/* "pysiosend()" */
	int (*gets)();		/* null since this is end of stream */
	int (*sctl)();		/* "pysioctl()" */
	long xpand;		/* not used */

	/* common to all physical I/O block */

	int suid;		/* SetUp id / major id */
	int mnid;		/* minor id */
	struct Logio *host;	/* shortcut pointer to logical IO block */
	Iomapping mapping;	/* mapping information for this host */

	int (*open)();		/* init routine */
	int (*close)();		/* uninit routine (drops DTR) */
	int (*pulse)();		/* check for line "pulse" */
	int (*xpoke)();		/* transmit start-up routine */
	struct Queue *rcvq;	/* receive queue */
	struct Queue *outq;	/* output queue */
	long *hrdwrptr;		/* pointer to hardware specific information */

	struct Proc *wakeproc;	/* process to wake up on rcv chars */
	short status;		/* misc. status */
	short active;		/* archaic, but still used */

	char *pname;		/* ASCII name pointer */ 
	struct Tmenu *pmenu;	/* Setup menu pointer */
	int (*stpbrk)();	/* stop break function */
	int (*stpdscn)();	/* stop disconnect function */

	long pad[5];		/* for the future */
} Pysio;


/* supported devices */
#define PYS_HOST		0	/* historical */
#define PYS_MAIN		0x0
#define PYS_AUX			0x1
#define PYS_SIO			0x2
#define PYS_SSI			0x3

/* for the future */
#define PYS_EXPANSION1		0x4	/* used for StarLAN ISO-10 port */
#define PYS_EXPANSION2		0x5	/* used for TCP/IP/Telnet port */
#define PYS_EXPANSION3		0x6
#define PYS_EXPANSION4		0x7
#define PYS_EXPANSION5		0x8
#define PYS_EXPANSION6		0x9


/* number of physical devices above 
** also defined in setup.h, match definitions
*/
#define PYS_COUNT		10

/* states (mutual exclusive): */
#define AVAILABLE	0
#define EIA		1
#define SSI		2
#define EXPANSION1	3
#define EXPANSION2	4
#define EXPANSION3	5
#define EXPANSION4	6

/* for the "active" field */
#define INACTIVE	0		/* closed */
#define ACTIVE		1		/* opened */
#define SUPERACTIVE	2		/* opened and having window */


/* mapping capabilities (OR'able): */
#define HOST		0x1	/* can be mapped as a host */
#define PRINTER		0x2	/* can be mapped as a printer */

/* status: */
#define ACIASTOP	0x1	/* don't transmit because received ^S */
#define AUTOBLOCKED	0x2	/* autoblocked */
#define USERBLOCKED	0x4	/* blocked by a user with typed ^S */
#define OK2UNBLOCK	0x8	/* ok to unblock */
#define PYSIOMPX	0x10	/* this pys port running mux */
#define PYSRCVFLOW	0x20	/* receive flow control enabled */
#define PYSNOQUEUE	0x40	/* ignore received chars - used for printer */
#define PYSTRUST	0x80	/* medium can be trustworthy */
#define TRYBLOCK	0x100	/* Flag to indicate trying to block sender */
#define HWFLWCNTL	0x200	/* supports hardware flow control */
#define HWBLOCKED	0x400	/* Autoblocked by hardware flow control */
#define TRYUNBLOCK	0x800	/* Time to send a Gen Flow DC1 to unblock host */


#ifdef INIT_C
struct Pysio pysio[PYS_COUNT];
#else
extern struct Pysio pysio[PYS_COUNT];
#endif



/* MAPPING:
 *
 * Options stored by log_to_pys
 *
 * The BRAM array log_to_pys contained logical to physical mapping in
 * the old scheme used in Release 1.1.
 *
 * For this release, the array is used for several mappings:
 * 	- element 0 [LOG_TKPORT]: defaults to pysio[0] (Main) is the
 *	  port that will not be closed on softboot (boot button or
 *	  takeover download). See init.c.
 *
 *	- element 4 [LOG_DEFPRINT]: default printer. [The value 4 is for
 *	  compatibility with Release 1.1].
 *
 * Other elements of the array are opened for the future, such as
 * other printer mappings if we decide to offer that feature. 
*/
#define LOG_TKPORT	0
#define LOG_DEFPRINT	4
#define LOG_COUNT	5	/* number of logical mapping */

#define LOG_PRINTER	LOG_DEFPRINT	/* compatibility */
/*#define LOG_HOST1	LOG_TKPORT	/* compatibility */


#ifdef INIT_C
int Num_devices;		/* Number of installed devices */
int Num_hosts;			/* Number of installed devices possibly configured as host */
int Num_printers;		/* Number of installed devices possibly configured as printer*/
struct Logio *Maplog[LOG_COUNT];	/* Mapping array */
#else
extern int Num_devices;
extern int Num_hosts;
extern int Num_printers;
extern struct Logio *Maplog[LOG_COUNT];
#endif


/* These are the known codes for expansion cards.
** These codes are used to answer the Terminal Configuration Request
** Escape Sequence. See description in $Libsys/idex.c.
*/

/* I/O slot:
*/
#define	NO_IO_EXPAND	0x00	/* no card installed on the I/O slot */
#define SSISIO_INSTALL	0x01	/* SSI/SIO card */
#define ISDN_INSTALL	0x02	/* ISDN card */

/* Memory slot:
*/
#define NO_MEM_INSTALL	0x00	/* no card installed on the Memory slot */
#define MEM_INSTALL	0x10	/* 512K expansion RAM card */
#define ISO10_INSTALL	0x20	/* StarLAN ISO-10 NAU card */
#define TCP_IP_INSTALL	0x30	/* TCP/IP NAU card */
E 1
