#ifndef FSOUND_H_FILE
#define FSOUND_H_FILE

#include "FBase.h"
#include <vector>
#include <iostream>


#define FTDS_COMPRESS_NONE		(0 << 4)
#define FTDS_COMPRESS_ADPCM		(1 << 4)
#define FTDS_COMPRESS_MP3		(2 << 4)

#define FTDS_RATE_5_5_KHZ		(0 << 2)
#define FTDS_RATE_11_KHZ		(1 << 2)
#define FTDS_RATE_22_KHZ		(2 << 2)
#define FTDS_RATE_44_KHZ		(3 << 2)

#define FTDS_SND_16BIT			(1 << 1)
#define FTDS_SND_8BIT			(0 << 1)

#define FTDS_SND_STEREO			(1)
#define FTDS_SND_MONO			(0)

class FlashTagDefineSound : public FlashTag, public FlashIDEnabled
{
	DEFINE_RW_INTERFACE
		FlashTagDefineSound() {}
public:
	FlashTagDefineSound(unsigned char _flags, UDWORD _samplecount, char *_samples, UDWORD _len)
		: samples(_samples), flags(_flags), samplecount(_samplecount), len(_len) {}

	friend class FlashImportHandler;	
	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagDefineSound &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagDefineSound &data);


	char GetFlags() const { return flags; }
	UDWORD GetSampleCount() const { return samplecount; };
	UDWORD GetSampleLength() const { return len; };
	char *GetSamples() { return samples; };
protected:
	char *samples;
	gc_vector<char *>samplevec;
	unsigned char flags;
	UDWORD samplecount;
	UDWORD len;
};


class FlashTagDefineSoundMP3 : public FlashTagDefineSound
{
	DEFINE_RW_INTERFACE
public:
	FlashTagDefineSoundMP3() {}
	FlashTagDefineSoundMP3(unsigned char _flags, UDWORD _samplecount, char *_samples, UDWORD _len, UWORD _delayseek)
		: FlashTagDefineSound((unsigned char)((_flags & 0xf) | (2 << 4)) , _samplecount, _samples, _len), delayseek(_delayseek) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagDefineSoundMP3 &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagDefineSoundMP3 &data);
private:
	gc_vector<char *>samplevec;
	UWORD delayseek;
public:
	UWORD GetDelaySeek() const { return delayseek; }
};

class FlashSoundEnvelope
{
public:	
	FlashSoundEnvelope(UDWORD _mark44, UWORD _level0, UWORD _level1) : mark44(_mark44), 
		level0(_level0), level1(_level1)   {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashSoundEnvelope &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashSoundEnvelope &data);

private:
	UDWORD mark44;
	UWORD level0;
	UWORD level1;
};

#define FSI_SYNC_NO_MULTIPLE	(1 << 4)
#define FSI_SYNC_STOP			(2 << 4)
#define FSI_HAS_ENVELOPE		(1 << 3)
#define FSI_HAS_LOOP			(1 << 2)
#define FSI_HAS_OUT				(1 << 1)
#define FSI_HAS_IN				(1)

class FlashTagStartSound;

class FlashSoundInfo 
{
public:
	FlashSoundInfo() {}
	FlashSoundInfo(unsigned char _flags) : flags(_flags & 0xf0) {}
	FlashSoundInfo(unsigned char _flags, UDWORD _inPoint, UDWORD _outPoint, UWORD _loopCount)  :
		flags(_flags & 0xfe), inPoint(_inPoint), outPoint(_outPoint), loopCount(_loopCount) {}
	FlashSoundInfo(unsigned char _flags, UDWORD _inPoint, UDWORD _outPoint, UWORD _loopCount, 
		N_STD::vector<FlashSoundEnvelope> &v) : flags(_flags),
		inPoint(_inPoint), outPoint(_outPoint), loopCount(_loopCount), v_snd_env(v) {}
private:
	friend class FlashTagStartSound;
//	friend class FlashTagDefineButtonSound;
	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashSoundInfo &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashSoundInfo &data);

public:
	
	void SetFlags(unsigned char _flags) { flags = _flags; };
	void SetLoopCount(UWORD lc) { flags |= FSI_HAS_LOOP; loopCount = lc; }
	void SetInPoint(UDWORD p) { flags |= FSI_HAS_IN; inPoint = p; }
	void SetOutPoint(UDWORD p) { flags |= FSI_HAS_OUT; outPoint = p; }
	void AddEnvelope(FlashSoundEnvelope e) { flags |= FSI_HAS_ENVELOPE; v_snd_env.push_back(e); }

	unsigned char GetFlags() { return flags; }
	UDWORD GetInPoint() { return inPoint; }
	UDWORD GetOutPoint() { return outPoint; }
	UDWORD GetLoopCount() { return loopCount; }
	N_STD::vector<FlashSoundEnvelope> &GetEnvelopes() { return v_snd_env; }


private:
	unsigned char flags;
	UDWORD inPoint;
	UDWORD outPoint;
	UWORD loopCount;
	N_STD::vector<FlashSoundEnvelope> v_snd_env;
};

class FlashTagStartSound : public FlashSpriteEnabled
{
	DEFINE_RW_INTERFACE
public:
	FlashTagStartSound() {}
	FlashTagStartSound(UWORD _id, FlashSoundInfo &s) : id(_id), si(s) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, FlashTagStartSound &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagStartSound &data);

	UWORD GetID() const { return id; }
	FlashSoundInfo GetSoundInfo() { return si; }

