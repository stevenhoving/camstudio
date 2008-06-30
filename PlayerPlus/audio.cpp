 /****************************************************************************
 *
 *  CamStudio Player
 *
 **************************************************************************/

#define STRICT
#define INC_OLE2
#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "muldiv32.h"
#include <vfw.h>

extern int ErrMsg (LPSTR sz,...);

BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
void CALLBACK aviaudioMessage(HWND, UINT, WPARAM, LPARAM);
void CALLBACK aviaudioStop(void);
LONG CALLBACK aviaudioTime(void);

//AUDIO PLAYING STUFF
#define MAX_AUDIO_BUFFERS       16
#define MIN_AUDIO_BUFFERS       2
#define AUDIO_BUFFER_SIZE       16384

HWAVEOUT        shWaveOut = 0;  /* Current MCI device ID */
LONG            slBegin;
LONG            slCurrent;
LONG            slEnd;
BOOL            sfLooping;
BOOL            sfPlaying = FALSE;

WORD            swBuffers;          // total # buffers
WORD            swBuffersOut;       // buffers device has
WORD            swNextBuffer;       // next buffer to fill
LPWAVEHDR       salpAudioBuf[MAX_AUDIO_BUFFERS];

PAVISTREAM      spavi;              // stream we're playing
LONG            slSampleSize;       // size of an audio sample
LONG            sdwBytesPerSec;
LONG            sdwSamplesPerSec;

int recalc = 0;
long streamEnd = 0;
long streamStart = 0;

extern int audioPlayable;



void aviaudioCloseDevice(void)
{
    if (shWaveOut)
    {
		while (swBuffers > 0)
		{
		--swBuffers;
		waveOutUnprepareHeader(shWaveOut, salpAudioBuf[swBuffers],
					sizeof(WAVEHDR));
		GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
		}
	waveOutClose(shWaveOut);

	shWaveOut = NULL;
    }
}


BOOL CALLBACK aviaudioOpenDevice(HWND hwnd, PAVISTREAM pavi)
{
    MMRESULT            mmResult;
    LPVOID              lpFormat;
    LONG                cbFormat;
    AVISTREAMINFO       strhdr;

	if (!pavi)          // no wave data to play
		return FALSE;

    if (shWaveOut)      // already something playing
		return TRUE;
	
    spavi = pavi;

	recalc = 1;

    AVIStreamInfo(pavi, &strhdr, sizeof(strhdr));

    slSampleSize = (LONG) strhdr.dwSampleSize;
    if (slSampleSize <= 0 || slSampleSize > AUDIO_BUFFER_SIZE)
		return FALSE;

    //AVIStreamFormatSize(pavi, 0, &cbFormat);
	AVIStreamFormatSize(pavi, AVIStreamStart(pavi), &cbFormat);

    lpFormat = GlobalAllocPtr(GHND, cbFormat);
    if (!lpFormat)
		return FALSE;

    //AVIStreamReadFormat(pavi, 0, lpFormat, &cbFormat);
	AVIStreamReadFormat(pavi, AVIStreamStart(pavi), lpFormat, &cbFormat);
	

    sdwSamplesPerSec = ((LPWAVEFORMAT) lpFormat)->nSamplesPerSec;
    sdwBytesPerSec = ((LPWAVEFORMAT) lpFormat)->nAvgBytesPerSec;

    mmResult = waveOutOpen(&shWaveOut, (UINT)WAVE_MAPPER, (WAVEFORMATEX *) lpFormat,
			(DWORD) (UINT) hwnd, 0L, CALLBACK_WINDOW);



    // Maybe we failed because someone is playing sound already.
    // Shut any sound off, and try once more before giving up.
    if (mmResult) {
	sndPlaySound(NULL, 0);
	mmResult = waveOutOpen(&shWaveOut, (UINT)WAVE_MAPPER, (WAVEFORMATEX *) lpFormat,
			(DWORD) (UINT)hwnd, 0L, CALLBACK_WINDOW);
    }
		
    if (mmResult != 0)
    {

	return FALSE;
    }

    for (swBuffers = 0; swBuffers < MAX_AUDIO_BUFFERS; swBuffers++)
    {
		if (!(salpAudioBuf[swBuffers] =
				(LPWAVEHDR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE,
				(DWORD)(sizeof(WAVEHDR) + AUDIO_BUFFER_SIZE))))
		break;
		salpAudioBuf[swBuffers]->dwFlags = WHDR_DONE;
		salpAudioBuf[swBuffers]->lpData = (LPSTR) salpAudioBuf[swBuffers]
						    + sizeof(WAVEHDR);
		salpAudioBuf[swBuffers]->dwBufferLength = AUDIO_BUFFER_SIZE;
		if (!waveOutPrepareHeader(shWaveOut, salpAudioBuf[swBuffers],
					sizeof(WAVEHDR)))
		continue;
	
		GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
		break;
    }

    if (swBuffers < MIN_AUDIO_BUFFERS)
    {
		aviaudioCloseDevice();
		return FALSE;
    }

    swBuffersOut = 0;
    swNextBuffer = 0;

    sfPlaying = FALSE;

    return TRUE;
}



