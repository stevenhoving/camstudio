#pragma once

extern void BuildSpeakerRecordingFormat();
extern void SuggestSpeakerRecordingFormat(int i);
extern void mciRecordOpen(HWND hWndCallback);
extern void mciRecordStart();
extern void mciRecordStop(HWND hWndCallback, const CString& strFile);
extern void mciRecordClose();
extern void mciRecordPause(HWND hWndCallback, const CString& strFile);
extern void mciRecordResume(HWND hWndCallback, const CString& strFile);
extern void mciSetWaveFormat(); //add before mcirecord

extern int isMciRecordOpen;		// TODO: replace with cMCIDevice.IsOpen()
extern bool bAlreadyMCIPause;	// TODO: replace with cMCIDevice.IsPause()

extern WAVEFORMATEX m_FormatSpeaker;
extern int iAudioBitsPerSampleSpeaker;
extern int iAudioNumChannelsSpeaker;
extern int iAudioSamplesPerSecondsSpeaker;

class CMCIDevice
{
public:
	CMCIDevice();
	~CMCIDevice();

	bool IsOpen() const {return m_bOpen;}
	bool IsPause() const {return m_bPause;}

private:
	bool m_bOpen;
	bool m_bPause;
	MCI_OPEN_PARMS m_sOpenParams;
public:
	static void OnError(MCIERROR dwError);
	MCIERROR Open(const CWnd& rWnd);
	MCIERROR Close();
	MCIERROR Save(const CWnd& rWnd, const CString& strFile);
	MCIERROR Start();
	MCIERROR Stop();
	MCIERROR Stop(const CWnd& rWnd, const CString& strFile);
	MCIERROR Pause();
	MCIERROR Resume();
	MCIERROR Set(const WAVEFORMATEX& rWaveFormatEx);
};
extern CMCIDevice cMCIDevice;
