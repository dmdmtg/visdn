h63195
s 00001/00001/00045
d D 1.11 93/09/07 15:49:01 trl 11 10
c sun fix, null ptr in audix msg
e
s 00004/00001/00042
d D 1.10 93/09/02 19:28:18 trl 10 9
c sun fixes
e
s 00001/00001/00042
d D 1.9 93/09/02 19:18:52 trl 9 8
c fix
e
s 00001/00001/00042
d D 1.8 93/09/02 19:18:25 trl 8 7
c fix for sun
e
s 00001/00001/00042
d D 1.7 92/07/23 15:19:20 trl 7 6
c 
e
s 00001/00001/00042
d D 1.6 92/07/23 14:55:05 trl 6 5
c 
e
s 00001/00001/00042
d D 1.5 92/07/23 14:50:10 trl 5 4
c 
e
s 00006/00000/00037
d D 1.4 92/07/23 14:49:37 trl 4 3
c 
e
s 00001/00001/00036
d D 1.3 92/07/23 14:09:03 trl 3 2
c 
e
s 00001/00001/00036
d D 1.2 92/07/23 14:04:26 trl 2 1
c 
e
s 00037/00000/00000
d D 1.1 92/07/23 13:56:37 trl 1 0
c date and time created 92/07/23 13:56:37 by trl
e
u
U
t
T
I 1
# visdn host side

# Alan Hastings August 1990

# these may be overridden from upstairs:
DEFINES = -DPOSTQUERY -DDEBUG
D 10
LIBS = /usr/add-on/post/src/pq/libpq.a -ldk -lrx -lnsl -lsocket
E 10
I 10
# LIBPQ = /usr/add-on/post/src/pq/libpq.a 
LIBPQ = $(HOME)/lib/c/libpq.a
# LIBS = $(LIBPQ) -ldk -lrx -lnsl -lsocket
LIBS = $(LIBPQ)
E 10

LDFLAGS =
I 4
BIN = $(HOME)/bin
E 4
D 3
HDRDIR = ../hdr
E 3
I 3
HDRDIR = $(HOME)/lib/include/visdn
E 3

D 8
INCLUDES = -I$(HDRDIR)
E 8
I 8
D 9
INCLUDES = -I$(HDRDIR) -I$(DMD)
E 9
I 9
D 11
INCLUDES = -I$(HDRDIR) -I$(DMD)/include
E 11
I 11
INCLUDES = -I/usr/5include -I$(HDRDIR) -I$(DMD)/include
E 11
E 9
E 8

CFLAGS = -O $(DEFINES) $(INCLUDES)

HDRS = \
	visdn.h \
	$(HDRDIR)/Common.h

OBJS = \
	BootTerm.o \
	Data.o \
	Help.o \
	Info.o \
	Log.o \
	main.o

visdn: $(OBJS)
	cc $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS): $(HDRS)

clean:
	rm -f $(OBJS)

clobber: clean
D 2
	rm -f visdn
E 2
I 2
D 6
	rm -f visdn `cat -s FILES`
E 6
I 6
D 7
	rm -f visdn `cat -s FILES` cscope.out tags core
E 7
I 7
	rm -f visdn `cat -s FILES` cscope.out tags core makefile
E 7
E 6
I 4

install: visdn
D 5
	cp ../host/dev/visdn $(BIN)/visdn
E 5
I 5
	cp visdn $(BIN)/visdn
E 5
	strip $(BIN)/visdn
	chmod 775 $(BIN)/visdn
E 4
E 2
E 1
