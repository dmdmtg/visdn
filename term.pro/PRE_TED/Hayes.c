/* The X and DMD versions of this file are the same since the guts of the
 * phone logic are in here, and it's easier to maintain one file.
 * As such, you'll see lots of "#ifdef DMD" code.
 *
 * Eric Claeys, 1/95
*/

#ifdef	DMD630
#include "visdn.h"
#else
#include "xisdn.h"
#endif

#include "Hayes.h"

/* USE730WINDOWS is only used with DMD630, so to lessen the number of
 * #ifdef's, don't include USE730WINDOWS in DMD630's.
*/
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */

#ifndef	DMD630
#include <sys/signal.h>
#include <sys/types.h>
#include <errno.h>
#endif

#ifdef	DMD630
#define	time_t	unsigned long	/* to make sharing code with "host" easier */
unsigned long realtime();
#else
#define	realtime()	time((time_t *)NULL)
#endif

#ifdef	DMD630
extern void HayesRcv();
extern void HayesXmit();
extern void HayesCancel();
extern void HandlePhoneEvent();
#else
	/* These normally would be passed as arguments,
	 * but to make the X and DMD code more similar,
	 * make them global.
	*/
extern	FILE	*Pifp,	/* Phone line input */
		*Pofp,	/* Phone line output */
		*Xofp;	/* Xtentol output */
extern int  DispMode;
#endif
extern void HayesMsgProc();
extern int  HayesInit();
extern char *HayesMsgLookup();
extern void Zap_ca();
extern void set_Inid(), SendCallInfo(), SendFeatureInfo();

#define FEATUREMAP(x) (	(x==FEAT_ACTIVE)?LAMP_ON:\
			(x==FEAT_LOC_HOLD)?LAMP_FLASH:\
			LAMP_OFF)

typedef struct HayesMsg HayesMsg;
struct HayesMsg {
	char *	msg;
	short	nargs;
	short	type;
};

#ifndef	DMD630
/* LineApp[0] isn't really used, so add 1 */
LineApp LineApps[MAXCALLAPPS+1];	/* MAXCALLAPPS is in xisdn.h */
LineApp *CurLineApp;
int CurCallId;
#endif

/*
 * all Hayes messages are defined here.  See HayesMsgLookup() and 
 * HayesMsgProc() for details.
 *
 * (static read-only data structure) !!!Sorted by msg!!!
 */
static HayesMsg HayesMessages[] =
{
	/* msg string		nargs	type */
	"ASSOCIATED: ",		3,	TYPE_ASSOCIATED,
	"AT COMMAND MODE",	0,	TYPE_OK,
	"AT&T CAUSE: ",		2,	TYPE_ATT_CAUSE,
	"AT&T DISPLAY: ",	2,	TYPE_ATT_DISPLAY,
	"AT&T LAMP: ",		3,	TYPE_ATT_LAMP,
	"AT*I",			0,	TYPE_STATUS_QUERY,
	"BUSY: ",		2,	TYPE_BUSY,
	"CALL: ",		1,	TYPE_CALL,
	"CLEARED: ",		1,	TYPE_CLEARED,
	"COMPLETED: ",		2,	TYPE_COMPLETED,
	"CONNECTED: ",		1,	TYPE_CONNECTED,
	"DELIVERED: ",		1,	TYPE_DELIVERED,
	"DISCONNECT: ",		1,	TYPE_DISCONNECT,
	"DISPLAY: ",		2,	TYPE_DISPLAY,
	"ERROR",		0,	TYPE_ERROR,
	"FEATURE: ",		3,	TYPE_FEATURE,
	"G: ",			2,	TYPE_GREEN_LAMP,
	"L3_ERROR: ",		3,	TYPE_L3_ERROR,
	"OK",			0,	TYPE_OK,
	"PROCEEDING: ",		1,	TYPE_PROCEEDING,
	"PROGRESS: ",		3,	TYPE_PROGRESS,
	"PROMPT: ",		1,	TYPE_PROMPT,
	"ProPhone ",		0,	TYPE_VERSION,
	"R: ",			2,	TYPE_RED_LAMP,
	"REJECTED: ",		2,	TYPE_REJECTED,
	"RING: ",		1,	TYPE_RING,
	"SIGNAL: ",		2,	TYPE_SIGNAL,
	"SWITCHHOOK: ",		1,	TYPE_SWITCHHOOK,
	(char *)0,		0,	0,
};

#ifdef	USE730WINDOWS

Word tiledata[] = {
	0xFFFF,0xFFFF,0xFFFF,0xE000,
	0xFFFF,0xFFFF,0xFFFF,0xE000,
	0xC000,0x0000,0x0000,0xE000,
	0xC000,0x0000,0x0000,0xE000,
	0xC001,0xFFFF,0xFFFE,0xE000,
	0xC7F1,0x0000,0x0002,0xE000,
	0xC809,0x7FFF,0xFF82,0xE000,
	0xC809,0x4000,0x00B2,0xE000,
	0xC809,0x5465,0x24B2,0xE000,
	0xC809,0x4000,0x00B2,0xE000,
	0xC809,0x4000,0x00B2,0xE000,
	0xC809,0x4000,0x00B2,0xE000,
	0xC809,0x7FFF,0xFF82,0xE000,
	0xC7F1,0x0000,0x0002,0xE000,
	0xC411,0x7F3F,0x9FF2,0xE000,
	0xC411,0x7120,0x9012,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x7F3F,0x9FF2,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x7F3F,0x9FF2,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x7F3F,0x9FF2,0xE000,
	0xC411,0x7020,0x9012,0xE000,
	0xC411,0x7020,0x9012,0xE000,
	0xC411,0x7F3F,0x9FF2,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x4120,0x9012,0xE000,
	0xC411,0x7F3F,0x9FF2,0xE000,
	0xC411,0x0000,0x1012,0xE000,
	0xC411,0x1B6C,0x1012,0xE000,
	0xC411,0x1B6C,0x1FF2,0xE000,
	0xC411,0x0000,0x1012,0xE000,
	0xC7F1,0x1B6C,0x1012,0xE000,
	0xC809,0x1B6C,0x1FF2,0xE000,
	0xC809,0x0000,0x1012,0xE000,
	0xC809,0x1B6C,0x1012,0xE000,
	0xC809,0x1B6C,0x1FF2,0xE000,
	0xC809,0x0000,0x0002,0xE000,
	0xC809,0x1B6C,0x0002,0xE000,
	0xC809,0x1B6C,0x4002,0xE000,
	0xC7F1,0x0000,0xAAD2,0xE000,
	0xC001,0x0000,0x4002,0xE000,
	0xC001,0x0000,0x0002,0xE000,
	0xC001,0xFFFF,0xFFFE,0xE000,
	0xC000,0x0000,0x0000,0xE000,
	0xC000,0x0000,0x0000,0xE000,
	0xFFFF,0xFFFF,0xFFFF,0xE000,
	0xFFFF,0xFFFF,0xFFFF,0xE000,
	0xFFFF,0xFFFF,0xFFFF,0xE000,
};

Bitmap tilemap =
{
	(Word *)tiledata, 4, 0, 0, 51, 51, 0,
};
#endif /* USE730WINDOWS */

#ifdef	DMD630
#define Redialstring "*BR"
#define Dropstring "*H"

/*
 * event loop for the phone interface
 */
