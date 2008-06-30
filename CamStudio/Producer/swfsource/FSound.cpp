#include <strstream>
#include "FSound.h"

N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagDefineSound &data)
{
	out << FlashTagHeader(14,2+1+4+data.len);
	WRITE_UWORD(data.GetID());
	out.put(data.flags);
	WRITE_UDWORD(data.samplecount);
	for(UDWORD i=0; i < data.len; i++)
	{
		out.put((char)data.samples[i]);
	}
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagDefineSound &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	int flags = in.get();
	//if(flags == EOF) throw;
	data.flags = flags;
	READ_UDWORD(data.samplecount);
	
	data.len = data.importsize - (2+1+4);
	
	data.samples = (char *)malloc(data.len);
	data.samplevec.push_back(data.samples);
	in.read(data.samples,data.len);
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagDefineSoundMP3 &data)
{
	out << FlashTagHeader(14,2+1+4+data.len+2);
	WRITE_UWORD(data.GetID());
	out.put(data.flags);
	WRITE_UDWORD(data.samplecount);
	WRITE_UWORD(data.delayseek);
	for(UDWORD i=0; i < data.len; i++)
	{
		out.put((char)data.samples[i]);
	}
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagDefineSoundMP3 &data)
{
	UWORD id;
	READ_UWORD(id);
	data.SetID(id);
	int flags = in.get();
	//if(flags == EOF) throw;
	data.flags = flags;
	READ_UDWORD(data.samplecount);
	READ_UWORD(data.delayseek);
	data.len = data.importsize - (2+1+6);
	
	data.samples = (char *)malloc(data.len);
	data.samplevec.push_back(data.samples);
	in.read(data.samples,data.len);
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, FlashSoundEnvelope &data)
{
	WRITE_UDWORD(data.mark44);
	WRITE_UWORD(data.level0);
	WRITE_UWORD(data.level1);
	return out;
}

N_STD::istream& operator>>(N_STD::istream& in, FlashSoundEnvelope &data)
{
	READ_UDWORD(data.mark44);
	READ_UWORD(data.level0);
	READ_UWORD(data.level1);
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, FlashSoundInfo &data)
{
	out.put(data.flags);
	if(data.flags & 0x1) WRITE_UDWORD(data.inPoint);
	if(data.flags & 0x2) WRITE_UDWORD(data.outPoint);
	if(data.flags & 0x4) WRITE_UWORD(data.loopCount);
	if(data.flags & 0x8) 
	{
		out.put((char)data.v_snd_env.size());
		for(N_STD::vector<FlashSoundEnvelope>::iterator i=data.v_snd_env.begin();
		i != data.v_snd_env.end(); i++)
		{
			out << *i;
		}
	}
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashSoundInfo &data)
{
	int flags = in.get();
	//if(flags == EOF) throw;
	data.flags = flags;

	if(data.flags & 0x1) READ_UDWORD(data.inPoint);
	if(data.flags & 0x2) READ_UDWORD(data.outPoint);
	if(data.flags & 0x4) READ_UWORD(data.loopCount);
	if(data.flags & 0x8) 
	{
		int size = in.get();
		//if(size != EOF) throw;
		
		for(int i = 0; i < size; i++)
		{
			FlashSoundEnvelope fse(0,0,0);
			in >> fse;
		}
	}
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagStartSound &data)
{
	N_STD::ostrstream tmp;
	WRITE_UWORD2(data.id,tmp);
	tmp << data.si;
	out << FlashTagHeader(15,tmp.pcount());
	out.write(tmp.rdbuf()->str(), tmp.pcount());
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagStartSound &data)
{
	READ_UWORD(data.id);
	in >> data.si;
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamHead &data)
{
	out << FlashTagHeader(18,4);
	out.put((char)((int)(data.mplay_rate << 2) | ((int)data.mplay_16bit << 1) | ((int)data.mplay_stereo)));
	out.put((char)(((int)data.mcompression << 4) | (int)(data.mstream_rate << 2) | ((int)data.mstream_16bit << 1) | ((int)data.mstream_stereo)));
	WRITE_UWORD(data.msamplecountavg);
	return out;
}

