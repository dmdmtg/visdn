h36923
s 00074/00000/00000
d D 1.1 92/07/23 13:58:55 trl 1 0
c date and time created 92/07/23 13:58:55 by trl
e
u
U
t
T
I 1
/*
 * LampMod module declaration
 *
 * LampMod.Init()		- initialize Lamp module
 * LampMod.On()			- turn on status lamp
 * LampMod.Off()		- turn off status + inuse lamps
 * LampMod.Inuse()		- turn on inuse lamp
 * LampMod.Flash()		- flash the inuse lamp
 * LampMod.Fluuter()		- flutter the inuse lamp
 * LampMod.New()		- create a new lamp
 * LampMod.Delete()		- Delete a lamp
 * LampMod.Ctrl()		- obsolete
 *
 * Feature lamps only use the status side.  Call Appearance lamps use both
 * status and inuse sides.  The Mute and Speaker lamps are separate from
 * the feature lamps, but behave the same.
 */

typedef struct ButLamp ButLamp;
struct ButLamp {
	ButLamp *	next;
	FeatBut	*	but;
	TimerItem *	timer;
	char		num;
	char		iuse_status;
	char		stat_status;
	char		time_cnt;
};

MODULE(LampMod)
{
PUBLIC
	FUNC(void, Init);	/* Init() - boot time Lamp initialization */
	FUNC(void, On);		/* On(n) - lamp n status on */
	FUNC(void, Off);	/* Off(n) - lamp n status + inuse off */
	FUNC(void, Inuse);	/* Inuse(n) - lamp n inuse on */
	FUNC(void, Flash);	/* Flash(n) - flash inuse of lamp n */
	FUNC(void, Flutter);	/* Flutter(n) - flutter inuse of lamp n */
	FUNC(ButLamp *, New);	/* New(n) - create a new lamp n */
	FUNC(void, Delete);	/* Delete(lp) - remove lamp lp */
	FUNC(void, Ctrl);	/* Ctrl(t, n, c, x) - obsolete? */
PRIVATE
	ButLamp *	featlamps;	/* feature lamps */
	ButLamp *	localamps;	/* local lamps (Speaker, Mute) */
};

extern void Lamp_Init();

/* special lamp numbers */
#define MSGLAMPNO	0x3f
#define	SPKRLAMPNO	0x4f
#define DATALAMPNO	0x5f
#define	MUTELAMPNO	0x6f
#define	SHIFTLAMPNO	0x7f

/* lamp type encoding */
#define LTYPE_INUSE	0
#define LTYPE_STATUS	1

/* lamp control codes */
#define LAMP_OFF	0
#define LAMP_WINK	1
#define LAMP_ON		2
#define LAMP_FLASH	3
#define	LAMP_FLUTTER	4
#define LAMP_BFLUTTER	5

/* flag bits for lamp status */
#define LFLAG_OFF	(1<<LAMP_OFF)
#define LFLAG_WINK	(1<<LAMP_WINK)
#define LFLAG_ON	(1<<LAMP_ON)
#define LFLAG_FLASH	(1<<LAMP_FLASH)
#define LFLAG_FLUTTER	(1<<LAMP_FLUTTER)
#define LFLAG_BFLUTTER	(1<<LAMP_BFLUTTER)
E 1
