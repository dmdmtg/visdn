#include "visdn.h"
#ifdef	USE730WINDOWS
#include "label.h"
#include <730windows.h>
#endif

char *FButs7506[] =
{
	"1,1;a;*B1;bL01;;",
	"2,1;b;*B2;bL02;;",
	"3,1;c;*B3;bL03;;",
	"4,1;Message;;nLM;;",
	"5,1;Confrnce;*K;s;;",
	"6,1;Drop;*J;s;;",
	"1,2;d;*B4;bL04;;",
	"2,2;e;*B5;bL05;;",
	"3,2;f;*B6;bL06;;",
	"4,2;DATA;*B7;bL07;;",
	"5,2;Transfer;*T;s;;",
	"6,2;Hold;*W;s;;",
	"1,3;h;*B8;bL08;;",
	"2,3;i;*B9;bL09;;",
	"3,3;j;*B10;bL0a;;",
	"4,3;k;*B11;b;;",
	"5,3;l;*B12;b;;",
	"6,3;m;*B13;b;;",
	"7,3;Redial;*BR;b;;",
	"8,3;Shift;*BT;bLT;;",	/* box, shift lamp */
	"9,3;Mute;*BM;bLU;;",	/* box, mute lamp */
	"10,3;Spkr;*BS;bLS;;",	/* box, speaker lamp */
	"11,3;v  Vol;*BD;<;;",
	"11,3;   ^;*BU;>;;",
	NULL,
};

char *FButs7507[] =
{
	"1,1;a;*B1;bL01;;",
	"2,1;b;*B2;bL02;;",
	"3,1;c;*B3;bL03;;",
	"4,1;d;*B4;bL04;;",
	"5,1;e;*B5;bL05;;",
	"7,1;E;*B32;bL20;;",
	"8,1;F;*B33;bL21;;",
	"9,1;G;*B34;bL22;;",
	"10,1;H;*B35;bL23;;",
	"11,1;I;*B36;bL24;;",

	"1,2;f;*B6;bL06;;",
	"2,2;g;*B7;bL07;;",
	"3,2;h;*B8;bL08;;",
	"4,2;i;*B9;bL09;;",
	"5,2;j;*B10;bL0a;;",
	"7,2;J;*B37;bL25;;",
	"8,2;K;*B38;bL26;;",
	"9,2;L;*B39;bL27;;",
	"10,2;M;*B40;bL28;;",
	"11,2;v  Vol;*BD;<;;",
	"11,2;   ^;*BU;>;;",

	"1,3;Confrnce;*K;s;;",
	"2,3;Drop;*J;s;;",
	"10,3;Message;;nLM;;",
	"11,3;Spkr;*BS;bLS;;",

	"1,4;Transfer;*T;s;;",
	"2,4;Hold;*W;s;;",
	"10,4;Select;*BT;bLT;;",
	"11,4;Mute;*BM;bLU;;",

	"1,5;k;*B11;bL0b;;",
	"2,5;l;*B12;bL0c;;",
	"3,5;m;*B13;bL0d;;",
	"4,5;n;*B14;bL0e;;",
	"5,5;o;*B15;bL0f;;",
	"6,5;p;*B16;bL10;;",
	"7,5;q;*B17;bL11;;",
	"8,5;r;*B18;bL12;;",
	"9,5;s;*B19;bL13;;",
	"10,5;Redial;*BR;b;;",
	"11,5;DATA;;bLX;;",

	"1,6;t;*B20;bL14;;",
	"2,6;u;*B21;bL15;;",
	"3,6;v;*B22;bL16;;",
	"4,6;w;*B23;bL17;;",
	"5,6;x;*B24;bL18;;",
	"6,6;y;*B25;bL19;;",
	"7,6;z;*B26;bL1a;;",
	"8,6;A;*B27;bL1b;;",
	"9,6;B;*B28;bL1c;;",
	"10,6;C;*B90;bL1d;;",
	"11,6;D;*B30;bL1e;;",

	NULL,
};

