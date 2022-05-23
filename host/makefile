# visdn host side

# Alan Hastings August 1990

#HAS_POLL=-DNO_POLL	# while testing

# These work for the AT&T 486 UNIX running RDS.
# LIBPQ=/opt/post/lib/libpq.a
LIBPQ=./i486_libpq.a
# LIBPQ=/usr/add-on/post/src/pq/libpq.a
LIBDK=-ldk
# LIBRX=-lrx
# LIBNSL=-lnsl_s
LIBNSL=-lnsl -lsocket
# LIBBSD=-lbsd	# UTS needs this for ftruncate()
# The i486's libsocket.so has an undefined "_abi_innetgr", which we define
# in i486_other.o.
# OTHER=i486_other.o

# these may be overridden from upstairs:
HUSE730WINDOWS = -DUSE730WINDOWS
DEFINES = -DPOSTQUERY -DDEBUG $(HUSE730WINDOWS) $(HAS_POLL) -DDMD630
LIBS = $(LIBPQ) $(LIBDK) $(LIBRX) $(LIBNSL) $(TOOLS)/lib/agent/lib/libwindows.a $(LIBBSD) $(OTHER)

LDFLAGS =
HDRDIR = ../hdr

INCLUDES = -I$(HDRDIR)

CFLAGS = -O $(DEFINES) $(INCLUDES)

HDRS = \
	visdn.h \
	$(HDRDIR)/Debug.h \
	$(HDRDIR)/Common.h

OBJS = \
	BootTerm.o \
	Data.o \
	Help.o \
	Info.o \
	Log.o \
	malloc.o \
	main.o \
	hostversion.o \
	pipe.o

visdn: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS): $(HDRS)

clean:
	rm -f $(OBJS)

clobber: clean
	rm -f visdn
