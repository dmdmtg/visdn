h06670
s 00001/00001/00121
d D 1.5 92/07/23 15:18:29 trl 5 4
c 
e
s 00006/00007/00116
d D 1.4 92/07/23 14:57:59 trl 4 3
c 
e
s 00001/00001/00122
d D 1.3 92/07/23 14:46:34 trl 3 2
c 
e
s 00005/00005/00118
d D 1.2 92/07/23 14:00:45 trl 2 1
c trl'ed
e
s 00123/00000/00000
d D 1.1 92/07/23 13:58:58 trl 1 0
c date and time created 92/07/23 13:58:58 by trl
e
u
U
t
T
I 1
# visdn terminal side

# Alan Hastings August 1990

D 3
HDRDIR = ../hdr
E 3
I 3
HDRDIR = $(HOME)/lib/include/visdn
E 3
D 4
BIN = $(HOME)/lib/630
E 4
I 4
LIBDIR = $(HOME)/lib/630
E 4

# these may be overridden from upstairs:
DEFINES = -DDEBUG -DUSE730WINDOWS

INCLUDES = -I$(HDRDIR) -I$(TOOLS)/lib/730windows

DMDCC = $(DMD)/bin/dmdcc -DDMD630
DMDCFLAGS = -g $(DEFINES) $(INCLUDES)

HDRS = \
	Data.h \
	Buttons.h \
	Debug.h \
D 2
	DialPlanMod.h \
E 2
I 2
	DialPlnMod.h \
E 2
	CallApp.h \
	Info.h \
	LampMod.h \
	Layout.h \
	DisplayMod.h \
	Menu.h \
	Hayes.h \
	KeyBoard.h \
	OutQueue.h \
	TermProc.h \
	visdn.h \
	$(HDRDIR)/Common.h

OBJS = \
	Buttons.o \
	Exit.o \
	Help.o \
	Info.o \
	Init.o \
	KeyBoard.o \
	KeyPad.o \
	LampMod.o \
	DisplayMod.o \
D 2
	DialPlanMod.o \
E 2
I 2
	DialPlnMod.o \
E 2
	Mouse.o \
	OutQueue.o \
	Screen.o \
	Timer.o \
	Hayes.o \
	TermProc.o \
	regex.o \
	main.o

visdn.m: $(OBJS)
	$(DMDCC) -o $@ $(OBJS)

D 4
install: $(BIN)/visdn.m

$(BIN)/visdn.m: visdn.m
	cp visdn.m $@

E 4
Buttons.o: Buttons.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Buttons.c

D 2
DialPlanMod.o: DialPlanMod.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c DialPlanMod.c
E 2
I 2
DialPlnMod.o: DialPlnMod.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c DialPlnMod.c
E 2

DisplayMod.o: DisplayMod.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c DisplayMod.c

Exit.o: Exit.c Fw_io.h
	$(DMDCC) $(DMDCFLAGS) -c Exit.c

Help.o: Help.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Help.c

Info.o: Info.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Info.c

Init.o: Init.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Init.c

KeyBoard.o: KeyBoard.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c KeyBoard.c

KeyPad.o: KeyPad.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c KeyPad.c

LampMod.o: LampMod.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c LampMod.c

Mouse.o: Mouse.c $(HDRS) reggie.i
	$(DMDCC) $(DMDCFLAGS) -c Mouse.c

OutQueue.o: OutQueue.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c OutQueue.c

Screen.o: Screen.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Screen.c

Timer.o: Timer.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Timer.c

Hayes.o: Hayes.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Hayes.c

TermProc.o: TermProc.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c TermProc.c

regex.o: regex.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c regex.c

main.o: main.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c main.c

reggie.i: reggie
	regcmp reggie

clean:
	rm -f $(OBJS)

clobber: clean
D 2
	rm -f visdn.m
E 2
I 2
D 4
	rm -f visdn.m `cat -s FILES`
E 4
I 4
D 5
	rm -f visdn.m `cat -s FILES` cscope.out tags core
E 5
I 5
	rm -f visdn.m `cat -s FILES` cscope.out tags core makefile
E 5

install: visdn.m
	cp visdn.m $(LIBDIR)/visdn.m
	chmod 664 $(LIBDIR)/visdn.m
E 4
E 2
E 1
