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
#include "DialPlanMod.h"
#include "LampMod.h"
#include "Menu.h"
#include "OutQueue.h"
#include "TermProc.h"
#include "KeyBoard.h"
#include "Info.h"
#include "CallApp.h"

#define TermVersion	"3.2"

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

/* djr--fp3 for calling HayesInit to re-initialize phone */
#define HAYES_ORIG_INIT 0 /* passed to HayesInit(), first init */
#define HAYES_PHONE_INIT 1     /* passed to HayesInit(), subsequent phone re-init */
