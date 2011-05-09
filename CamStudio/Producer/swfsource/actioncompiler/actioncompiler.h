#include <iostream>
#include <stdio.h>
#include <Windows.h>

#include "../fbase.h"
#include "../faction.h"
#include "compile.h"

class CompiledActions : public FlashActionRecord
{
public:
	CompiledActions(char *pdata, int ilen)
	{
		data = pdata;
		len = ilen;
	}
	
	void Write(std::ostream &out)
	{		
		out.write(data, len);
	}
	void Read(std::istream /*&in*/)
	{

	}
private:
	char * data;
	int len;
};

extern FILE *error_fp;

class ActionCompiler
{
	int version;
public: 
	ActionCompiler(int iversion = 5)
	{
		version = iversion;		
	}	

	bool Compile(char *script, std::ostream &outputStream)
	{
		Buffer outputBuffer = {'\0'};	// otherwise C4071 warning

		int success = 0;

		if (version == 4) {
			swf4ParseInit(script,0);
			success = swf4parse(&outputBuffer);
		} else if(version == 5) {	
			swf5ParseInit(script,0);
			success = swf5parse(&outputBuffer);
		} else {
			//throw Exception("Unsupported flash version");
		}
		char logFile[MAX_PATH];
		strcpy(logFile,getenv("TEMP"));
		strcat(logFile,".\\asoutput.log");
		error_fp = fopen(logFile, "w");

		CompiledActions actions(outputBuffer->buffer, bufferLength(outputBuffer));
		FlashTagDoAction actionTag;
		actionTag.AddAction(&actions);
		outputStream << actionTag;	        
		destroyBuffer(outputBuffer);
	
		fclose(error_fp);

		return (success == 0);
	}

};
