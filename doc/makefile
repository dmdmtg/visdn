all: 1.visdn

1.visdn: visdn.1
	tbl visdn.1 | nroff -T37 -man | col > 1.visdn

clean:
	rm -f 1.visdn

clobber: clean
