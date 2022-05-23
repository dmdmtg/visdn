# visdn - Visual ISDN interface

# Alan Hastings August 1992

# to make programs, just type make
# to install, type make install INSDIR=whatever

# DEFINES parameters:
#	-DDEBUG		- turn on terminal side debugging
#	-DPOSTQUERY	- use post 4.0 access for calling id information
#	-DUSE730WINDOWS	- include code for Gregg Wonderly's window gadgets

# If you have post 4.0 installed, you should be able to compile with
# POSTQUERY and the post query libraries.  Depending on libpq.a, the
# $(LIBDK) and $(LIBRX) may or may not be needed.  Experiment.

LIBPQ=/opt/post/lib/libpq.a
# LIBPQ=/usr/add-on/post/src/pq/libpq.a	# -lpq may also work
LIBDK=/usr/add-on/dkit/lib/libdk.a	# -ldk may also work
LIBRX=-lrx

HUSE730WINDOWS = -DUSE730WINDOWS
DEFINES = -DPOSTQUERY -DDEBUG $(HUSE730WINDOWS)

# For 3B2s, try this:
#LIBS = $(LIBPQ) $(LIBDK) $(LIBRX) -lnsl_s

# For VAX-785 SVR2.0.4, Paging, Datakit-server
# For SunOS 4.1.*, Datakit
# LIBS = $(LIBPQ) -lnsl

# For UTS, try this:
LIBS = $(LIBPQ) $(LIBDK) $(LIBRX) -lnsl -lsocket -lnetdb

# Use these if you don't have post 4.0
#DEFINES = -DUSE730WINDOWS
#LIBS =

INSDIR = test
BINDIR = $(INSDIR)/bin
LIBDIR = $(INSDIR)/lib
MANDIR = $(INSDIR)/man
INSTALLDIRS =	$(INSDIR) \
		$(BINDIR) \
		$(MANDIR) \
		$(LIBDIR) \
		$(LIBDIR)/toolnews \
		$(LIBDIR)/visdn \
		$(LIBDIR)/630

all:	Dhost Dterm.pro Dterm.fp3 Ddoc

# Name of the .m files:
FP3_M=visdn3.m
PRO_M=visdn.m

Dterm.pro:
	cd term.pro; make DEFINES="$(DEFINES)" VISDN_TYPE="PRO_PHONE" DOT_M="$(PRO_M)"

Dterm.fp3:
	cd term.fp3; make DEFINES="$(DEFINES)" VISDN_TYPE="FP3_PHONE" DOT_M="$(FP3_M)"

# rmake target
#### visdn:	Dhost
####	cp host/visdn .
visdn:	Dterm.pro
	cp term.pro/visdn.m ./visdn

Dhost:
	cd host; make DEFINES="$(DEFINES)" LIBS="$(LIBS)"

Ddoc:
	cd doc; make DEFINES="$(DEFINES)" LIBS="$(LIBS)"

install:	all $(INSTALLDIRS)
	cp host/visdn $(BINDIR)/visdn
	strip $(BINDIR)/visdn
	chmod 775 $(BINDIR)/visdn
	cp term.pro/$(PRO_M) $(LIBDIR)/630/$(PRO_M)
	chmod 664 $(LIBDIR)/630/$(PRO_M)
	cp term.fp3/$(FP3_M) $(LIBDIR)/630/$(FP3_M)
	chmod 664 $(LIBDIR)/630/$(FP3_M)
	cp doc/visdnrc.7506 doc/visdnrc.7507 $(LIBDIR)/visdn
	chmod 664 $(LIBDIR)/visdn/visdnrc.7506 $(LIBDIR)/visdn/visdnrc.7507
	cp doc/visdn.help $(LIBDIR)/visdn/visdn.help
	chmod 664 $(LIBDIR)/visdn/visdn.help
	cp doc/visdn.news $(LIBDIR)/toolnews/visdn
	chmod 664 $(LIBDIR)/toolnews/visdn
	cp doc/1.visdn $(MANDIR)
	chmod 664 $(MANDIR)/1.visdn