private:
	UWORD id;
	FlashSoundInfo si;	
};

#define FTSSH_RATE_5_5_KHZ 0
#define FTSSH_RATE_11_KHZ  1
#define FTSSH_RATE_22_KHZ  2
#define FTSSH_RATE_44_KHZ  3

class FlashTagSoundStreamHead : public FlashSpriteEnabled
{
	DEFINE_RW_INTERFACE
public:
	FlashTagSoundStreamHead() {}
	FlashTagSoundStreamHead(char play_rate, bool play_16bit, bool play_stereo, 
							char stream_rate, bool stream_16bit, bool stream_stereo, UWORD samplecountavg)
							: mplay_rate(play_rate), mplay_16bit(play_16bit), mplay_stereo(play_stereo), 
							mcompression(1), mstream_rate(stream_rate), mstream_16bit(stream_16bit), 
							mstream_stereo(stream_stereo), msamplecountavg(samplecountavg) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamHead &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamHead &data);

public:
	char GetPlayRate() { return mplay_rate; }
	bool GetPlay16bit() { return mplay_16bit; }
	bool GetPlayStereo() { return mplay_stereo; }
	void SetCompression(char c) { mcompression = c; }
	char GetCompression() { return mcompression; }
	char GetStreamRate() { return mstream_rate; }
	bool GetStream16bit() { return mstream_16bit; }
	bool GetStreamStereo() { return mstream_stereo; }
	UWORD GetSampleCountAvg() { return msamplecountavg; }
private:	
	char mplay_rate;
	bool mplay_16bit;
	bool mplay_stereo;
	char mcompression;
	char mstream_rate;
	bool mstream_16bit;
	bool mstream_stereo; 
	UWORD msamplecountavg;	
};

#define FTSSH2_RATE_5_5_KHZ 0
#define FTSSH2_RATE_11_KHZ  1
#define FTSSH2_RATE_22_KHZ  2
#define FTSSH2_RATE_44_KHZ  3

#define FTSSH2_COMPRESS_NONE   0
#define FTSSH2_COMPRESS_ADPCM  1
#define FTSSH2_COMPRESS_MP3    2

class FlashTagSoundStreamHead2 : public FlashSpriteEnabled
{
	DEFINE_RW_INTERFACE
public:
	FlashTagSoundStreamHead2() {}
	FlashTagSoundStreamHead2(char play_rate, bool play_16bit, bool play_stereo, char compression,
							char stream_rate, bool stream_16bit, bool stream_stereo, UWORD samplecountavg)
							: mplay_rate(play_rate), mplay_16bit(play_16bit), mplay_stereo(play_stereo), 
							mcompression(compression), mstream_rate(stream_rate), mstream_16bit(stream_16bit), 
							mstream_stereo(stream_stereo), msamplecountavg(samplecountavg) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamHead2 &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamHead2 &data);

public:
	char GetPlayRate() { return mplay_rate; }
	bool GetPlay16bit() { return mplay_16bit; }
	bool GetPlayStereo() { return mplay_stereo; }
	char GetCompression() { return mcompression; }
	char GetStreamRate() { return mstream_rate; }
	bool GetStream16bit() { return mstream_16bit; }
	bool GetStreamStereo() { return mstream_stereo; }
	UWORD GetSampleCountAvg() { return msamplecountavg; }
private:
	char mplay_rate;
	bool mplay_16bit;
	bool mplay_stereo;
	char mcompression;
	char mstream_rate;
	bool mstream_16bit;
	bool mstream_stereo; 
	UWORD msamplecountavg;	
};


class FlashTagSoundStreamBlock : public FlashSpriteEnabled
{
	DEFINE_RW_INTERFACE
public:
	FlashTagSoundStreamBlock() {}
	FlashTagSoundStreamBlock(char *_data, UDWORD _len) : data(_data), len(_len) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamBlock &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamBlock &data);

	UDWORD GetSampleLength() const { return len; }
	char *GetSamples() { return data; }
protected:
        char *data;
	UDWORD len;

	
	gc_vector<char *> gc;
};

class FlashTagSoundStreamBlockMP3 : public FlashTagSoundStreamBlock
{
	DEFINE_RW_INTERFACE
public:
        FlashTagSoundStreamBlockMP3() {}
        FlashTagSoundStreamBlockMP3(UWORD _samplecount, UWORD _delayseek, char *_data, UDWORD _len) : FlashTagSoundStreamBlock(_data,_len), samplecount(_samplecount), delay_seek(_delayseek) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamBlockMP3 &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamBlockMP3 &data);

private:
    UWORD samplecount;
	UWORD delay_seek;

};


// CamStudio ver 2.2 extension
/*
class FlashTagSoundStreamBlockADPCM : public FlashSpriteEnabled
{
	DEFINE_RW_INTERFACE
public:
	FlashTagSoundStreamBlockADPCM() {}
	FlashTagSoundStreamBlockADPCM(char *_data, UDWORD _len) : data(_data), len(_len) {}

	friend N_STD::ostream& operator<<(N_STD::ostream& out, const FlashTagSoundStreamBlockADPCM &data);
	friend N_STD::istream& operator>>(N_STD::istream& in, FlashTagSoundStreamBlockADPCM &data);
	
	char *GetSamples() { return data; }
protected:
    char *data;	
	UDWORD len;
	
	gc_vector<char *> gc;
};
*/

#endif


