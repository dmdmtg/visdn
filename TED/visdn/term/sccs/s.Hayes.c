h48028
s 00001/00001/01003
d D 1.6 92/12/03 11:19:16 trl 6 5
c separate openit and maketop
e
s 00005/00000/00999
d D 1.5 92/09/16 18:11:29 trl 5 4
c 
e
s 00066/00005/00933
d D 1.4 92/09/16 17:51:58 trl 4 3
c 
e
s 00010/00000/00928
d D 1.3 92/08/28 15:10:36 trl 3 2
c 
e
s 00001/00001/00927
d D 1.2 92/08/19 15:25:02 trl 2 1
c leave on top
e
s 00928/00000/00000
d D 1.1 92/07/23 13:58:28 trl 1 0
c date and time created 92/07/23 13:58:28 by trl
e
u
U
t
T
I 1
#include "visdn.h"
#include "Hayes.h"
#include "io.h"
#ifdef	USE730WINDOWS
#include <730windows.h>
#endif

extern void HayesRcv();
extern void HayesXmit();
extern void HandlePhoneEvent();
extern void HayesMsgProc();
extern char *HayesMsgLookup();

#define FEATUREMAP(x) (	(x==FEAT_ACTIVE)?LAMP_ON:\
			(x==FEAT_LOC_HOLD)?LAMP_FLASH:\
			LAMP_OFF)

typedef struct HayesMsg HayesMsg;
struct HayesMsg {
	char *	msg;
	short	nargs;
	short	type;
};
/*
 * all Hayes messages are defined here.  See HayesMsgLookup() and 
 * HayesMsgProc() for details.
 *
 * (static read-only data structure) !!!Sorted by msg!!!
 */
static HayesMsg HayesMessages[] =
{
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
#endif

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
#endif

	P->uargv = argv;
	P->uargv[0] = "visdn";
	P->uargv[1] = 0;
	P->uargc = 1;
	PhoneMsgProc = HayesMsgProc;
	HayesInit();
	/*
	 * Hayes initialization sequence:
	 *
	 * V1		set verbose (must be first!)
	 * E1		set echo on
	 * S11=nnn	set DTMF duration to nnn
	 * &&L		query lamp status
	 */
	OutEnQueue(&PhoneOutQueue, "V1;E1;S11=120;&&L");
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
#endif

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
#endif

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
I 3

		case DOPOST:
			{
				char *t, *strchr();
				if( t = strchr( s, '\n' ) )
					*t = 0;
				SnarfDial( s, (MainItem *)1 );
			}
			break;

E 3
		case DIALINFO:
			DialPlan.Parse(s);
			break;
I 4
		case RMTACT0:
			rmtact0( s );
			break;
E 4
		}
		PhoneEvent &= ~EVENT_RCVBUF;
	}
}
I 4
rmtact0( s )
char *s;
{
	if ( ! strcmp( "toggle", s ) )
	{
#ifdef	USE730WINDOWS
		if( ISTILED( P ) )
			untile730wins( P );
		else
			tile730wins( P );
	}
	else if ( ! strcmp( "close", s ) )
	{
		if( ! ISTILED( P ) )
			tile730wins( P );
	}
	else if ( ! strcmp( "open", s ) )
	{
		if( ISTILED( P ) )
			untile730wins( P );
	}
/*
	else if ( ! strcmp( "clr", s ) )
	{
		KBclear( KBrects );
		KBrects->buf[ 0 ] = '\0';
		KBrects->bufptr = KBrects->buf;
	}
*/
I 5
	else if ( ! strcmp( "exit", s ) )
	{
		PhoneExit();
	}
E 5
	else if ( ! strcmp( "Exit/uncache", s ) )
	{
I 5
		CacheOnExit = 1;
E 5
		PhoneExit();
	}
	else if ( ! strcmp( "audix", s ) )
	{
	/*???hardcode*/	OutEnQueue(&PhoneOutQueue, "CA36003>,#2003#,20");
	}
	else if ( ! strcmp( "speaker", s ) )
	{
	/*???hardcode*/	OutEnQueue(&PhoneOutQueue, "*BS");
	}
	else if ( ! strcmp( "mute", s ) )
	{
	/*???hardcode*/	OutEnQueue(&PhoneOutQueue, "*BM");
	}
	else if ( ! strcmp( "drop", s ) )
	{
	/*???hardcode*/	OutEnQueue(&PhoneOutQueue, "*J");
	}
	else if ( ! strcmp( "redial", s ) )
	{
	/*???hardcode*/	OutEnQueue(&PhoneOutQueue, "*BR");
	}
	else
	/*???hardcode*/	OutEnQueue(&PhoneOutQueue, s);
#endif
}
E 4

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

