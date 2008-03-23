#include "FTarga.h"
#include <fstream>

void FlashTarga::Read()
{
	N_STD::ifstream in(fname,N_STD::ios::binary);
	if(in.fail())
	{
		//	throw(); // ERROR
	}
	
	unsigned char header[18];
	
	if(in.read((char *)header,18).fail())
	{
		//throw();
	}
	if(header[1] == 1)
	{		
		UWORD length  = ((UWORD)header[5] | ((UWORD)header[6] << 8)) + ((UWORD)header[3] | ((UWORD)header[4] << 8));
		UWORD size    = ((UWORD)header[7]);

		UDWORD count = length * (size >> 3);
		char *c=(char *)malloc(count);
		if(in.read(c,count).fail())
		{
			//throw();
		}
		free (c);
	}
	if(header[2] != 2)
	{
		//throw();
	}
	if(header[16] != 24)
	{
		//throw();
	}
	if(GetIsolatedBits(header[17],0,4) != 0)
	{
		//throw();
	}
	
	if(header[0] > 0)
	{
		char *c=(char *)malloc(header[0]);
		if(in.read(c,header[0]).fail())
		{
			//throw();
		}
		free (c);
	}
	
	
	width  = ((UWORD)header[12] | ((UWORD)header[13] << 8));
	
	height = ((UWORD)header[14] | ((UWORD)header[15] << 8));

	UDWORD size = width*height*3;
	data = (unsigned char *)malloc(size);

	for(int i = 0; i < height; i++)
	{				
		
		in.read((char *)(data+i*width*3),(width)*3);
		if(in.fail())
		{
			N_STD::cout << "error\n";
			//throw();
		}	  		
		//if(in.get() == EOF) throw;
	}
}

