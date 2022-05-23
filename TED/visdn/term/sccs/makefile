# visdn terminal side

# Alan Hastings August 1990

HDRDIR = $(HOME)/lib/include/visdn
LIBDIR = $(HOME)/lib/630

# these may be overridden from upstairs:
DEFINES = -DDEBUG -DUSE730WINDOWS

INCLUDES = -I$(HDRDIR) -I$(TOOLS)/lib/730windows

DMDCC = $(DMD)/bin/dmdcc -DDMD630
DMDCFLAGS = -g $(DEFINES) $(INCLUDES)

HDRS = \
	Data.h \
	Buttons.h \
	Debug.h \
	DialPlnMod.h \
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
	DialPlnMod.o \
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

Buttons.o: Buttons.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c Buttons.c

DialPlnMod.o: DialPlnMod.c $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c DialPlnMod.c

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
	rm -f visdn.m `cat -s FILES` cscope.out tags core makefile

install: visdn.m
	cp visdn.m $(LIBDIR)/visdn.m
	chmod 664 $(LIBDIR)/visdn.m
