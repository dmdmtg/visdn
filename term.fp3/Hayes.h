/*
 * Hayes message types
 */
/*
 * all Hayes messages are defined here.  See HayesMsgLookup() and 
 * HayesMsgProc() for details.
 *
 * (static read-only data structure) !!!Sorted by msg!!!
 *
 * trailing spaces in messages removed for fp3--djr
 * see fp3 user's manual pg 5-15
 * NOTE 2/3 TYPE extended arg count set to zero, only one type, need
 * to update
 * 
 * some messages have varying number of arguments
 * COMPLETED (2-3) MODE (2-3)   PARTY(5-7) PROGRESS(3-4) SIGNAL(2-3)
 * msg string   number of arguments   type
 */
#define TYPE_ASSOCIATED 0 /* fp3 extended */
#define TYPE_OK 1 /* not in fp3? */
#define TYPE_ATT_CAUSE 2 /* not in fp3? */
#define TYPE_ATT_DISPLAY 3 /* not in fp3? */
#define TYPE_ATT_LAMP 4  /* not in fp3? */
#define TYPE_STATUS_QUERY 5 /* not in fp3? */
#define TYPE_BUFFER 6 /* fp3 extended */
#define TYPE_BUSY 7
#define TYPE_CALL 8
#define TYPE_CAUSE 9 /* fp3 extended */
#define TYPE_CLEARED 10
#define TYPE_COMPLETED 11
#define TYPE_CONNECTED 12
#define TYPE_DELIVERED 13
#define TYPE_DISCONNECT 14 /* not in fp3 ? */
#define TYPE_DISPLAY 15 /* 2 includes call id, disp.type_data, fp3 */
#define TYPE_ERROR 16
#define TYPE_FEATURE 17
#define TYPE_GREEN_LAMP 18 /* not in fp3? */
#define TYPE_HI 19 /* fp3 extended, human interface code */
#define TYPE_KEYPAD 20 /* fp3 extended */
#define TYPE_L3_ERROR 21 /* not in fp3? */
#define TYPE_MODE 22 /* fp3 extended */
#define TYPE_PARTY 23 /* fp3 extended */
#define TYPE_PROCEEDING 24
#define TYPE_PROGRESS 25
#define TYPE_PROMPT 26
#define TYPE_RED_LAMP 27 /* not in fp3? */
#define TYPE_REJECTED 28
#define TYPE_RING 29
#define TYPE_SCA 30/* fp3 extended */
#define TYPE_SIGNAL 31
#define TYPE_SUBADDRESS 32 /* fp3 extended */
#define TYPE_SWITCHHOOK 33 /* not in fp3? */
/* left over from prophone, not result codes? */
#define	TYPE_ATCOMMODE		34
#define	TYPE_LAMP_QUERY		35

/*
 * Hayes action codes
 */
#define	ACT_HOLD	0x9B
#define	ACT_RECONN	0x9E
#define	ACT_CONF	0x90
#define	ACT_DROP	0x98
#define	ACT_XFER	0x99

/*
 * Mode codes
 */
#define	MODE_NO_CHANGE	00
#define	MODE_NORMAL	10
#define	MODE_INSPECT	20
#define	MODE_MISC_DISP	30
#define	MODE_MSG_RET	40
#define	MODE_QUERY	50

/*
 * Submode codes
 */
#define	SUBMODE_DIRECT		01
#define	SUBMODE_REDIRECT	02

/*
 * Feature Status codes
 */
#define	FEAT_ACTIVE	00
#define	FEAT_INACTIVE	01
#define	FEAT_PENDING	02
#define	FEAT_LOC_HOLD	03
#define	FEAT_REM_HOLD	04
#define	FEAT_REQ_CONF	05
#define	FEAT_REQ_STATE	06
#define	FEAT_REQ_REJ	07

/*
 * Progress Location codes
 */
#define	PROG_LOC_USER		00
#define	PROG_LOC_PRIV_NET_LOCAL	01
#define	PROG_LOC_PUB_NET_LOCAL	02
#define	PROG_LOC_TRANS_NET	03
#define	PROG_LOC_PUB_NET_REM	04
#define	PROG_LOC_REM_PRIV_NET	05

/*
 * Progress Description codes
 */
#define	PROG_DESC_NON_ISDN	01
#define	PROG_DESC_NON_ISDN_DEST	02
#define	PROG_DESC_NON_ISDN_ORIG	03
#define	PROG_DESC_ISDN_RETURN	04
#define	PROG_DESC_INBAND_TREAT	08

/*
 * Signal codes
 */
#define	SIG_DIALTONE	0x00
#define	SIG_RINGBACK	0x01
#define	SIG_INTERCEPT	0x02
#define	SIG_REORDER	0x03
#define	SIG_BUSY	0x04
#define	SIG_CONFIRM	0x05
#define	SIG_ANSWER	0x06
#define	SIG_CALLWAIT	0x07
#define	SIG_OFFHOOK	0x08
#define	SIG_CUSTOM	0x09
#define	SIG_BUSYVFY	0x0B
#define	SIG_ERROR	0x0C
#define	SIG_STUTTER	0x0D
#define	SIG_NOTONE	0x3F
#define	SIG_ALERT0	0x40
#define	SIG_ALERT1	0x41
#define	SIG_ALERT2	0x42
#define	SIG_ALERT3	0x43
#define	SIG_ALERT4	0x44
#define	SIG_ALERT5	0x45
#define	SIG_ALERT6	0x46
#define	SIG_ALERT7	0x47
#define	SIG_NOALERT	0x4F
#define	SIG_RESERVED	0x60
#define	SIG_UNDEFINED	0x7F
#define	SIG_EXPROUTE	0xFD

/*
 * Set State codes
 */
#define	STATE_ALERTING		01
#define	STATE_CALL_ACTIVE	02
#define	STATE_CALL_HOLD		03
#define	STATE_CALL_INITIATED	04
#define	STATE_DISCONN_REQ	05
#define	STATE_OVERLAP_SEND	06
#define	STATE_CONNECT_REQ	07

/*
 * Switchhook codes
 */
#define	SH_HANDSET_UP		0x00
#define	SH_HANDSET_DN		0x01
#define	SH_ADJUNCT_ON		0x02
#define	SH_ADJUNCT_OFF		0x03
#define	SH_SPEAKER_ON		0x04
#define	SH_SPEAKER_OFF		0x05
#define	SH_SPOKESMAN_ON		0x06
#define	SH_SPOKESMAN_OFF	0x07
#define	SH_MUTE_ON		0x08
#define	SH_MUTE_OFF		0x09
#define	SH_REMOTE_ON		0x0a
#define	SH_REMOTE_OFF		0x0b

/*
 * L3_ERROR message types
 */
#define	L3_ALERT	0x01
#define	L3_CALLPROC	0x02
#define	L3_SETUP	0x05
#define	L3_CONNECT	0x07
#define	L3_SETUP_ACK	0x0d
#define	L3_CONNECT_ACK	0x0f
#define	L3_DISC		0x45
#define	L3_CONF_ACK	0x91
#define	L3_CONF_REJ	0x92
#define	L3_TRANS_ACK	0x94
#define	L3_TRANS_REJ	0x95
#define	L3_DROP_ACK	0x99
#define	L3_DROP_REJ	0x9a
#define	L3_HOLD_ACK	0x9c
#define	L3_HOLD_REJ	0x9d
#define	L3_RECONN_ACK	0x9f
#define	L3_RECONN_REJ	0xb0
#define	L3_REDIRECT	0xb1
