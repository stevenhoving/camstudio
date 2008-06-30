#include "FBitmap.h"
#include "zlib.h"
#include <sstream>
N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsJPEG1 &data)
{
	out << FlashTagHeader(6, 2+data.len);
	WRITE_UWORD(data.GetID());
	for(UDWORD d=0; d < data.len; d++)
	{
		out.put((unsigned char)data.data[d]);
	}
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsJPEG1 &data)
{
	
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	
	data.data = (unsigned char*)malloc(data.importsize - 2);
	data.datagc.push_back(data.data);

	for(UDWORD d=0; d < data.importsize - 2; d++)
	{
		int i = in.get();		
		data.data[d] = i;
	}
	data.len = data.importsize-2;
	
	return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagJPEGTables &data)
{
	out << FlashTagHeader(8,data.len);
	for(UDWORD d=0; d < data.len; d++)
	{
		out.put((unsigned char)data.data[d]);
	}
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagJPEGTables &data)
{
	data.data = (unsigned char*)malloc(data.importsize);
	data.datagc.push_back(data.data);

	for(UDWORD d=0; d < data.importsize; d++)
	{
		int i = in.get();		
		data.data[d] = i;
	}
	data.len = data.importsize;
	return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsJPEG2 &data)
{
	out << FlashTagHeader(21, 6+2+data.len);
	
	
	WRITE_UWORD(data.GetID());

	out.put((unsigned char)0xff);
	out.put((unsigned char)0xd9);
		
	out.put((unsigned char)0xff);
	out.put((unsigned char)0xd8);
			
	out.write((const char *)data.data, data.len);
	
	out.put((unsigned char)0xff);
	out.put((unsigned char)0xd9);
	
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsJPEG2 &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);	
	{
		data.data = (unsigned char *)malloc(data.importsize-2);
		data.gc.push_back(data.data);
		in.read((char *)data.data, data.importsize-2); //throw if not proper amount read
		data.len = data.importsize-2;
	}
	return in;
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsJPEG3 &data)
{
	
	unsigned long csize = data.len2+(UDWORD)(data.len2/100)+12;
	unsigned char *compression_buffer = (unsigned char*)malloc(csize);
	
	if(compress2(compression_buffer, &csize, (const unsigned char*)data.data2, data.len2, Z_BEST_COMPRESSION) == Z_OK)
	{
		out << FlashTagHeader(35, 2+4+data.len+csize);
		
		WRITE_UWORD(data.GetID());
		WRITE_UDWORD(data.len);
		{
			for(UDWORD d=0; d < data.len; d++)
			{
				out.put((unsigned char)data.data[d]);		
			}
		}

		out.write((char*)compression_buffer,csize);

	}
	else
	{		
		//throw
	}
	free(compression_buffer);
	
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsJPEG3 &data)
{
	UWORD id;
	READ_UWORD(id);
	READ_UDWORD(data.len);
	data.SetID(id);	
	{
		data.data = (unsigned char *)malloc(data.len);
		data.gc.push_back(data.data);
		in.read((char *)data.data, data.len); //throw if not proper amount read
	}

	{
		data.data2 = (unsigned char *)malloc(data.importsize-data.len-6);
		data.gc.push_back(data.data2);
		in.read((char *)data.data2, data.importsize-data.len-6); //throw if not proper amount read
		data.len2 = data.importsize-data.len-6;
	}

	return in;
}

