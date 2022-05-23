/* in struct FBut added value2 for fp3 
 * to operate keypad buttons in fp3 one sends two commands 
 * at&&x2,1,1 to press button 1 
 * at&&x2,1,1 to release button 1
 * for feature buttons one command is used, it is equivalent
 * to a press and release
 */
typedef struct FeatBut FeatBut;
struct FeatBut {
	FeatBut *next;
	char *name;
	struct ButLamp *lamp;
	int flags;
	int row, col;
	Rectangle rect;
	int (*action)();
	int (*bound)();
	char *arg;
	char *arg2; /* added for fp3, keypad needs two values */
};

/* flags for buttons */
#define DO_BOX		000001
#define DO_SHADE	000002
#define DO_LAMP		000004
#define DO_LABEL	000010
#define DO_NOTHING	000020
#define DO_NOCHANGE	000040
#define	DO_LEFT		000100
#define DO_RIGHT	000200
#define DO_DELETE	000400
#define DO_AUTOSAVE	001000

#define FBLABELSIZ	32
#define FBVALUESIZ	64

typedef struct FBut FBut;
struct FBut {
	int row, col;
	char label[FBLABELSIZ];
	char value[FBVALUESIZ];
	char value2[FBVALUESIZ]; /* added for fp3, keypad needs two  values*/
	int flags;
	int lampno;
	int (*bind)();
};
