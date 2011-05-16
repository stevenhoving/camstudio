#include "StdAfx.h"
#include "Recorder.h"
#include "MCI.h"

const int MCI_MAXSTRINGLEN = 128;

int isMciRecordOpen = 0;		// TODO: replace with cMCIDevice.IsOpen().  Looks like that isMciRecordOpen is only assigned and never used anymore.

WAVEFORMATEX m_FormatSpeaker;
int iAudioBitsPerSampleSpeaker = 16;
int iAudioNumChannelsSpeaker = 2;
int iAudioSamplesPerSecondsSpeaker = 44100;

CMCIDevice cMCIDevice;

namespace {
//MCI_OPEN_PARMS mop;
//MCI_SAVE_PARMS msp;
}

//ver 1.6
//ver 1.6 Capture waveout
//MCI functions
void mciRecordOpen(HWND hWndCallback)
{
	//mop.dwCallback = reinterpret_cast<DWORD_PTR>(hWndCallback);
	//mop.lpstrDeviceType = reinterpret_cast<LPCTSTR>(MCI_DEVTYPE_WAVEFORM_AUDIO);
	//mop.lpstrElementName = TEXT("");
	//mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, reinterpret_cast<DWORD_PTR>(&mop));
	//isMciRecordOpen = 1;

	CWnd * pWnd = CWnd::FromHandle(hWndCallback);
	isMciRecordOpen = (0 == cMCIDevice.Open(*pWnd));
}
void mciRecordClose()
{
	//mciSendCommand(mop.wDeviceID, MCI_CLOSE, 0, 0);
	//isMciRecordOpen = 0;
	cMCIDevice.Close();
	isMciRecordOpen = cMCIDevice.IsOpen();
}
void mciRecordStart()
{
	//DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_RECORD, 0, 0);
	//if (dwReturn) {
	//	char buffer[300];
	//	mciGetErrorString(dwReturn, buffer, sizeof(buffer));
	//	MessageBox(NULL, buffer, "MCI_RECORD", MB_ICONEXCLAMATION | MB_OK);
	//}
	cMCIDevice.Start();
}
void mciRecordStop(HWND hWndCallback, const CString& strFile)
{
	//DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_STOP, MCI_WAIT, 0);
	//msp.dwCallback = (DWORD)hWndCallback;
	//msp.lpfilename = LPCTSTR(strFile);
	//dwReturn = mciSendCommand(mop.wDeviceID, MCI_SAVE, MCI_WAIT | MCI_SAVE_FILE, (DWORD)&msp);
	CWnd * pWnd = CWnd::FromHandle(hWndCallback);
	cMCIDevice.Stop(*pWnd, strFile);
}

void mciRecordPause(HWND /*hWndCallback*/, const CString& /*strFile*/)
{
	////can call this only in the same thread as the one opening the device?
	//if (isMciRecordOpen == 0)
	//	return;

	//msp.dwCallback = (DWORD)hWndCallback;
	//msp.lpfilename = (LPCTSTR)strFile;

	//DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_PAUSE, MCI_WAIT, 0);
	////if (dwReturn) {
	////char buffer[300];
	////mciGetErrorString(dwReturn, buffer, sizeof (buffer));
	////MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
	////}
	cMCIDevice.Pause();
}

void mciRecordResume(HWND /*hWndCallback*/, const CString& /*strFile*/)
{
	//can call this only in the same thread as the one opening the device?
	//msp.dwCallback = (DWORD)hWndCallback;
	//msp.lpfilename = strFile;

	//if (isMciRecordOpen == 0) {
	//	return;
	//}

	//DWORD dwReturn = mciSendCommand(mop.wDeviceID, MCI_RESUME, MCI_WAIT, 0);
	////if (dwReturn) {
	////char buffer[300];
	////	mciGetErrorString(dwReturn, buffer, sizeof (buffer));
	////	MessageBox( NULL, buffer, "MCI_RECORD",MB_ICONEXCLAMATION | MB_OK);
	////}
	cMCIDevice.Resume();
}

