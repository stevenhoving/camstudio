#include <stdio.h>

typedef union
{
  Buffer action;
  char *str;
  SWFGetUrl2Method getURLMethod;
  int op;
  int intVal;
  int len;
  double doubleVal;

  struct
  {
    Buffer buffer;
    int count;
  } exprlist;
  struct
  {
	Buffer obj, ident, memexpr;
  } lval;
} YYSTYPE;
#define	BREAK	257
#define	CONTINUE	258
#define	FUNCTION	259
#define	ELSE	260
#define	SWITCH	261
#define	CASE	262
#define	FOR	263
#define	IN	264
#define	IF	265
#define	WHILE	266
#define	DO	267
#define	VAR	268
#define	NEW	269
#define	DELETE	270
#define	RETURN	271
#define	END	272
#define	WITH	273
#define	ASM	274
#define	EVAL	275
#define	RANDOM	276
#define	GETTIMER	277
#define	LENGTH	278
#define	CONCAT	279
#define	SUBSTR	280
#define	TRACE	281
#define	INT	282
#define	ORD	283
#define	CHR	284
#define	GETURL	285
#define	GETURL1	286
#define	NEXTFRAME	287
#define	PREVFRAME	288
#define	PLAY	289
#define	STOP	290
#define	TOGGLEQUALITY	291
#define	STOPSOUNDS	292
#define	DUP	293
#define	SWAP	294
#define	POP	295
#define	PUSH	296
#define	SETREGISTER	297
#define	CALLFUNCTION	298
#define	CALLMETHOD	299
#define	AND	300
#define	OR	301
#define	XOR	302
#define	MODULO	303
#define	ADD	304
#define	LESSTHAN	305
#define	EQUALS	306
#define	INC	307
#define	DEC	308
#define	TYPEOF	309
#define	ENUMERATE	310
#define	INITOBJECT	311
#define	INITARRAY	312
#define	GETMEMBER	313
#define	SETMEMBER	314
#define	SHIFTLEFT	315
#define	SHIFTRIGHT	316
#define	SHIFTRIGHT2	317
#define	VAREQUALS	318
#define	OLDADD	319
#define	SUBTRACT	320
#define	MULTIPLY	321
#define	DIVIDE	322
#define	OLDEQUALS	323
#define	OLDLESSTHAN	324
#define	LOGICALAND	325
#define	LOGICALOR	326
#define	NOT	327
#define	STRINGEQ	328
#define	STRINGLENGTH	329
#define	SUBSTRING	330
#define	GETVARIABLE	331
#define	SETVARIABLE	332
#define	SETTARGETEXPRESSION	333
#define	DUPLICATEMOVIECLIP	334
#define	REMOVEMOVIECLIP	335
#define	STRINGLESSTHAN	336
#define	MBLENGTH	337
#define	MBSUBSTRING	338
#define	MBORD	339
#define	MBCHR	340
#define	BRANCHALWAYS	341
#define	BRANCHIFTRUE	342
#define	GETURL2	343
#define	POST	344
#define	GET	345
#define	LOADVARIABLES	346
#define	LOADMOVIE	347
#define	LOADVARIABLESNUM	348
#define	LOADMOVIENUM	349
#define	CALLFRAME	350
#define	STARTDRAG	351
#define	STOPDRAG	352
#define	GOTOFRAME	353
#define	SETTARGET	354
#define	NULLVAL	355
#define	INTEGER	356
#define	DOUBLE	357
#define	BOOLEAN	358
#define	REGISTER	359
#define	STRING	360
#define	IDENTIFIER	361
#define	EQ	362
#define	LE	363
#define	GE	364
#define	NE	365
#define	LAN	366
#define	LOR	367
#define	INCR	368
#define	DECR	369
#define	IEQ	370
#define	DEQ	371
#define	MEQ	372
#define	SEQ	373
#define	SHL	374
#define	SHR	375
#define	SHR2	376
#define	SHLEQ	377
#define	SHREQ	378
#define	SHR2EQ	379
#define	NOELSE	380
#define	UMINUS	384
#define	POSTFIX	385


extern YYSTYPE swf5lval;
