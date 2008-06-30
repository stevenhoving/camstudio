/* MP3 Encoding Extension Class
   Author: Jesse Ezell <JesseEzell@netscape.net>
   Last Updated: Jan. 27, 2000 

*/

/* mp3 reading tables */

#include "FSound.h"
#include "FlashMP3Encoder.h"
#include <stdio.h>
#include <iostream>

static const int vertab[4]={2,3,1,0};
static const int freqtab[4]={44100,48000,32000};
static const int ratetab[2][3][16]=
 {
   {
     {  0, 32, 64,
 96,128,160,192,224,256,288,320,352,384,416,448,  0},
     {  0, 32, 48, 56, 64, 80,
 96,112,128,160,192,224,256,320,384,  0},
     {  0, 32, 40, 48, 56, 64, 80,
 96,112,128,160,192,224,256,320,  0},
   },
   {
     {  0, 32, 48, 56, 64, 80,
 96,112,128,144,160,176,192,224,256,  0},
     {  0,  8, 16, 24, 32, 40, 48, 56, 64, 80,
 96,112,128,144,160,  0},
     {  0,  8, 16, 24, 32, 40, 48, 56, 64, 80,
 96,112,128,144,160,  0},
   },
 };
 
int FlashMP3Encoder::SeekToNextValidHeader (FILE *fp, 
										 int &layer, 
										 int &ver, 
										 int &freq, 
										 int &stereo, 
										 int &rate)
 {
     unsigned char hdr[4];
     
	 long nFileEnd, nCurPos;
	 
	 nCurPos = ftell(fp);
	 
     bool          bFound = false;
     long          nRead;
     
	 if (nCurPos == -1) return (false);

     ::fseek(fp,0,SEEK_END);
     nFileEnd = ftell (fp);
 
     /* what a mess */
     while (!bFound && nCurPos + 3 < nFileEnd)
     {
         ::fseek(fp,nCurPos,SEEK_SET);
		 nRead = (long)fread(hdr,1,4,fp);
         if (nRead < 4) 
		 {			 
			 return (false);
		 }
         ver=vertab[((hdr[1]>>3)&3)];
        layer=3-((hdr[1]>>1)&3);
        //int pad=(hdr[2]>>1)&1;
        stereo=((hdr[3]>>6)&3)!=3;
        if (hdr[0]!=0xFF || hdr [1] < 0xE0 || ver==3 || layer != 2)
        {   // not a header 
            nCurPos++;
        }
        else
        {
            freq=freqtab[(hdr[2]>>2)&3]>>ver;
            rate=ratetab[ver?1:0][layer][(hdr[2]>>4)&15]*1000;
            if (!freq || !rate)
            {
                nCurPos++;
            }
            else
            {                 
                 fseek(fp,nCurPos,SEEK_SET);
                 bFound = true;
            }
         }
     }
	 return (bFound);
 }
 
 
 int FlashMP3Encoder::ReadMp3Frame (FILE *fp, char *lpData, long *pnSize)
 {
	 int   nFrameSize = 0;
     long  lResult = 0;
     if (fp==NULL) return (false);
     int layer1, ver1, freq1, stereo1, rate1;
    
     if (SeekToNextValidHeader (fp, layer1, ver1, freq1, stereo1, rate1))
     {
         nFrameSize = ((ver1?72:144)*rate1)/freq1;
		 long nRead = (long)fread(lpData, 1, nFrameSize,fp);
         // clear the padding bit
         if (nRead == nFrameSize)
         {
             if (lpData [2] & 2) // pad bit! 
             {    
                 nRead = (long)fread(&lpData [nFrameSize], 1,1,fp);
				 *pnSize+=nRead;
			 }
             lResult = 0;
             
         }
     }
     return (lResult); 
 }
 
 
 
 int FlashMP3Encoder::GetMp3FrameSize (char *szMp3File, long *pnFrameSize, long *pnPCMFrameSize)
 {
	 FILE *fp;
	 fp=fopen(szMp3File,"rb");
     int layer, ver, freq, stereo, rate;
     *pnFrameSize = 0;
     if (fp==NULL) return (false);
	 if (!SeekToNextValidHeader (fp, layer, ver, freq, stereo, rate))
     {
         return (false);
     }
     *pnFrameSize = ((ver ? 72:144)*rate)/freq;

	 if(freq > 32000) *pnPCMFrameSize = 1152;
		else *pnPCMFrameSize = 576;
    fclose(fp);
	 return (0);
}

FlashMP3Encoder::FlashMP3Encoder(const char *filename, int FrameRate) :
	m_delay(0), m_frameRate(FrameRate), m_pos(0), m_done(false), m_frame(0), m_sampleRate(-1)
{

     FILE *fp = fopen(filename,"rb");

     int &layer = m_layer;
	 int &ver = m_ver;
	 int &freq = m_freq;
	 int &stereo = m_stereo;
	 int &rate = m_rate;

     long pnFrameSize = 0;
	 long pnPCMFrameSize = 0;

     if (fp==NULL) return; // ERROR

	 char *data;
	 while (SeekToNextValidHeader (fp, layer, ver, freq, stereo, rate))
     {
		pnFrameSize = ((ver?72:144)*rate)/freq;
		if(freq > 32000) pnPCMFrameSize = 1152;
		else pnPCMFrameSize = 576;

		 data = (char*)malloc((pnFrameSize+1));
		 
		ReadMp3Frame(fp,data,&pnFrameSize);
		
		m_sizes.push_back(pnFrameSize);	
		m_frameData.push_back(data);		
     }
    fclose(fp);
	
	pcmFrameSize=pnPCMFrameSize;
	
	int samplerate;
	if(m_freq==44100) samplerate=3;
	if(m_freq==22050) samplerate=2;
	if(m_freq==11025) samplerate=1;
	if(m_freq==5512) samplerate=0;
	else samplerate = -1;
	if(samplerate == -1)
	{
		//throw ERROR;
	}

}

