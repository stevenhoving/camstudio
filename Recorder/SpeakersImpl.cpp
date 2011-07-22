//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"

#include "AutoSearchDialog.h"
#include "soundfile.h"
#include "Buffer.h"
#include "AudioMixer.h"
#include "MCI.h"
#include "CStudioLib.h"
#include "resource.h"

#include <mmsystem.h>

extern HWND hWndGlobal;

extern CSoundFile * pSoundFile;

/////////////////////////////////////////////////////////////////////////////
void FreeWaveoutResouces();
BOOL useVolume(int operation,DWORD &dwVal,int silence_mode);
BOOL configWaveOutManual();
BOOL configWaveOut();
BOOL useWaveout(BOOL silence_mode,int feedback_skip_namesearch);
BOOL useWavein(BOOL silence_mode,int feedback_skip_namesearch);

BOOL WaveoutUninitialize();

namespace {	// annonymous

	BOOL WaveoutInitialize();
	BOOL WaveoutGetSelectControl(DWORD lineToSearch,CString namesearch,int feedback_skip_namesearch);
	BOOL WaveoutSetSelectValue(LONG lVal,DWORD dwIndex,BOOL zero_others);
	BOOL WaveoutSearchSrcLine(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch,int feedback_skip_namesearch);
	BOOL WaveoutSetSelectArray(MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue);
	BOOL WaveoutInternalAdjustVolume(long lineID);
	BOOL WaveoutVolumeUninitialize();
	BOOL WaveoutVolumeInitialize();
	BOOL WaveoutSetVolume(DWORD dwVal);
	BOOL WaveoutGetVolume(DWORD &dwVal);
	BOOL WaveoutGetVolumeControl();

	BOOL useWave(DWORD lineToSearch,CString namesearch,BOOL silence_mode,int feedback_skip_namesearch);
	BOOL ManualSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch);
	BOOL AutomaticSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch);
	BOOL SafeUseWaveoutOnLoad();

	CAutoSearchDlg SearchDlg;

	bool bSearchDlgCreated = false;

	CString strDstLineName;
	CString strSelectControlName;
	CString strMicName;
	CString strVolumeControlName;
	//DWORD m_dwMinimum = 0;
	//DWORD m_dwMaximum = 0;
	int dwVolumeControlID = -1;

	//all thsee variables will be ready after WaveoutGetSelectControl()
	DWORD dwControlType = 0;		// MUX or MIXER ...we are searching for these
	DWORD dwSelectControlID = 0;	// the found controlID of the MUX/MIXER
	DWORD dwMultipleItems = 0;		// max source lines connected to the MUX/MIXER
	DWORD dwIndex = 0;

	// assume there is less than 100 lines;
	// this array is used for manual search
	int storedID[100];

	//version 1.6
	// =============== Capture waveout ===================
	UINT uNumMixers = 0;

	CAudioMixer AudioMixer;

	MIXERCAPS sMixerCaps;
	MIXERCONTROLDETAILS_BOOLEAN *pmcdbSelectArray = 0;
	MIXERCONTROLDETAILS_BOOLEAN *pmcdbSelectArrayInitialState = 0;
	BOOL usingWaveout = FALSE;

	int manual_mode = 0;

	double dAnalyzeTotal = 0.0;
	double dAnalyzeAggregate = 0.0;
	double dAnalyzeCount = 0.0;

	int maximum_line = -1;
	int second_maximum_line = -1;
	double maximum_value = -1.0;
	double second_maximum_value = -1.0;

}	// namespace annonymous

/////////////////////////////////////////////////////////////////////////////

//Ver 1.6
//Mixer Routines

//Note:
//We can use the following variables after calling WaveoutGetSelectControl()
//dwMultipleItems ...number of MUX/MIXER source lines
//dwIndex ... ranging from 0..to..dwMultipleItems, test each value to see with is the analogue feedback from speakers
BOOL useWaveout(BOOL silence_mode,int feedback_skip_namesearch)
{
	return useWave(MIXERLINE_COMPONENTTYPE_SRC_ANALOG,"Stereo Mix",silence_mode,feedback_skip_namesearch);
}

BOOL useWavein(BOOL silence_mode,int feedback_skip_namesearch)
{
	return useWave(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE,"",silence_mode,feedback_skip_namesearch);
}

