
#ifndef COMPILE_H_INCLUDED
#define COMPILE_H_INCLUDED

#include <strstream>

typedef char sbyte;

struct _buffer
{
  sbyte *buffer;
  sbyte *pos;
  int buffersize;
  int free;
  sbyte *pushloc;
};

typedef struct _buffer *Buffer;

#include <stdio.h>
#include "action.h"

/* shut up bison.simple */
void yyerror(char *msg);
int yylex();

#ifndef max
  #define max(x,y)	(((x)>(y))?(x):(y))
#endif

enum
{
  PUSH_STRING = 0,
  PUSH_PROPERTY = 1,
  PUSH_NULL = 3,
  PUSH_REGISTER = 4,
  PUSH_BOOLEAN = 5,
  PUSH_DOUBLE = 6,
  PUSH_INT = 7,
  PUSH_CONSTANT = 8
};

typedef enum
{
  FUNCTION_RANDOM,
  FUNCTION_LENGTH,
  FUNCTION_TIME,
  FUNCTION_INT,
  FUNCTION_CONCAT,
  FUNCTION_DUPLICATECLIP
} SWFActionFunction;

typedef enum
{
  GETURL_METHOD_NOSEND = 0,
  GETURL_METHOD_GET    = 1,
  GETURL_METHOD_POST   = 2
} SWFGetUrl2Method;

#define GETURL_LOADMOVIE 0x40
#define GETURL_LOADVARIABLES 0x80

#define MAGIC_CONTINUE_NUMBER 0x7FFE
#define MAGIC_BREAK_NUMBER    0x7FFF

#define MAGIC_CONTINUE_NUMBER_LO 0xFE
#define MAGIC_CONTINUE_NUMBER_HI 0x7F
#define MAGIC_BREAK_NUMBER_LO    0xFF
#define MAGIC_BREAK_NUMBER_HI    0x7F

#define BUFFER_INCREMENT 128

#define BUFFER_SIZE sizeof(struct _buffer)

void checksbyteOrder();

/* This is the only function needs be visible: */
Buffer compileSWFActionCode(char *script);

/* create/destroy buffer object */
Buffer newBuffer();
void destroyBuffer(Buffer out);
int bufferConcat(Buffer a, Buffer b);        /* destroys b. */
int bufferWriteBuffer(Buffer a, Buffer b);   /* doesn't. */

/* utilities for writing */
void bufferGrow(Buffer out);
void bufferCheckSize(Buffer out, int sbytes);

int bufferLength(Buffer out);

/* constant pool stuff */
int addConstant(char *s);
int bufferWriteConstants(Buffer out);

/* write data to buffer */
int bufferWriteOp(Buffer out, int data);
int bufferWritePushOp(Buffer out);
int bufferWriteU8(Buffer out, int data);
int bufferWriteS16(Buffer out, int data);
int bufferWriteData(Buffer out, sbyte *buffer, int sbytes);
int bufferWriteHardString(Buffer out, sbyte *string, int length);
int bufferWriteConstantString(Buffer out, sbyte *string, int length);
int bufferWriteString(Buffer out, sbyte *string, int length);
int bufferWriteInt(Buffer out, int i);
int bufferWriteDouble(Buffer out, double d);
int bufferWriteNull(Buffer out);
int bufferWriteBoolean(Buffer out, int val);
int bufferWriteRegister(Buffer out, int num);
int bufferWriteSetRegister(Buffer out, int num);
int bufferWriteGetProperty(Buffer out, char *string);
int bufferWriteSetProperty(Buffer out, char *string);
int bufferWriteWTHITProperty(Buffer out);

/* concat b to a, destroy b */
char *stringConcat(char *a, char *b);

/* resolve magic number standins to relative offsets */
void bufferResolveJumps(Buffer out);

/* rather than setting globals... */
void swf4ParseInit(char *string, int debug);
void swf5ParseInit(char *string, int debug);

int swf4parse(void *b);
int swf5parse(void *b);

#endif /* COMPILE_H_INCLUDED */