N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamHead &data)
{
	int flags = in.get();
	//if(flags == EOF) throw;
	
	data.mplay_rate = (GetIsolatedBits((unsigned char)flags, 2, 4));
	data.mplay_16bit = (GetIsolatedBits((unsigned char)flags, 1, 2) == 1);
	data.mplay_stereo = (GetIsolatedBits((unsigned char)flags, 0, 1) == 1);
	
	flags = in.get();
	//if(flags == EOF) throw;
	data.mcompression = (GetIsolatedBits((unsigned char)flags, 4, 6));
	data.mstream_rate = (GetIsolatedBits((unsigned char)flags, 2, 4));
	data.mstream_16bit = (GetIsolatedBits((unsigned char)flags, 1, 2) == 1);
	data.mstream_stereo = (GetIsolatedBits((unsigned char)flags, 0, 1) == 1);

	READ_UWORD(data.msamplecountavg);

	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamHead2 &data)
{
	out << FlashTagHeader(45,4);
	out.put((char)(((int)data.mplay_rate << 2) | ((int)data.mplay_16bit << 1) | ((int)data.mplay_stereo)));
	out.put((char)(((int)data.mcompression << 4) | ((int)data.mstream_rate << 2) | ((int)data.mstream_16bit << 1) | ((int)data.mstream_stereo)));
	WRITE_UWORD(data.msamplecountavg);
	return out;
}

N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamHead2 &data)
{
	int flags = in.get();
	//if(flags == EOF) throw;
	data.mplay_rate = (GetIsolatedBits((unsigned char)flags, 2, 4));
	data.mplay_16bit = (GetIsolatedBits((unsigned char)flags, 1, 2) == 1);
	data.mplay_stereo = (GetIsolatedBits((unsigned char)flags, 0, 1) == 1);
	
	flags = in.get();
	//if(flags == EOF) throw;
	data.mcompression = (GetIsolatedBits((unsigned char)flags, 4, 6));
	data.mstream_rate = (GetIsolatedBits((unsigned char)flags, 2, 4));
	data.mstream_16bit = (GetIsolatedBits((unsigned char)flags, 1, 2) == 1);
	data.mstream_stereo = (GetIsolatedBits((unsigned char)flags, 0, 1) == 1);

	READ_UWORD(data.msamplecountavg);

	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamBlock &data)
{
	out << FlashTagHeader(19,data.len);
	for(UDWORD i = 0; i < data.len; i++)
	{
		out.put(data.data[i]);
	}
	return out;
}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamBlock &data)
{
	data.data = (char *)malloc(data.importsize);
	data.len = data.importsize;
	in.read(data.data, data.importsize);
	data.gc.push_back(data.data);
	return in;
}

N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamBlockMP3 &data)
{
	out << FlashTagHeader(19,data.len+4);
	WRITE_UWORD(data.samplecount);
	WRITE_UWORD(data.delay_seek);
	for(UDWORD i = 0; i < data.len; i++)
	{
		out.put(data.data[i]);
	}
	return out;
	}
N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamBlockMP3 &data)
{
	READ_UWORD(data.samplecount);
	READ_UWORD(data.delay_seek);
	data.data = (char *)malloc(data.importsize-4);
	data.len = data.importsize-4;
	in.read(data.data, data.importsize-4);
	data.gc.push_back(data.data);
	return in;
}



// *******************************
// CamStudio v2.2 extension
// *******************************
// 
// ADPCM tables
//
/*

int indexTable2[2] = {
    -1, 2, 
};

int indexTable3[4] = {
    -1, -1, 2, 4,
};

int indexTable4[8] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
};

int indexTable5[16] = {
	-1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16, 
};

int* indexTables[] = {
	indexTable2,
	indexTable3,
	indexTable4,
	indexTable5 
};

static const int stepsizeTable[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};



N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamBlockADPCM &data)
{
	out << FlashTagHeader(19,data.len);
	for(UDWORD i = 0; i < data.len; i++)
	{
		out.put(data.data[i]);
	}
	return out;
}

N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamBlockADPCM &data)
{
	data.data = (char *)malloc(data.importsize);
	data.len = data.importsize;
	in.read(data.data, data.importsize);
	data.gc.push_back(data.data);
	return in;
}
*/