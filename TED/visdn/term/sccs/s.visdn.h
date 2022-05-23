h00564
s 00001/00001/00071
d D 1.2 92/07/23 14:01:55 trl 2 1
c 
e
s 00072/00000/00000
d D 1.1 92/07/23 13:58:59 trl 1 0
c date and time created 92/07/23 13:58:59 by trl
e
u
U
t
T
I 1
/*
 * VISDN ProPhone interface terminal side definitions
 *
 * Alan Hastings July 1990
 */

#include <dmd.h>
#include <font.h>
#include <menu.h>

#include "Debug.h"
#include "Module.h"
#include "Common.h"
#include "Layout.h"
#include "Timer.h"
#include "DisplayMod.h"
#include "Buttons.h"
D 2
#include "DialPlanMod.h"
E 2
I 2
#include "DialPlnMod.h"
E 2
#include "LampMod.h"
#include "Menu.h"
#include "OutQueue.h"
#include "TermProc.h"
#include "KeyBoard.h"
#include "Info.h"
#include "CallApp.h"

#define TermVersion	"2.8"

#define AREACODE	"708"	/* enough of this NJ-centric view */

/* built-in display types */
#define BRANDX	0
#define ATT7506	1
#define ATT7507	2

#define BUT_FONT	mediumfont
#define KB_FONT		mediumfont
#define HELP_FONT	mediumfont
#define INFO_FONT	largefont
#define KP_FONT		largefont
#define LCD_FONT	largefont
#define VERS_FONT	largefont

#define Boolean	short
#define TRUE	1
#define FALSE	0

/* char that causes left justification in menus */
#define	LJCHAR	'\240'

extern char *alloc();

extern void BoldBox();
extern void InitData();
extern void InitDisplay();
extern void ReDisplay();
extern void ReLocate();
extern void PhoneExit();
extern void DoMouse();
extern void DispBut();
extern char *String();
extern char *LJString();
extern void DispBut();
extern void DispClear();
extern int NextFreeLine();
extern FeatBut *AddFeatBut();
extern FeatBut *AddKeyBut();
extern TimerItem *TimerAdd();

#define new(x) (x *)alloc(sizeof(x))

#include "Data.h"
E 1
