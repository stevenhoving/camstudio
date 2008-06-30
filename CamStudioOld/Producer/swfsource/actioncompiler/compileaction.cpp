#include <strstream>

#ifndef WIN32
  #include <unistd.h>
#endif

#include <time.h>
#include <stdio.h>

#include "compile.h"

extern int SWF_versionNum;

Buffer compileSWFActionCode(char *script)
{
  Buffer b;

  /* yydebug = 1; */

  if(SWF_versionNum == 4)
  {
    swf4ParseInit(script, 0);

    if(swf4parse((void *)&b) != 0)
      return NULL;
  }
  else
  {
    swf5ParseInit(script, 0);

    if(swf5parse((void *)&b) != 0)
      return NULL;
  }
  
  return b;  
}
