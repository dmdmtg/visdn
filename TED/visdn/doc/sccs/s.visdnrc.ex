h36208
s 00065/00000/00000
d D 1.1 92/07/23 13:52:53 trl 1 0
c date and time created 92/07/23 13:52:53 by trl
e
u
U
t
T
I 1
# set areacode for post query interpretation
areacode=708
# map local exchanges for 5-digit dialing at IH
exchange=979->9
exchange=713->3

# ISDN feature buttons
# button=row,col	flags...
# speedlist=name	name=	value=

button=1,2	name=FORWD
button=2,2	name=INSPT
button=3,2	name=LWC
button=1,3	name=QUERY
button=2,3	name=NEXT
# the next line binds the AUTO button to the SaveQuery function so the name
# is saved on the "Query History" speed call list when you initiate a
# call to a query response
button=3,3	name=AUTO	value=*B10	bind=SaveQuery	box	lamp=0a

button=1,1	name=34976
button=2,1	name=34976
button=3,1	name=34976
# speed calling buttons
button=4,3	name=TDH
button=5,3	name=AUDIX
button=6,3	name=LAB

# AUDIX buttons
button=6,7	box	name=Restart	value=*7
button=5,4	box	name=PASSWD	value=#,9#2
button=5,5	box	name=Listen	value=0
button=5,6	box	name=Delete	value=*3
button=6,5	box	name=Skip	value=#
button=6,6	box	name=Replay	value=23

# CORNET prefixes
button=1,4	donothing	name=Cornet:
button=1,5	delete
button=1,5	box	left	name=MH	value=CA*8+231
button=1,5	box	right	name=LC	value=CA*8+243
button=1,6	delete
button=1,6	box	left	name=HO	value=CA*8+233
button=1,6	box	right	name=WH	value=CA*8+232
button=1,7	delete
button=1,7	box	left	name=HL	value=CA*8+622
button=1,7	box	right	name=DR	value=CA*8+385

# this button invokes the button 3 "snarf post" menu item
button=9,4	name=  POST	box	value=	bind=SnarfPost

# speed calling lists for button 2 menu

speedlist=IHCC		name=Klein, Dave	value=CA94260
speedlist=IHCC		name=Loura, Ralph	value=CA98522
speedlist=IHCC		name=Patterson, Steve	value=CA94757
speedlist=IHCC/Operators		name=IH 2G2 CC		value=CA94869
speedlist=IHCC/Operators		name=IH 4E2 CC		value=CA92928
speedlist=IHCC/Operators		name=IH 2F2 CC		value=CA94345
speedlist=IHCC/Operators		name=IX CC		value=CA91359
speedlist=IHCC	name=MASALAWALA, R K	value=CA92263

# include other speed calling lists
include=.dept.speedlst

E 1
