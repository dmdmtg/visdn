/*
 * global read/write data is referenced through a pointer for
 * sharing text on a 630
 */

#define	GLOBREF(x)	(((struct GlobalData *)P->appl)->x)

struct GlobalData {
	DisplayMod	GD_LCD;
	LampMod		GD_LAMP;
	DialPlanMod	GD_DialPlan;
	char		GD_CErase;
	char		GD_CKill;
	short		GD_Type;
	char		GD_HostVersion[16];
	TermProc	GD_TermProcs[TERM_PROCS];
	TermProc *	GD_UnixRcvProc;
	char *		GD_UnixBufPtr;
	int		GD_UnixExpectCnt;
	short		GD_UnixCurCmd;
	short		GD_UnixRcvBlockCnt;
#ifdef DEBUG
	int		GD_Debug;
	Tmenu		GD_DebugMenu;
	char *		GD_DebugFlags[NDEBUGFLAGS];
#endif /* DEBUG */
	int		GD_GlobalEvent;
	int		GD_Ox;
	int		GD_Oy;
	int		GD_ReqRow;
	int		GD_ReqCol;
	int		GD_CacheOnExit;
	int		GD_SendAUDIXMsg;
	int		GD_MakeTop;
	Rectangle	GD_SaveRect;
	OutQueueData	GD_PhoneOutQueue;
	char *		GD_PhoneBufPtr;
	char		GD_PhoneInBuf[256];
	void		(*GD_PhoneMsgProc)();
	int		GD_HostPort;
	Boolean		GD_ReShaped;
	FeatBut *	GD_FeatButs;
	FeatBut *	GD_KeyPad;
	KBrect *	GD_KBrects;
	KBrect *	GD_KBcurrent;
	int		GD_KPflags;
	short		GD_KeyPadX;
	short		GD_KeyPadY;
	FBut		GD_tmpFBut;
	int		GD_MaxFBRow;
	int		GD_MaxFBCol;
	int		GD_NeedReSize;
	Tmenu		GD_MainMenu;
	Tmenu		GD_OptionMenu;
	Tmenu		GD_SaveQueryMenu;
	HelpTmenu	GD_HelpMenu;
	OptionFlag	GD_Options[MAXOPTION];
	SpeedItem	GD_tmpSpeedItem;
	MainItem	GD_tmpMainItem;
	HelpItem	GD_tmpHelpItem;
	HelpItem	GD_HelpCurTopic;
	int		GD_HelpTextLines;
	Rectangle	GD_HelpTextRect;
	Rectangle	GD_HelpDoneRect;
	Point		GD_HelpOrigin;
	HelpItem *	GD_HelpItems;
	HelpLine *	GD_HelpLines;
	char		GD_InfoBuf[HOSTBUFSIZE];
	UserInfo	GD_InfoData;
	Rectangle	GD_InfoRect;
	Rectangle	GD_InfoTimeRect;
	Rectangle	GD_InfoDoneRect;
	Word		GD_InfoFace[FACEWORDS];
	LineApp	*	GD_InfoLineApp;
	SpeedList *	GD_SpeedLists;
	int		GD_SpeedListBut;
		/* LineApps[0] is unused (so we don'e have to subtract 1
		 * everywhere (the Phone gives LineApp numbers starting at 1),
		 * so add 1 in the definition below.
		*/
	LineApp		GD_LineApps[MAXCALLAPPS+1];
	LineApp *	GD_CurLineApp;
	int		GD_CurCallId;
	int		GD_CallApps;
	int		GD_DispMode;
	unsigned long	GD_LastTime;
	TimerItem *	GD_TimerList;
	int		GD_Tries;
	int		GD_Trynumber;
	int		GD_Connectseconds;
	int		GD_Retryseconds;
};