FlashMP3Encoder::FlashMP3Encoder(const char *filename, int FrameRate, int sampleRate) :
	m_delay(0), m_frameRate(FrameRate), m_pos(0), m_done(false), m_frame(0), m_sampleRate(sampleRate)
{

     FILE *fp = fopen(filename,"rb");

     int &layer = m_layer;
	 int &ver = m_ver;
	 int &freq = m_freq;
	 int &stereo = m_stereo;
	 int &rate = m_rate;

     long pnFrameSize = 0;
	 long pnPCMFrameSize = 0;

     if (fp==NULL) return; // ERROR

	 char *data;
	 while (SeekToNextValidHeader (fp, layer, ver, freq, stereo, rate))
     {
		pnFrameSize = ((ver?72:144)*rate)/freq;
		if(freq > 32000) pnPCMFrameSize = 1152;
		else pnPCMFrameSize = 576;

		 data = (char*)malloc((pnFrameSize+1));
		 
		ReadMp3Frame(fp,data,&pnFrameSize);
		
		m_sizes.push_back(pnFrameSize);	
		m_frameData.push_back(data);		
     }
    fclose(fp);
	
	pcmFrameSize=pnPCMFrameSize;
	
	int samplerate;
	if(m_freq==44100) samplerate=3;
	if(m_freq==22050) samplerate=2;
	if(m_freq==11025) samplerate=1;
	if(m_freq==5512) samplerate=0;
	else samplerate = -1;
	if(samplerate == -1)
	{
		//throw ERROR;
	}

}

FlashMP3Encoder::~FlashMP3Encoder()
{
	for(N_STD::vector<char*>::iterator i = m_frameData.begin(); i != m_frameData.end(); i++)
	{
		free(*i);
	}
}

void FlashMP3Encoder::WriteStreamHeader(N_STD::ostream &out)
{
	
	const double mspf_mp3 = 1000.0 / ((float) m_freq / pcmFrameSize );
	
	double avg_samples = (double)((1000.0/m_frameRate)/mspf_mp3)*(double)(pcmFrameSize);
	
	int samplerate = 0;
	if(m_sampleRate == -1)
	{
		if(m_freq==44100) samplerate=3;
		if(m_freq==22050) samplerate=2;
		if(m_freq==11025) samplerate=1;
		if(m_freq==5512) samplerate=0;
	}
	else samplerate = m_sampleRate;

	out << FlashTagSoundStreamHead2(3, 1,  1, 2, samplerate, 1, m_stereo ? true : false, (UWORD)avg_samples);
}


void FlashMP3Encoder::WriteStreamBlock(N_STD::ostream &out)
{
	if(m_frameRate < 0)
	{
		m_done = true;
		return;
	}

	if(!m_done)
	{
		double		 mspf	  = 1000.0/m_frameRate;
		const double mspf_mp3 = 1000.0 / ((float) m_freq / pcmFrameSize );
	
		unsigned int i=0;
	
		for(;;)
		{
			if((double)mspf_mp3*i+(m_delay*mspf_mp3/pcmFrameSize) > mspf) break; 			
			i++;
		}
		long size=0;
		unsigned int index;
		for(index = 0; (index < i) && (m_pos+index < m_sizes.size()); index++)
		{
			size+=m_sizes[m_pos+index];

		}
		//long delay = (long)((double)index*sps);
	
		char *out2 = (char*)malloc(size+1);
	
		long offset=0;
		for(i=0; i < index; i++)
		{
			memcpy(out2+offset,m_frameData[m_pos+i],m_sizes[m_pos+i]);
			offset+=m_sizes[m_pos+i];
		}
		
		out << FlashTagSoundStreamBlockMP3((UWORD)pcmFrameSize*index, (UWORD)m_delay, (char *)out2, (UDWORD)size); // 1152*index, 
	    
		m_frame++;
		m_pos+=index;
		m_delay = (long)(((m_pos * mspf_mp3) - (m_frame*mspf))*pcmFrameSize/mspf_mp3);

		if(m_pos >= m_frameData.size()) m_done = true;
	}
}

UWORD FlashMP3Encoder::WriteDefineTag(N_STD::ostream &out, int characterId)
{
	int samplerate = 0;
	if(m_sampleRate == -1)
	{	
	if(m_freq==44100) samplerate=3;
	if(m_freq==22050) samplerate=2;
	if(m_freq==11025) samplerate=1;
	if(m_freq==5512) samplerate=0;
	}
	else samplerate = m_sampleRate;
	
	UDWORD len = 0;
	for(N_STD::vector<UDWORD>::iterator i = m_sizes.begin(); i != m_sizes.end(); i++)
	{ 
	  len += *i;	
	}
	char *data = (char *)malloc(len);
	UDWORD it=0;
	UDWORD pos=0;
	for(N_STD::vector<char *>::iterator i2 = m_frameData.begin(); i2 != m_frameData.end(); i2++)
	{ 
		memcpy(data+pos, *i2, m_sizes[it]);
		pos+=m_sizes[it];
		it++;
	}
	FlashTagDefineSoundMP3 tag((samplerate << 2) | (1 << 1) | (m_stereo), (UDWORD)pcmFrameSize*m_sizes.size(), data, (UDWORD)len, (UDWORD)0);
	if(characterId != 0)
	{
		tag.SetID(characterId);
	}
	out << tag;
	free (data);
	
	return(tag.GetID());
}