void InitData(argc, argv)
int argc;
char *argv[];
{
	char *tmem;
	register int i;
	char *opt;
	char *type;
	extern char *alloc();
	extern void LCD_Init();

	if ((tmem = alloc(sizeof(struct GlobalData)))==0)
		exit();
	P->appl = (long)tmem;

	/* shared global data semaphores */
	GlobalEvent = 0;

	InitProcs();
	UnixRcvProc = (TermProc *)0;
	UnixBufPtr = NULL;

	Type = ATT7506;
	CallApps = 0;

	/* initialize module initializers */
	LCD.Init = LCD_Init;
	LAMP.Init = Lamp_Init;
	DialPlan.Init = DialPlanInit;

	HostPort = 1;		/* default to Aux port */
	ReqRow = ReqCol = 0;	/* no extra rows 'n columns */
	CacheOnExit = MakeTop = LeaveTop = SendAUDIXMsg = OpenIt = 0;
#ifdef DEBUG
	{
		extern MainItem *DebugItemGen();

		Debug = 0;
		DebugMenu.item = (Titem *)0;
		DebugMenu.generator = (Titem *(*)())DebugItemGen;
		DebugMenu.menumap = MAINMAP;
		DebugFlags[0] = "HOSTCOMM";
		DebugFlags[1] = "PHONE";
		DebugFlags[2] = "HELP";
		DebugFlags[3] = "INFO";
		DebugFlags[4] = "LAMP";
		DebugFlags[5] = "TIMER";
		DebugFlags[6] = "MOUSE";
	}
#endif /* DEBUG */

	/* process command line args */
	for (opt=*++argv; opt && *opt; opt=*++argv)
	{
		if (opt[0]=='-' && opt[1]=='p')
		{
			if (opt[2])
				HostPort = atoi(&opt[2]);
			else if (opt = *++argv)
			{
				HostPort = atoi(opt);
			} else
				break;
		} else if (opt[0]=='-' && opt[1]=='a')
		{
			if (opt[2])
				CallApps = atoi(&opt[2]);
			else if (opt = *++argv)
			{
				CallApps = atoi(opt);
			} else
				break;
			if (CallApps > MAXCALLAPPS)
				CallApps = MAXCALLAPPS;
		} else if (opt[0]=='-' && opt[1]=='A')
		{
			SendAUDIXMsg = 1;
		} else if (opt[0]=='-' && opt[1]=='C')
		{
			CacheOnExit = 1;
		} else if (opt[0]=='-' && opt[1]=='P')
		{
			MakeTop = 1;
		} else if (opt[0]=='-' && opt[1]=='L')
		{
			LeaveTop = 1;
		} else if (opt[0]=='-' && opt[1]=='O')
		{
			OpenIt = 1;
		} else if (opt[0]=='-' && opt[1]=='c')
		{
			if (opt[2])
				ReqCol = atoi(&opt[2]);
			else if (opt = *++argv)
			{
				ReqCol = atoi(opt);
			} else
				break;
		} else if (opt[0]=='-' && opt[1]=='r')
		{
			if (opt[2])
				ReqRow = atoi(&opt[2]);
			else if (opt = *++argv)
			{
				ReqRow = atoi(opt);
			} else
				break;
		} else if (opt[0]=='-' && opt[1]=='t')
		{
			if (opt[2])
				type = &opt[2];
			else if (opt = *++argv)
				type = opt;
			else
				break;
			if (strcmp(type, "7506")==0)
				Type = ATT7506;
			else if (strcmp(type, "7507")==0)
				Type = ATT7507;
			else
				Type = BRANDX;
#ifdef DEBUG
		} else if (opt[0]=='-' && opt[1]=='D')
		{
			if (opt[2])
				Debug = atoi(&opt[2]);
			else if (opt = *++argv)
			{
				Debug = atoi(opt);
			} else
				break;
#endif /* DEBUG */
		}
	}

	/* default KeyBoard ctrl chars */
	CErase = '\b';
	CKill = '@';
	strcpy(HostVersion, "?.?");

	/* save current display location and size */
	SaveRect = display.rect;	/* struct copy */
	ReShaped = FALSE;

	LAMP.Init();
	DialPlan.Init();

	FeatButs = (FeatBut *)NULL;
	KeyPad = (FeatBut *)NULL;
	KeyPadX = KeyPadY = 0;

	/* buffer for chars received from phone */
	OutQueueInit(&PhoneOutQueue);
	PhoneBufPtr = PhoneInBuf;
	*PhoneBufPtr = '\0';

	/* empty list of KeyBoard interfaces */
	KBcurrent = KBrects = (KBrect *)0;

	/* KeyPad rendering flag */
	KPflags = 0;

	MaxFBCol = 3;
	MaxFBRow = 0;
	NeedReSize = 0;

	/* list of speed calling lists for button 2 menu */
	/* initialize to a single member */
	SpeedLists = new(SpeedList);
	SpeedLists->next = (SpeedList *)0;
	SpeedLists->tmenu.item = (Titem *)0;
	SpeedLists->tmenu.generator = (Titem *(*)())SpeedListGen;
	SpeedLists->tmenu.menumap = SPEEDMAP;
	SpeedLists->first = new(SpeedEntry);
	SpeedLists->first->next = (SpeedEntry *)0;
	SpeedLists->first->list = (SpeedList *)0;
	SpeedLists->first->text = String("Speed Call List");
	SpeedLists->first->value = NULL;

	/* button 3 menu gets generated each time */
	MainMenu.item = (Titem *)0;
	MainMenu.generator = (Titem *(*)())MainMenuGen;
	MainMenu.menumap = MAINMAP;
	MainMenu.prevhit = MainMenu.prevtop = 0;

	/* options are flagged on the fly */
	OptionMenu.item = (Titem *)0;
	OptionMenu.generator = (Titem *(*)())OptionItemGen;
	OptionMenu.menumap = MAINMAP;

	Options[0].text = "cache on exit";
	Options[0].flag = &CacheOnExit;
	Options[1].text = "send AUDIX msg";
	Options[1].flag = &SendAUDIXMsg;
	Options[2].text = "open window on ring";
	Options[2].flag = &MakeTop;
	Options[3].text = "open window on origination";
	Options[3].flag = &OpenIt;
	Options[4].text = "leave layer open after hangup";
	Options[4].flag = &LeaveTop;

	HelpMenu.item = (HelpItem *)0;
	HelpMenu.generator = (HelpItem *(*)())HelpMenuGen;
	HelpMenu.menumap = HELPMAP;
	HelpMenu.prevhit = HelpMenu.prevtop = 0;
	HelpMenu.first = (HelpItem *)0;
	HelpLines = (HelpLine *)0;
	HelpItems = (HelpItem *)0;

	/* initialize call appearances */
	for (i=0; i<=MAXCALLAPPS; i++)
	{
		LineApps[i].status = 0;
	}

	/* current line appearance and display field */
	CurLineApp = &LineApps[1];
	DispMode = M_NORMAL;

	TimerList = (TimerItem *)0;
}

