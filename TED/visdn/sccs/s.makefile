h23598
s 00008/00000/00096
d D 1.9 92/09/10 16:54:03 trl 9 8
c 
e
s 00002/00001/00094
d D 1.8 92/07/23 15:28:19 trl 8 7
c 
e
s 00001/00000/00094
d D 1.7 92/07/23 14:44:41 trl 7 6
c 
e
s 00006/00006/00088
d D 1.6 92/07/23 14:34:39 trl 6 5
c 
e
s 00003/00003/00091
d D 1.5 92/07/23 14:06:42 trl 5 4
c 
e
s 00000/00000/00094
d D 1.4 92/07/23 13:54:00 trl 4 3
c 
e
s 00001/00000/00093
d D 1.3 92/07/23 13:49:46 trl 3 2
c 
e
s 00008/00008/00085
d D 1.2 92/07/23 13:48:34 trl 2 1
c 
e
s 00093/00000/00000
d D 1.1 92/07/23 13:26:59 trl 1 0
c date and time created 92/07/23 13:26:59 by trl
e
u
U
t
T
I 1
# visdn - Visual ISDN interface

# Alan Hastings August 1990

# to make programs, just type make
# to install, type make install INSDIR=whatever

# DEFINES parameters:
#	-DDEBUG		- turn on terminal side debugging
#	-DPOSTQUERY	- use post 4.0 access for calling id information

# If you have post 4.0 installed, you should be able to compile with
# POSTQUERY and the post query libraries.  Depending on libpq.a, the
# -ldk and -lrx may or may not be needed.  Experiment.

# For 3B2s, try this:
D 8
DEFINES = -DPOSTQUERY
E 8
I 8
#DEFINES = -DPOSTQUERY
DEFINES = -DDEBUG -DUSE730WINDOWS -DPOSTQUERY
E 8
D 6
LIBS = -lpq -ldk -lrx -lnsl_s
E 6
I 6
#LIBS = -lpq -ldk -lrx -lnsl_s
I 7
LIBS = /usr/add-on/post/src/pq/libpq.a -ldk -lrx -lnsl -lsocket
E 7
E 6

# For VAX-785 SVR2.0.4, Paging, Datakit-server
#DEFINES = -DPOSTQUERY
#LIBS = /k1/lsrc/cmd/post/pq/libpq.a /usr/add-on/dkit/lib/libdk.a -lnsl

# For UTS, try this:
#DEFINES = -DPOSTQUERY
#LIBS = libpq.a -ldk -lrx -lnsl -lsocket

# Use these if you don't have post 4.0
#DEFINES =
#LIBS =

D 2
INSDIR = ../obj/ibm
E 2
I 2
INSDIR = $(HOME)
E 2
BINDIR = $(INSDIR)/bin
LIBDIR = $(INSDIR)/lib
D 2
MANDIR = $(INSDIR)/man
E 2
I 2
MANDIR = $(INSDIR)/lib/man
E 2
INSTALLDIRS =	$(INSDIR) \
		$(BINDIR) \
		$(MANDIR) \
		$(LIBDIR) \
		$(LIBDIR)/toolnews \
		$(LIBDIR)/visdn \
		$(LIBDIR)/630

all:	Dhost Dterm Ddoc

Dterm:
D 2
	cd term; make DEFINES="$(DEFINES)"
E 2
I 2
D 5
	cd ../term/dev; get ../sccs/s.makefile;make DEFINES="$(DEFINES)"
E 5
I 5
	cd ../term/dev; get ../sccs;make DEFINES="$(DEFINES)"
E 5
E 2

Dhost:
D 2
	cd host; make DEFINES="$(DEFINES)" LIBS="$(LIBS)"
E 2
I 2
D 5
	cd ../host/dev; get ../sccs/s.makefile;make DEFINES="$(DEFINES)" LIBS="$(LIBS)"
E 5
I 5
	cd ../host/dev; get ../sccs;make DEFINES="$(DEFINES)" LIBS="$(LIBS)"
E 5
E 2

Ddoc:
D 2
	cd doc; make DEFINES="$(DEFINES)" LIBS="$(LIBS)"
E 2
I 2
D 5
	cd ../doc/dev; get ../sccs/s.makefile;make DEFINES="$(DEFINES)" LIBS="$(LIBS)"
E 5
I 5
	cd ../doc/dev; get ../sccs;make DEFINES="$(DEFINES)" LIBS="$(LIBS)"
E 5
E 2

I 9
hdr: Common.h
	rm -f $(HOME)/lib/include/visdn/Common.h
	cp Common.h $(HOME)/lib/include/visdn
	chmod 444 $(HOME)/lib/include/visdn/Common.h

Common.h: ../sccs/s.Common.h
	sget Common.h

E 9
install:	all $(INSTALLDIRS)
D 6
	cp host/visdn $(BINDIR)/visdn
E 6
I 6
	cp ../host/dev/visdn $(BINDIR)/visdn
E 6
	strip $(BINDIR)/visdn
	chmod 775 $(BINDIR)/visdn
D 6
	cp term/visdn.m $(LIBDIR)/630/visdn.m
E 6
I 6
	cp ../term/dev/visdn.m $(LIBDIR)/630/visdn.m
E 6
	chmod 664 $(LIBDIR)/630/visdn.m
D 6
	cp doc/visdn.help $(LIBDIR)/visdn/visdn.help
E 6
I 6
	cp ../doc/dev/visdn.help $(LIBDIR)/visdn/visdn.help
E 6
	chmod 664 $(LIBDIR)/visdn/visdn.help
D 6
	cp doc/visdn.news $(LIBDIR)/toolnews/visdn
E 6
I 6
	cp ../doc/dev/visdn.news $(LIBDIR)/toolnews/visdn
E 6
	chmod 664 $(LIBDIR)/toolnews/visdn
D 6
	cp doc/1.visdn $(MANDIR)
E 6
I 6
	cp ../doc/dev/1.visdn $(MANDIR)
E 6
	chmod 664 $(MANDIR)/1.visdn

$(INSTALLDIRS):
	mkdir $@
	chmod 775 $@

clean clobber:
I 3
	rm -f makefile FILES
E 3
D 2
	cd Ddoc; make $@
	cd Dhost; make $@
	cd Dterm; make $@
E 2
I 2
	cd ../doc/dev; get ../sccs/s.makefile;make $@
	cd ../host/dev; get ../sccs/s.makefile;make $@
	cd ../term/dev; get ../sccs/s.makefile;make $@
E 2

XSRC = \
	hdr/Common.h \
	host/BootTerm.c \
	host/Data.c \
	host/Help.c \
	host/Info.c \
	host/Log.c \
	host/main.c \
	host/makefile \
	host/visdn.h \
	makefile

VISDN.cpio: $(XSRC)
	ls $(XSRC) | cpio -oc > $@

VISDN: Dhost
	mv host/visdn VISDN
E 1
