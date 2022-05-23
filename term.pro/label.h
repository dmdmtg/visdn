#define LABEL_HEIGHT	20	/* height of label area */
#define LABEL_GAP	2	/* gap between label icons and text */

/* the following are icon position in the label area */

#define L_HOST_POSITION		1
#define L_MUX_POSITION		2
#define L_PRINT_POSITION	3
#define L_SCROLL_POSITION	4
#define L_CAP_POSITION		5
#define L_USER_POSITION		6	/* first space the user can use */

#define L_NUM_SYS		3	/* Print, Scroll, Caps */

/* where the text appears in the label (same as wproc escape sequence) */

#define L_LEFT		0
#define L_CENTER	1
#define L_RIGHT		2
#define L_FIRST		3


/* how the label area looks like */

#define LABEL_ON	1
#define LABEL_SYS	2
#define LABEL_OFF	3

#ifdef NOVECTOR
void loutline();
void labelon();
void labeloff();
void setlabel();
void labelicon();
void labelknob();
void labelpt();
void labeltext();
void labeltxt();

extern Bitmap 	B_local,
		B_h1,
		B_h2,
		B_mux,
		B_cap,
		B_scroll,
		B_print,
		B_compose;
#endif
