SRC = call cfa
SRCDIR = $(HOME)/bin
INSTSRCMODE=555
LINKS = call:acall call:hcall call:ncall call:pcall call:wcall call:ycall \
	call:vcall

install: $(SRC)
	for I in $(SRC);\
	do	/bin/rm -f $(SRCDIR)/$$I;\
		/bin/cp $$I $(SRCDIR)/$$I;\
		/bin/chmod $(INSTSRCMODE) $(SRCDIR)/$$I;\
	done
	cd $(SRCDIR);for I in $(LINKS);\
	do	/bin/rm -f `echo $$I | cut -f2 -d:`;\
		/bin/ln `echo $$I | tr ':' ' '`;\
	done

clean:
	/bin/rm -f $(SRC)

clobber: clean
	/bin/rm -f `cat -s FILES`

$(SRC):
	for I in $(SRC);\
		do get -s ../sccs/s.$$I;\
		done
