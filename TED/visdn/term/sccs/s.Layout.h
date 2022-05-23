h65355
s 00046/00000/00000
d D 1.1 92/07/23 13:58:55 trl 1 0
c date and time created 92/07/23 13:58:55 by trl
e
u
U
t
T
I 1
/*
 * visdn control panel layout
 */

/* KeyPad positions for built-in display types */
#define KP7506POS	"48,336"
#define KP7507POS	"268,220"

#define SCREEN_OFFSET 6

/* Feature Button stuff */
#define FB_OFF	8
#define FB_LEN	80
#define FB_WID	30
#define FB_GAP	4*FB_OFF

#define LCD_ROWS	2
#define LCD_GAP		6
#define LCD_WID		80
#define LCD_YOFF	(FB_OFF+FONTWIDTH(VERS_FONT)+FB_OFF)

#define LCD7506_COLS	24
#define LCD7506_XOFF	20

#define LCD7507_COLS	48
#define LCD7507_XOFF	50

#define MAXFBROW	14
#define MAXFBCOL	9

#define LAMP_WID 20

/* LCD display stuff */
#define LCD_LEN	(FB_OFF+2*(FB_LEN+FB_GAP)+FB_LEN)

/* Keypad stuff */

#define MAXKPROW	4
#define MAXKPCOL	3

#define KP_WID FB_WID
#define KP_LEN KP_WID
#define KP_GAP 15

#define KB_QUERY_LEN	(LCD_LEN-FB_OFF)
#define KB_QUERY_WID	FB_WID
E 1