/*
 * process one Hayes message
 */
void HayesMsgProc(str)
char *str;
{
	register char *cp;
	register int i;
	register char *s;
	int mdata[4];

	if (str==NULL || *str=='\0' || *str=='\n' || *str=='\r')
		return;

	s = HayesMsgLookup(str, mdata);

	switch (mdata[0])
	{
	case TYPE_ERROR:
		DPRINTF("ERROR ");
		/* fall through */
	case TYPE_OK:
		/* ack message */
		OQackmsg(&PhoneOutQueue);
		DPRINTF("OK\n");
		return;
	case TYPE_FEATURE:	/* Feature lamp Selection */
		DPRINTF("FEATURE: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		/*
		 * LAMP.Ctrl(LTYPE_STATUS, mdata[2], FEATUREMAP(mdata[3]), 0);
		 */
		return;
	case TYPE_ATT_LAMP:	/* Feature lamp Selection */
		DPRINTF("ATT LAMP: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		LAMP.Ctrl(mdata[1], (mdata[2]==0) ? MSGLAMPNO : mdata[2], mdata[3], 0);
		return;
	case TYPE_DISPLAY:	/* Display Message */
		DPRINTF("DISP: %x %x (%x) '%s'\n", mdata[1], mdata[2], CurCallId, s);
		LCD.Setmsg(mdata[1]-1, mdata[2], s);
		return;
	case TYPE_ATT_DISPLAY:	/* Display Mode Change */
		DPRINTF("DISPMODE: %x %x curmode=%x\n", mdata[1], mdata[2], DispMode);
		LCD.Setmode(mdata[2]);
		return;
	case TYPE_RING:		/* call appearance ringing */
		DPRINTF("RING: %x\n", mdata[1]);
D 4
		if (MakeTop)
E 4
I 4
D 6
		if (MakeTop&&OpenIt)
E 6
I 6
		if (MakeTop)
E 6
E 4
		{
#ifdef	USE730WINDOWS
			if( ISTILED( P ) )
				untile730wins( P );
#endif
			top();
		}
		return;
	case TYPE_CALL:		/* call appearance active */
#ifdef	USE730WINDOWS
D 4
		if( ISTILED( P ) )
E 4
I 4
		if( ISTILED( P ) && OpenIt)
E 4
			untile730wins( P );
#endif
		DPRINTF("CALL: %x\n", mdata[1]);
		return;
	case TYPE_SIGNAL:	/* line signal received */
		DPRINTF("SIG: %x %x", mdata[1], mdata[2]);
		CurCallId = mdata[1];
		CurLineApp = &LineApps[CurCallId];
		switch (mdata[2])
		{
		case SIG_ALERT0:	/* ringing */
			DPRINTF(" ringing");
			break;
		case SIG_DIALTONE:
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
		LCD.Zap(M_NORMAL);
		if (LineApps[mdata[1]].status & CONNECTED)
		{
			SendCallInfo(&LineApps[mdata[1]], 0);
			LineApps[mdata[1]].status &= ~CONNECTED;
		}
#ifdef	USE730WINDOWS
		{
			int i, con = 0;

			for( i = 0; i < MAXCALLAPPS; ++i )
			{
				if( LineApps[ i ].status & CONNECTED)
					con++;
			}
D 2
			if( !con )
E 2
I 2
			if( !con && !LeaveTop)
E 2
				tile730wins( P );
		}
#endif
		return;
	case TYPE_L3_ERROR:	/* possible hidden connect message */
		DPRINTF("L3_ERROR: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		if (mdata[2]==L3_CONNECT)
		{
			LineApps[mdata[1]].status |= CONNECTED;
			/* SendCallInfo(&LineApps[mdata[1]], CONNECTED); */
		}
		return;
	case TYPE_CONNECTED:	/* call connected */		
		if ((i = mdata[1]) >= 0 && i <= CallApps)
		{
			CurCallId = i;
			CurLineApp = &LineApps[i];
			CurLineApp->status |= CONNECTED;
			SendCallInfo(CurLineApp, CONNECTED);
		}
		DPRINTF("CONNECT: %x\n", mdata[1]);
		return;
	case TYPE_DISCONNECT:	/* call disconnected */
		if ((i = mdata[1]) >= 0 && i <= CallApps)
		{
			CurLineApp = &LineApps[i];
			SendCallInfo(CurLineApp, 0);
			CurLineApp->status &= ~(HELD|SETUP_ACK|CONNECTED);
		}
		DPRINTF("DISCONN: %x\n", mdata[1]);
		return;
	case TYPE_COMPLETED:	/* some action completed */
		DPRINTF("COMPLETED: %x %x\n", mdata[1], mdata[2]);
		switch (mdata[2])
		{
		case ACT_HOLD:
			LCD.Zap(M_NORMAL);
			break;
		case ACT_RECONN:
			break;
		}
		return;
	case TYPE_PROCEEDING:
		DPRINTF("PROCEEDING: %x\n", mdata[1]);
		/* fake message to indicate which line appearance was chosen */
		s[0] = ' ';
		s[1] = '0' + mdata[1];
		s[2] = '\0';
		LCD.Setmsg(LCD_MSG_CALLAPP, M_NORMAL, s);
		return;
	case TYPE_SWITCHHOOK:
		DPRINTF("SH %x\n", mdata[1]);
		switch (mdata[1])
		{
		case SH_SPEAKER_ON:
#ifdef	USE730WINDOWS
D 4
			untile730wins( P );
E 4
I 4
			if ( OpenIt )
				untile730wins( P );
E 4
#endif
		case SH_REMOTE_ON:
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_ON, 0);
			break;
		case SH_SPEAKER_OFF:
		case SH_REMOTE_OFF:
		case SH_HANDSET_UP:
		case SH_ADJUNCT_ON:
			LAMP.Ctrl(LTYPE_STATUS, SPKRLAMPNO, LAMP_OFF, 0);
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_OFF, 0);
			break;
		case SH_HANDSET_DN:
			break;
		case SH_MUTE_ON:
			/* only if SPEAKER_ON or HANDSET_UP */
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_ON, 0);
			break;
		case SH_MUTE_OFF:
			LAMP.Ctrl(LTYPE_STATUS, MUTELAMPNO, LAMP_OFF, 0);
			break;
		}
		return;
	case TYPE_RED_LAMP:
		DPRINTF("RED LAMP %x %x\n", mdata[1], mdata[2]);
		LAMP.Ctrl(LTYPE_INUSE, mdata[1], mdata[2], 0);
		return;
	case TYPE_GREEN_LAMP:
		DPRINTF("GREEN LAMP %x %x\n", mdata[1], mdata[2]);
		LAMP.Ctrl(LTYPE_STATUS, (mdata[1]==0) ? MSGLAMPNO : mdata[1], mdata[2], 0);
		return;
	}

	DPRINTF("? %s (%x %x %x %x)\n", str, mdata[0], mdata[1], mdata[2], mdata[3]);
}

/*
 * Send info to UNIX host side for possible processing
 */
SendCallInfo(lp, event)
register LineApp *lp;
int event;
{
	char sendbuf[128];
	register int i;
	register char *s, *cp;

	/*
	 * first char indicates result: c=connect, d=disconnect, u=unanswered
	 */
	s = sendbuf;
	if (lp->status&CONNECTED)
	{
		if (event==0)
			*s++ = 'd';
		else
			*s++ = 'c';
	}
	else if (event==0)
		*s++ = 'u';
	*s++ = ';';

	/* append each display field, ';' separated */
	for (i=0; i<LCD_MAXFIELDS; i++)
	{
		for (cp=LCD.Msgstring(M_NORMAL, i); *cp; )
			*s++ = *cp++;
		*s++ = ';';
	}
	*s = '\0';

	SendHost(PROC_PHONE, CALLINFO, sendbuf);
	if( event == 0 && ( lp->status&CONNECTED) == 0 )
	{
		int n;
		char buf[ 120 ];
		char snarfbuf[48];
		char area[8], exch[8], ext[8];
		char dialbuf[20];
		extern char snarfstr[];

		area[0] = exch[0] = ext[0] = '\0';
		strcpy( snarfbuf, LCD.Msgstring(M_NORMAL, LCD_MSG_CALLINGDN ) );
		MPRINTF("Add Missed '%s'\n", snarfbuf);
		if (!regex(snarfstr, snarfbuf, area, exch, ext))
		{
			MPRINTF("regex %s-%s-%s\n", area, exch, ext);

			sprintf(dialbuf, "CA%s%s", DialPlan.MapExch(
				area[0] ? area : DialPlan.areacode, exch), ext);
			sprintf( buf, "Missed Calls;%s;%s;",
					LCD.Msgstring(M_NORMAL, LCD_MSG_CALLINGNM),
					dialbuf );
			AddSpeedCall(buf);
		}
		else
		{
			MPRINTF("regex failed %s-%s-%s\n", area, exch, ext);
		}
	}
}

/*
 * clean exit -- restore original shape and notify all procs
 */
D 4
void PhoneExit()
E 4
I 4
void PhoneExit()	/* attached to aux port */
E 4
{
	extern Proc *phoneproc;

	if (ReShaped)
		(void) reshape(SaveRect);
	PhoneEvent = 0;			/* clear my semaphore */
	GlobalEvent |= GLOBAL_QUIT;	/* set global event */
	phoneproc = 0;

D 4
	wait(CPU);
E 4
I 4
/*	wait(CPU);*/
E 4
	exit(0);
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
		DPRINTF("OQtimeout status %x, %lx-%lx=%lx >= %x\n", lp->status, realtime(), PhoneOutQueue.start_time, realtime()-PhoneOutQueue.start_time, oqp->timeout);
		oqp->flags &= ~OQ_WAIT_FOR_TONE;
		oqp->timeout = 0;
	}

	/* check to see if we are waiting for more dial tone */
	if (oqp->flags & OQ_WAIT_FOR_CONN)
	{
		if ((lp->status & CONNECTED)==0 && oqp->timeout > 0 &&
			(realtime()-PhoneOutQueue.start_time) < oqp->timeout)
				return;
		DPRINTF("OQtimeout status %x, %lx-%lx=%lx >= %x\n", lp->status, realtime(), PhoneOutQueue.start_time, realtime()-PhoneOutQueue.start_time, oqp->timeout);
		oqp->flags &= ~OQ_WAIT_FOR_CONN;
		oqp->timeout = 0;
	}

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
				oqp->timeout = 30;	/* 1/2 sec timeout */
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
		case '>':	/* send first part and wait for connect */
			if (cp != oqp->buf)
			{
				/* we have some characters to send */
				HayesSend(oqp->buf, n, dialit);
				oqp->timeout = 10*60;	/* 10 sec timeout */
				oqp->flags |= OQ_WAIT_FOR_CONN;
			}
			/* skip '+' and copy string to front of buffer */
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

	/* no match -- return original string (vec[0] is still 0) */
	return s;
}

#define CBIT 001
#define MBIT 002
#define DBIT 004

/*
 * initialize communication with the phone
 */
HayesInit()
{
	int trys;
	int cmd;
	int c;

	local();	/* go local just in case */

	if (attach(HostPort)==0)
	{
		lprintf("Cannot attach to host %d\n", HostPort);
		/* pause forever */
		while (1)
			wait(CPU);
	}
	/*P->host->numprocs++;*/

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
		lprintf("\nALARM %d\n", trys+1);
	}
}
E 1
