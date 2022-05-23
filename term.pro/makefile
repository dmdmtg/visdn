# visdn terminal side

# Alan Hastings August 1990

HDRDIR = ../hdr
BIN = $(HOME)/lib/630

# these may be overridden from upstairs:
DEFINES = -DDEBUG -DUSE730WINDOWS

INCLUDES = -I$(HDRDIR) -I$(TOOLS)/lib/730windows

DMDCC = $(DMD)/bin/dmdcc -DDMD630
DMDCFLAGS = -O $(DEFINES) $(INCLUDES)

HDRS = \
	Data.h \
	Buttons.h \
	$(HDRDIR)/Debug.h \
	DialPlanMod.h \
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
	Fw_io.h \
	pro_strings.h \
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
	DialPlanMod.o \
	Mouse.o \
	OutQueue.o \
	Screen.o \
	Timer.o \
	Hayes.o \
	TermProc.o \
	alpha_map.o \
	regex.o \
	lookup.o \
	rmtact.o \
	snarf.o \
	termversion.o \
	main.o

visdn.m: $(OBJS)
	$(DMDCC) -o $@ $(OBJS)
	chmod 664 $@

install: $(BIN)/visdn.m

$(BIN)/visdn.m: visdn.m
	cp visdn.m $@

$(OBJS): $(HDRS)
	$(DMDCC) $(DMDCFLAGS) -c $*.c

#Buttons.o: Buttons.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Buttons.c
#
#DialPlanMod.o: DialPlanMod.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c DialPlanMod.c
#
#DisplayMod.o: DisplayMod.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c DisplayMod.c
#
#Exit.o: Exit.c Fw_io.h
#	$(DMDCC) $(DMDCFLAGS) -c Exit.c
#
#Help.o: Help.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Help.c
#
#Info.o: Info.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Info.c
#
#Init.o: Init.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Init.c
#
#KeyBoard.o: KeyBoard.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c KeyBoard.c
#
#KeyPad.o: KeyPad.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c KeyPad.c
#
#LampMod.o: LampMod.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c LampMod.c
#
#Mouse.o: Mouse.c $(HDRS) reggie.i
#	$(DMDCC) $(DMDCFLAGS) -c Mouse.c
#
#OutQueue.o: OutQueue.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c OutQueue.c
#
#Screen.o: Screen.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Screen.c
#
#Timer.o: Timer.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Timer.c
#
#Hayes.o: Hayes.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c Hayes.c
#
#TermProc.o: TermProc.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c TermProc.c
#
#regex.o: regex.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c regex.c
#
#rmtact.o: rmtact.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c rmtact.c
#
#main.o: main.c $(HDRS)
#	$(DMDCC) $(DMDCFLAGS) -c main.c
#
#reggie.i: reggie
#	regcmp reggie
#
clean:
	rm -f $(OBJS)

clobber: clean
	rm -f visdn.m