// Return the time in milliseconds corresponding to the  currently playing audio sample, or -1 if no audio is playing. 
LONG CALLBACK aviaudioTime(void)
{
    MMTIME      mmtime;

	if (audioPlayable<=0) return -1;

		
    if (!sfPlaying)
		return -1;

	
	//not sure	
	if (recalc) {
		streamEnd = AVIStreamEnd(spavi);
		streamStart = AVIStreamStart(spavi);
		recalc = 0;
		//ErrMsg("recalc");
	}	
	
	if ((streamEnd<=streamStart) || (streamEnd<=0))
		return -1;
		

    mmtime.wType = TIME_SAMPLES;

    waveOutGetPosition(shWaveOut, &mmtime, sizeof(mmtime));

    if (mmtime.wType == TIME_SAMPLES)
		return AVIStreamSampleToTime(spavi, slBegin)
				+ muldiv32(mmtime.u.sample, 1000, sdwSamplesPerSec);
    else if (mmtime.wType == TIME_BYTES)
		return AVIStreamSampleToTime(spavi, slBegin)
				+ muldiv32(mmtime.u.cb, 1000, sdwBytesPerSec);
    else
		return -1;
}



// Fill up any empty audio buffers and ship them out to the  device.                                                       
BOOL aviaudioiFillBuffers(void)
{
    LONG            lRead;
	MMRESULT        mmResult;
    LONG            lSamplesToPlay;


    if (!sfPlaying)
		return TRUE;


    while (swBuffersOut < swBuffers)
    {
		if (slCurrent >= slEnd)
		{
		if (sfLooping)
		{

			slCurrent = slBegin;
		}
	    else
			break;
		}

	
	lSamplesToPlay = slEnd - slCurrent;
	if (lSamplesToPlay > AUDIO_BUFFER_SIZE / slSampleSize )
	    lSamplesToPlay = AUDIO_BUFFER_SIZE / slSampleSize ;

	//ErrMsg("slCurrent %ld, lSamplesToPlay %ld, toplay %ld",slCurrent, lSamplesToPlay, slEnd - slCurrent);

	long retval = AVIStreamRead(spavi, slCurrent, lSamplesToPlay,
		      salpAudioBuf[swNextBuffer]->lpData,
		      AUDIO_BUFFER_SIZE,
		      (long *)&salpAudioBuf[swNextBuffer]->dwBufferLength,
		      &lRead);

	//ErrMsg("slCurrent %ld, lSamplesToPlay %ld, toplay %ld",slCurrent, lSamplesToPlay, slEnd - slCurrent);

	
	//over here
	//This seems to be the condition related to the non-stopping at end of movie
	if ((lRead <= 0) && (lSamplesToPlay>0)) {
		
			
			//retry 

			
		
			retval = AVIStreamRead(spavi, slCurrent, lSamplesToPlay,
		      salpAudioBuf[swNextBuffer]->lpData,
		      AUDIO_BUFFER_SIZE,
		      (long *)&salpAudioBuf[swNextBuffer]->dwBufferLength,
		      &lRead);
			  

			if ((lRead <= 0) && (lSamplesToPlay>0)) {

			//if (retval == AVIERR_FILEREAD) {
			
				slCurrent += lSamplesToPlay;
				break;

			//}
			//else return FALSE;

			}

			
	}
	

	if (lRead != lSamplesToPlay) {
	 
		if (lRead == lSamplesToPlay-1)
		{
			//do nothing...allow it to pass on
		}
		else {
			return FALSE;

		}
	}
	

	slCurrent += lRead;
	
	mmResult = waveOutWrite(shWaveOut, salpAudioBuf[swNextBuffer],sizeof(WAVEHDR));
		
	if (mmResult != 0) {
	 
		//::MessageBox(NULL,"Waveoutwrite problem","note",MB_OK);	
		return FALSE;

	}
		
	++swBuffersOut;
	++swNextBuffer;
	if (swNextBuffer >= swBuffers)
	    swNextBuffer = 0;
    }//while


	if ((swBuffersOut == 0) && (slCurrent >= slEnd)) {
			aviaudioStop();

	}
	
    // All buffers Filled
    return TRUE;
}


// Play audio, starting at a given frame/sample
BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait)
{

	if (audioPlayable<=0) 
		return FALSE;

	recalc = 1;

	//CString tx;
	//tx.Format("audioPlayable %d",audioPlayable);
	//MessageBox(NULL,tx,"Note",MB_OK);
    
	CString msx;	
	
	if (lStart < 0)
		lStart = AVIStreamStart(pavi);

    if (lEnd < 0)
		lEnd = AVIStreamEnd(pavi);




    if (lStart >= lEnd) {
		
		return FALSE;
	}

    if (!aviaudioOpenDevice(hwnd, pavi)) {

		MessageBox(NULL,"AudioOpen failed","Note",MB_OK | MB_ICONEXCLAMATION);
		return FALSE;

	}

    if (!sfPlaying)
    {

		// We're beginning play, so pause until we've filled the buffers
		// for a seamless start		
		waveOutPause(shWaveOut);

			
		slBegin = lStart;
		slCurrent = lStart;
		slEnd = lEnd;
		sfPlaying = TRUE;

	
    }
    else
    {
		slEnd = lEnd;
    }


	

    aviaudioiFillBuffers();

	
    
    // Now unpause the audio and away it goes!    
    waveOutRestart(shWaveOut);

    
    // Caller wants us not to return until play is finished    
    if(fWait)
    {
		while (swBuffersOut > 0)
		Yield();
    }

    return TRUE;
}


void CALLBACK aviaudioMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	
    if (msg == MM_WOM_DONE) {
        --swBuffersOut;
        aviaudioiFillBuffers();
    }
	
}


// Stop playing, close the device.               
void CALLBACK aviaudioStop(void)
{
	MMRESULT        mmResult;

    if (shWaveOut != 0)
    {
		
		sfPlaying = FALSE;
		mmResult = waveOutReset(shWaveOut);		
	
		aviaudioCloseDevice();
    }
}
