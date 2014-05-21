#ifndef FLASH_HEADER_TARGA
#define FLASH_HEADER_TARGA

#include "FBase.h"

class FlashTarga
{
public:
	FlashTarga(char *_fname) : fname(_fname), data(NULL) {}
	~FlashTarga() { if(data != NULL) free(data); }
	
	const unsigned char *GetData() { return data; }
	void Read();
	
	UWORD GetWidth() { return width; }
	UWORD GetHeight() { return height; }

	UWORD width;
	UWORD height;

	char *fname;
	unsigned char *data;

};

#endif

