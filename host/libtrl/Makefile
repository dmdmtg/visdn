INC = 
EFT = 

libtrl.a:  basename.o dirname.o err.o warn.o rand.o lock.o log.o run_sys.o inform.o fgetline.o getline.o int_sort.o index.o index_ign.o itoa.o reverse.o numpostfix.o atol.o get_logdir.o ngetlogdir.o get_login.o get_flds.o pr_menu.o sel_menu.o atof.o any.o get_word.o getword.o strsub.o strlower.o strinstr.o eurodate.o eurotime.o hexval.o raw.o strdup.o
	rm -f libtrl.a
	ar qv libtrl.a  basename.o dirname.o err.o warn.o rand.o lock.o log.o run_sys.o inform.o fgetline.o getline.o int_sort.o index.o index_ign.o itoa.o reverse.o numpostfix.o atol.o get_logdir.o ngetlogdir.o get_login.o get_flds.o pr_menu.o sel_menu.o atof.o any.o get_word.o getword.o strsub.o strlower.o strinstr.o eurodate.o eurotime.o hexval.o raw.o strdup.o

basename.o: basename.c
	cc $(EFT) $(INC) -O -c basename.c

dirname.o: dirname.c
	cc $(EFT) $(INC) -O -c dirname.c

err.o: err.c
	cc $(EFT) $(INC) -O -c err.c

warn.o: warn.c
	cc $(EFT) $(INC) -O -c warn.c

rand.o: rand.c
	cc $(EFT) $(INC) -O -c rand.c

lock.o: lock.c
	cc $(EFT) $(INC) -O -c lock.c

log.o: log.c
	cc $(EFT) $(INC) -O -c log.c

run_sys.o: run_sys.c
	cc $(EFT) $(INC) -O -c run_sys.c

inform.o: inform.c
	cc $(EFT) $(INC) -O -c inform.c

fgetline.o: fgetline.c
	cc $(EFT) $(INC) -O -c fgetline.c

getline.o: getline.c
	cc $(EFT) $(INC) -O -c getline.c

int_sort.o: int_sort.c
	cc $(EFT) $(INC) -O -c int_sort.c

index.o: index.c
	cc $(EFT) $(INC) -O -c index.c

index_ign.o: index_ign.c
	cc $(EFT) $(INC) -O -c index_ign.c

itoa.o: itoa.c
	cc $(EFT) $(INC) -O -c itoa.c

reverse.o: reverse.c
	cc $(EFT) $(INC) -O -c reverse.c

numpostfix.o: numpostfix.c
	cc $(EFT) $(INC) -O -c numpostfix.c

atol.o: atol.c
	cc $(EFT) $(INC) -O -c atol.c

get_logdir.o: get_logdir.c
	cc $(EFT) $(INC) -O -c get_logdir.c

ngetlogdir.o: ngetlogdir.c
	cc $(EFT) $(INC) -O -c ngetlogdir.c

get_login.o: get_login.c
	cc $(EFT) $(INC) -O -c get_login.c

get_flds.o: get_flds.c
	cc $(EFT) $(INC) -O -c get_flds.c

pr_menu.o: pr_menu.c
	cc $(EFT) $(INC) -O -c pr_menu.c

sel_menu.o: sel_menu.c
	cc $(EFT) $(INC) -O -c sel_menu.c

atof.o: atof.c
	cc $(EFT) $(INC) -O -c atof.c

any.o: any.c
	cc $(EFT) $(INC) -O -c any.c

get_word.o: get_word.c
	cc $(EFT) $(INC) -O -c get_word.c

getword.o: getword.c
	cc $(EFT) $(INC) -O -c getword.c

strsub.o: strsub.c
	cc $(EFT) $(INC) -O -c strsub.c

strlower.o: strlower.c
	cc $(EFT) $(INC) -O -c strlower.c

strinstr.o: strinstr.c
	cc $(EFT) $(INC) -O -c strinstr.c

eurodate.o: eurodate.c
	cc $(EFT) $(INC) -O -c eurodate.c

eurotime.o: eurotime.c
	cc $(EFT) $(INC) -O -c eurotime.c

hexval.o: hexval.c
	cc $(EFT) $(INC) -O -c hexval.c

raw.o: raw.c
	cc $(EFT) $(INC) -O -c raw.c

strdup.o: strdup.c
	cc $(EFT) $(INC) -O -c strdup.c

clobber:
	rm -f *.a *.c *.o makefile Makefile tags cscope.out core `cat -s FILES`

clean:
	rm -f *.c *.o

cpio:
	mk.clib.mf  -r trl
	ls Makefile *.c | cpio -ocv | compress > clib.trl.io.Z

install: libtrl.a
	rm -f $(HOME)/lib/c/$(MACHARCH)/libtrl.a
	cp libtrl.a $(HOME)/lib/c/$(MACHARCH)
	chmod 444 $(HOME)/lib/c/$(MACHARCH)/libtrl.a
