
typedef struct UserInfo {
	char	*key,
		*name,
		*org,
		*tl,
		*com,
		*loc,
		*room,
		*tel,
		*ema;
	Word	*face;
} UserInfo;

#define FACECHARS	(48*48/(sizeof(char)*8))
#define FACEWORDS	(48*48/(sizeof(Word)*8))
