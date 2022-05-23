#include <stdio.h>
#include <string.h>	/* for str*() definitions */

#include "Common.h"

#define Boolean int

#define TRUE	1
#define FALSE	0

extern void SendCmd();
extern char *Malloc();

#define new(x) (x *)Malloc(sizeof(x))

#define DEFICONPATH	"/lib/faces/"

typedef struct CallInfo {
	char *status;
	char *callapp;
	char *calleddn;
	char *callingdn;
	char *callednm;
	char *callingnm;
	char *cos;
	char *inid;
	char *miscinfo;
	char *fulldisp;
	char *datetime;
	char *duration;
} CallInfo;