// The setting/suggesting of format for recording Speakers is deferred until recording starts
// Default is to use best settings avalable.
// TODO:
void mciSetWaveFormat()
{
	//char buffer[128];

	//// Suggest 10 formats
	//// TODO: wrap the mci api.
	//MCI_WAVE_SET_PARMS set_parms;
	//DWORD dwReturn = -1;
	//for (int i = 0; ((i < 10) && (dwReturn != 0)); i++)
	//{
	//	SuggestSpeakerRecordingFormat(i);
	//	BuildSpeakerRecordingFormat();

	//	// Set PCM format of recording.
	//	::ZeroMemory(&set_parms, sizeof(set_parms));
	//	set_parms.wFormatTag		= m_FormatSpeaker.wFormatTag;
	//	set_parms.wBitsPerSample	= m_FormatSpeaker.wBitsPerSample;
	//	set_parms.nChannels			= m_FormatSpeaker.nChannels;
	//	set_parms.nSamplesPerSec	= m_FormatSpeaker.nSamplesPerSec;
	//	set_parms.nAvgBytesPerSec	= m_FormatSpeaker.nAvgBytesPerSec;
	//	set_parms.nBlockAlign		= m_FormatSpeaker.nBlockAlign;

	//	dwReturn = mciSendCommand(mop.wDeviceID, MCI_SET,
	//		MCI_WAIT
	//		| MCI_WAVE_SET_FORMATTAG
	//		| MCI_WAVE_SET_BITSPERSAMPLE
	//		| MCI_WAVE_SET_CHANNELS
	//		| MCI_WAVE_SET_SAMPLESPERSEC
	//		| MCI_WAVE_SET_AVGBYTESPERSEC
	//		| MCI_WAVE_SET_BLOCKALIGN,
	//		(DWORD)(LPVOID)&set_parms);
	//}

	//if (dwReturn) {
	//	mciGetErrorString(dwReturn, buffer, sizeof(buffer));
	//	CString tstr;
	//	VERIFY(tstr.LoadString(IDS_STRING_MCIWAVE));
	//	::MessageBox(NULL, buffer, tstr,MB_ICONEXCLAMATION | MB_OK);
	//}

	// Suggest 10 formats
#pragma warning ( push )
#pragma warning ( disable : 4245 )
	DWORD dwReturn = -1;	// Cause C4245 because we want an unexpected value here  -1 = FFFF
#pragma warning ( pop )

	for (int i = 0; ((i < 10) && (dwReturn != 0)); i++) {
		SuggestSpeakerRecordingFormat(i);
		BuildSpeakerRecordingFormat();
		dwReturn = cMCIDevice.Set(m_FormatSpeaker);
	}
}

// TODO: Put these into a collection
void SuggestSpeakerRecordingFormat(int i)
{
	//Ordered in preference of choice
	switch (i)
	{
	case 0:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4S16;
			break;
		}
	case 1:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2S16;
			break;
		}
	case 2:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4S08;
			break;
		}
	case 3:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2S08;
			break;
		}
	case 4:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4M16;
			break;
		}
	case 5:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 44100;
			//waveinselected_Speaker = WAVE_FORMAT_4M08;
			break;
		}
	case 6:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2M16;
			break;
		}
	case 7:
		{
			iAudioBitsPerSampleSpeaker = 16;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 11025;
			//waveinselected_Speaker = WAVE_FORMAT_1S16;
			break;
		}
	case 8:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 22050;
			//waveinselected_Speaker = WAVE_FORMAT_2M08;
			break;
		}
	case 9:
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 2;
			iAudioSamplesPerSecondsSpeaker = 11025;
			//waveinselected_Speaker = WAVE_FORMAT_1S08;
			break;
		}
	default :
		{
			iAudioBitsPerSampleSpeaker = 8;
			iAudioNumChannelsSpeaker = 1;
			iAudioSamplesPerSecondsSpeaker = 11025;
			//waveinselected_Speaker = WAVE_FORMAT_1M08;
		}
	}
}

// copy speaker format settins from global variables.
// TOSO: encapsulate WAVEFORMATEX and provide a copy op and assignment op
// for a structure that holds the global variables.

void BuildSpeakerRecordingFormat()
{
	m_FormatSpeaker.wFormatTag		= WAVE_FORMAT_PCM;
	m_FormatSpeaker.wBitsPerSample	= static_cast<WORD>(iAudioBitsPerSampleSpeaker);
	m_FormatSpeaker.nSamplesPerSec	= iAudioSamplesPerSecondsSpeaker;
	m_FormatSpeaker.nChannels		= static_cast<WORD>(iAudioNumChannelsSpeaker);
	m_FormatSpeaker.nBlockAlign		= m_FormatSpeaker.nChannels * (m_FormatSpeaker.wBitsPerSample/8);
	m_FormatSpeaker.nAvgBytesPerSec	= m_FormatSpeaker.nSamplesPerSec * m_FormatSpeaker.nBlockAlign;
	m_FormatSpeaker.cbSize			= 0;
}

CMCIDevice::CMCIDevice()
: m_bOpen(false)
, m_bPause(false)
{
}

CMCIDevice::~CMCIDevice()
{
}

void CMCIDevice::OnError(MCIERROR dwError)
{
	if (0 == dwError)
		return;

	CString strErr(_T("Unknown"));
	TCHAR szErr[MCI_MAXSTRINGLEN + 1];
	if (::mciGetErrorString(dwError, szErr, sizeof(MCI_MAXSTRINGLEN))) {
		strErr = szErr;
	}
	TRACE(_T("CMCIDevice::OnError: (%u) : %s\n"), dwError, (LPCTSTR)strErr);
	//::MessageBox(0, szErr, "MCI_RECORD", MB_ICONEXCLAMATION | MB_OK);
}

