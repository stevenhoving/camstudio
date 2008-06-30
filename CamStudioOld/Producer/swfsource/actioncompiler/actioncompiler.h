#include <iostream>
#include <stdio.h>

#include "../fbase.h"
#include "../faction.h"
#include "compile.h"

class CompiledActions : public FlashActionRecord
{
	char * data;
	int len;
public:
	CompiledActions(char *data, int len)
	{
		this->data = data;
		this->len = len;
	}
	
	void Write(std::ostream &out)
	{		
		out.write(data,len);
	}
	void Read(std::istream &in)
	{
	
	}
};

extern FILE *error_fp;

class ActionCompiler
{
	int version;
public: 
	ActionCompiler(int version = 5)
	{
		this->version = version;		
	}	

	bool Compile(char *script, std::ostream &outputStream)
	{
		Buffer outputBuffer;

		int success = 0;

		if(version == 4)
		{
			swf4ParseInit(script,0);
			success = swf4parse(&outputBuffer);
		}
		else if(version == 5)
		{	
			swf5ParseInit(script,0);
			success = swf5parse(&outputBuffer);
		}
		else
		{
			//throw Exception("Unsupported flash version");
		}

		char *logFile = "\\asoutput.log";
		error_fp = fopen(logFile, "w");
		
		FlashTagDoAction actionTag;
		CompiledActions actions(outputBuffer->buffer, bufferLength(outputBuffer));
		actionTag.AddAction(&actions);
		outputStream << actionTag;	        
		destroyBuffer(outputBuffer);
	
		fclose(error_fp);

		if(success != 0)
		{
			return false;
		}
		return true;

	}

};
