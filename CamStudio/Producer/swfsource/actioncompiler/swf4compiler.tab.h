typedef union {
  Buffer action;
  char *str;
  SWFActionFunction function;
  SWFGetUrl2Method getURLMethod;
} YYSTYPE;
#define	BREAK	257
#define	FOR	258
#define	CONTINUE	259
#define	IF	260
#define	ELSE	261
#define	DO	262
#define	WHILE	263
#define	THIS	264
#define	EVAL	265
#define	TIME	266
#define	RANDOM	267
#define	LENGTH	268
#define	INT	269
#define	CONCAT	270
#define	DUPLICATECLIP	271
#define	REMOVECLIP	272
#define	TRACE	273
#define	STARTDRAG	274
#define	STOPDRAG	275
#define	ORD	276
#define	CHR	277
#define	CALLFRAME	278
#define	GETURL	279
#define	GETURL1	280
#define	LOADMOVIE	281
#define	LOADVARIABLES	282
#define	POSTURL	283
#define	SUBSTR	284
#define	GETPROPERTY	285
#define	NEXTFRAME	286
#define	PREVFRAME	287
#define	PLAY	288
#define	STOP	289
#define	TOGGLEQUALITY	290
#define	STOPSOUNDS	291
#define	GOTOFRAME	292
#define	GOTOANDPLAY	293
#define	FRAMELOADED	294
#define	SETTARGET	295
#define	TELLTARGET	296
#define	STRING	297
#define	NUMBER	298
#define	IDENTIFIER	299
#define	PATH	300
#define	GETURL_METHOD	301
#define	EQ	302
#define	LE	303
#define	GE	304
#define	NE	305
#define	LAN	306
#define	LOR	307
#define	INC	308
#define	DEC	309
#define	IEQ	310
#define	DEQ	311
#define	MEQ	312
#define	SEQ	313
#define	STREQ	314
#define	STRNE	315
#define	STRCMP	316
#define	PARENT	317
#define	END	318
#define	UMINUS	319
#define	POSTFIX	320
#define	NEGATE	321


extern YYSTYPE swf4lval;