PhoneProc()
{
	register int state;
	unsigned long rt;
	extern Proc *mainproc;
	unsigned long realtime();
	char *argv[ 2 ];
#ifdef	USE730WINDOWS
	int labelsyet = 0;
#endif /* USE730WINDOWS */

	P->uargv = argv;
	P->uargv[0] = "visdn";
	P->uargv[1] = 0;
	P->uargc = 1;
	PhoneMsgProc = HayesMsgProc;

	Tries = 1;
	Trynumber = 0;
	Connectseconds = 20;
	Retryseconds = 60*2;

	HayesInit();
	/*
	 * Hayes initialization sequence:
	 *
	 * V1		set verbose (must be first!)
	 * E1		set echo on
	 * S11=nnn	set DTMF duration to nnn
	 * &&L		query lamp status
	 * &&I		query ProPhone version
	 */
	OutEnQueue(&PhoneOutQueue, "V1;E1;S11=120;&&L;&&I");
	InitDisplay(Type);
	SendHost(PROC_PHONE, INITTERM, (char *)0);

	P->state |= LOCKLAYER;
	(void) request(DELETE|CPU | ALARM | MOUSE | KBD | RESHAPED | SEND | RCV);

	while (1)
	{
#ifdef	USE730WINDOWS
		/* Turn on 730windows labels */
		if( !labelsyet )
		{
			if( has730wins() )
			{
				labelon();
				set730wins(P, 1, &tilemap );
				labelsyet = 1;
				reshape( P->layer->rect );
			}
		}
#endif /* USE730WINDOWS */

		state = wait(CPU | ALARM | MOUSE | KBD | RCV | DELETE);

		if( own() & DELETE )
		{
			PhoneExit();
		}

#ifdef	USE730WINDOWS
		/* Do 730windows gadgets */
		if( own() & MOUSE )
		{
			if( chk730wins() != 0 )
				continue;
		}
#endif /* USE730WINDOWS */

		if (!UnixThere)
		{
			/* host comm window gone! */
			PhoneExit();
		}

		if (P->state & (RESHAPED|MOVED))
		{
			if( ! ISTILED( P ) )
			{
				ReShaped = 1;
				ReLocate();
				if (!(P->state & MOVED))
					ReDisplay();
			}
			P->state &= ~(RESHAPED | MOVED);
		}

		/* check to see if we are still waiting for OK from the phone */
		if (state & ALARM)
		{
			DPRINTF("OUTPUT QUEUE ALARM FIRED!\n");
			/* pretend we got an OK */
			OQackmsg(&PhoneOutQueue);
		}

		if (PhoneEvent)
		{
			HandlePhoneEvent();
		}

		if (state & MOUSE)
			DoMouse();

		if (state & KBD)
		{
			if (KBcurrent==(KBrect *)0)
				ringbell();
			else
				(*KBcurrent->rcv)(KBcurrent);
		}

		if (state & RCV)
			HayesRcv();

		if (TimerList && ((rt = realtime())-LastTime) >= TIMER_INT)
			TimerEvent(rt);

		if (!OQEMPTY(&PhoneOutQueue))
			HayesXmit();

		HelpThere = 0;
		UnixThere--;		/* give UNIXPROC a chance */
		InfoThere = 0;
	}
}

/*
 * main receive loop for characters coming from the phone
 *
 * More than one message can be processed here if characters have
 * been queued from the phone.
 */
void HayesRcv()
{
	register int c;
	register char *cp;

	/* PhoneBufPtr remembers where we are in the current receive line */
	for (cp=PhoneBufPtr; (c = rcvchar()) != -1; )
		if (c=='\r' || c=='\n')
		{
			/* end of line -- process it */
			*cp = '\0';
			cp = PhoneInBuf;
			(*PhoneMsgProc)(cp);
		} else
			*cp++ = c;
	/* store ending point */
	PhoneBufPtr = cp;
}

/*
 * handle events from other processes
 */
void HandlePhoneEvent()
{
	register char *s;

	if (PhoneEvent & EVENT_RCVBUF)
	{
		s = TermProcs[PROC_PHONE].rcvbuf;
		switch (*s++)
		{
		case ERASECHAR:
			if (*s)
				CErase = *s;
			break;
		case KILLCHAR:
			if (*s)
				CKill = *s;
			break;
		case HOSTVERSION:
			DispVersion(F_XOR);	/* erase old version */
			strncpy(HostVersion, s, sizeof(HostVersion));
			DispVersion(F_XOR);	/* display new version */
			break;
		case KEYPADPOS:
			KeyPadMove(s);
			break;
		case SPDCALL:
			AddSpeedCall(s);
			break;
		case FBLABEL:
			if (*s)
			{
				FeatBut *fp, *ButLabel();
	
				if ((fp = ButLabel(s)) != 0)
				{
					if (NeedReSize)
					{
						ReSizeScreen();
						ReLocate();
						ReDisplay();
					} else
						DispBut(fp, &BUT_FONT);
				}
			}
			break;
		case FBCREAT:
			if (*s)
			{
				FeatBut *fp, *AddFeatBut();

				if ((fp = AddFeatBut(s)) != 0)
				{
					if (NeedReSize)
					{
						ReSizeScreen();
						ReLocate();
						ReDisplay();
					} else
						DispBut(fp, &BUT_FONT);
				}
			}
			break;
		case DIALPARAM:
			if (strncmp(s, "tries=", 6) == 0)
				Tries = atoi(&s[6]);
			else if (strncmp(s, "connectseconds=", 15) == 0)
				Connectseconds = atoi(&s[15]);
			else if (strncmp(s, "retryseconds=", 13) == 0)
				Retryseconds = atoi(&s[13]);
			break;
		case DIALINFO:
			DialPlan.Parse(s);
			break;
		}
		PhoneEvent &= ~EVENT_RCVBUF;
	}
}

/*
 * return index of first available call appearance
 */
NextFreeLine()
{
	register int i;

	for (i=1; i<=CallApps; i++)
		if ((LineApps[i].status & CONNECTED)==0)
		{
			DPRINTF("next free=%d\n", i);
			return i;
		}
	return 1;
}

#else	/* DMD630 */

/* global data */
char		PhoneInBuf[256];

#include <setjmp.h>

jmp_buf pipesigenv;

pipesigproc(sig)
{
	longjmp(pipesigenv, sig);
}

/*
 * main receive loop for characters coming from the phone
 * Uses the global FILE pointers Pifp, Pofp, and Xofp.
 */
PhoneProc()
{
	int	got_line = 1;	/* assume 1st read works */

	(void) HayesInit();

	CurLineApp = &LineApps[1];

	setjmp(pipesigenv);
	signal(SIGPIPE, pipesigproc);

	clearerr(Pifp); clearerr(Pofp);
	clearerr(Xofp);
	while (1)
	{
		extern int errno; errno = 0;

		if (fgets(PhoneInBuf, sizeof(PhoneInBuf), Pifp) != NULL)
		{
			got_line = 1;
			if (PhoneInBuf[0] != '\r' && PhoneInBuf[0] != '\n')
			{
				HayesMsgProc(PhoneInBuf);	/* writes to Xofp */
				fflush(Xofp);
			}
		} else {
			/* If the line to the phone 
			 * is disconnected, fgets() returns NULL and we'll
			 * go into a busy loop until the line is
			 * reconnected and the user sends the phone
			 * something and the phone sends us something back.
			 * Rather than waste resources, sleep between tries.
			 * Print a message the first time it happens after
			 * a successful time (in case the line gets reconnected
			 * then disconnected again).
			*/
			if (errno == 0) {
				/* If the line is disconnected, errno = 0 */
				if (got_line == 1) {
					(void) fprintf(stderr, "\n");
					(void) fprintf(stderr, "xisdn: Read of phone line failed!\n");
					(void) fprintf(stderr, "Is the line to the phone connected?  If not reconnected it.\n");
					(void) fprintf(stderr, "Either way, try sending something to the phone using the mouse.\n");
					(void) fprintf(stderr, "\n");
				}
			} else if (errno != EINTR) {
				/* Should we exit here? */
				perror("xisdn: fgets() failed!");
			}
			sleep(5);
			got_line = 0;
		}
		clearerr(Xofp);
		clearerr(Pifp); clearerr(Pofp);
	}
}
#endif	/* DMD630 */

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX	FIX (?)
 * The functionality in set_Inid() should probably be moved
 * to the LCD_MSG_INID case in Setmsg() in DisplayMod.c (where there is
 * similar functionality anyway).
*/

