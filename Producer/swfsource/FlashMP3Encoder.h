/* MP3 Encoding Extension Class
   Author: Jesse Ezell <JesseEzell@netscape.net>
   Last Updated: Oct. 12, 2000
*/

#ifndef H_FLASHMP3_JE
#define H_FLASHMP3_JE

#include "FBase.h"
#include <stdio.h>
#include <vector>

class FlashMP3Encoder
{
public:
	FlashMP3Encoder(const char *filename, int FrameRate);
	FlashMP3Encoder(const char *filename, int FrameRate, int sampleRate);
	~FlashMP3Encoder();

	UWORD WriteDefineTag(N_STD::ostream &out, int characterId = 0);
	
	void WriteStreamHeader(N_STD::ostream &out);
	void WriteStreamBlock(N_STD::ostream &out);
	
	bool Done() const { return m_done; }

private:
	int GetMp3FrameSize (char *szMp3File, long *pnFrameSize, long *pnPCMFrameSize);    
	int ReadMp3Frame (FILE *fp, char *lpData, long *pnSize);
	int SeekToNextValidHeader (FILE *fp, int &layer, int &ver, int &freq, int &stereo, int &rate);

	N_STD::vector<char *> m_frameData;	
	N_STD::vector<UDWORD>	m_sizes;
	
    long m_delay;
    int m_frameRate;
    unsigned long m_pos;
    bool m_done;
    long m_frame;
	int m_sampleRate;

	int m_layer;
	int m_ver;
	int m_freq;
	int m_stereo;
	int m_rate;
	long pcmFrameSize;
};

#endif