$(INSTALLDIRS):
	mkdir $@
	chmod 775 $@

clean clobber:
	cd doc; make $@
	cd host; make $@
	cd term.pro; make $@
	cd term.fp3; make $@

RMAKESRC = \
	makefile \
	hdr/Common.h \
	host/BootTerm.c \
	host/Data.c \
	host/Help.c \
	host/Info.c \
	host/Log.c \
	host/main.c \
	host/makefile \
	host/visdn.h

ALLSRC = \
	$(RMAKESRC) \
	doc/makefile \
	doc/visdn.1 \
	doc/visdn.help \
	doc/visdn.news \
	doc/visdnrc.7506 \
	doc/visdnrc.7507 \
	term.fp3/Buttons.c \
	term.fp3/Buttons.h \
	term.fp3/CallApp.h \
	term.fp3/Data.h \
	term.fp3/Debug.h \
	term.fp3/DialPlanMod.c \
	term.fp3/DialPlanMod.h \
	term.fp3/DisplayMod.c \
	term.fp3/DisplayMod.h \
	term.fp3/Exit.c \
	term.fp3/Fw_io.h \
	term.fp3/Hayes.c \
	term.fp3/Hayes.h \
	term.fp3/Help.c \
	term.fp3/Info.c \
	term.fp3/Info.h \
	term.fp3/Init.c \
	term.fp3/KeyBoard.c \
	term.fp3/KeyBoard.h \
	term.fp3/KeyPad.c \
	term.fp3/Lamp.save \
	term.fp3/LampMod.c \
	term.fp3/LampMod.h \
	term.fp3/Layout.h \
	term.fp3/Menu.h \
	term.fp3/Module.h \
	term.fp3/Mouse.c \
	term.fp3/OutQueue.c \
	term.fp3/OutQueue.h \
	term.fp3/Screen.c \
	term.fp3/TermProc.c \
	term.fp3/TermProc.h \
	term.fp3/Timer.c \
	term.fp3/Timer.h \
	term.fp3/label.h \
	term.fp3/list \
	term.fp3/main.c \
	term.fp3/makefile \
	term.fp3/regex.c \
	term.fp3/reggie \
	term.fp3/reggie.i \
	term.fp3/visdn3.h \
	term.pro/Buttons.c \
	term.pro/Buttons.h \
	term.pro/CallApp.h \
	term.pro/Data.h \
	term.pro/Debug.h \
	term.pro/DialPlanMod.c \
	term.pro/DialPlanMod.h \
	term.pro/DisplayMod.c \
	term.pro/DisplayMod.h \
	term.pro/Exit.c \
	term.pro/Fw_io.h \
	term.pro/Hayes.c \
	term.pro/Hayes.h \
	term.pro/Help.c \
	term.pro/Info.c \
	term.pro/Info.h \
	term.pro/Init.c \
	term.pro/KeyBoard.c \
	term.pro/KeyBoard.h \
	term.pro/KeyPad.c \
	term.pro/Lamp.save \
	term.pro/LampMod.c \
	term.pro/LampMod.h \
	term.pro/Layout.h \
	term.pro/Menu.h \
	term.pro/Module.h \
	term.pro/Mouse.c \
	term.pro/OutQueue.c \
	term.pro/OutQueue.h \
	term.pro/Screen.c \
	term.pro/TermProc.c \
	term.pro/TermProc.h \
	term.pro/Timer.c \
	term.pro/Timer.h \
	term.pro/io.h \
	term.pro/label.h \
	term.pro/main.c \
	term.pro/makefile \
	term.pro/regex.c \
	term.pro/reggie \
	term.pro/reggie.i \
	term.pro/visdn.h

visdn.cpio: $(ALLSRC)
	@ls $(ALLSRC) | cpio -oc > $@

VISDN.cpio: $(RMAKESRC)
	@ls $(RMAKESRC) | cpio -oc > $@

VISDN: Dhost
	mv host/visdn VISDN