void InitDisplay(type)
{
	register int i;
	register char **fbp;

	Ox = Drect.origin.x;
	Oy = Drect.origin.y;

	LCD.Init(Drect.origin);

	if (type==ATT7506)
	{
		if (CallApps==0) CallApps = 3;
		for (fbp=FButs7506; *fbp; fbp++)
			(void) AddFeatBut(*fbp);
		KeyPadPos(KP7506POS);
		KeyPadInit();
	} else if (type==ATT7507)
	{
		if (CallApps==0) CallApps = 3;
		for (fbp=FButs7507; *fbp; fbp++)
			(void) AddFeatBut(*fbp);
		KeyPadPos(KP7507POS);
		KeyPadInit();
	}

	if ((MaxFBRow += ReqRow) > MAXFBROW)
		MaxFBRow = MAXFBROW;
	if ((MaxFBCol += ReqCol) > MAXFBCOL)
		MaxFBCol = MAXFBCOL;

	KeyBoardInit();

	ReSizeScreen();
	ReLocate();
	ReDisplay();
}

void ReLocate()
{
	register FeatBut *fp;
	register KBrect *kp;
	register int i;
	Point relOrig;

	relOrig = sub(Drect.origin, Pt(Ox, Oy));

	Ox = Drect.origin.x;
	Oy = Drect.origin.y;

	LCD.Relocate(relOrig);

	for (kp=KBrects; kp; kp=kp->next)
		kp->rect = raddp(kp->rect, relOrig);

	for (fp=FeatButs; fp; fp=fp->next)
		fp->rect = raddp(fp->rect, relOrig);
	for (fp=KeyPad; fp; fp=fp->next)
		fp->rect = raddp(fp->rect, relOrig);
}