BOOL configWaveOut()
{
	//set to undetected state to force detection
	cAudioFormat.m_iFeedbackLine = -1;

	int orig_recordaudio = cAudioFormat.m_iRecordAudio;

	//Automatically Configure feedback line by simply selecting it
	useWaveout(FALSE,TRUE); //report errors, skip (1st Pass) name search

	//restore
	if (orig_recordaudio==1)
		useWavein(TRUE,FALSE); //set back to record from microphone

	if (bSearchDlgCreated)
		SearchDlg.ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL WaveoutUninitialize()
{
	BOOL bSucc = (AudioMixer.isValid()) ? (MMSYSERR_NOERROR == AudioMixer.Close()) : TRUE;
	return bSucc;
}

BOOL finalRestoreMMMode()
{
	BOOL bResult = (0 < waveInGetNumDevs())
		&& (0 < waveOutGetNumDevs())
		&& (0 < mixerGetNumDevs());

	//Safety code
	if (!bResult) {
		//Do not proceed with mixer code unless soundcard with mic/speaker is detected
		return bResult;
	}

	//select the waveout as recording source
	bResult = WaveoutInitialize();
	if (!bResult) {
		return bResult;
	}

	// ***************************************
	// get the Control ID, index and the names
	// ***************************************
	bResult = AudioMixer.isValid();
	if (!bResult)
		return bResult;

	// get dwLineID
	MIXERLINE mxl;
	::ZeroMemory(&mxl, sizeof(MIXERLINE));
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	MMRESULT mmResult = AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
	bResult = (MMSYSERR_NOERROR != mmResult);
	if (!bResult)
		return bResult;

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = dwControlType;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	bResult = (MMSYSERR_NOERROR != AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE));
	if (!bResult) {
		// no mixer, try MUX
		dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		bResult = (MMSYSERR_NOERROR != AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE));
		if (!bResult) {
			return bResult;
		}
	}

	// store dwControlID, cMultipleItems
	strDstLineName = mxl.szName;
	strSelectControlName = mxc.szName;
	dwSelectControlID = mxc.dwControlID;
	dwMultipleItems = mxc.cMultipleItems;
	bResult = (0 < dwMultipleItems);
	if (!bResult)
		return bResult;

	// *******************
	// Restore mixer array
	// *******************
	bResult = AudioMixer.isValid();	// Q: did this change since we checked above?
	if (!bResult)
		return bResult;

//	BOOL bRetVal = FALSE;

	// get all the values first
	MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = pmcdbSelectArrayInitialState;
	bResult = (NULL != pmxcdSelectValue);
	if (bResult) {
		MIXERCONTROLDETAILS mxcd;
		{
			ASSERT(dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER
				|| dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			bResult = (MMSYSERR_NOERROR == AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE));
		}
	}

	WaveoutUninitialize();

	return bResult;
}

// this function should be used only inside oncreate
// it does not have restoreWave at the beginning to protect it from use when
// the mixer is already opened
BOOL initialSaveMMMode()
{
	BOOL bResult = FALSE;
	//Safety code
	if (!::waveInGetNumDevs() || !::waveOutGetNumDevs() || !::mixerGetNumDevs()) {
		//Do not proceed with mixer code unless soundcard with mic/speaker is detected
		TRACE("initialSaveMMMode: no devices!\n");
		return bResult;
	}

	//select the waveout as recording source
	if (WaveoutInitialize()) {
		// ***************************************
		// get the Control ID, index and the names
		// ***************************************
		if (!AudioMixer.isValid()) {
			TRACE("initialSaveMMMode: WaveoutInitialize failed\n");
			return bResult;
		}

		// get dwLineID
		MIXERLINE mxl;
		::ZeroMemory(&mxl, sizeof(mxl));
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		MMRESULT mmResult = AudioMixer.GetLineInfo(&mxl, (MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE));
		if (MMSYSERR_NOERROR != mmResult) {
			::OnError("initialSaveMMMode: mixerGetLineInfo");
			return bResult;
		}

		// get dwControlID
		MIXERCONTROL mxc;
		MIXERLINECONTROLS mxlc;
		dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		mmResult = AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (MMSYSERR_NOERROR != mmResult) {
			// no mixer, try MUX
			dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
			mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = dwControlType;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = &mxc;
			mmResult = AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
			if (MMSYSERR_NOERROR != mmResult) {
				OnError("initialSaveMMMode: mixerGetLineControls");
				return bResult;
			}
		}

		// store dwControlID, cMultipleItems
		strDstLineName = mxl.szName;
		strSelectControlName = mxc.szName;
		dwSelectControlID = mxc.dwControlID;
		dwMultipleItems = mxc.cMultipleItems;

		if (dwMultipleItems == 0)
		{
			return bResult;
		}

		// ****************
		// Save mixer array
		// ****************
		if (!AudioMixer.isValid() || dwMultipleItems == 0)
		{
			return bResult;
		}

		MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[dwMultipleItems];
		if (pmxcdSelectValue != NULL) {
			MIXERCONTROLDETAILS mxcd;
			::ZeroMemory(&mxcd, sizeof(MIXERCONTROLDETAILS));
			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			mmResult = AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
			if (MMSYSERR_NOERROR == mmResult)
			{
				if (pmcdbSelectArrayInitialState)
				{
					delete []pmcdbSelectArrayInitialState;
				}

				pmcdbSelectArrayInitialState = pmxcdSelectValue;
				bResult = TRUE;
			}
		}

		WaveoutUninitialize();
	}

	return bResult;
}

