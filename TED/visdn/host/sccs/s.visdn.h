h42242
s 00029/00000/00000
d D 1.1 92/07/23 13:56:38 trl 1 0
c date and time created 92/07/23 13:56:38 by trl
e
u
U
t
T
I 1
#include <stdio.h>
#include <sys/jioctl.h>

#include "Common.h"

#define Boolean int

#define TRUE	1
#define FALSE	0

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
} CallInfo;
E 1
