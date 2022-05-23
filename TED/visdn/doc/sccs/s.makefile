h26511
s 00001/00001/00009
d D 1.3 92/07/23 15:18:59 trl 3 2
c 
e
s 00001/00000/00009
d D 1.2 92/07/23 13:53:11 trl 2 1
c 
e
s 00009/00000/00000
d D 1.1 92/07/23 13:51:50 trl 1 0
c date and time created 92/07/23 13:51:50 by trl
e
u
U
t
T
I 1
all: 1.visdn

1.visdn: visdn.1
	nroff -T37 -man visdn.1 > 1.visdn

clean:
	rm -f 1.visdn

clobber: clean
I 2
D 3
	rm -f `cat -s FILES`
E 3
I 3
	rm -f `cat -s FILES` makefile
E 3
E 2
E 1