/* Set the current call's inid if known.
 * This is used to help us log calls better.
*/
void
set_Inid(lp)
register LineApp	*lp;
{
	char	*inid;

	if (lp->line == -1 && lp->direction == IS_LOGGED) {
		/* The line is disconnected and already logged,
		 * so don't do anything with it.
		*/
		return;
	}
	/* If there already is an inid for this line, use it. */
	if (*lp->inid != '\0') {
		inid = lp->inid;
	} else {
		lp->direction = IS_NONE;
		/* Not set yet, so can't do it. */
		return;
	}

	/* There are around 100 different possible values for inid.
	 * We really only care whether or not the call is an incoming
	 * or an outgoing call.
	 * We could probably replace all the strncmp()'s with the check for
	 * outgoing number, but I didn't bother to change the code (I have
	 * a feeling that ProPhone/ISDN sometimes doesn't have an outgoing
	 * number (i.e., I wouldn't be surprized if there was a bug somewhere),
	 * and we don't want to treat an outgoing call as an incoming one.
	 *
	 * Besides, probably 99% of all calls are InI, InX, OuI, or OuX.
	*/
	if (strncmp(inid, "InI", 3) == 0 || strncmp(inid, "InX", 3) == 0) {
		lp->direction = IS_INCOMING;
	} else if (strncmp(inid, "OuI", 3) == 0 || strncmp(inid, "OuX", 3) == 0) {
		lp->direction = IS_OUTGOING;
	} else {
		/* If there is an outgoing number, assume the call
		 * is outgoing.  Else assume it's incoming.
		 * THIS ISN'T ALWAYS TRUE, THOUGH:
		 * Inbound CFA calls will have BOTH incoming and outgoing
		 * names and numbers, and the code below will assume they
		 * are outgoing calls.
		 * THERE'S GOT TO BE A BETTER, 100% guaranteed way to determine
		 * if a call is inbound or outbound.
		*/
		if (lp->calleddn[0] != '\0')
			lp->direction = IS_OUTGOING;
		else
			lp->direction = IS_INCOMING;
	}
}

/*
 * We just connected -- process it
 */
static void
do_connect(ci)
int ci;		/* call id */
{
	CurCallId = ci;
	CurLineApp = &LineApps[CurCallId];
	CurLineApp->status |= CONNECTED;
	CurLineApp->line = CurCallId;
	SendCallInfo(CurLineApp, CONNECTED);
}

/*
 * process one Hayes message
 */
