
#include <stdio.h>
#include <stdarg.h>

#include "compile.h"

extern FILE *yyin;
extern Buffer out;
#define print(x)	{fputs(x,stdout);}

char *buffer;
int bufferLen;

#define BUFFER_INC 1024

void printDoAction(Buffer f, int length);

//#include "../blocks/error.h"

void (*SWF_error)(char *msg, ...);
void (*SWF_warn)(char *msg, ...);

int SWF_versionNum = 5;

void print_error(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
  exit(0);
}

void print_warn(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
}

int main(int argc, char *argv[])
{
  FILE *f;
  Buffer b;
  int size = 0;

  SWF_error = print_error;
  SWF_warn = print_warn;

  buffer = (char *) malloc(BUFFER_INC);
  bufferLen = 0;

  while (argc > 1) {
    if (strcmp(argv[1], "--4") == 0) {
      SWF_versionNum = 4;
      argc--;
      argv++;
    } else
    if (strcmp(argv[1], "--5") == 0) {
      SWF_versionNum = 5;
      argc--;
      argv++;
    } else
      break;
  }

  if(argc > 1)
  {
    f = fopen(argv[1], "r");

    if(f == NULL)
    {
      printf("Couldn't open input file %s\n", argv[1]);
      exit(-1);
    }
  }
  else
    f = stdin;

  while((size = fread(buffer+bufferLen, 1, BUFFER_INC, f)) == BUFFER_INC)
  {
    buffer = (char *) realloc(buffer, bufferLen+2*BUFFER_INC);
    bufferLen += BUFFER_INC;
  }

  bufferLen += size;

  if (SWF_versionNum == 4) {
    printf("======================\n");
    printf("Using Flash 4 compiler\n");
    printf("======================\n");
    swf4ParseInit(buffer, 1);
    swf4parse((void *)&b);
  } else {
    printf("======================\n");
    printf("Using Flash 5 compiler\n");
    printf("======================\n");
      swf5ParseInit(buffer, 1);
      swf5parse((void *)&b);
  }

  if(b == NULL)
  {
    printf("\nOutput buffer empty\n");
    exit(1);
  }

  bufferWriteU8(b, SWFACTION_END);

  putchar('\n');
  putchar('\n');

  printDoAction(b, bufferLength(b));

  putchar('\n');

  exit(0);
}
