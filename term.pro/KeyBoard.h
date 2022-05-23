typedef struct KBrect KBrect;
struct KBrect {
	KBrect *next;
	char startrow, startcol;
	char endrow, endcol;
	Rectangle rect;
	Point displaypt;
	Font *font;
	void (*rcv)();
	int bufsiz;
	char *buf;
	char *bufptr;
};