void HayesMsgProc(str)
char *str;
{
	register int i;
	register char *s;
	int mdata[4];
	static int	speaker_on = 0, handset_up = 0;

	if (str==NULL || *str=='\0' || *str=='\n' || *str=='\r')
		return;

	/* optionally save the line */
#ifdef	DMD630
	PPRINTF(str);
#else
	/* The X version writes ALL debugging to stdout, whereas the DMD
	 * version writes PPRINTF to stderr, and the rest to the window.
	 * Should these be the same ????
	*/
	DPRINTF(">> %s", str);
#endif	/* DMD630 */

#ifndef DMD630		/* Helps in debug printing */
	/* XXXXXX should we do this on the DMD also ???????? */
	str[strlen(str)-1] = '\0';	/* zap the trailing newline */
#endif

	s = HayesMsgLookup(str, mdata);

	switch (mdata[0])
	{
	case TYPE_ERROR:
		DPRINTF("ERROR ");
		/* fall through */
	case TYPE_OK:
#ifdef	DMD630
		/* ack message */
		OQackmsg(&PhoneOutQueue);
#endif	/* DMD630 */
		DPRINTF("OK\n");
		return;

	case TYPE_FEATURE:	/* Feature lamp Selection */
		DPRINTF("FEATURE: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		/*
		 * LAMP.Ctrl(LTYPE_STATUS, mdata[2], FEATUREMAP(mdata[3]), 0);
		 */
		SendFeatureInfo(mdata[1], mdata[2], mdata[3]);
		return;

	case TYPE_ATT_LAMP:	/* Feature lamp Selection */
		DPRINTF("ATT LAMP: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
#ifdef	DMD630
		LAMP.Ctrl(mdata[1], (mdata[2]==0) ? MSGLAMPNO : mdata[2], mdata[3], 0);
#else	/* XXXX why are these different ????? */
		LAMP.Ctrl(mdata[1], mdata[2], mdata[3], 0);
#endif	/* DMD630 */
		return;

	case TYPE_DISPLAY:	/* Display Message */
		/* Make the assumption (I'm pretty sure it's valid) that all
		 * DISPLAY lines are sent to us one after the other with no
		 * intervening other lines, i.e., we don't have to worry about
		 * the DISPLAY messages being interspersed with other messages.
		 * Also assume that if there's a call appearance DISPLAY message
		 * that it will be the FIRST one, so we can set CurCallId to it
		 * and the other DISPLAY messages can use CurCallId.
		*/

		/* Format:	DISPLAY: field mode/submode string */
		i = mdata[1]-1;		/* LCD field - ProPhone needs -1 */

		/* Save the CALLAPP so when we see an INID, we know what callapp
		 * to save other information in.
		*/
		if (i == LCD_MSG_CALLAPP && (mdata[2] & M_MASK) == M_NORMAL) {
			/* "s" is a 2 character hex number with the 1st a space
			 * but HexNibble() needs 2 digits.
			*/
			s[0] = '0';
			CurCallId = HexNibble(s);
			CurLineApp = &LineApps[CurCallId];
		}
		DPRINTF("DISP: %x %x (%x) '%s'\n", mdata[1], mdata[2], CurCallId, s);

#ifdef	DMD630
		if (!OQEMPTY(&PhoneOutQueue) &&
			    (OQTOP(&PhoneOutQueue)->flags & OQ_WAIT_FOR_RETRY))
		{
			/* Cancel a retry in progress immediately */
			HayesCancel();
		}
#endif	/* DMD630 */

		/* This will also save the necessary CurLineApp fields. */
		LCD.Setmsg(i, mdata[2], s, CurLineApp);
		return;

	case TYPE_ATT_DISPLAY:	/* Display Mode Change */
		DPRINTF("DISPMODE: %x %x curmode=%x\n", mdata[1], mdata[2], DispMode);
		LCD.Setmode(mdata[2]);
		return;

	case TYPE_RING:		/* call appearance ringing */
		DPRINTF("RING: %x\n", mdata[1]);
/* XXX FIX (?): should we set the call app's ->direction to IS_INCOMING?? */
#ifdef	DMD630
		if (MakeTop)
		{
#ifdef	USE730WINDOWS
			if( ISTILED( P ) )
				untile730wins( P );
			else
#endif /* USE730WINDOWS */
			top();
		}
		if (!OQEMPTY(&PhoneOutQueue) &&
			    (OQTOP(&PhoneOutQueue)->flags & OQ_WAIT_FOR_CONN))
		{
			/* Cancel - don't try any more after this one */
			Trynumber = Tries;
		}
#else
		(void) fprintf(Xofp, "xtent.XtRing: %x\n", mdata[1]);
#endif	/* DMD630 */
		return;

	case TYPE_CALL:		/* call appearance active */
		DPRINTF("CALL: %x\n", mdata[1]);
#ifdef	USE730WINDOWS
		if( ISTILED( P ) )
			untile730wins( P );
#endif /* USE730WINDOWS */
/* XXX FIX (?): should we set the call app's ->direction to IS_OUTGOING?? */
		return;

	case TYPE_SIGNAL:	/* line signal received */
		DPRINTF("SIG: %x %x", mdata[1], mdata[2]);
		CurCallId = mdata[1];
		CurLineApp = &LineApps[CurCallId];
		/* Either a call is coming in, or we're
		 * making (or about to make) a call,
		 * so save the CurCallId so we can keep
		 * track of 'c'onnected and 'u'nanswered calls.
		 * In some cases, no call will be made (e.g., you lift up
		 * the receiver then put it back).
		 * In those cases, the "inid" will never be set, so
		 * we don't log those calls.
		*/
		switch (mdata[2])
		{
		case SIG_ALERT0:	/* ringing pattern 0 */
		case SIG_ALERT1:	/* ringing pattern 1 */
		case SIG_ALERT2:	/* ringing pattern 2 */
		case SIG_ALERT3:	/* ringing pattern 3 */
		/* don't use 4 - 7 */

			/* New call; clear out old */
			CurLineApp->line = CurCallId;
			if (CurLineApp->direction == IS_LOGGED)
				CurLineApp->direction = IS_NONE;
			DPRINTF(" ringing");
			break;
		case SIG_DIALTONE:
			/* New call; clear out old */
			CurLineApp->line = CurCallId;
			if (CurLineApp->direction == IS_LOGGED)
				CurLineApp->direction = IS_NONE;
			CurLineApp->status |= DIALTONE;
			DPRINTF(" dialtone on");
			break;
		case SIG_NOTONE:
			CurLineApp->status &= ~DIALTONE;
			DPRINTF(" dialtone off");
			break;
		case SIG_NOALERT:
			CurLineApp->status &= ~(DIALTONE);
			if (CurLineApp->status & CONNECTED)
			{
				DPRINTF(" lamp on");
			} else
			{
				DPRINTF(" no alerting");
			}
			break;
		}
		DPRINTF("\n");
		return;

	case TYPE_CLEARED:	/* call appearance free */
		DPRINTF("CLEARED: %x (%x)\n", mdata[1], CurCallId);
		CurLineApp = &LineApps[mdata[1]];
		SendCallInfo(CurLineApp, 0);
		if (CurLineApp->status & CONNECTED) {
			CurLineApp->status &= ~CONNECTED;
		} else if (CurLineApp->line != -1) {
			/* SendCallInfo() is called for the "else" part also
			 * since it may not have
			 * been called in TYPE_DISCONNECT (e.g., if you hang up
			 * the phone, TYPE_DISCONNECT is NOT called, but if
			 * you press the "Drop" button, it is).
			 * Calling it here makes sure
			 * that the log file contains an 'u'nanswered if
			 * the other party didn't answer.
			*/
			CurLineApp->line = -1;	/* don't call again */
			/* CurLineApp->direction set in SendCallInfo() */
			CurLineApp->direction = IS_NONE;
		}
		LCD.Zap(M_NORMAL);	/* do AFTER SendCallInfo() */
		Zap_ca(CurLineApp, 1);	/* 1 == NULL out the inid */
#ifdef	notdef
#ifdef	USE730WINDOWS
		{
			int i, con = 0;

			for( i = 0; i < MAXCALLAPPS; ++i )
			{
				if( LineApps[ i ].status & CONNECTED )
					con++;
			}
			if( !con )
				tile730wins( P );
		}
#endif	/* USE730WINDOWS */
#endif	/* notdef */
#ifdef	DMD630
		if (!OQEMPTY(&PhoneOutQueue) &&
			    (OQTOP(&PhoneOutQueue)->flags & OQ_WAIT_FOR_RETRY))
		{
			char msg[60];
			sprintf(msg, "Waiting for retry %d.  Use menu to cancel.", Trynumber);
			LCD.Setmsg(LCD_MSG_FULLDISP, M_NORMAL, msg, (struct LineApp *)NULL);
		}
#endif	/* DMD630 */
		return;

	case TYPE_L3_ERROR:	/* possible hidden connect message */
		/* mdata[1] == callapp, 2 == msgtype, 3 == previous state */
		DPRINTF("L3_ERROR: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		if (mdata[2] == L3_CONNECT) {
#ifdef NOT_NOW

XXXXXXXX for now, don't do this.  I'm not 100% sure the following comment
	 is really true.

			/* For whatever reason, some incoming calls get
			 * TYPE_L3_ERROR instead of TYPE_CONNECTED.
			 * Treat them the same.
			 * If a call gets both, SendCallInfo() will ignore
			 * the 2nd one.
			*/
			do_connect(mdata[1]);
#else
			LineApps[mdata[1]].status |= CONNECTED;
#endif
		}
		return;

	case TYPE_CONNECTED:	/* call connected */		
		DPRINTF("CONNECT: %x\n", mdata[1]);
		if ((i = mdata[1]) >= 0 && i <= CallApps) {
			do_connect(i);
		} /* else ignore it */
		return;

	case TYPE_DISCONNECT:	/* call disconnected */
		DPRINTF("DISCONN: %x\n", mdata[1]);
		if ((i = mdata[1]) >= 0 && i <= CallApps)
		{
			CurLineApp = &LineApps[i];
			SendCallInfo(CurLineApp, 0);
			CurLineApp->status &= ~(HELD|SETUP_ACK|CONNECTED);
			/* InX calls that aren't answered have no CONNECT or
			 * SIG (or anything else) to tell us 
			 * there was an incoming call.
			 * (But, when the caller hangs up (or Audix picks up),
			 * we get DISCONNECT.)
			 * So, CurLineApp->line will be == -1, and
			 * CurLineApp->direction may be IS_LOGGED (if a previous
			 * call was logged).  This will keep future InX calls
			 * from being logged, so set it to IS_NONE now if there
			 * was no line (== -1).
			 * Sometimes, however, unanswered InX calls give BOTH
			 * DISCONNECT *and* CLEARED, which means we try to log
			 * the call here, AND in the CLEARED code above.
			 * SendCallInfo() will only log the call once.
			 *
			 * If instead, the InX call is answered, we get a
			 * TYPE_CONNECTED and all is fine.
			*/
			if (CurLineApp->line == -1)
				CurLineApp->direction = IS_NONE;
			else {
				/* CurLineApp->direction set in SendCallInfo()*/
				CurLineApp->line = -1;
			}
		} /* else ignore it */
		return;

	case TYPE_BUSY:		/* busy */		
		DPRINTF("BUSY: %x\n", mdata[1]);
		if ((i = mdata[1]) >= 0 && i <= CallApps)
		{
			CurCallId = i;
			CurLineApp = &LineApps[i];
			CurLineApp->status |= BUSY;
		}
		return;

	case TYPE_COMPLETED:	/* some action completed */
		DPRINTF("COMPLETED: %x %x ", mdata[1], mdata[2]);
		CurCallId = mdata[1];
		CurLineApp = &LineApps[CurCallId];
		switch (mdata[2])
		{
		case ACT_HOLD:
			DPRINTF("(ACT_HOLD)\n");
			/* ACT_HOLD happens (and not TYPE_CONNECTED) if you
			 * call yourself.
			*/
			do_connect(CurCallId);
			break;
		case ACT_RECONN:
			DPRINTF("(ACT_RECONN)\n");
			break;
		case ACT_CONF:
			/* This drops one line, then ACT_RECONN's to the other line */
			DPRINTF("(ACT_CONF)\n");
			break;
		case ACT_DROP:
			DPRINTF("(ACT_DROP)\n");
			break;
		case ACT_XFER:
			DPRINTF("(ACT_XFER)\n");
			break;
		}
		return;

	case TYPE_PROCEEDING:
		DPRINTF("PROCEEDING: %x\n", mdata[1]);
		/* fake message to indicate which line appearance was chosen */
		s[0] = ' ';
		s[1] = '0' + mdata[1];
		s[2] = '\0';
		/* XXXX Don't save this call appearance's info since I think
		 * it's already saved -- every time I saw a TYPE_PROCEEDING in
		 * my debug output, there was always a TYPE_DISPLAY line that
		 * set the callapp a couple lines above.
		 * Although I never saw any other TYPE_DISPLAY lines,
		 * I'm not sure if it's possible to have them right before
		 * a TYPE_PROCEEDING -- if so, and we have a non-NULL 4th
		 * arguement to LCD.Setmsg(), then all the other TYPE_DISPLAY
		 * info we just saved would be wiped out.
		 *
		 * In fact, I'm not even sure if the following LCD.Setmsg()
		 * line and the above s[?] lines are even needed here since
		 * the callapp appears to already be saved.
		LCD.Setmsg(LCD_MSG_CALLAPP, M_NORMAL, s, &LineApps[mdata[1]]);
		XXXX */

		LCD.Setmsg(LCD_MSG_CALLAPP, M_NORMAL, s, (struct LineApp *)NULL);
		/* Because the above doesn't save the callapp in "ca", do it here. */
		(void) strncpy(LineApps[mdata[1]].callapp, LCD.Msgstring(M_NORMAL, LCD_MSG_CALLAPP), LCD_CALLAPPSIZE);
		return;

	case TYPE_SWITCHHOOK:	/* Switchhook Transitions */
		DPRINTF("SH: %x ", mdata[1]);
		switch (mdata[1])
		{
		case SH_SPEAKER_ON:
			DPRINTF("(SPEAKER ON)\n");
#ifdef	USE730WINDOWS
			untile730wins( P );
#endif	/* USE730WINDOWS */
			speaker_on = 1;
#ifdef	DMD630
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_ON, 0);
#else
			fprintf(Xofp, "xtent.XtSpeakerLamp: on\n");
#endif	/* DMD630 */
			break;
		case SH_REMOTE_ON:
			DPRINTF("(REMOTE ON)\n");
#ifdef	DMD630
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_ON, 0);
#else
			fprintf(Xofp, "xtent.XtSpeakerLamp: on\n");
#endif	/* DMD630 */
			break;
		case SH_ADJUNCT_ON:
			DPRINTF("(ADJUNCT ON)\n");
			/* Turning the adjunct on when the speaker is on SHOULD
			 * cause the speaker to go off, but we never get the
			 * SH_SPEAKER_OFF message (prophone bug?), so do
			 * it manually here.
			 * Ditto for the mute lamp.
			*/
#ifdef	DMD630
	/* XXXXXX ECC: FIX per X version */
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_OFF, 0);
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_OFF, 0);
#else
			fprintf(Xofp, "xtent.XtAdjunctLamp: on\n");
			fprintf(Xofp, "xtent.XtSpeakerLamp: off\n");
			fprintf(Xofp, "xtent.XtMuteLamp: off\n");
#endif	/* DMD630 */
			speaker_on = 0;
			break;
		case SH_SPEAKER_OFF:
			DPRINTF("(SPEAKER OFF)\n");
			speaker_on = 0;
			/* FALLTHROUGH */
		case SH_REMOTE_OFF:
			if (mdata[1] == SH_REMOTE_OFF)
				DPRINTF("(REMOTE OFF)\n");
			/* Turning the speakerphone off SHOULD cause the mute
			 * lamp to go off, but we never get the SH_MUTE_OFF
			 * message (prophone bug?), so do it manually here.
			*/
#ifdef	DMD630
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_OFF, 0);
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_OFF, 0);
#else
			fprintf(Xofp, "xtent.XtSpeakerLamp: off\n");
			fprintf(Xofp, "xtent.XtMuteLamp: off\n");