void FlashZLibBitmapData::Write(N_STD::ostream &out, char format)
{
	N_STD::ostringstream tmp;

	for(N_STD::vector<FlashRGB>::iterator i=colors.begin(); i != colors.end(); i++)
	{
		(*i).SetAlphaWriteMode(withalpha);
		tmp << *i;		
	}
	tmp.write((const char *)data,len);

	unsigned long csize = tmp.str().size()+(UDWORD)(tmp.str().size()/100)+12;
	unsigned char *compression_buffer = (unsigned char*)malloc(csize);
	
	if(compress2(compression_buffer, &csize, (const unsigned char*)tmp.str().c_str(), tmp.str().size(), Z_BEST_COMPRESSION) == Z_OK)
	{
		out.write((char*)compression_buffer,csize);
	}
	else
	{		
		//throw
	}
	free(compression_buffer);
}
void FlashZLibBitmapData::Read(N_STD::istream &in, char format, UDWORD input_buffer_size, UDWORD size_buffer)
{
	UDWORD sizec = -1;
	UDWORD len2 = size_buffer;
	if(format==3)
	{
		input_buffer_size--;     // must not include the UI8 to be read below
		sizec = (UDWORD)in.get();
		sizec++;
		size_buffer +=sizec * 3;
		if(GetAlpha()) size_buffer += sizec;
	}
	else if(format==4)
	{
		input_buffer_size -= 2;  // must not include the UI16 to be read below
		UWORD    size_uword;
		READ_UWORD(size_uword);
		sizec = size_uword;
		sizec++;
// TODO - to be completed?
//		size_buffer +=sizec * 3;
//		if(GetAlpha()) size_buffer += sizec;
	}
	else if(format==5)
	{
		input_buffer_size -= 4;  // must not include the UI32 to be read below
		READ_UDWORD(sizec);
		sizec++;
// TODO - to be completed?
//		size_buffer +=sizec * 3;
//		if(GetAlpha()) size_buffer += sizec;
	}
	unsigned char *datatmp = (unsigned char *)malloc(input_buffer_size);
	in.read((char *)datatmp,input_buffer_size);

	unsigned long csize = size_buffer;
	unsigned char *compression_buffer = (unsigned char*)malloc(csize);
	
	gc.push_back(compression_buffer);

	if(uncompress(compression_buffer, &csize, datatmp, input_buffer_size) == Z_OK)
	{
		if(format == 3)
		{
			for(UDWORD it = 0; it < sizec; it++)
			{			
				if(GetAlpha()) colors.push_back(FlashRGB(compression_buffer[it*4],compression_buffer[it*4]+1,compression_buffer[it*4]+2,compression_buffer[it*4]+3));
				else colors.push_back(FlashRGB(compression_buffer[it*3],compression_buffer[it*3]+1,compression_buffer[it*3]+2));
			}
			if(GetAlpha()) data = compression_buffer+sizec*4;
			else		  data = compression_buffer+sizec*3;			
			len = len2;
		}
		else
		{
			data = compression_buffer;
			len = len2;
		}
	}
	else
	{		
		//throw
	}
	free(datatmp);
}

N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsLossless &data)
{
	N_STD::ostringstream tmp;
	WRITE_UWORD2(data.GetID(),tmp);
	tmp.put(data.format);
	WRITE_UWORD2(data.width,tmp);
	WRITE_UWORD2(data.height,tmp);
	if(data.format==3) tmp.put((unsigned char)data.d.GetSize()-1);
	data.d.SetAlpha(false);
	data.d.Write(tmp,data.format);
	out << FlashTagHeader(20, tmp.str().size());
	out << tmp.str();
	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsLossless &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	data.format = in.get();
	// if(data.format == EOF) throw;
	READ_UWORD(data.width);
	READ_UWORD(data.height);
	data.d.SetAlpha(false);
	int datasize = 0;
	if(data.format == 3) datasize = data.width*data.height;
	else if(data.format == 4) datasize = data.width*data.height*2;
	else if(data.format == 5) datasize = data.width*data.height*4;
	//else throw;
	
	data.d.Read(in,data.format,data.importsize - 7, datasize);

	return in;
}
N_STD::ostream &operator << (N_STD::ostream &out, FlashTagDefineBitsLossless2 &data)
{
	N_STD::ostringstream tmp;
	WRITE_UWORD2(data.GetID(),tmp);
	tmp.put(data.format);
	WRITE_UWORD2(data.width,tmp);
	WRITE_UWORD2(data.height,tmp);
	if(data.format==3) tmp.put((unsigned char)data.d.GetSize()-1);
	data.d.SetAlpha(true);
	data.d.Write(tmp,data.format);
	out << FlashTagHeader(36, tmp.str().size());
	out << tmp.str();

	return out;
}

N_STD::istream &operator >> (N_STD::istream &in,  FlashTagDefineBitsLossless2 &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	data.format = in.get();
	// if(data.format == EOF) throw;
	READ_UWORD(data.width);
	READ_UWORD(data.height);
	data.d.SetAlpha(true);
	int datasize = 0;
	if(data.format == 3) datasize = data.width*data.height;
	else if(data.format == 4) datasize = data.width*data.height*2;
	else if(data.format == 5) datasize = data.width*data.height*4;
	//else throw;
	
	data.d.Read(in,data.format,data.importsize - 7, datasize);

	return in;
}
