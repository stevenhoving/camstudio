#include "StdAfx.h"
#include "ACM.h"
#include "../Profile.h"	// TODO: remove; for cAudioFormat

void SuggestCompressFormat()
{
	cAudioFormat.m_bCompression = TRUE;

	//AllocCompressFormat(cAudioFormat.m_dwCbwFX);
	//cAudioFormat.NewAudio();

	//1st try MPEGLAYER3
	cAudioFormat.BuildRecordingFormat();
	MMRESULT mmr = 0;
	if ((cAudioFormat.AudioFormat().nSamplesPerSec == 22050)
		&& (cAudioFormat.AudioFormat().nChannels == 2)
		&& (cAudioFormat.AudioFormat().wBitsPerSample <= 16)) {
		cAudioFormat.AudioFormat().wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = ::acmFormatSuggest(NULL, &(cAudioFormat.AudioFormat()), &(cAudioFormat.AudioFormat()), cAudioFormat.m_dwCbwFX, ACM_FORMATSUGGESTF_WFORMATTAG);
	}

	if (0 != mmr) {
		//ver 1.6, use PCM if MP3 not available

		//Then try ADPCM
		//BuildRecordingFormat();
		//pwfx->wFormatTag = WAVE_FORMAT_ADPCM;
		//MMRESULT mmr = ::acmFormatSuggest(NULL, &m_Format, pwfx, dwCbwFX, ACM_FORMATSUGGESTF_WFORMATTAG);
		//if (0 != mmr) {
			//Use the PCM as default
			cAudioFormat.BuildRecordingFormat();
			cAudioFormat.AudioFormat().wFormatTag = WAVE_FORMAT_PCM;
			mmr = ::acmFormatSuggest(NULL, &(cAudioFormat.AudioFormat()), &(cAudioFormat.AudioFormat()), cAudioFormat.m_dwCbwFX, ACM_FORMATSUGGESTF_WFORMATTAG);
			if (0 != mmr) {
				cAudioFormat.m_bCompression = FALSE;
			}
		//}
	}
}

void AttemptCompressFormat()
{
	cAudioFormat.m_bCompression = TRUE;
	cAudioFormat.BuildRecordingFormat();
	//Test Compatibility
	MMRESULT mmr = ::acmFormatSuggest(NULL, &(cAudioFormat.AudioFormat()), &(cAudioFormat.AudioFormat()), cAudioFormat.m_dwCbwFX, ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC | ACM_FORMATSUGGESTF_WBITSPERSAMPLE | ACM_FORMATSUGGESTF_WFORMATTAG);
	if (mmr != 0) {
		SuggestCompressFormat();
	}
}

//ver .12
void AttemptRecordingFormat()
{
	WAVEINCAPS wic;
	::ZeroMemory(&wic, sizeof(WAVEINCAPS));
	MMRESULT mmr = ::waveInGetDevCaps(cAudioFormat.m_uDeviceID, &wic, sizeof(wic));
	if (MMSYSERR_NOERROR != mmr) {
		// TODO: handle error code

		// reset wic.dwFormats to force SuggestRecordingFormat call
		wic.dwFormats = 0;
	}
	if (wic.dwFormats & cAudioFormat.m_dwWaveinSelected) {
		cAudioFormat.BuildRecordingFormat();
	} else {
		SuggestRecordingFormat();
	}
}

//Suggest Save/Compress Format to pwfx
void SuggestRecordingFormat()
{
	WAVEINCAPS wic;
	::ZeroMemory(&wic, sizeof(WAVEINCAPS));
	MMRESULT mmResult = ::waveInGetDevCaps(cAudioFormat.m_uDeviceID, &wic, sizeof(WAVEINCAPS));
	if (MMSYSERR_NOERROR != mmResult)
	{
		// report error
		TRACE("SuggestRecordingFormat: waveInGetDevCaps failed %d\n", mmResult);
		return;
	}

	//Ordered in preference of choice
	if ((wic.dwFormats) & WAVE_FORMAT_2S16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2S16;
	} else if ((wic.dwFormats) & WAVE_FORMAT_2M08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2M08;
	} else if ((wic.dwFormats) & WAVE_FORMAT_2S08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2S08;
	} else if ((wic.dwFormats) & WAVE_FORMAT_2M16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2M16;
	} else if ((wic.dwFormats) & WAVE_FORMAT_1M08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1M08;
	} else if ((wic.dwFormats) & WAVE_FORMAT_1M16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1M16;
	} else if ((wic.dwFormats) & WAVE_FORMAT_1S08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1S08;
	} else if ((wic.dwFormats) & WAVE_FORMAT_1S16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 11025;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_1S16;
	} else if ((wic.dwFormats) & WAVE_FORMAT_4M08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4M08;
	} else if ((wic.dwFormats) & WAVE_FORMAT_4M16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4M16;
	} else if ((wic.dwFormats) & WAVE_FORMAT_4S08) {
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4S08;
	} else if ((wic.dwFormats) & WAVE_FORMAT_4S16) {
		cAudioFormat.m_iBitsPerSample = 16;
		cAudioFormat.m_iNumChannels = 2;
		cAudioFormat.m_iSamplesPerSeconds = 44100;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_4S16;
	} else {
		//Arbitrarily choose one
		cAudioFormat.m_iBitsPerSample = 8;
		cAudioFormat.m_iNumChannels = 1;
		cAudioFormat.m_iSamplesPerSeconds = 22050;
		cAudioFormat.m_dwWaveinSelected = WAVE_FORMAT_2M08;
	}

	//Build Format
	cAudioFormat.BuildRecordingFormat();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CACM::CACM()
{
}

CACM::~CACM()
{
}

CACMFormat::CACMFormat()
{
}

CACMFormat::~CACMFormat()
{
}