#endif	/* DMD630 */
			break;
		case SH_ADJUNCT_OFF:
			DPRINTF("(ADJUNCT OFF)\n");
#ifdef	DMD630
	/* XXXXXX ECC: FIX per X version */
#else
			fprintf(Xofp, "xtent.XtAdjunctLamp: off\n");
#endif	/* DMD630 */
			break;
		case SH_HANDSET_UP:
			DPRINTF("(HANDSET UP)\n");
			handset_up = 1;
#ifdef	DMD630
	/* XXXXXX ECC: FIX per X version */
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_OFF, 0);
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_OFF, 0);
#endif	/* DMD630 */
			break;
		case SH_HANDSET_DN:
			DPRINTF("(HANDSET DOWN)\n");
			handset_up = 0;
			break;
		case SH_MUTE_ON:
			DPRINTF("(MUTE ON)\n");
			/* only if SPEAKER_ON or HANDSET_UP */
			if (speaker_on || handset_up) {
#ifdef	DMD630
				LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_ON, 0);
#else
				fprintf(Xofp, "xtent.XtMuteLamp: on\n");
#endif	/* DMD630 */
			}
			break;
		case SH_MUTE_OFF:
			DPRINTF("(MUTE OFF)\n");
#ifdef	DMD630
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_OFF, 0);
#else
			fprintf(Xofp, "xtent.XtMuteLamp: off\n");
#endif	/* DMD630 */
			break;
		case SH_SPOKESMAN_ON:
			DPRINTF("(SPOKESMAN ON)\n");
			break;	/* What, if anything, should be done? */
		case SH_SPOKESMAN_OFF:
			DPRINTF("(SPOKESMAN OFF)\n");
			break;	/* What, if anything, should be done? */
		default:
			DPRINTF("(INTERNAL ERROR: Unknown SH_* entry)\n");
			break;
		}
		return;

	case TYPE_RED_LAMP:
		DPRINTF("RED LAMP %x %x\n", mdata[1], mdata[2]);
		LAMP.Ctrl(LTYPE_INUSE, mdata[1], mdata[2], 0);
		return;

	case TYPE_GREEN_LAMP:
		DPRINTF("GREEN LAMP %x %x\n", mdata[1], mdata[2]);
#ifdef	DMD630
		LAMP.Ctrl(LTYPE_STATUS, (mdata[1]==0) ? MSGLAMPNO : mdata[1], mdata[2], 0);
#else	/* XXXXX why are these different ???? */
		LAMP.Ctrl(LTYPE_STATUS, mdata[1], mdata[2], 0);
