typedef struct MainItem {
	char *			text;
	struct {
		unsigned short uval;
		unsigned short grey;
	}			ufield;
	Tmenu *			next;
	Bitmap *		icon;
	/* extra stuff along for the ride */
	int			(*action)();
	int *			flag;
} MainItem;

#define MAINMAP	TM_TEXT | TM_UFIELD | TM_NEXT | TM_ICON

typedef struct OptionFlag {
	char *text;
	int *flag;
} OptionFlag;

#define MAXOPTION 3

extern MainItem *MainMenuGen();
extern MainItem *OptionItemGen();
extern MainItem *SaveQueryGen();

/*
 * actual struct handed back to menuhit
 */
typedef struct SpeedItem {
	char *			text;
	struct {
	  unsigned short uval;
	  unsigned short grey;
	}			ufield;
	Tmenu *			next;
	/* extra stuff for after menuhit() */
	char *			value;
	int			(*action)();
	struct SpeedEntry *	entry;
} SpeedItem;

/* only three fields valid in SpeedItem struct */
#define SPEEDMAP	TM_TEXT | TM_UFIELD | TM_NEXT

/*
 * entries in speed call lists
 */
typedef struct SpeedEntry {
	struct SpeedEntry *	next;
	struct SpeedEntry *	parent;
	struct SpeedList *	list;
	char *			text;
	char *			value;
} SpeedEntry;

/*
 * speed call lists are lists of entries which can point to
 * other lists
 */
typedef struct SpeedList {
	struct SpeedList *	next;
	struct SpeedEntry *	first;
	Tmenu			tmenu;
} SpeedList;

extern SpeedItem *SpeedMenuGen();
extern SpeedItem *SpeedListGen();

/* for keeping track of which button was originally pressed */
#define BUTTON1	01
#define BUTTON2	02
#define	BUTTON3	04

typedef struct HelpItem HelpItem;
typedef struct HelpTmenu HelpTmenu;
typedef struct HelpLine HelpLine;

struct HelpTmenu {
	HelpItem *item;
	short prevhit;
	short prevtop;
	HelpItem *(*generator)();
	short menumap;
	/* extra stuff for dynamic table of contents */
	HelpItem *first;	/* first menu item */
};

struct HelpLine {
	HelpLine *next;
	char *tag;
	char *text;
};

struct HelpItem {
	char *text;
	HelpTmenu *next;
	/* extra stuff for linked list of items */
	HelpItem *nextitem;	/* next in submenu list */
	HelpItem *link;		/* next in list of all items */
	char *tag;		/* unique string identifier from host */
};

#define HELPMAP		TM_TEXT | TM_NEXT;

extern HelpItem *HelpMenuGen();