ReSizeScreen()
{
	int originx, originy, cornerx, cornery;
	int Wx, Wy;
	register KBrect *kp;
	extern int FBRowOff[], FBColOff[];

	NeedReSize = 0;

	originx = Drect.origin.x;
	originy = Drect.origin.y;

	/* this tedious bit of code allows us to specify placement relative
	 * to the edges of the display.  It's all the error checking that
	 * makes it ugly.
	 */
	for (kp=KBrects; kp; kp=kp->next)
	{
		kp->rect.origin.x = originx + FB_OFF;
		if (kp->startcol <= 0)
		{
			if (kp->startcol==0 || (MaxFBCol+kp->startcol) < 0)
				kp->rect.origin.x += FBColOff[0];
			else
				kp->rect.origin.x += FBColOff[MaxFBCol+kp->startcol];
		} else
		{
			if (kp->startcol > MaxFBCol)
				kp->rect.origin.x += FBColOff[MaxFBCol-1];
			else
				kp->rect.origin.x += FBColOff[kp->startcol-1];
		}
		kp->rect.origin.y = originy + 2*FB_OFF + LCD_WID;
		if (kp->startrow <= 0)
		{
			/* kp->startrow==0 means after last row of buttons */
			if ((MaxFBRow+kp->startrow) < 0)
				kp->rect.origin.y += FBRowOff[0];
			else
				kp->rect.origin.y += FBRowOff[MaxFBRow+kp->startrow];
		} else
		{
			if (kp->startrow > MaxFBRow)
				kp->rect.origin.y += FBRowOff[MaxFBRow-1];
			else
				kp->rect.origin.y += FBRowOff[kp->startrow-1];
		}
		kp->rect.corner.x = originx + FB_OFF + FB_LEN;
		if (kp->endcol <= 0)
		{
			if (kp->endcol==0 || (MaxFBCol+kp->endcol) < 0)
				kp->rect.corner.x += FBColOff[0];
			else
				kp->rect.corner.x += FBColOff[MaxFBCol+kp->endcol];
		} else
		{
			if (kp->endcol > MaxFBCol)
				kp->rect.corner.x += FBColOff[MaxFBCol-1];
			else
				kp->rect.corner.x += FBColOff[kp->endcol-1];
		}
		kp->rect.corner.y = originy + 2*FB_OFF + LCD_WID + FB_WID;
		if (kp->endrow <= 0)
		{
			/* kp->startrow==0 means after last row of buttons */
			if ((MaxFBRow+kp->endrow) < 0)
				kp->rect.corner.y += FBRowOff[0];
			else
				kp->rect.corner.y += FBRowOff[MaxFBRow+kp->endrow];
		} else
		{
			if (kp->endrow > MaxFBRow)
				kp->rect.corner.y += FBRowOff[MaxFBRow-1];
			else
				kp->rect.corner.y += FBRowOff[kp->endrow-1];
		}
		kp->displaypt = fPt(10, (FB_WID-FONTHEIGHT((*kp->font)))/2);
	}

	Wx = FB_OFF+FBColOff[MaxFBCol-1]+FB_LEN+FB_OFF+SCREEN_OFFSET;
	Wy = 2*FB_OFF+LCD_WID+FBRowOff[MaxFBRow]+2*FB_OFF+FB_WID+SCREEN_OFFSET;

#ifdef	USE730WINDOWS
	/* 730windows support */
	if( has730wins() )
		Wy += LABEL_HEIGHT;
#endif

	originx = display.rect.origin.x;
	originy = display.rect.origin.y;
	cornerx = display.rect.corner.x;
	cornery = display.rect.corner.y;

	if ((originx+Wx) > (XMAX-SCREEN_OFFSET))
	{
		cornerx = XMAX - SCREEN_OFFSET;
		originx = cornerx - Wx;
	} else
		cornerx = originx + Wx;

	if ((originy+Wy) > (YMAX-SCREEN_OFFSET))
	{
		cornery = YMAX - SCREEN_OFFSET;
		originy = cornery - Wy;
	} else
		cornery = originy + Wy;

	if (   originx != display.rect.origin.x
	    || originy != display.rect.origin.y
	    || cornerx != display.rect.corner.x
	    || cornery != display.rect.corner.y)
	{
		(void) reshape(Rect(originx, originy, cornerx, cornery));
		ReShaped = TRUE;
	}

	P->state &= ~(MOVED|RESHAPED);
}

InitProcs()
{
	register int p;
	for (p=0; p < TERM_PROCS; p++)
	{
		TermProcs[p].event = 0;
		TermProcs[p].alive = 0;
		TermProcs[p].sndbuf[0] = '\0';
		TermProcs[p].rcvbuf[0] = '\0';
	}
}