MCIERROR CMCIDevice::Open(const CWnd& rWnd)
{
	TRACE(_T("CMCIDevice::Open\n"));
	MCIERROR dwResult = 0;
	if (m_bOpen) {
		VERIFY(0 == (dwResult = Close()));
		if (dwResult) {
			return dwResult;
		}
	}
	ASSERT(!IsOpen());
	m_sOpenParams.dwCallback = reinterpret_cast<DWORD_PTR>((HWND)rWnd);
	m_sOpenParams.lpstrDeviceType = reinterpret_cast<LPCTSTR>(MCI_DEVTYPE_WAVEFORM_AUDIO);
	m_sOpenParams.lpstrElementName = TEXT("");
	dwResult = ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, reinterpret_cast<DWORD_PTR>(&m_sOpenParams));
	OnError(dwResult);
	m_bOpen = (0 == dwResult);
	return dwResult;
}

MCIERROR CMCIDevice::Close()
{
	TRACE(_T("CMCIDevice::Close\n"));
	if (!m_bOpen) {
		return 0;
	}
	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_CLOSE, 0, 0);
	OnError(dwResult);
	m_bOpen = (0 != dwResult);
	if (!m_bOpen) {
		::ZeroMemory(&m_sOpenParams, sizeof(MCI_OPEN_PARMS));
	}
	return dwResult;
}
MCIERROR CMCIDevice::Save(const CWnd& rWnd, const CString& strFile)
{
	TRACE(_T("CMCIDevice::Save\n"));
	MCI_SAVE_PARMS sSaveParams;
	sSaveParams.dwCallback = reinterpret_cast<DWORD_PTR>((HWND)rWnd);
	sSaveParams.lpfilename = (LPCTSTR)strFile;
	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_SAVE, MCI_WAIT | MCI_SAVE_FILE, (DWORD)&sSaveParams);
	OnError(dwResult);
	return dwResult;
}
MCIERROR CMCIDevice::Start()
{
	TRACE(_T("CMCIDevice::Start\n"));
	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_RECORD, 0, 0);
	OnError(dwResult);
	return dwResult;
}

MCIERROR CMCIDevice::Stop()
{
	TRACE(_T("CMCIDevice::Stop\n"));
	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_STOP, MCI_WAIT, 0);
	OnError(dwResult);
	return dwResult;
}
// stop and save
MCIERROR CMCIDevice::Stop(const CWnd& rWnd, const CString& strFile)
{
	TRACE(_T("CMCIDevice::Stop + Save\n"));
	MCIERROR dwResult = Stop();
	return (0 == dwResult)
		? Save(rWnd, strFile)
		: dwResult;
}

MCIERROR CMCIDevice::Pause()
{
	TRACE(_T("CMCIDevice::Pause\n"));
	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_PAUSE, MCI_WAIT, 0);
	OnError(dwResult);
	return dwResult;
}

MCIERROR CMCIDevice::Resume()
{
	TRACE(_T("CMCIDevice::Resume\n"));
	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_RESUME, MCI_WAIT, 0);
	OnError(dwResult);
	return dwResult;
}

MCIERROR CMCIDevice::Set(const WAVEFORMATEX& rWaveFormatEx)
{
	TRACE(_T("CMCIDevice::Set\n"));
	MCI_WAVE_SET_PARMS sParams;
	// Set PCM format of recording.
	::ZeroMemory(&sParams, sizeof(MCI_WAVE_SET_PARMS));
	sParams.wFormatTag		= rWaveFormatEx.wFormatTag;
	sParams.wBitsPerSample	= rWaveFormatEx.wBitsPerSample;
	sParams.nChannels		= rWaveFormatEx.nChannels;
	sParams.nSamplesPerSec	= rWaveFormatEx.nSamplesPerSec;
	sParams.nAvgBytesPerSec	= rWaveFormatEx.nAvgBytesPerSec;
	sParams.nBlockAlign		= rWaveFormatEx.nBlockAlign;

	MCIERROR dwResult = ::mciSendCommand(m_sOpenParams.wDeviceID, MCI_SET,
		MCI_WAIT
		| MCI_WAVE_SET_FORMATTAG
		| MCI_WAVE_SET_BITSPERSAMPLE
		| MCI_WAVE_SET_CHANNELS
		| MCI_WAVE_SET_SAMPLESPERSEC
		| MCI_WAVE_SET_AVGBYTESPERSEC
		| MCI_WAVE_SET_BLOCKALIGN,
		(DWORD)(LPVOID)&sParams);
	OnError(dwResult);
	return dwResult;
}