BOOL configWaveOutManual()
{
	//int retv = ::MessageBox(NULL,"Please note : This step is unecessary if the Auto Search function already managed to detect the line for recording speakers output. You should use this only if the Auto Search function fails. Proceed?","Note",MB_YESNO | MB_ICONQUESTION);
	int retv = MessageOut(NULL,IDS_STRING_UNNECESSARY ,IDS_STRING_NOTE,MB_YESNO | MB_ICONQUESTION);
	if (retv == IDNO)
		return FALSE;

	//set to undetected state to force detection
	cAudioFormat.m_iFeedbackLine = -1;

	int orig_recordaudio = cAudioFormat.m_iRecordAudio;

	manual_mode = 1;
	//Record the wave out for each line
	//This should not set any thing because
	useWaveout(TRUE,TRUE); //do not report errors, skip (1st Pass) name search
	manual_mode = 0;

	//CString anstr;
	//anstr.Format("You will now be asked several questions. A tone may or may not be heard after you click 'OK'. Please listen carefully before answering the questions.");
	//int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_OK | MB_ICONEXCLAMATION);
	MessageOut(hWndGlobal,IDS_STRING_ASKQUESTIONS ,IDS_STRING_ANALYZE,MB_OK | MB_ICONEXCLAMATION);

	for (DWORD dwi = 0; dwi < dwMultipleItems; dwi++) {
		//testfile=GetProgPath()+"\\testsnd.wav";
		//sndPlaySound(testfile, SND_SYNC);
		//anstr.Format("This is a sample tone. Did you hear a tone?");
		////int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_YESNO | MB_ICONQUESTION);
		//if (ret==IDNO) {
		// break;
		//}

		CString testfile("");
		testfile.Format("%s\testrec%d.wav", GetMyVideoPath(), dwi);
		sndPlaySound(testfile, SND_SYNC);

		//anstr.Format("Testing line %d of %d. Did you hear a tone?",dwi+1,dwMultipleItems);
		//int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_YESNO | MB_ICONQUESTION);

		int ret = MessageOut(hWndGlobal,IDS_STRING_TESTINGLINE, IDS_STRING_ANALYZE, MB_YESNO | MB_ICONQUESTION, dwi+1,dwMultipleItems);
		if (ret == IDYES) {
			//anstr.Format("Line %d set for recording sound from speakers !",dwi+1);
			//int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(hWndGlobal,IDS_STRING_SETTINGLINE, IDS_STRING_ANALYZE, MB_OK | MB_ICONEXCLAMATION, dwi+1);

			cAudioFormat.m_iFeedbackLine = dwi;
			cAudioFormat.m_iFeedbackLineInfo = storedID[cAudioFormat.m_iFeedbackLine]; //storedID s crated in the manual mode of useWaveout/useWave
			break;
		}
	}

	//Clean up
	for (DWORD dwi = 0; dwi < dwMultipleItems; ++dwi) {
		CString testfile("");
		testfile.Format("%s\testrec%d.wav", GetMyVideoPath(), dwi);

		DeleteFile(testfile);
	}

	if (cAudioFormat.m_iFeedbackLine == -1)
		MessageOut(hWndGlobal,IDS_STRING_NODETECT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	//restore
	if (orig_recordaudio==1)
		useWavein(TRUE,FALSE); //set to record from microphone
	//else if (orig_recordaudio==2)
	// useWaveout(TRUE,FALSE); //set to record from speakers

	return TRUE;
}

BOOL onLoadSettings(int iRecordAudio)
{
	//Safety code
	if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0) || (mixerGetNumDevs() == 0)) {
		//Do not proceed with mixer code unless soundcard with mic/speaker is detected
		return FALSE;
	}

	if (iRecordAudio==1)
		useWavein(TRUE,FALSE); //silence mode
	else if (iRecordAudio==2) {

		//useWaveout(TRUE,FALSE); //silence mode
		SafeUseWaveoutOnLoad();
	}

	return TRUE;
}

#define SETVOLUME 0
#define GETVOLUME 1
#define GETVOLUMEINFO 2