#define LCD		GLOBREF(GD_LCD)
#define LAMP		GLOBREF(GD_LAMP)
#define DialPlan	GLOBREF(GD_DialPlan)
#define CErase		GLOBREF(GD_CErase)
#define CKill		GLOBREF(GD_CKill)
#define Type		GLOBREF(GD_Type)
#define GlobalEvent	GLOBREF(GD_GlobalEvent)
#define TermProcs	GLOBREF(GD_TermProcs)
#define UnixRcvProc	GLOBREF(GD_UnixRcvProc)
#define UnixBufPtr	GLOBREF(GD_UnixBufPtr)
#define UnixExpectCnt	GLOBREF(GD_UnixExpectCnt)
#define UnixCurCmd	GLOBREF(GD_UnixCurCmd)
#define UnixRcvBlockCnt	GLOBREF(GD_UnixRcvBlockCnt)
#ifdef DEBUG
#define Debug		GLOBREF(GD_Debug)
#define DebugMenu	GLOBREF(GD_DebugMenu)
#define DebugFlags	GLOBREF(GD_DebugFlags)
#endif /* DEBUG */
#define HostVersion	GLOBREF(GD_HostVersion)
#define Ox		GLOBREF(GD_Ox)
#define Oy		GLOBREF(GD_Oy)
#define ReqRow		GLOBREF(GD_ReqRow)
#define ReqCol		GLOBREF(GD_ReqCol)
#define CacheOnExit	GLOBREF(GD_CacheOnExit)
#define SendAUDIXMsg	GLOBREF(GD_SendAUDIXMsg)
#define MakeTop		GLOBREF(GD_MakeTop)
#define SaveRect	GLOBREF(GD_SaveRect)
#define PhoneOutQueue	GLOBREF(GD_PhoneOutQueue)
#define PhoneBufPtr	GLOBREF(GD_PhoneBufPtr)
#define PhoneInBuf	GLOBREF(GD_PhoneInBuf)
#define PhoneMsgProc	GLOBREF(GD_PhoneMsgProc)
#define HostPort	GLOBREF(GD_HostPort)
#define ReShaped	GLOBREF(GD_ReShaped)
#define FeatButs	GLOBREF(GD_FeatButs)
#define KeyPad		GLOBREF(GD_KeyPad)
#define KBrects		GLOBREF(GD_KBrects)
#define KBcurrent	GLOBREF(GD_KBcurrent)
#define KPflags		GLOBREF(GD_KPflags)
#define KeyPadX		GLOBREF(GD_KeyPadX)
#define KeyPadY		GLOBREF(GD_KeyPadY)
#define tmpFBut		GLOBREF(GD_tmpFBut)
#define MaxFBRow	GLOBREF(GD_MaxFBRow)
#define MaxFBCol	GLOBREF(GD_MaxFBCol)
#define NeedReSize	GLOBREF(GD_NeedReSize)
#define MainMenu	GLOBREF(GD_MainMenu)
#define HelpMenu	GLOBREF(GD_HelpMenu)
#define OptionMenu	GLOBREF(GD_OptionMenu)
#define SaveQueryMenu	GLOBREF(GD_SaveQueryMenu)
#define Options		GLOBREF(GD_Options)
#define tmpSpeedItem	GLOBREF(GD_tmpSpeedItem)
#define tmpMainItem	GLOBREF(GD_tmpMainItem)
#define tmpHelpItem	GLOBREF(GD_tmpHelpItem)
#define HelpCurTopic	GLOBREF(GD_HelpCurTopic)
#define HelpTextLines	GLOBREF(GD_HelpTextLines)
#define HelpTextRect	GLOBREF(GD_HelpTextRect)
#define HelpDoneRect	GLOBREF(GD_HelpDoneRect)
#define HelpOrigin	GLOBREF(GD_HelpOrigin)
#define HelpItems	GLOBREF(GD_HelpItems)
#define HelpLines	GLOBREF(GD_HelpLines)
#define InfoBuf		GLOBREF(GD_InfoBuf)
#define InfoData	GLOBREF(GD_InfoData)
#define InfoRect	GLOBREF(GD_InfoRect)
#define InfoTimeRect	GLOBREF(GD_InfoTimeRect)
#define InfoDoneRect	GLOBREF(GD_InfoDoneRect)
#define InfoFace	GLOBREF(GD_InfoFace)
#define InfoLineApp	GLOBREF(GD_InfoLineApp)
#define SpeedLists	GLOBREF(GD_SpeedLists)
#define SpeedListBut	GLOBREF(GD_SpeedListBut)
#define LineApps	GLOBREF(GD_LineApps)
#define CurLineApp	GLOBREF(GD_CurLineApp)
#define CurDisp		GLOBREF(GD_CurDisp)
#define CurCallId	GLOBREF(GD_CurCallId)
#define CallApps	GLOBREF(GD_CallApps)
#define DispMode	GLOBREF(GD_DispMode)
#define LastTime	GLOBREF(GD_LastTime)
#define TimerList	GLOBREF(GD_TimerList)
#define Tries		GLOBREF(GD_Tries)
#define Trynumber	GLOBREF(GD_Trynumber)
#define Connectseconds	GLOBREF(GD_Connectseconds)
#define Retryseconds	GLOBREF(GD_Retryseconds)
