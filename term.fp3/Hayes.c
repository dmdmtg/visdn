#include "visdn3.h"
#include "Hayes.h"
#ifdef  USE730WINDOWS
#include <730windows.h>
#endif /* USE730WINDOWS */
/*
 * This following section may be broken out to separate
 * humand interface file
 */
#define HI_TYPE_FEATBUT 0x01
#define HI_TYPE_KEYPAD 0x02
#define HI_TYPE_FIXFUNC 0x03
#define HI_TYPE_LOCALBUT 0x04
#define HI_TYPE_SWITCHHK 0x05
#define HI_TYPE_DISPLAY 0x11
#define HI_TYPE_LAMPS 0x12
#define HI_TYPE_TONES 0x15
#define HI_TYPE_VOLUME 0x16
#define HI_TYPE_ADJUNCT 0x17
#define HI_TYPE_ALERTER 0x15
/* end human interface */


extern void HayesInit();
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
 *
 * trailing spaces in messages removed for fp3--djr
 * see fp3 user's manual pg 5-15
 * NOTE: need to add case statements for all messages in HayesMsgProc()
 * 
 * some messages have varying number of arguments
 * COMPLETED (2-3) MODE (2-3)   PARTY(5-7) PROGRESS(3-4) SIGNAL(2-3)
 * msg string   number of arguments   type
 */
