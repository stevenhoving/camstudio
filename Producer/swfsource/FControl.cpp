#include "FControl.h"


N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsPtr &data)
{
	out << FlashTagHeader(1023, sizeof(UDWORD));
	WRITE_UDWORD(data.ref)
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsPtr &data)
{
	READ_UDWORD(data.ref)
	return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagProtect &data)
{
	if(data.hasPass){
		out << FlashTagHeader(24, data.md5Pass.size() + 1);
		out << data.md5Pass;
		out.put((char)0);
	} else out << FlashTagHeader(24, 0);	
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagProtect &data)
{	
	char c;
	while((c = in.get()) != 0)
	{
		data.md5Pass += ((char)c);		
	}
	data.hasPass = (data.md5Pass.size()>0);
	return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagEnableDebugger &data)
{
	if(data.hasPass){
		out << FlashTagHeader(58, data.md5Pass.size() + 1);
		out << data.md5Pass;
		out.put((char)0);
	} else out << FlashTagHeader(58, 0);
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagEnableDebugger &data)
{	
	char c;
	while((c = in.get()) != 0)
	{
		data.md5Pass += ((char)c);		
	}
	data.hasPass = (data.md5Pass.size()>0);
	return in;
}


N_STD::ostream &operator << (N_STD::ostream &out, FlashTagBackgroundColor &data)
{
	out << FlashTagHeader(0x09,3);
	out.put(data.r);
	out.put(data.g);
	out.put(data.b);
	return out;
}
N_STD::istream &operator >> (N_STD::istream &in,  FlashTagBackgroundColor &data)
{
	data.r = in.get();
	data.g = in.get();
	data.b = in.get();
	return in;
}
N_STD::ostream &operator << (N_STD::ostream &out, FlashTagLabelFrame &data)
{
	out << FlashTagHeader(43,strlen(data.str)+1+(data.IsNamedAnchor()?1:0));
	out << data.str;
	out.put((char)0);
	if(data.IsNamedAnchor())
		out.put((char)1);
	return out;
}
N_STD::istream &operator >> (N_STD::istream &in,  FlashTagLabelFrame &data)
{
	N_STD::streampos start = in.tellg();
	int c;
	N_STD::vector<char> str;
	while((c = in.get()) != 0)
	{
		str.push_back((char)c);		
	}
	data.str = (char*)malloc(str.size()+1);
	data.gc.push_back(data.str);
	unsigned int i;
	for( i = 0; i < str.size(); i++)
	{
		data.str[i]=str[i];
	}
	data.str[i] = '\0';

	if(data.importsize + start != (UDWORD)in.tellg())
	{
		// Introduced in FlashMX, but when published to Flash 5 using FlashMX, this
		// flag will be saved as well (Flash5 plugin seems to handle this format properly)
		data.namedAnchor = (in.get() != 0);
	}
	return in;
}

DEFINE_SIMPLE_TAG(FlashTagEnd,0x00)

void FlashTagExportAssets::AddAsset(char *str, UWORD id)
{
	assets_str.push_back(str);
	assets_id.push_back(id);
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagExportAssets &data)
{
	UWORD len = 2;
	{
		for(UWORD i=0; i < (UWORD)data.assets_str.size(); i++)
		{
			len+=2;
			len+=strlen(data.assets_str[i])+1;		
		}
	}
	out << FlashTagHeader(56,len);

	WRITE_UWORD(data.assets_str.size());
	{
		for(UWORD i=0; i < (UWORD)data.assets_str.size(); i++)
		{
			WRITE_UWORD(data.assets_id[i]);
			out << data.assets_str[i];
			out.put((char)0);		
		}
	}
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagExportAssets &data)
{	
	UWORD size;
	READ_UWORD(size);
	{
		data.assets_id.clear();
		data.assets_str.clear();
				
		for(UWORD i=0; i < size; i++)
		{
			data.assets_id.push_back(0);
			READ_UWORD(data.assets_id[i]);
			int c;
			N_STD::vector<char> tmp;
			while((c = in.get()) != 0)
			{
				tmp.push_back(c);
			}
			char * tmpp = ((char*)malloc(tmp.size()+1));
			data.assets_str.push_back(tmpp);
			data.gc.push_back(tmpp);
			for(unsigned int i2 = 0; i2 < tmp.size(); i2++)
			{
				(data.assets_str[i])[i2]=tmp[i2];
			}
			(data.assets_str[i])[tmp.size()] = 0;
		}
	}
	return in;
}

void FlashTagImportAssets::AddAsset(char *str, UWORD id)
{
	assets_str.push_back(str);
	assets_id.push_back(id);
}


N_STD::ostream &operator << (N_STD::ostream &out, FlashTagImportAssets &data)
{
	
	UWORD len = 2;
	{
		for(UWORD i=0; i < (UWORD)data.assets_str.size(); i++)
		{
			len+=2;
			len+=strlen(data.assets_str[i])+1;		
		}
	}
	len+=strlen(data.swfurl)+1;

	out << FlashTagHeader(57,len);

	out << data.swfurl;
	out.put((char)0);
	WRITE_UWORD(data.assets_str.size());
	{
		for(UWORD i=0; i < (UWORD)data.assets_str.size(); i++)
		{
			WRITE_UWORD(data.assets_id[i]);
			out << data.assets_str[i];
			out.put((char)0);
		}
	}
	return out;

}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagImportAssets &data)
{
	unsigned int i2;
	int c;
	N_STD::vector<char> tmp;
	while((c = in.get()) != 0)
	{
		tmp.push_back(c);
	}
	data.swfurl = ((char*)malloc(tmp.size()+1));
	for(i2 = 0; i2 < tmp.size(); i2++)
	{
		data.swfurl[i2]=tmp[i2];
	}
	data.swfurl[tmp.size()] = '\0';
	
	UWORD size;
	READ_UWORD(size);
	{
		data.assets_id.clear();
		data.assets_str.clear();
				
		for(UWORD i=0; i < size; i++)
		{
			data.assets_id.push_back(0);
			READ_UWORD(data.assets_id[i]);
			int c;
			N_STD::vector<char> tmp;
			while((c = in.get()) != 0)
			{
				tmp.push_back(c);
			}
			char * tmpp = ((char*)malloc(tmp.size()+1));
			data.assets_str.push_back(tmpp);
			data.gc.push_back(tmpp);
			for(i2 = 0; i2 < tmp.size(); i2++)
			{
				(data.assets_str[i])[i2]=tmp[i2];
			}
			(data.assets_str[i])[tmp.size()] = '\0';
		}
	}

	return in;
}

//extern void MsgC(const char fmt[], ...);
N_STD::ostream &operator << (N_STD::ostream &out, FlashTagFreeCharacter &data)
{
	
	//modified by CamStudio v2.27 	
	//out << FlashTagHeader(3,4);	
	//MsgC("data.td %d",data.id);
	//WRITE_UDWORD(data.id);

	out << FlashTagHeader(3,2);
	WRITE_UWORD(data.id);
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagFreeCharacter &data)
{
	
	//modified by CamStudio v2.27 	
	//READ_UDWORD(data.id);

	READ_UWORD(data.id);
	return in;
}