#endif	/* DMD630 */
		/* We need this to determine if the feature is active or
		 * inactive when we start the program.
		 * Only the status for the lamps is given during
		 * initialization, not the status of the features.
		 * Only look for the lamp on; if it's another state,
		 * consider it to be off.
		 * There's no good way (short of rewriting major parts of the
		 * code) to determine if we're initializing the phone.  We
		 * really only want to call SendFeatureInfo() while we're
		 * initializing the phone, because after that, the TYPE_FEATURE
		 * is sent when the status of a feature changes.
		*/
		SendFeatureInfo(0, mdata[1], mdata[2] == LAMP_ON ? FEAT_ACTIVE : FEAT_INACTIVE);
		return;

	case TYPE_ASSOCIATED:
		DPRINTF("ASSOCIATED %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		return;		/* Do nothing */
	case TYPE_ATT_CAUSE:
		DPRINTF("ATT CAUSE %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		return;		/* Do nothing */
	case TYPE_REJECTED:
		DPRINTF("REJECTED %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		return;		/* Do nothing */
	case TYPE_DELIVERED:
		DPRINTF("DELIVERED %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		return;		/* Do nothing */
	case TYPE_PROGRESS:
		DPRINTF("PROGRESS %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		return;		/* Do nothing */
	case TYPE_PROMPT:
		DPRINTF("PROMPT %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		return;		/* Do nothing */

	case TYPE_VERSION:
		DPRINTF("VERSION: '%s'\n", str);
		LCD.Setmsg(LCD_MSG_FULLDISP, M_NORMAL, str, (struct LineApp *)NULL);
		return;
	}

	/* Is this a new type??  All known ones should be in the switch()
	 * statement above.
	*/
	DPRINTF("? %s\n", str);
}

/* Convert the given number to hh:mm:ss format. */
char *
convert_to_hhmmss(t)
time_t	t;
{
	int	hours, min, sec;
	time_t	tt;
	static	char	time_string[15];

	/* use hh:mm:ss for duration */
	hours = t / 3600;
	tt = t % 3600;		/* time minus the hours */
	min = tt / 60;
	sec = tt % 60;

#ifdef	DMD630
	Sprintf(time_string,	/* for %.2d */
#else
	sprintf(time_string,
#endif
		"%.2d:%.2d:%.2d", hours, min, sec);
	return(time_string);
}

#ifdef	DMD630
/*
 * Send info to UNIX host side for possible processing
 */
#else
/*
 * Send info to the Xtentol program for processing
*/
#endif	/* DMD630 */
void
SendCallInfo(lp, event)
register LineApp *lp;
int event;
{
	char sendbuf[128];
	register int i;
	register char *s, *cp;
	int	type;
	char	duration_string[50];
	time_t	duration;

	set_Inid(lp);

	if (lp->direction == IS_LOGGED) {
		/* will be reset when new call is on this line */
		DPRINTF("SendCallInfo(): ignoring call (already logged)\n");
		return;
	}
	if (lp->direction == IS_NONE) {
		/* No call was made, so don't log anything.
		 * e.g., the user lifted up the handset and then put it down.
		*/
		DPRINTF("SendCallInfo(): ignoring call (no direction)\n");
		return;
	}
	if (*lp->callapp == '\0') {
		/* Ignore entries without a call appearance.
		 * As far as I know, all actual phone calls have one.
		 * It's possible to get an entry without one, if, e.g.,
		 * you do a "return last in" and the person is busy,
		 * we'll get a "c"onnect (but no "d"isconnect),
		 * although I don't really consider that a valid
		 * phone call.
		*/
		DPRINTF("SendCallInfo(): ignoring call (no callapp)\n");
		return;
	}

	/*
	 * first char indicates result: c=connect, d=disconnect, u=unanswered
	 */

	s = sendbuf;
	if (lp->status&CONNECTED)
	{
		if (event != CONNECTED)
			type = 'd';
		else
			type = 'c';
	}
	else if (event != CONNECTED) {
		type = 'u';
	} else {
		/* SHOULD NEVER GET HERE ??? */
#ifdef	DMD630
		lprintf(
#else
		fprintf(stderr,
#endif	/* DMD630 */
			"lp->status (0x%x) != CONNECTED and event %d != 0 in SendCallInfo().\n", lp->status, event);
		type = '?';
	}

	duration = 0;
	duration_string[0] = '\0';

	switch (type) {
	case 'd':
		/* See if we can find the 'c' entry for this line;
		 * if so, determine the duration of call in seconds.
		*/
		if (lp->line != -1) {
			time_t end_time;
			if (lp->start_time > 0) {
				end_time = realtime();
				duration = end_time - lp->start_time;
#ifdef	DMD630
				duration /= 60;
#endif
				lp->start_time = 0;	/* we're done with it */
			} else {
				/* This will happen if we start up the program
				 * while on the phone (which is valid).
				 * It also happens at other times (which is
				 * invalid).
				*/
				DPRINTF("saved start_time for line %d was 0\n", lp->line);
			}

		} else {
#ifdef	DMD630
			lprintf(
#else
			fprintf(stderr,
#endif	/* DMD630 */
				"On 'd'isconnect in SendCallInfo, lp->line == -1\n");
			/* XXXX Should we return here instead ?? */
		}
		(void) strcpy(duration_string, convert_to_hhmmss(duration));
		break;

	case 'c':
		/* Save the start time and phone number for this line */
		if (lp->start_time > 0) {
			/* Already connected on this line, so don't log it
			 * again.  This can happen if you go to another
			 * call appearance, then come back to this one.
			*/
			DPRINTF("SendCallInfo(): ignoring call (already connected)\n");
			return;
		}
		lp->start_time = realtime();
		break;

	case 'u':
		/* duration doesn't apply, so don't do anything with it */
		break;
	}

	DPRINTF("SendCallInfo(), type=%c, line=%d (%s), lp->direction=%d, lp->inid=%s\n",
		type, lp->line, lp->callapp, lp->direction, lp->inid);

	s = sendbuf;
	*s++ = (char) type;
	*s++ = LOG_SEP;

	/* Append each display field, separating them with the appropriate char.
	 * Where present, use the saved "lp" fields,
	 * otherwise use what's on the LCD (which may be wrong if another call
	 * came in/went out while we were on the 1st call).
	*/
	for (i=0; i<LCD_MAXFIELDS; i++)
	{
		switch(i) {
		case LCD_MSG_CALLAPP:
			cp = lp->callapp;
	/* I'm not sure if these "strcpy(LCD....)"'s are needed.
	 * They don't seem to hurt, and I never had time to
	 * test without them.
	*/
			(void) strcpy(LCD.Msgstring(M_NORMAL, i), cp);
			break;
		case LCD_MSG_INID:
			cp = lp->inid;
			(void) strcpy(LCD.Msgstring(M_NORMAL, i), cp);
			break;
		case LCD_MSG_CALLEDDN:
			cp = lp->calleddn;
			(void) strcpy(LCD.Msgstring(M_NORMAL, i), cp);
			break;
		case LCD_MSG_CALLEDNM:
			cp = lp->callednm;
			(void) strcpy(LCD.Msgstring(M_NORMAL, i), cp);
			break;
		case LCD_MSG_CALLINGDN:
			cp = lp->callingdn;
			(void) strcpy(LCD.Msgstring(M_NORMAL, i), cp);
			break;
		case LCD_MSG_CALLINGNM:
			cp = lp->callingnm;
			(void) strcpy(LCD.Msgstring(M_NORMAL, i), cp);
			break;
		default:
			cp = LCD.Msgstring(M_NORMAL, i);
			break;
		}
		for (; *cp; ) {
#ifndef	DMD630
			/* Escape all unescaped ',' (for Xtentol) */
			if (*cp == ',' && *(cp-1) != '\\')
				*s++ = '\\';
#endif
			*s++ = *cp++;
		}
		*s++ = LOG_SEP;
	}

	/* s is within setbuf */
	(void) sprintf(s, "%c%s%c", LOG_SEP, duration_string, LOG_SEP);

#ifdef	DMD630
	PPRINTF(sendbuf);		/* optionally write to debug file */
	SendHost(PROC_PHONE, CALLINFO, sendbuf);     /* send to host to log */
#else
	DPRINTF(">>> %s\n", sendbuf);		/* optionally write to debug file */
	fprintf(Xofp, "xtent.XtLogCall: (%s)\n", sendbuf);
#endif	/* DMD630 */

	if (type != 'c') {
		lp->direction = IS_LOGGED;
		*lp->inid = '\0';
	}

#ifdef	DMD630
	if( event == 0 && ( lp->status&CONNECTED) == 0 )
	{
		int n;
		char buf[ 120 ];
		char snarfbuf[48];
		char area[8], exch[8], ext[8];
		char dialbuf[20];
		extern char snarfstr[];

		area[0] = exch[0] = ext[0] = '\0';
		strcpy( snarfbuf, lp->callingdn );
		MPRINTF("Add Missed '%s'\n", snarfbuf);
		if (!regex(snarfstr, snarfbuf, area, exch, ext))
		{
			MPRINTF("regex %s-%s-%s\n", area, exch, ext);

			sprintf(dialbuf, "CA%s%s",
				DialPlan.MapExch(area[0] ? area : DialPlan.areacode, exch),
				ext);
			sprintf( buf, "Missed Calls;%s;%s;", lp->callingnm, dialbuf );
			AddSpeedCall(buf);
		}
		else
		{
			MPRINTF("regex failed %s-%s-%s\n", area, exch, ext);
		}
	}
#else
	/* XXXXXX Should we do the above in the X version also????? */
#endif	/* DMD630 */
}

/*
 * Send info about the status of the specified feature.
 */
/*ARGSUSED1*/
void
SendFeatureInfo(junk, feature, status)
int	junk;		/* not used -- I don't know what it's for! */
int	feature, status;
{
	extern	int	num_features_monitored;
	extern	int	features_monitored[];
	extern	int	fm_status[];
	extern	time_t	fm_time[];
	char	*s;
	register i;
#ifndef	DMD630
	extern	FILE	*FeatureFd;
#endif	/* DMD630 */

	for (i=0; i<num_features_monitored; i++) {
		if (feature == features_monitored[i]) {
			switch (status) {
			case FEAT_ACTIVE:	s = "ACTIVE"; break;
			case FEAT_INACTIVE:	s = "INACTIVE"; break;
			case FEAT_PENDING:	s = "PENDING"; break;
			case FEAT_LOC_HOLD:	s = "HOLD"; break;
			case FEAT_REM_HOLD:	s = "REM_HOLD"; break;
			case FEAT_REQ_CONF:	s = "REQ_CONF"; break;
			case FEAT_REQ_STATE:	s = "REQ_STATE"; break;
			case FEAT_REQ_REJ:	s = "REQ_REJ"; break;
			default:		s = "unknown_status"; break;
			}
			DPRINTF("(FEATURE %d %s", feature, s);
			/* Only log the status if it's different than the
			 * current status.  ISDN sends FEATURE commands every
			 * once in a while, which usually do nothing except
			 * confuse people reading the log file.  It's not
			 * obvious why there are multiple "FEATURE x ACTIVE"
			 * type lines in a row, without any INACTIVE ones
			 * inbetween.
			*/
			if (status != fm_status[i]) {
				time_t	now, duration;
				char	*t;
#ifdef	DMD630
				char	sendbuf[128];
#endif	/* DMD630 */

				now = realtime();
#ifdef	DMD630
				now /= 60;	/* make it in seconds */
#endif	/* DMD630 */
				if (fm_time[i] != (time_t) 0)
					duration = now - fm_time[i];
				else
					duration = 0;
				t = convert_to_hhmmss(duration);
				DPRINTF(" (was %d for %s))\n", fm_status[i], t);
#ifdef	DMD630
				sprintf(sendbuf, "FEATURE\t%x\t%s\t%s", feature, s, t);
				SendHost(PROC_PHONE, FEATURE_STATUS, sendbuf);
#else
				fprintf(Xofp, "xtent.XtLogFeature: (FEATURE %d %s %s)\n", feature, s, t);
				if (FeatureFd != NULL) {
					char    time_buf[27];
					extern	char	*getenv();
					static	char	*logname = NULL;

					/* Also log it in the feature log file */
					(void) strcpy( time_buf, ctime(&now) );
					time_buf[24] = '\0';	/* get rid of \n */
					if (logname == NULL)
						logname = getenv("LOGNAME");
					fprintf(FeatureFd, "%s\tFEATURE\t%x\t%s\t%s\t%s\n",
						time_buf, feature, s, t, logname);
					(void) fflush(FeatureFd);
				}
#endif	/* DMD630 */
				fm_status[i] = status;
				fm_time[i] = now;
			} else
				DPRINTF(" (no change))\n");
			return;
		}
	}

}

#ifdef	DMD630
/*
 * clean exit -- restore original shape and notify all procs
 */
void PhoneExit()
{
	extern Proc *phoneproc;

	if (ReShaped)
		(void) reshape(SaveRect);
	PhoneEvent = 0;			/* clear my semaphore */
	GlobalEvent |= GLOBAL_QUIT;	/* set global event */

	phoneproc = 0;
	exit(0);
}

/*
 * cancel a dial command
 */
void HayesCancel()
{
	register OutQueue *oqp;

	DPRINTF("HayesCancel\n");
	Trynumber = 0;
	if (!OQEMPTY(&PhoneOutQueue))
	{
		oqp = OQTOP(&PhoneOutQueue);
		if (oqp->flags & OQ_WAIT_FOR_RETRY)
			LCD.Zap(M_NORMAL);
		OQfree(&PhoneOutQueue, oqp);
	}
}

/*
 * check the output queue for timeout, etc.
 */
void HayesXmit()
{
	register OutQueue *oqp;
	register char *cp, *tcp;
	LineApp *lp;
	int dialit;		/* only keypad digits seen -- prepend *D */
	int n;

	/* wait for "OK" or "ERROR" from phone */
	if (!OQMSGACK(&PhoneOutQueue))
		return;

	/* phone has OK'd last message -- grab top queue item and continue */
	oqp = OQTOP(&PhoneOutQueue);
	lp = &LineApps[CurCallId];

	/* check to see if we are waiting for more dial tone */
	if (oqp->flags & OQ_WAIT_FOR_TONE)
	{
		if ((lp->status & DIALTONE)==0 && oqp->timeout > 0 &&
			(realtime()-PhoneOutQueue.start_time) < oqp->timeout)
				return;
		DPRINTF("OQ dialtone timeout status %x, %lx-%lx=%lx >= %x\n", lp->status, realtime(), PhoneOutQueue.start_time, realtime()-PhoneOutQueue.start_time, oqp->timeout);
		oqp->flags &= ~OQ_WAIT_FOR_TONE;
		oqp->timeout = 0;
	}

	/* check to see if we are waiting for connection */
	if (oqp->flags & OQ_WAIT_FOR_CONN)
	{
		if (lp->status & CONNECTED) 
		{
			oqp->flags &= ~OQ_WAIT_FOR_CONN;
			oqp->timeout = 0;
			DPRINTF("OQ status: got connection\n");
		}
		else if (!(lp->status & BUSY) && ((oqp->timeout > 0 &&
			(realtime()-PhoneOutQueue.start_time) < oqp->timeout)))
		{
			/* wait for timeout */
			return;
		}
		else {
			if (lp->status & BUSY) {
				DPRINTF("OQ conn busy\n");
				lp->status &= ~BUSY;
			}
			else
				DPRINTF("OQ conn timeout status %x, %lx-%lx=%lx >= %x\n", lp->status, realtime(), PhoneOutQueue.start_time, realtime()-PhoneOutQueue.start_time, oqp->timeout);

			HayesSend(Dropstring, strlen(Dropstring), 0);

			if (Trynumber == Tries) {
				OQfree(&PhoneOutQueue, oqp);
				Trynumber = 0;
				return;
			}
			Trynumber++;
			DPRINTF("Setting retry timer, try number %d\n",
						Trynumber);
			oqp->flags &= ~OQ_WAIT_FOR_CONN;
			oqp->flags |= OQ_WAIT_FOR_RETRY;
			PhoneOutQueue.start_time = realtime();
			oqp->timeout = Retryseconds*60;
			return;
		}
	}

	/* wait for retry */
	if (oqp->flags & OQ_WAIT_FOR_RETRY)
	{
		if (oqp->timeout > 0 &&
			(realtime()-PhoneOutQueue.start_time) < oqp->timeout)
		{	
			/* wait for timeout */
			return;
		}
		DPRINTF("OQ retry timeout status %x, %lx-%lx=%lx >= %x\n", lp->status, realtime(), PhoneOutQueue.start_time, realtime()-PhoneOutQueue.start_time, oqp->timeout);
		HayesSend(Redialstring, strlen(Redialstring), 0);
		PhoneOutQueue.start_time = realtime();
		oqp->timeout = Connectseconds*60;
		oqp->flags &= ~OQ_WAIT_FOR_RETRY;
		oqp->flags |= OQ_WAIT_FOR_CONN;
		return;
	}

	/* waiting for "," */
	if (oqp->flags & OQ_PAUSE)
	{
		if ((realtime()-PhoneOutQueue.start_time) < oqp->timeout)
		{
			return;
		}
		DPRINTF("OQPAUSE timeout\n");
		oqp->flags &= ~OQ_PAUSE;
		oqp->timeout = 0;
	}

	/* do we need a new call appearance? */
	if (oqp->flags & OQ_NEED_CA)
	{
		oqp->flags &= ~OQ_NEED_CA;
		oqp->flags |= OQ_WAIT_FOR_TONE;
		oqp->timeout = 2*60;
		DPRINTF("Need CA for %s tmout=%d rt=%lx\n", oqp->buf, oqp->timeout, realtime());
		/* find first available CA and grab it */
		HayesSend("*BF", 3, 0);
		/* leave message on the top */
		return;
	}

	DPRINTF("HayesXmit %x %s\n", oqp->flags, oqp->buf);

	/* all clear to send next segment */
	cp = oqp->buf;
	dialit = 1;
	n = 0;
	while (*cp)
	{
		switch (*cp)
		{
		case '+':	/* send first part and wait for dialtone */
			if (cp != oqp->buf)
			{
				/* we have some characters to send */
				HayesSend(oqp->buf, n, dialit);
				PhoneOutQueue.start_time = realtime();
				oqp->timeout = 120;	/* 2 sec timeout */
				/* if dialtone already here, just pause */
				if (lp->status & DIALTONE)
					oqp->flags |= OQ_PAUSE;
				else
					oqp->flags |= OQ_WAIT_FOR_TONE;
			}
			/* skip '+' and copy string to front of buffer */
			while (*cp=='+') cp++;
			for (tcp=oqp->buf; *tcp = *cp; tcp++, cp++)
				;
			return;
		case '>':	/* If the 1st char is ">", send redial,
				 * otherwise send first part and wait
				 * for connect.
				*/
			if (cp != oqp->buf)
			{
				/* we have some characters to send */
				HayesSend(oqp->buf, n, dialit);
			}
			else
				HayesSend(Redialstring, strlen(Redialstring), 0);
			PhoneOutQueue.start_time = realtime();
			oqp->timeout = Connectseconds*60;
			oqp->flags |= OQ_WAIT_FOR_CONN;
			Trynumber = 1;
			/* skip '>' and copy string to front of buffer */
			while (*cp=='>') cp++;
			for (tcp=oqp->buf; *tcp = *cp; tcp++, cp++)
				;
			return;
		case ',':	/* send first part and wait for 1/2 second */
			if (cp != oqp->buf)
			{
				/* we have some characters to send */
				HayesSend(oqp->buf, n, dialit);
			}
			PhoneOutQueue.start_time = realtime();
			oqp->timeout = 30;	/* 1/2 sec timeout */
			oqp->flags |= OQ_PAUSE;
			/* copy string to front of buffer, incl. more ','s */
			for (cp++, tcp=oqp->buf; *tcp = *cp; tcp++, cp++)
				;
			return;
		case ';':	/* send first part, wait for ack, continue */
			if (cp != oqp->buf)
			{
				/* we have some characters to send */
				HayesSend(oqp->buf, n, dialit);
			}
			PhoneOutQueue.start_time = realtime();
			oqp->timeout = 30;	/* 1/2 sec timeout */
			/* copy string to front of buffer */
			while (*cp==';') cp++;
			for (tcp=oqp->buf; *tcp = *cp; tcp++, cp++)
				;
			return;
		default:
			if (!((*cp>='0' && *cp<='9') || *cp=='*' || *cp=='#'))
				dialit = 0;	/* non-keypad char seen */
			n++;
			cp++;
		}
	}

	HayesSend(oqp->buf, n, dialit);
	OQfree(&PhoneOutQueue, oqp);
}

HayesSend(s, n, f)
register char *s;
register int n;
int f;
{
	char obuf[256];
	register char *cp;
	extern unsigned long realtime();

	cp = obuf;
	*cp++ = 'A';
	*cp++ = 'T';
	if (f)
	{
		/* keypad digits require the *D command */
		*cp++ = '*';
		*cp++ = 'D';
	}

	while (n-- > 0)
		*cp++ = *s++;

	*cp++ = '\r';
	*cp++ = '\0';

	DPRINTF("sendnchars %d, %s\n", strlen(obuf), obuf);
	sendnchars(strlen(obuf), obuf);

	OQstartmsg(&PhoneOutQueue);
}
#endif	/* DMD630 */

/*
 * look up Hayes message and parse arguments into vec[]
 */
char *HayesMsgLookup(s, vec)
register char *s;
int vec[];
{
	register HayesMsg *hp;
	register char *cp, *tp;
	register int i;

	vec[0] = vec[1] = vec[2] = vec[3] = 0;

	/* quit asap if message is not in the table */
	for (hp=HayesMessages; hp->msg && *hp->msg <= *s; hp++)
	{
		for (cp=hp->msg, tp=s; ; cp++, tp++)
		{
			if (*cp=='\0')
			{
				/* found a match -- set type, get args */
				vec[0] = hp->type;
				for (i=1; i<=hp->nargs; i++)
				{
					/* each arg is 2 hex digits */
					while (*tp==' ')
						tp++;
					if (tp[0] && tp[1])
					{
						vec[i] = HexNibble(tp);
						tp += 2;
					}
				}
				if (*tp==' ') tp++;
				return tp;
			} else if (*cp != *tp || *tp=='\0')
					break;	/* mismatch or overflow */
		}
	}

	/* No match -- return original string and set vec[0] to -1 which
	 * signifies no match.
	*/
	vec[0] = -1;
	return s;
}

#define CBIT 001
#define MBIT 002
#define DBIT 004

/*
 * initialize communication with the phone
 */
#ifdef	DMD630
int
HayesInit()
{
	int trys;
	int cmd;
	int c;

	local();	/* go local just in case */

	if (attach(HostPort)==0)
	{
		lprintf("Cannot attach to host %d\n", HostPort);
		lprintf("Is this port already in use, e.g., by the printer?\n");
		/* pause forever */
		while (1)
			wait(CPU);
	}

	request(ALARM | CPU | KBD | RCV | SEND);

	/* loop 3 times trying for CMD: prompt */
	for (trys=0; trys < 3; trys++)
	{
		sendnchars(6, "AT&&T\r");
		/* wait 2 seconds each try */
		alarm(2*60);
		cmd = 0;
		while (1)
		{
			wait(CPU | RCV);
			if (own() & ALARM)
				break;
			else if (own() & RCV)
			{
				while ((c = rcvchar()) != -1)
				{
					lputchar(c);
					switch (c)
					{
					case 'C': cmd |= CBIT; break;
					case 'M': cmd |= MBIT; break;
					case 'D': cmd |= DBIT; break;
					case ':':
						/* have we seen CMD:? */
						if (cmd == (CBIT|MBIT|DBIT))
						{
							/* go into Hayes mode */
							sendnchars(4, "&&H\r");
							return;
						}
						/* nope - fall through */
					default: cmd = 0; break;
					}
				}
			}
		}
	}
	/* We failed to initialize, although we opened the line. */
	lprintf("Unable to initialize the line to the phone on line %d.\n", HostPort);
	lprintf("Check the baud, parity, etc. on both the phone and your terminal.\n");
	lprintf("They must match.\n");
	/* pause forever */
	while (1)
		wait(CPU);

#else	/* DMD630 */

int
HayesInit()
{
	int cmd;
	int c;

	for (c=0; c<=MAXCALLAPPS; c++) {
		LineApps[c].status = 0;
		LineApps[c].line = -1;			/* No lineapp yet */
		LineApps[c].direction = IS_NONE;
	}

	/*
	 * Hayes initialization sequence:
	 *
	 * V1		set verbose (must be first!)
	 * E1		set echo on
	 * S11=nnn	set DTMF duration to nnn
	 * &&L		query lamp status
	 * &&I		query ProPhone version
	 */

	fprintf(Pofp, "AT&&T\r"); fflush(Pofp);	/* Go into CMD mode */
/**XXXXXXX ??????? are the next two lines needed ???? */
	sleep(1);
	fprintf(Pofp, "\r"); fflush(Pofp);
/**XXXXXXX ??????? */

	cmd = 0;
	while (cmd != (CBIT|MBIT|DBIT))
	{
		switch (c = fgetc(Pifp))
		{
		case EOF: return 0;
		case 'C': cmd |= CBIT; break;
		case 'M': cmd |= MBIT; break;
		case 'D': cmd |= DBIT; break;
		default: cmd = 0;
		}
	}

	/* If we don't flush the input here, the next write we do
	 * we be read by the phone as a "CMD&&H", which it ignores,
	 * and then all subsequent AT-style commands are ignored also.
	*/
	fflush(Pifp);
	fprintf(Pofp, "&&H\r"); fflush(Pofp);	/* Go into Hayes mode */
/* XXXX the DMD version does a "V1;E1;S11=120;&&L;&&I" */
	fprintf(Pofp, "ATV1\r");
	/* We can't yet handle echo on, because then we see all our commands
	 * and don't know what to do with them.
	*/
	fprintf(Pofp, "ATE0\r");		/* 0 == off, 1 == on */
	fprintf(Pofp, "ATS11=120\r");
	fprintf(Pofp, "AT&&L\r");
	fprintf(Pofp, "AT&&I\r");
	fflush(Pofp);

#endif	/* DMD630 */
}

int HexNibble(s)
register char *s;
{
	register int n;

	if (*s>='0' && *s<='9')
		n = *s++ - '0';
	else if (*s>='a' && *s<='f')
		n = 10 + *s++ - 'a';
	else if (*s>='A' && *s<='F')
		n = 10 + *s++ - 'A';
	else
		return 0;
	if (*s>='0' && *s<='9')
		n = n*16 + *s++ - '0';
	else if (*s>='a' && *s<='f')
		n = n*16 + 10 + *s++ - 'a';
	else if (*s>='A' && *s<='F')
		n = n*16 + 10 + *s++ - 'A';
	else
		return 0;

	return n;
}