BOOL useVolume(int operation,DWORD &dwVal,int silence_mode)
{
	if (WaveoutVolumeInitialize()) {
		// get the Control ID, index and the names
		if (WaveoutGetVolumeControl()) {
			if (operation == SETVOLUME) {
				if (WaveoutSetVolume(dwVal)) {
				} else {
					if (!silence_mode)
						//::MessageBox(NULL,"WaveoutSetVolume() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
						MessageOut(NULL,IDS_STRING_WSETVOL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

					return FALSE;
				}
			} else if (operation == GETVOLUME) {
				if (WaveoutGetVolume(dwVal)) {
				} else {
					if (!silence_mode)
						//::MessageBox(NULL,"WaveoutGetVolume() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
						MessageOut(NULL,IDS_STRING_WGETVOL,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

					return FALSE;
				}
			}
		} else {
			if (!silence_mode)
				//::MessageBox(NULL,"WaveoutGetVolumeControl() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRING_WGETVOLCTRL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

			return FALSE;
		}

		WaveoutVolumeUninitialize();
	} else {
		if (!silence_mode)
			//::MessageBox(NULL,"WaveoutVolumeInitialize() failed.","Note",MB_OK |MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_WVOLINIT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

void AnalyzeData(const CBuffer& buffer, int wBitsPerSample)
{
	//if (buffer == NULL)
	//	return;

	if (wBitsPerSample == 8) {
		for (long i = 0; i < (buffer.ByteLen); i++) {
			int value = (byte)(*(buffer.ptr.b + i)) - 128;
			dAnalyzeTotal += labs(value);
		}

		dAnalyzeTotal /= buffer.ByteLen; //divide by the number of samples

		dAnalyzeAggregate += dAnalyzeTotal;
		dAnalyzeCount += 1;
	} else if (wBitsPerSample == 16) {
		for (long i = 0; i < (buffer.ByteLen); i += 2) {
			long offset = i/2;
			int value = *(buffer.ptr.s + offset);
			dAnalyzeTotal += labs(value);
		}
		dAnalyzeTotal /= ((buffer.ByteLen)/2); //divide by the number of samples
		dAnalyzeAggregate += dAnalyzeTotal;
		dAnalyzeCount += 1;
	}
}

void FreeWaveoutResouces()
{
	if (pmcdbSelectArray) {
		delete []pmcdbSelectArray;
		pmcdbSelectArray=NULL;
	}

	if (pmcdbSelectArrayInitialState) {
		delete []pmcdbSelectArrayInitialState;
		pmcdbSelectArrayInitialState=NULL;
	}
}

namespace {	// annonymous
BOOL WaveoutInternalAdjustVolume(long lineID)
{
	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = lineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		return FALSE;

	// store dwControlID
	long dwMinimum = mxc.Bounds.dwMinimum;
	long dwMaximum = mxc.Bounds.dwMaximum;
	long dwVolumeControlID = mxc.dwControlID;

	if (dwVolumeControlID == -1)
		return FALSE;

	//Get Volume
	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (MMSYSERR_NOERROR != AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
		return FALSE;

	long dwVal = mxcdVolume.dwValue;

	double fraction = 0;
	long targetVal = 0;
	float denom = (float) dwMaximum - dwMinimum;
	float numer = (float) dwVal;
	float volumeThresholdPercent = 7.0;
	if (denom > 0.00001) {
		fraction = (numer/denom)*100.0;
		if (fraction<volumeThresholdPercent) { //if volume less than 7% of maximum
			targetVal = (long) (volumeThresholdPercent/100.0 * (dwMaximum - dwMinimum)) + dwMinimum;

			mxcdVolume.dwValue = targetVal;
			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwVolumeControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = 0;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
			mxcd.paDetails = &mxcdVolume;
			if (AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
				return FALSE;
		}
	}

	return TRUE;
}

//We do not perform autosearch on load
BOOL SafeUseWaveoutOnLoad()
{
	BOOL val = TRUE;

	if (cAudioFormat.m_iFeedbackLine >= 0) //if iFeedbackLine already found
		useWaveout(TRUE,FALSE);
	else if (cAudioFormat.m_iFeedbackLine < 0) {
		cAudioFormat.m_iRecordAudio = MICROPHONE;
		useWavein(TRUE,FALSE); //silence mode
		//MessageOut(NULL,IDS_STRING_NODETECTLINE,IDS_STING_NOTE,MB_OK | MB_ICONEXCLAMATION);
	}

	return val;
}

BOOL AutomaticSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch)
{
	if (lineToSearch==MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
		//if searching for speakers line
		//ver 1.6
		double analyze_threshold =3.0;

		if (!bSearchDlgCreated) {
			SearchDlg.Create(IDD_AUTOSEARCH,NULL);
			bSearchDlgCreated = true;
		} else {
			//This line is needed to ensure the AutoSearchDialog (SearchDlg) is not shown before the searching proceeds
			//This can happen if the user forgets to close it after a previous search
			SearchDlg.ShowWindow(SW_HIDE);
		}

		if (dwIndex>dwMultipleItems) { //if still not found
			//Assume searching for MIXERLINE_COMPONENTTYPE_SRC_ANALOG means searching for speaker source line
			//int ret = ::MessageBox(NULL,"Not all soundcards support the recording of sound from your speakers. CamStudio will attempt to find the appropriate line on your system. \n\nIt is strongly recommended that you detach your microphone from your soundcard, or at least make sure that there is no background noise around your microphone. When you are ready, click 'OK'.","Note",MB_OK | MB_ICONEXCLAMATION | MB_OK);
			MessageOut(NULL,IDS_STRING_NOTALL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			//if (ret==IDNO) return FALSE;

			SearchDlg.ShowWindow(SW_RESTORE);
			CString anstr;
			//anstr.Format("You will hear several tones while CamStudio is searching your system. Please wait.....");
			anstr.LoadString(IDS_STRING_HEARSEARCH);
			SearchDlg.SetVarText(anstr);
			SearchDlg.SetButtonEnable(FALSE);

			//analyze every source line

			maximum_line=-1;
			second_maximum_line=-1;
			maximum_value=-1;
			second_maximum_value=-1;
			for (DWORD dwi = 0; dwi < dwMultipleItems; dwi++) {
				CString anstr,fmtstr;
				fmtstr.LoadString(IDS_STRING_ANALINE);
				anstr.Format(LPCTSTR(fmtstr),dwi+1,dwMultipleItems);
				SearchDlg.SetVarTextLine2(anstr);
				//((CStatic *) GetDlgItem(SearchDlg.m_hWnd,IDC_TEXT1))->SetWindowText(anstr);

				WaveoutSetSelectValue(TRUE,dwi,TRUE);

				WaveoutInternalAdjustVolume(pmxcdSelectText[dwi].dwParam1);

				CString fnum;
				fnum.Format("%d",dwi);
				CString testfile;
				testfile=GetMyVideoPath()+"\\testrec" + fnum + ".wav";

				mciRecordOpen(hWndGlobal);
				mciSetWaveFormat();
				mciRecordStart();

				CString soundpath=GetMyVideoPath()+"\\testsnd.wav";
				sndPlaySound(soundpath, SND_SYNC);

				mciRecordStop(hWndGlobal, testfile);
				mciRecordClose();

				//Open file for Analysis
				CSoundFile *pFile = NULL;
				pFile = new CSoundFile(testfile);
				if (pFile->GetMode()==FILE_ERROR)
					MessageOut(NULL,IDS_STRING_ERRSND ,IDS_STRING_ANALYZE,MB_OK | MB_ICONEXCLAMATION);

//				int BasicBufSize = 32768;

				if (pFile->BitsPerSample()==16)
					analyze_threshold=300.0;

				dAnalyzeTotal = 0;
				dAnalyzeAggregate = 0;
				dAnalyzeCount = 0;
				CBuffer* buf = pFile->Read();
				while (buf) {
					AnalyzeData(*buf, pFile->BitsPerSample());
					buf = pFile->Read();
				}
				dAnalyzeAggregate /= dAnalyzeCount;

				if (dAnalyzeAggregate > maximum_value) {
					second_maximum_value = maximum_value;
					second_maximum_line = maximum_line;

					maximum_value = dAnalyzeAggregate;
					maximum_line = dwi;
				}

				delete pFile;
				pSoundFile = NULL;

				DeleteFile(testfile);
			}

			if ((second_maximum_value>0) && (maximum_value>0) && (maximum_value>second_maximum_value)) {
				if (second_maximum_value<0.00000001)
					second_maximum_value=0.00000001; //prevent denominator being 0

				double ratio;
				ratio = maximum_value/second_maximum_value;

				//The criteria (of line detection) assumes there is only one recording line and the rest of the lines return silence
				if (ratio>5.0) {
					//if (maximum_value>analyze_threshold) {

					dwIndex = maximum_line;
					cAudioFormat.m_iFeedbackLine = dwIndex;
					cAudioFormat.m_iFeedbackLineInfo = pmxcdSelectText[dwIndex].dwParam1;

					CString anstr,fmtstr;
					fmtstr.LoadString(IDS_STRING_LINEDETECTED);

					//anstr.Format("CamStudio detected line %d for recording sound from speakers! \n[%.2f,%.2f]",maximum_line+1,ratio,maximum_value);
					anstr.Format(LPCTSTR(fmtstr),maximum_line+1);
					SearchDlg.SetVarTextLine2(anstr);
					//}
				}
			}

			if (dwIndex > dwMultipleItems) {
				CString anstr;
				//anstr.Format("CamStudio is unable to detected the line on your system for recording sound from your speakers. You may want to try manual configuration in Audio Options.");
				anstr.LoadString(IDS_STRING_AUTODETECTFAILS);
				SearchDlg.SetVarTextLine2(anstr);
			}
			SearchDlg.SetButtonEnable(TRUE);
		}
	}
	return TRUE;
}

BOOL ManualSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch)
{
	if (lineToSearch==MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
		//if searching for speakers line
		//ver 1.6

		if (!bSearchDlgCreated) {
			SearchDlg.Create(IDD_AUTOSEARCH,NULL);
			bSearchDlgCreated = true;
		} else {
			//This line is needed to ensure the AutoSearchDialog (SearchDlg) is not shown before the searching proceeds
			//This can happen if the user forgets to close it after a previous search
			SearchDlg.ShowWindow(SW_HIDE);
		}

		if (dwIndex>dwMultipleItems) { //if still not found
			//Assume searching for MIXERLINE_COMPONENTTYPE_SRC_ANALOG means searching for speaker source line
			//int ret = ::MessageBox(NULL,"We will now proceed with the manual search for the line used for recording speakers output. Not all soundcards support this function. CamStudio will play several tones and then ask you a series of questions. Please turn on your speakers now. \n\nIt is strongly recommended that you detach your microphone from your soundcard, or at least make sure that there is no background noise around your microphone. When you are ready, click 'OK'.","Manual Search",MB_OK | MB_ICONEXCLAMATION | MB_OK);
			MessageOut(NULL,IDS_STRING_MANUALSEARCH ,IDS_STRING_MANUAL,MB_OK | MB_ICONEXCLAMATION);

			//if (ret==IDNO) return FALSE;

			SearchDlg.ShowWindow(SW_RESTORE);
			CString anstr;
			anstr.LoadString(IDS_STRING_HEARTONES);
			//anstr.Format("You will hear several tones while CamStudio is searching your system. Please wait.....");
			SearchDlg.SetVarText(anstr);
			SearchDlg.SetButtonEnable(FALSE);

			//analyze every source line
			for (DWORD dwi = 0; dwi < dwMultipleItems; dwi++) {
				storedID[dwi] = pmxcdSelectText[dwi].dwParam1;

				CString fmtstr;
				fmtstr.LoadString(IDS_STRING_RECLINE);

				CString anstr;
				anstr.Format(LPCTSTR(fmtstr), dwi+1, dwMultipleItems);
				SearchDlg.SetVarTextLine2(anstr);

				WaveoutSetSelectValue(TRUE,dwi,TRUE);
				WaveoutInternalAdjustVolume(pmxcdSelectText[dwi].dwParam1);

				// TODO: why different files? Record: testrecX.wav; Play: testsnd.wav ???
				//CString fnum;
				//fnum.Format("%d", dwi);
				CString testfile;
				//testfile = GetProgPath() + "\\testrec" + fnum + ".wav";
				testfile.Format("%s\\testrec%d.wav", GetMyVideoPath(), dwi);

				// TODO: How is testfile used here?
				mciRecordOpen(hWndGlobal);
				mciSetWaveFormat();
				mciRecordStart();

				// TODO: Isn't testfile the file we want to play?
				CString soundpath = GetMyVideoPath() + "\\testsnd.wav";
				sndPlaySound(soundpath, SND_SYNC);

				mciRecordStop(hWndGlobal, testfile);
				mciRecordClose();
			}

			SearchDlg.ShowWindow(SW_HIDE);
			//SearchDlg.SetButtonEnable(TRUE);
		}
	}

	return TRUE;
}

BOOL useWave(DWORD lineToSearch,CString namesearch,BOOL silence_mode,int feedback_skip_namesearch)
{
	//select the waveout as recording source
	if (WaveoutInitialize()) {
		// get the Control ID, index and the names
		if (WaveoutGetSelectControl(lineToSearch,namesearch,feedback_skip_namesearch)) {
			//if (WaveoutSaveSelectArray()) {
			MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue =new MIXERCONTROLDETAILS_BOOLEAN[dwMultipleItems];
			::ZeroMemory(pmxcdSelectValue, dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));
			pmxcdSelectValue[dwIndex].fValue = TRUE;
			WaveoutSetSelectArray(pmxcdSelectValue);
			delete []pmxcdSelectValue;
			//WaveoutSetSelectValue(TRUE,dwIndex);
			usingWaveout=TRUE;

			//}
			//else {
			// if (!silence_mode)
			// ::MessageBox(NULL,"WaveoutSaveSelectArray() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
			// return FALSE;
			//}
		} else {
			if (!silence_mode)
				//::MessageBox(NULL,"WaveoutGetSelectControl() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
				MessageOut(NULL,IDS_STRING_GETSELCTRL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}

		if (WaveoutUninitialize()) {
		} else {
			//::MessageBox(NULL,"WaveoutUnInitialize() failed.","Note",MB_OK |MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_UNINIT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return FALSE;
		}
	} else {
		if (!silence_mode)
			//::MessageBox(NULL,"WaveoutInitialize() failed.","Note",MB_OK |MB_ICONEXCLAMATION);
			MessageOut(NULL,IDS_STRING_INIT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

BOOL WaveoutSetSelectArray(MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue)
{
	if (!AudioMixer.isValid() || dwMultipleItems == 0 )
		return FALSE;

	BOOL bRetVal = FALSE;

	if (pmxcdSelectValue != NULL) {
		MIXERCONTROLDETAILS mxcd;
		{
			ASSERT(dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER
				|| dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			if (AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE)
				== MMSYSERR_NOERROR)
				bRetVal = TRUE;
		}

		//delete []pmxcdSelectValue;
	}

	return bRetVal;
}

// use this function only inside WaveoutGetSelectControl()
// passing MIXERLINE_COMPONENTTYPE_SRC_ANALOG to this function indicates not only that we are
// only searching for this line but also, if we cannot find it, we should also try to find for
// MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT
BOOL WaveoutSearchSrcLine(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch,int feedback_skip_namesearch)
{
	BOOL bResult = FALSE;

	// This code allows the manual config to override the 2 pass searching

	// if searching for speakers line
	if (lineToSearch == MIXERLINE_COMPONENTTYPE_SRC_ANALOG)
	{
		bResult = (0 <= cAudioFormat.m_iFeedbackLine);
		if (bResult)
		{
			dwIndex = cAudioFormat.m_iFeedbackLine;

			// not necessary because the validity of iFeedbackLine ==> iFeedbackLineInfo
			// is also valid
			// iFeedbackLineInfo = pmxcdSelectText[dwIndex].dwParam1;
			return bResult;
		}
	}

	// 1st Pass Search (Name Search)
	if (!feedback_skip_namesearch)	// if skip the first pass
	{
		// determine which line controls the speaker feedback source
		for (DWORD dwi = 0; !bResult && dwi < dwMultipleItems; dwi++)
		{
			// get the line information
			MIXERLINE mxl;
			mxl.cbStruct = sizeof(MIXERLINE);
			mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
			MMRESULT mmResult = AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID);
			if (MMSYSERR_NOERROR == mmResult)
			{
				if ((mxl.dwComponentType == lineToSearch)
					|| ((mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT)
					&& (lineToSearch == MIXERLINE_COMPONENTTYPE_SRC_ANALOG)))
				{
					// if match
					// or if don't match, but we are searching for waveout,
					// and that dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT

					strMicName = pmxcdSelectText[dwi].szName;

					if (namesearch != "")
					{
						if (strMicName == namesearch)
						{
							// found, dwi is the index.
							bResult = TRUE;
							dwIndex = dwi;
							cAudioFormat.m_iFeedbackLine = dwIndex;
							cAudioFormat.m_iFeedbackLineInfo = pmxcdSelectText[dwIndex].dwParam1;
							//::MessageBox(NULL,strMicName,"Note",MB_OK |MB_ICONEXCLAMATION);
						}
					} else {
						bResult = TRUE;
						dwIndex = dwi;
					}
				}
			}
		}
	}

	// 2nd Pass Search
	// if line not found, if recording from microphone ...do nothing
	// if line not found, if recording from speakers ...do a automatic search
	if (manual_mode)
		ManualSearch(pmxcdSelectText, lineToSearch, namesearch);
	else
		AutomaticSearch(pmxcdSelectText, lineToSearch, namesearch);

	return bResult;
}

BOOL WaveoutSetSelectValue(LONG lVal,DWORD dwIndex,BOOL zero_others)
{
	if (!AudioMixer.isValid()
		|| dwMultipleItems == 0
		|| dwIndex >= dwMultipleItems)
		return FALSE;

	BOOL bRetVal = FALSE;

	// get all the values first
	MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[dwMultipleItems];
	if (pmxcdSelectValue != NULL) {
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mxcd.paDetails = pmxcdSelectValue;
		if (MMSYSERR_NOERROR == AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)) {
			ASSERT(dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER
				|| dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

			// MUX restricts the line selection to one source line at a time.
			if ((zero_others) || (lVal && dwControlType == MIXERCONTROL_CONTROLTYPE_MUX))
				::ZeroMemory(pmxcdSelectValue, dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

			// set the Microphone value
			pmxcdSelectValue[dwIndex].fValue = lVal;

			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			if ( MMSYSERR_NOERROR == AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE))
				bRetVal = TRUE;
		}

		delete []pmxcdSelectValue;
	}

	return bRetVal;
}

//BOOL WaveoutGetSelectValue(LONG &lVal,DWORD dwIndex);
//BOOL WaveoutGetSelectValue(LONG &lVal,DWORD dwIndex)
//{
//	if (!AudioMixer.isValid()
//		|| dwMultipleItems == 0
//		|| dwIndex >= dwMultipleItems)
//		return FALSE;
//
//	BOOL bRetVal = FALSE;
//
//	MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[dwMultipleItems];
//	if (pmxcdSelectValue != NULL) {
//		MIXERCONTROLDETAILS mxcd;
//		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
//		mxcd.dwControlID = dwSelectControlID;
//		mxcd.cChannels = 1;
//		mxcd.cMultipleItems = dwMultipleItems;
//		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
//		mxcd.paDetails = pmxcdSelectValue;
//		if (MMSYSERR_NOERROR == AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)) {
//			lVal = pmxcdSelectValue[dwIndex].fValue;
//			bRetVal = TRUE;
//		}
//
//		delete []pmxcdSelectValue;
//	}
//
//	return bRetVal;
//}
//

//The value return by this function is important
//it (is returned to the useWave function) and indicates whether a control and its source line is found
BOOL WaveoutGetSelectControl(DWORD lineToSearch, CString namesearch, int feedback_skip_namesearch)
{
	if (!AudioMixer.isValid()) {
		TRACE("WaveoutGetSelectControl: NULL m_hMixer\n");
		return FALSE;
	}

	// get dwLineID
	MIXERLINE mxl;
	::ZeroMemory(&mxl, sizeof(mxl));
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	MMRESULT mmResult = AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (MMSYSERR_NOERROR != mmResult) {
		OnError("WaveoutGetSelectControl: mixerGetLineInfo");
		return FALSE;
	}

	// get dwControlID
	dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
	MIXERCONTROL mxc;
	::ZeroMemory(&mxc, sizeof(mxc));
	mxc.cbStruct = sizeof(mxc);

	MIXERLINECONTROLS mxlc;
	::ZeroMemory(&mxlc, sizeof(mxlc));
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = dwControlType;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (MMSYSERR_NOERROR != AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
		// no mixer, try MUX
		dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
		//mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		if (MMSYSERR_NOERROR != AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
			OnError("WaveoutGetSelectControl: mixerGetLineControls");
			return FALSE;
		}
	}

	// store dwControlID, cMultipleItems
	strDstLineName = mxl.szName;
	strSelectControlName = mxc.szName;
	dwSelectControlID = mxc.dwControlID;
	dwMultipleItems = mxc.cMultipleItems;
	if (dwMultipleItems == 0) {
		TRACE("WaveoutGetSelectControl: dwMultipleItems == 0\n");
		return FALSE;
	}

	// get the index of the Select control
	MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText = new MIXERCONTROLDETAILS_LISTTEXT[dwMultipleItems];
	if (pmxcdSelectText != NULL) {
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
		mxcd.paDetails = pmxcdSelectText;
		if (MMSYSERR_NOERROR == AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT)) {
			//// determine which controls the speaker feedback source line
			//for (DWORD dwi = 0; dwi < dwMultipleItems; dwi++)
			//{
			// // get the line information
			// MIXERLINE mxl;
			// mxl.cbStruct = sizeof(MIXERLINE);
			// mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
			// if (::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer), &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR
			// && mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
			// // found, dwi is the index.
			// dwIndex = dwi;
			// strMicName = pmxcdSelectText[dwi].szName;
			// break;
			// }
			//}

			WaveoutSearchSrcLine(pmxcdSelectText,lineToSearch,namesearch,feedback_skip_namesearch);
		}

		delete []pmxcdSelectText;
	} else {
		OnError("WaveoutGetSelectControl: pmxcdSelectText");
	}

	return (dwIndex < dwMultipleItems);
}

BOOL WaveoutInitialize()
{
	// get the number of mixer devices present in the system
	uNumMixers = ::mixerGetNumDevs();

	if (AudioMixer.isValid())
		AudioMixer.Close();

	::ZeroMemory(&sMixerCaps, sizeof(MIXERCAPS));
	strDstLineName.Empty();
	strSelectControlName.Empty();
	strMicName.Empty();
	dwMultipleItems = 0;

	//#undef max // The max macro conflicts with the following function.
	// dwIndex = numeric_limits<DWORD>::max();
	dwIndex = 100000;

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (uNumMixers != 0) {
		if (MMSYSERR_NOERROR != AudioMixer.Open(cAudioFormat.m_iSelectedMixer, (DWORD) hWndGlobal, NULL, MIXER_OBJECTF_MIXER | CALLBACK_WINDOW)) {
			OnError("WaveoutInitialize");
			return FALSE;
		}

		if (MMSYSERR_NOERROR != AudioMixer.GetDevCaps(&sMixerCaps, sizeof(MIXERCAPS))) {
			OnError("WaveoutInitialize");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL WaveoutVolumeUninitialize()
{
	BOOL bSucc = AudioMixer.isValid() && (MMSYSERR_NOERROR == AudioMixer.Close());
	return bSucc;
}

BOOL WaveoutVolumeInitialize()
{
	// get the number of mixer devices present in the system
	uNumMixers = ::mixerGetNumDevs();

	AudioMixer.Close();
	::ZeroMemory(&sMixerCaps, sizeof(MIXERCAPS));

	strVolumeControlName.Empty();
	strDstLineName.Empty();

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (uNumMixers != 0) {
		if (MMSYSERR_NOERROR != AudioMixer.Open(cAudioFormat.m_iSelectedMixer, (DWORD) hWndGlobal, NULL, MIXER_OBJECTF_MIXER | CALLBACK_WINDOW))
			return FALSE;

		if (MMSYSERR_NOERROR != AudioMixer.GetDevCaps(&sMixerCaps, sizeof(MIXERCAPS)))
			return FALSE;
	}

	return TRUE;
}

BOOL WaveoutSetVolume(DWORD dwVal)
{
	if (!AudioMixer.isValid()) {
		return FALSE;
	}

	if (dwVolumeControlID == -1)
		return FALSE;

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
		return FALSE;

	return TRUE;
}

BOOL WaveoutGetVolume(DWORD &dwVal)
{
	if (!AudioMixer.isValid())
		return FALSE;

	if (dwVolumeControlID == -1)
		return FALSE;

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (MMSYSERR_NOERROR != AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
		return FALSE;

	dwVal = mxcdVolume.dwValue;

	return TRUE;
}

BOOL WaveoutGetVolumeControl()
{
	//We do not even know the iFeedbackLine, let alone its volume
	if (cAudioFormat.m_iFeedbackLine < 0) {
		return FALSE;
	}

	if (!AudioMixer.isValid())
		return FALSE;

	// get dwLineID
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	MMRESULT mmResult = AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (MMSYSERR_NOERROR != mmResult)
		return FALSE;

	// got the CD audio volume instead of Stereo mix????
	// for line 7, iFeedbackLine = 6, the source==6 ==> CD Audio
	MIXERLINE mxl2;
	mxl2.cbStruct = sizeof(MIXERLINE);
	mxl2.dwLineID = cAudioFormat.m_iFeedbackLineInfo;
	mmResult = AudioMixer.GetLineInfo(&mxl2, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID);
	if (MMSYSERR_NOERROR != mmResult)
		return FALSE;

	// get dwControlID
	MIXERCONTROL mxc;
	MIXERLINECONTROLS mxlc;
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl2.dwLineID;
	mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		return FALSE;

	// store dwControlID
	strDstLineName = mxl.szName;
	strVolumeControlName = mxc.szName;
	//m_dwMinimum = mxc.Bounds.dwMinimum;
	//m_dwMaximum = mxc.Bounds.dwMaximum;
	dwVolumeControlID = mxc.dwControlID;

	return TRUE;
}

}	// namespace annonymous