static HayesMsg HayesMessages[] =
{
	"ASSOCIATED:",		2,	TYPE_ASSOCIATED, 	/* fp3 extended */
	"AT COMMAND MODE",	0,	TYPE_OK, 		/* not in fp3? */
	"AT&T CAUSE:",		2,	TYPE_ATT_CAUSE, 	/* not in fp3? */
	"AT&T DISPLAY:",	2,	TYPE_ATT_DISPLAY, 	/* not in fp3? */
	"AT&T LAMP:",		3,	TYPE_ATT_LAMP, 		/* not in fp3? */
	"AT*I",			0,	TYPE_STATUS_QUERY, 	/* not in fp3? */
	"BUFFER:",		0,	TYPE_BUFFER, 		/* fp3 extended */
	"BUSY:",		2,	TYPE_BUSY,
	"CALL:",		1,	TYPE_CALL,
	"CAUSE:",		3,	TYPE_CAUSE, 		/* fp3 extended */
	"CLEARED:",		2,	TYPE_CLEARED,
	"COMPLETED:",		3,	TYPE_COMPLETED,
	"CONNECTED:",		1,	TYPE_CONNECTED,
	"DELIVERED:",		1,	TYPE_DELIVERED,
	"DISCONNECT:",		1,	TYPE_DISCONNECT, 	/* not in fp3 ? */
	"DISPLAY:",		2,	TYPE_DISPLAY, 		/* 2 includes call id, disp.type_data, fp3 */
	"ERROR",		0,	TYPE_ERROR,
	"FEATURE:",		3,	TYPE_FEATURE,
	"G:",			2,	TYPE_GREEN_LAMP, 	/* not in fp3? */
	"HI:",			4,	TYPE_HI, 		/* fp3 extended, human interface code */
								 /** check, number of arguments may vary */
	"KEYPAD:",			2,	TYPE_KEYPAD,	 /* fp3 extended */
	"L3_ERROR:",		3,	TYPE_L3_ERROR, 		/* not in fp3? */
	"MODE:",		3,	TYPE_MODE, 		/* fp3 extended */
	"OK",			1,	TYPE_OK,
	"PARTY:",		7,	TYPE_PARTY, 		/* fp3 extended */
	"PROCEEDING:",		1,	TYPE_PROCEEDING,
	"PROGRESS:",		4,	TYPE_PROGRESS,
	"PROMPT:",		2,	TYPE_PROMPT,
	"R:",			2,	TYPE_RED_LAMP, 		/* not in fp3? */
	"REJECTED:",		3,	TYPE_REJECTED,
	"RING:",		1,	TYPE_RING,
	"SCA:",		1,	TYPE_SCA, 			/* fp3 extended */
	"SIGNAL:",		3,	TYPE_SIGNAL,
	"SUBADDRESS:",		5,	TYPE_SUBADDRESS, 	/* fp3 extended */
	"SWITCHHOOK:",		1,	TYPE_SWITCHHOOK, 	/* not in fp3? */
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
#ifdef  USE730WINDOWS
	int labelsyet = 0;
#endif

	P->uargv = argv;
	P->uargv[0] = "visdn3";
	P->uargv[1] = 0;
	P->uargc = 1;
	PhoneMsgProc = HayesMsgProc;
	HayesInit(HAYES_ORIG_INIT);
	InitDisplay(Type);
	SendHost(PROC_PHONE, INITTERM, (char *)0);

	P->state |= LOCKLAYER;
	(void) request(DELETE | CPU | ALARM | MOUSE | KBD | RESHAPED | SEND | RCV);

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
		/* suspend application until resource is ready for service */
		state = wait(CPU | ALARM | MOUSE | KBD | RCV | DELETE);

		if (own() & DELETE)
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
#ifdef USE730WINDOWS
			if (!ISTILED(P))
#endif /* USE730WINDOWS */
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
			HandlePhoneEvent();

		if (state & MOUSE)
			DoMouse();

		if (state & KBD)
		{
			if (KBcurrent==(KBrect *)0)
				ringbell();
			else
				(*KBcurrent->rcv)(KBcurrent);
		}

		if (state & RCV) /* process message from phone */
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
			cp = PhoneInBuf; /* PhoneInBuf global 256 byte buffer */
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
		case DIALINFO:
			DialPlan.Parse(s);
			break;
		case TUNESTRING:
			free(TuneString);
			if (strncmp(s, "RingPattern", 11)==0)
			{
				TuneString = String("");
				if (s[11]>='1' && s[11]<='7')
					AlertPat = s[11]-'0' + 100;
				else
					AlertPat = 101;
			} else
			{
				if (*s=='"')
					s++;
				if (s[strlen(s)-1] == '"')
					s[strlen(s)-1] = '\0';
				TuneString = String(s);
			}
			break;
		case CAESCAPE:
			Escape = String(s);
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

/*
 * process one Hayes message
 */
void HayesMsgProc(str)
char *str;
{
	register char *cp;
	register int i;
	register char *s;
	int mdata[10]; /* holds parsed elements of Hayes msg from phone */

	if (str==NULL || *str=='\0'){ 
		return;
	}
	/** fp3--added to remove padding crs and white space **/
	while(*str=='\n' || *str=='\r' || *str=='\t' || *str==' '){
		str++;
	}

	s = HayesMsgLookup(str, mdata); /* string s only used in TYPE_ATT_DISPLAY */

	switch (mdata[0])
	{
	/* REMEMBER, mdata[0] is the type, mdate[1] is first arg from phone */
	case TYPE_ERROR:
		/* edited for fp3, seems ok */
		DPRINTF("ERROR ");
		/* fall through */
	case TYPE_OK:
		/* edited for fp3, seems ok */
		/* ack message */
		OQackmsg(&PhoneOutQueue);
		DPRINTF("OK\n");
		return;
	case TYPE_FEATURE:	/* Feature lamp Selection */
		/* edited for fp3, seems ok */
		/* djr--trying to fix bug where audix email is sent everytime the
		 * 5e resets phone lamps (every 6minutes or so)
		 * 5e sends FEATURE:00,63,01 then FEATURE:00,63,00
		 * turning button off then on which sends another audix email message
		 *
		 * this seems to work, no known bad consequences yet 5/13/92
		 */
		/* DPRINTF("FEATURE: %x %x %x\n", mdata[1], mdata[2], mdata[3]); */
		/* LAMP.Ctrl(LTYPE_STATUS, mdata[2], FEATUREMAP(mdata[3]), 0); */
		return;
	case TYPE_ATT_LAMP:	/* Feature lamp Selection */
		/* doesn't seem to exist in fp3 */
		DPRINTF("ATT LAMP: %x %x %x\n", mdata[1], mdata[2], mdata[3]);
		LAMP.Ctrl(mdata[1], (mdata[2]==0) ? MSGLAMPNO : mdata[2], mdata[3], 0);
		return;
	case TYPE_HI:	/* Human Interface message */
		/** probably pattern after case above this **/
		DPRINTF("ATT HI: %x %x %x %x \n", mdata[1], mdata[2], mdata[3], mdata[4]);
		DPRINTF("ATT HI: string = %s \n", s);
		human_intface(mdata,s);
		return;
	case TYPE_DISPLAY:	/* Display Message */
		/* edited for fp3, seems ok */
		DPRINTF("DISP: %x %x (%x) '%s'\n", mdata[1], mdata[2], CurCallId, s);
		/* djr 4/9 LCD.Setmsg(mdata[2]-1, DispMode, s); */
		DPRINTF("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
		LCD.Setmsg(mdata[2], DispMode, s);
		DPRINTF("TYPE_DISPLAY s=%s\n",s);
		return;
	case TYPE_ATT_DISPLAY:	/* Display Mode Change */
		/* doesn't seem to exist in fp3 */
		DPRINTF("DISPMODE: %x %x curmode=%x\n", mdata[1], mdata[2], DispMode);
		LCD.Setmode(mdata[2]);
		return;
	case TYPE_RING:		/* call appearance ringing */
		/* no change needed for fp3 */
		DPRINTF("RING: %x\n", mdata[1]);
		if (MakeTop)
		{
#ifdef	USE730WINDOWS
			if( ISTILED( P ) )
				untile730wins( P );
			else
#endif /* USE730WINDOWS */
			top();
		}
		return;
	case TYPE_CALL:		/* call appearance active */
#ifdef	USE730WINDOWS
		if( ISTILED( P ) )
			untile730wins( P );
#endif /* USE730WINDOWS */
		/* edited for fp3, seems ok */
		/* added line below, hoping CurCallId is what it says it is */
		CurCallId = mdata[1];
		DPRINTF("CALL: %x\n", mdata[1]);
		return;
	case TYPE_SIGNAL:	/* line signal received */
		/* seems ok for fp3, may want to check set of all signals */
		DPRINTF("SIG: %x %x", mdata[1], mdata[2]);
		CurCallId = mdata[1];
		CurLineApp = &LineApps[CurCallId];
		switch (mdata[2])
		{
		case SIG_ALERT0:	/* ringing pattern 0 */
/*ECC*/		case SIG_ALERT1:	/* ringing pattern 1 */
/*ECC*/		case SIG_ALERT2:	/* ringing pattern 2 */
/*ECC*/		case SIG_ALERT3:	/* ringing pattern 3 */
/*ECC*/		/* don't use 4 - 7 */

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

		/** djr--I took this out of the above if statment **/
		/** I believe it needs to be out so abandonded calls **/
		/** ilicit SendCallInfo for email of incoming calls not answered **/
		/* 
		 * To doug -- I told you wrong.  I pulled this out of 
		 * the if entirely.  If it was inside of the if statement
		 * it wouldn't work.  Putting it before seems to be it.
		 */
		SendCallInfo(&LineApps[mdata[1]], 0);

		if (LineApps[mdata[1]].status & CONNECTED)
		{
			LineApps[mdata[1]].status &= ~CONNECTED;
		}
		LCD.Zap(M_NORMAL); 
#ifdef	notdef
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
/* djr--needs fp3 work? */
		DPRINTF("COMPLETED: %x %x\n", mdata[1], mdata[2]);
		switch (mdata[2])
		{
		case ACT_HOLD:
			/* LCD.Zap(M_NORMAL); djr quest to get email working*/
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
#ifdef USE730WINDOWS
			untile730wins( P );
#endif /* USE730WINDOWS */
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
	case TYPE_MODE:
		/* added for fp3 */
		DPRINTF("MODE: %x %x curmode=%x\n", mdata[1], mdata[2], DispMode);
		LCD.Setmode(mdata[2]);
		return;
	case TYPE_CAUSE:
		/* added for fp3 */
		/** stub, does nothing yet **/
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

	DPRINTF("SendCallInfo()\n");

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
	else if (event==0){
		DPRINTF("SendCallInfo(): NOT ANSWERED\n");
		*s++ = 'u';
	}
	*s++ = ';';

	/* append each display field, ';' separated */
	for (i=0; i<LCD_MAXFIELDS; i++)
	{
		/* for (cp=LCD.Msgstring(M_NORMAL, i); *cp; ) */
		for (cp=LCD.Msgstring(DispMode, i); *cp; )
			*s++ = *cp++;
		*s++ = ';';
		DPRINTF("SendCallInfo(): LCD.Msgs[%x][%d]=%s\n",DispMode,i,cp);
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
		oqp->timeout = 5;
		DPRINTF("Need CA for %s tmout=%d rt=%lx\n", oqp->buf, oqp->timeout, realtime());
		/* find first available CA and grab it */
		HayesSend("*D/", 3, 0);
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
		case '~':	/* send first part and wait for 1/2 second */
			/* this used to be comma for prophone, in .vsindrc file
			 * this is a problem for fp3 because some messages
			 * need commas in them to reach the phone
	     */
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
		*cp++ = '/'; /* fp3 requires the slash */
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

	vec[0] = vec[1] = vec[2] = vec[3] =vec[4] =vec[5] =vec[6] =vec[7] =vec[8] =vec[9] = 0;

	/* quit asap if message is not in the table 
	 * step through Hayes messages, hp-> (defined at top of file)
   * needs to remain alphabetized 
	 * hp->msg is a null byte for last element in hp, anding it
   * in this for  statement quits at end of hp struct,
	 * the other relation steps hp up to the messages with same first 
	 * letter as received message
	 * inner loop compares chars in string
 	 */
	for (hp=HayesMessages; (hp->msg) && (*hp->msg <= *s); hp++)
	{
		for (cp=hp->msg, tp=s; ; cp++, tp++)
		{
			if (*cp=='\0')
			{
				DPRINTF("matched:%s \n",s);
				/* found a match -- set type, get args */
				vec[0] = hp->type;
				for (i=1; i<=hp->nargs; i++)
				{
					/* Format of fp3 messages:
					 *   msg:xx,x,x,xx,aattttttt
					 * Where x and xx are numerals and  ttttt is text for 
					 * display that immediately follows 
					 * the aa (numerals) which is the display type.
					 * See fp3 api ref manual pg7-35 and the rest of it.
					 * What fun, variable length sometimes-delimited fields.
					 */
					if(tp[1]==','){
						/* handles case of 1 char and comma */
						/* modifys tp, may be bad           */
						tp[1]=tp[0];
						tp[0]='0';
						/* next 4 lines handle keypad presses of # * */
						if(tp[1]=='#'){
							vec[i]='#';
						}else if(tp[1]=='*') {
							vec[i]='*';
						}else{
							vec[i] = HexNibble(tp);
						}
						/* DPRINTF("hexnibble is %c%c\n",tp[0],tp[1]); */
					}else if(tp[1]=='\0'){
						/* last argument was single character */
						tp[1]=tp[0];
						tp[0]='0';
						/* DPRINTF("hexnibble is %c%c\n",tp[0],tp[1]); */
						vec[i] = HexNibble(tp);
						break;
					}else if(tp[3]==','){
						/* this needs tp[1]==, case logically preceeding this */
						tp[3]='\0';
						/* 3 character argument */
						vec[i]=atoi(tp);
						tp[3]=',';
						tp++; /* two increments after if,else's handle rest of advance */
					}else{
						/* assume case of 2 char */
						/* DPRINTF("hexnibble is %c%c\n",tp[0],tp[1]); */
						vec[i] = HexNibble(tp);
					}
					tp++; 
					tp++; /* advance to next element, probably a comma */
					if(*tp == ',') tp++;
					/* here we have stepped past the commas following a
					 * two char code, leaving it pointing at the text
				 	 * for the case of aatttt described above
				 	 * or pointing to next code for other cases
					 * this assumes the last arg preceeding the text is 2 chars(?)
					 */
				}
				if(!(hp->type==TYPE_DISPLAY)){
					/* undo the stepping past diplay type sandwiched in string */
					tp--;
					tp--;
				}
				return tp; /* returns pointer to null byte if no string for text */
			} else if (*cp != *tp || *tp=='\0'){
					break;	/* mismatch or overflow */
			}
		}
	}

	/* no match -- return original string (vec[0] is still 0) */
	return s;
}

#define OBIT 001
#define KBIT 002

/*
 * initialize communication with the phone
 */
void
HayesInit(initcom)
int initcom;
{
	int trys;
	int ok; /* bits represent characters from phone's response */
	int c;
	
	local();	/* go local just in case */
	
	if(initcom==HAYES_ORIG_INIT) {
		/* original init, need to attach to port */
		if (attach(HostPort)==0) {
			lprintf("Cannot attach to host %d\n", HostPort);
			/** may want to add error exit **/
			PhoneExit();
		}
		
		request(ALARM | CPU | KBD | RCV | SEND);
		
		ok = 0;
	for (trys=0; trys < 3 && ok != (OBIT|KBIT); trys++) {
		sendnchars(3, "AT\r");
		/* wait 2 seconds each try */
		alarm(2*60);
		ok = 0;
		while (1) {
			wait(CPU | RCV);
			if (own() & ALARM){
				lprintf("\nPhone not in Hayes mode, try number = %d\n", trys+1);
				break;
			}else if (own() & RCV){
				while ((c = rcvchar()) != -1) {
					lputchar(c);
					switch (c) {
					case 'O': 
						ok |= OBIT; 
						break;
					case 'K': 
						ok |= KBIT;
						/* have we seen OK from phone? */
						if (ok == (OBIT|KBIT)) {
							/** test to flush input **/
							/** seems to have worked? **/
							while ((c = rcvchar()) != -1)
								/* we are in Hayes mode */
								lprintf("\nPhone in Hayes mode, try number = %d\n", trys+1);
							goto okset;
						}
						break;
						/* nope - fall through */
						/* comes up anyway, should improve */
					default: 
						ok = 0;
						break;
					}
				}
			}
		}
	}
	}
 okset:
	/*
	* Hayes initialization sequence:
	* &N0	go into single stream mode
	* %A0	set dte channel for voice control
	*
	* &D3		set DTR transition respnce to "follow"
	* V1		set verbose (must be first!)
	* E1		set echo on
	* S11=nnn	set DTMF duration to nnn
	* &&L		query lamp status
 	* &&X0,0,98 enable extended result codes
 	* &&X0,12,0,1 enable monitoring human interface leds
 	* &&X0,11,0,1 enable monitoring display
 	* &&X0,4,1,1 enable control and monitoring local buttons 
 	* &&X0,3,1,1 enable control and monitoring fixed function buttons 
 	* &&X0,2,1,1 enable control and monitoring keypad buttons 
 	* &&X0,15,1,1 enable control and monitoring tone alerter
	*/
	OutEnQueue(&PhoneOutQueue, "&D3");
	OutEnQueue(&PhoneOutQueue, "V1;E1;S11=50;&&L");
	OutEnQueue(&PhoneOutQueue, "&N0");
	OutEnQueue(&PhoneOutQueue, "%A0=3");
	OutEnQueue(&PhoneOutQueue, "&&X0,0,98");
	OutEnQueue(&PhoneOutQueue, "&&X0,12,0,1");
	OutEnQueue(&PhoneOutQueue, "&&X0,11,0,1");
	OutEnQueue(&PhoneOutQueue, "&&X0,4,1,1");
	OutEnQueue(&PhoneOutQueue, "&&X0,3,1,1");
	OutEnQueue(&PhoneOutQueue, "&&X0,2,1,1");
	OutEnQueue(&PhoneOutQueue, "&&X0,15,1,1");

}
/* human_intface():  function to handle many cases of AT&T ISDN Set 
 * Monitoring and Direct Control. This is called from the HI case of the switch
 * statement in HayesMsgProc which is processing the Hayes messages
 * from the phone. This was easier in prophone, alas.
 */
human_intface(mdata,s)
int *mdata;
char *s;
{
	char hicmd[200];

	switch(mdata[1]){
		case HI_TYPE_DISPLAY:
			LCD.Display(mdata[2],mdata[3],s);
			return;
		case HI_TYPE_LAMPS:
			DPRINTF("LAMP.Ctrl(%x,%x,%x 0)\n",mdata[2],mdata[3],mdata[4]);
			LAMP.Ctrl(mdata[2], mdata[3], mdata[4], 0); /* 0 always hard coded */
			return;
		case HI_TYPE_LOCALBUT:
			sprintf(hicmd,"&&x4,%x,%x",mdata[2],mdata[3]);
			DPRINTF("human_intface hicmd is %s\n",hicmd);
			OutEnQueue(&PhoneOutQueue,hicmd );
			LAMP.Ctrl(mdata[2], mdata[3], mdata[4], 0); /* 0 always hard coded */
			return;
		case HI_TYPE_KEYPAD:
			if(mdata[2]=='#'){
				sprintf(hicmd,"&&x2,#,%x",mdata[3]);
			}else if(mdata[2]=='*'){
				sprintf(hicmd,"&&x2,*,%x",mdata[3]);
			}else{
				sprintf(hicmd,"&&x2,%x,%x",mdata[2],mdata[3]);
			}
			OutEnQueue(&PhoneOutQueue,hicmd );
			return;
		case HI_TYPE_FIXFUNC:
			sprintf(hicmd,"&&x3,%x,%x",mdata[2],mdata[3]);
			DPRINTF("human_intface hicmd is %s\n",hicmd);
			OutEnQueue(&PhoneOutQueue,hicmd );
			return;
		case HI_TYPE_ALERTER:
			if((mdata[2])==0x15){
				/* turn alerting off */
				sprintf(hicmd,"&&X15,15,0");
				OutEnQueue(&PhoneOutQueue,hicmd );
			}else if((mdata[2])==0x04){
				Alert(hicmd, 1); /* ping ring, one ring */
				OutEnQueue(&PhoneOutQueue,hicmd );
			}else if(mdata[3]==0x00){
			/* }else if(mdata[2]==0x00){ */
				Alert(hicmd, 0); /* alerting pattern 0 on */
				OutEnQueue(&PhoneOutQueue,hicmd );
			}else if(mdata[3]==0x01){
				Alert(hicmd, 1); /* alerting pattern 0 on */
				OutEnQueue(&PhoneOutQueue,hicmd );
			}else if(mdata[3]==2){
				/* message => one cycle of tone, then previous tone, if any */
				/* (volume buttons pressed) */
				Alert(hicmd, 1); /* alerting on for one cycle*/
				OutEnQueue(&PhoneOutQueue,hicmd );  
			}
			return;
		case HI_TYPE_FEATBUT:
			return;
		case HI_TYPE_SWITCHHK:
			return;
		default:
			return;
	}
}

Alert(buf, dur)
char *buf;
int dur;
{
	if (*TuneString=='\0')
		sprintf(buf,"&&X15,%d,%d", AlertPat, dur);
	else
		sprintf(buf,"&&X15,203,%d,\"%s\"", dur, TuneString);
}
