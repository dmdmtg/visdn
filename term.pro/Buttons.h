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
	int flags;
	int lampno;
	int (*bind)();
};
