//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "resource.h"
#include "AutoSearchDialog.h"
#include "soundfile.h"
#include "Buffer.h"
#include "AudioMixer.h"
#include "CStudioLib.h"

#include <mmsystem.h>

extern void mciRecordOpen();
extern void mciRecordStart();
extern void mciRecordStop(CString strFile);
extern void mciRecordClose();
extern void mciSetWaveFormat(); //add before mcirecord

extern int asdCreated;

extern HWND hWndGlobal;

extern CSoundFile * pSoundFile;

/////////////////////////////////////////////////////////////////////////////
CAutoSearchDialog asd;
int asdCreated = FALSE;

//version 1.6
// =============== Capture waveout ===================
UINT m_nNumMixers = 0;

CAudioMixer cAudioMixer;

MIXERCAPS m_mxcaps;
MIXERCONTROLDETAILS_BOOLEAN *m_SelectArray = 0;
MIXERCONTROLDETAILS_BOOLEAN *m_SelectArrayInitialState = 0;
BOOL usingWaveout = FALSE;

CString m_strDstLineName;
CString m_strSelectControlName;
CString m_strMicName;
CString m_strVolumeControlName;
DWORD m_dwMinimum;
DWORD m_dwMaximum;
int m_dwVolumeControlID = -1;

//all thsee variables will be ready after WaveoutGetSelectControl()
DWORD m_dwControlType;		//MUX or MIXER ...we are searching for these
DWORD m_dwSelectControlID;	//the found controlID of the MUX/MIXER
DWORD m_dwMultipleItems;	//max source lines connected to the MUX/MIXER
DWORD m_dwIndex;

//int iNumberOfMixerDevices=0;
//int iSelectedMixer=0;
int manual_mode = 0;

double analyzeTotal=0;
double analyzeAggregate=0;
double analyzeCount=0;
void ErrMsg(char format[], ...);

int maximum_line=-1;
int second_maximum_line=-1;
double maximum_value=-1;
double second_maximum_value=-1;

//int iFeedbackLine = -1;
//int iFeedbackLineInfo = -1;
DWORD volume;

int storedID[100]; //assume there is less than 100 lines, this array is used for manual search

void FreeWaveoutResouces();

BOOL WaveoutUninitialize();
BOOL WaveoutInitialize();
BOOL WaveoutGetSelectControl(DWORD lineToSearch,CString namesearch,int feedback_skip_namesearch);
BOOL WaveoutGetSelectValue(LONG &lVal,DWORD dwIndex);
BOOL WaveoutSetSelectValue(LONG lVal,DWORD dwIndex,BOOL zero_others);
BOOL WaveoutSearchSrcLine(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch,int feedback_skip_namesearch);
BOOL WaveoutSetSelectArray(MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue);

BOOL restoreWave();
BOOL useWave(DWORD lineToSearch,CString namesearch,BOOL silence_mode,int feedback_skip_namesearch);
BOOL useWavein(BOOL silence_mode,int feedback_skip_namesearch);
BOOL useWaveout(BOOL silence_mode,int feedback_skip_namesearch);
BOOL configWaveOut();
BOOL configWaveOutManual();
BOOL ManualSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch);
BOOL AutomaticSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch);
BOOL useVolume(int operation,DWORD &dwVal,int silence_mode);
BOOL SafeUseWaveout();
BOOL SafeUseWaveoutOnLoad();
BOOL WaveoutInternalAdjustVolume(long lineID);

/////////////////////////////////////////////////////////////////////////////

//Ver 1.6
//Mixer Routines

//Note:
//We can use the following variables after calling WaveoutGetSelectControl()
//m_dwMultipleItems ...number of MUX/MIXER source lines
//m_dwIndex ... ranging from 0..to..m_dwMultipleItems, test each value to see with is the analogue feedback from speakers
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
	iFeedbackLine = -1;

	int orig_recordaudio = iRecordAudio;

	//Automatically Configure feedback line by simply selecting it
	useWaveout(FALSE,TRUE); //report errors, skip (1st Pass) name search

	//restore
	if (orig_recordaudio==1)
		useWavein(TRUE,FALSE); //set back to record from microphone

	if (asdCreated)
		asd.ShowWindow(SW_HIDE);

	return TRUE;
}

BOOL WaveoutUninitialize()
{
	BOOL bSucc = (cAudioMixer.isValid()) ? (MMSYSERR_NOERROR == cAudioMixer.Close()) : TRUE; 
	return bSucc;
}

BOOL WaveoutInitialize()
{
	// get the number of mixer devices present in the system
	m_nNumMixers = ::mixerGetNumDevs();

	if (cAudioMixer.isValid())
		cAudioMixer.Close();
	::ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));

	m_strDstLineName.Empty();
	m_strSelectControlName.Empty();
	m_strMicName.Empty();
	m_dwMultipleItems = 0;

	//#undef max // The max macro conflicts with the following function.
	// m_dwIndex = numeric_limits<DWORD>::max();
	m_dwIndex = 100000;

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (m_nNumMixers != 0) {
		if (MMSYSERR_NOERROR != cAudioMixer.Open(iSelectedMixer, (DWORD) hWndGlobal, NULL, MIXER_OBJECTF_MIXER | CALLBACK_WINDOW)) {
			OnError("WaveoutInitialize");
			return FALSE;
		}

		if (MMSYSERR_NOERROR != cAudioMixer.GetDevCaps(&m_mxcaps, sizeof(MIXERCAPS))) {
			OnError("WaveoutInitialize");
			return FALSE;
		}
	}

	return TRUE;
}

//The value return by this function is important
//it (is returned to the useWave function) and indicates whether a control and its source line is found
BOOL WaveoutGetSelectControl(DWORD lineToSearch, CString namesearch, int feedback_skip_namesearch)
{
	if (!cAudioMixer.isValid()) {
		TRACE("WaveoutGetSelectControl: NULL m_hMixer\n");
		return FALSE;
	}

	// get dwLineID
	MIXERLINE mxl;
	::ZeroMemory(&mxl, sizeof(mxl));
	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	MMRESULT mmResult = cAudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (MMSYSERR_NOERROR != mmResult) {
		OnError("WaveoutGetSelectControl: mixerGetLineInfo");
		return FALSE;
	}

	// get dwControlID
	m_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
	MIXERCONTROL mxc;
	::ZeroMemory(&mxc, sizeof(mxc));
	mxc.cbStruct = sizeof(mxc);

	MIXERLINECONTROLS mxlc;
	::ZeroMemory(&mxlc, sizeof(mxlc));
	mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mxlc.dwLineID = mxl.dwLineID;
	mxlc.dwControlType = m_dwControlType;
	mxlc.cControls = 1;
	mxlc.cbmxctrl = sizeof(MIXERCONTROL);
	mxlc.pamxctrl = &mxc;
	if (MMSYSERR_NOERROR != cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
		// no mixer, try MUX
		m_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
		//mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = m_dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		if (MMSYSERR_NOERROR != cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)) {
			OnError("WaveoutGetSelectControl: mixerGetLineControls");
			return FALSE;
		}
	}

	// store dwControlID, cMultipleItems
	m_strDstLineName = mxl.szName;
	m_strSelectControlName = mxc.szName;
	m_dwSelectControlID = mxc.dwControlID;
	m_dwMultipleItems = mxc.cMultipleItems;
	if (m_dwMultipleItems == 0) {
		TRACE("WaveoutGetSelectControl: m_dwMultipleItems == 0\n");
		return FALSE;
	}

	// get the index of the Select control
	MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText = new MIXERCONTROLDETAILS_LISTTEXT[m_dwMultipleItems];
	if (pmxcdSelectText != NULL) {
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = m_dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = m_dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
		mxcd.paDetails = pmxcdSelectText;
		if (MMSYSERR_NOERROR == cAudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT)) {
			//// determine which controls the speaker feedback source line
			//for (DWORD dwi = 0; dwi < m_dwMultipleItems; dwi++)
			//{
			// // get the line information
			// MIXERLINE mxl;
			// mxl.cbStruct = sizeof(MIXERLINE);
			// mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
			// if (::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer), &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR
			// && mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
			// // found, dwi is the index.
			// m_dwIndex = dwi;
			// m_strMicName = pmxcdSelectText[dwi].szName;
			// break;
			// }
			//}

			WaveoutSearchSrcLine(pmxcdSelectText,lineToSearch,namesearch,feedback_skip_namesearch);
		}

		delete []pmxcdSelectText;
	} else {
		OnError("WaveoutGetSelectControl: pmxcdSelectText");
	}

	return (m_dwIndex < m_dwMultipleItems);
}

BOOL WaveoutGetSelectValue(LONG &lVal,DWORD dwIndex)
{
	if (!cAudioMixer.isValid()
		|| m_dwMultipleItems == 0
		|| dwIndex >= m_dwMultipleItems)
		return FALSE;

	BOOL bRetVal = FALSE;

	MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[m_dwMultipleItems];
	if (pmxcdSelectValue != NULL) {
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = m_dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = m_dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mxcd.paDetails = pmxcdSelectValue;
		if (MMSYSERR_NOERROR == cAudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)) {
			lVal = pmxcdSelectValue[dwIndex].fValue;
			bRetVal = TRUE;
		}

		delete []pmxcdSelectValue;
	}

	return bRetVal;
}

BOOL WaveoutSetSelectValue(LONG lVal,DWORD dwIndex,BOOL zero_others)
{
	if (!cAudioMixer.isValid()
		|| m_dwMultipleItems == 0
		|| dwIndex >= m_dwMultipleItems)
		return FALSE;

	BOOL bRetVal = FALSE;

	// get all the values first
	MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[m_dwMultipleItems];
	if (pmxcdSelectValue != NULL) {
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = m_dwSelectControlID;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = m_dwMultipleItems;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mxcd.paDetails = pmxcdSelectValue;
		if (MMSYSERR_NOERROR == cAudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)) {
			ASSERT(m_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER
				|| m_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

			// MUX restricts the line selection to one source line at a time.
			if ((zero_others) || (lVal && m_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX))
				::ZeroMemory(pmxcdSelectValue, m_dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

			// set the Microphone value
			pmxcdSelectValue[dwIndex].fValue = lVal;

			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = m_dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = m_dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			if ( MMSYSERR_NOERROR == cAudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE))
				bRetVal = TRUE;
		}

		delete []pmxcdSelectValue;
	}

	return bRetVal;
}

BOOL WaveoutSetSelectArray(MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue)
{
	if (!cAudioMixer.isValid() || m_dwMultipleItems == 0 )
		return FALSE;

	BOOL bRetVal = FALSE;

	if (pmxcdSelectValue != NULL) {
		MIXERCONTROLDETAILS mxcd;
		{
			ASSERT(m_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER
				|| m_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = m_dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = m_dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			if (cAudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE)
				== MMSYSERR_NOERROR)
				bRetVal = TRUE;
		}

		//delete []pmxcdSelectValue;
	}

	return bRetVal;
}

BOOL finalRestoreMMMode()
{
	//Safety code
	if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0) || (mixerGetNumDevs() == 0)) {
		//Do not proceed with mixer code unless soundcard with mic/speaker is detected
		return FALSE;
	}

	//select the waveout as recording source
	if (WaveoutInitialize()) {
		// ***************************************
		// get the Control ID, index and the names
		// ***************************************
		if (!cAudioMixer.isValid())
			return FALSE;

		// get dwLineID
		MIXERLINE mxl;
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		MMRESULT mmResult = cAudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
		if (MMSYSERR_NOERROR != mmResult)
			return FALSE;

		// get dwControlID
		MIXERCONTROL mxc;
		MIXERLINECONTROLS mxlc;
		m_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = m_dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		if (cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR) {
			// no mixer, try MUX
			m_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
			mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = m_dwControlType;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = &mxc;
			if (cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
				return FALSE;
		}

		// store dwControlID, cMultipleItems
		m_strDstLineName = mxl.szName;
		m_strSelectControlName = mxc.szName;
		m_dwSelectControlID = mxc.dwControlID;
		m_dwMultipleItems = mxc.cMultipleItems;

		if (m_dwMultipleItems == 0)
			return FALSE;

		// *******************
		// Restore mixer array
		// *******************
		if (!cAudioMixer.isValid() || m_dwMultipleItems == 0 ) return FALSE;

		BOOL bRetVal = FALSE;

		// get all the values first
		MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = m_SelectArrayInitialState;
		if (pmxcdSelectValue != NULL) {
			MIXERCONTROLDETAILS mxcd;
			{
				ASSERT(m_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER
					|| m_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

				mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
				mxcd.dwControlID = m_dwSelectControlID;
				mxcd.cChannels = 1;
				mxcd.cMultipleItems = m_dwMultipleItems;
				mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
				mxcd.paDetails = pmxcdSelectValue;
				if (cAudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR)
					bRetVal = TRUE;
			}
		}

		WaveoutUninitialize();

		return bRetVal;
	} else {
		return FALSE;
	}

	return TRUE;
}

//this function should be used only inside oncreate
//it does not have restoreWave at the beginning to protect it from use when the mixer is already opened
BOOL initialSaveMMMode()
{
	//Safety code
	if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0) || (mixerGetNumDevs() == 0)) {
		//Do not proceed with mixer code unless soundcard with mic/speaker is detected
		TRACE("initialSaveMMMode: no devices!\n");
		return FALSE;
	}

	//select the waveout as recording source
	if (WaveoutInitialize()) {
		// ***************************************
		// get the Control ID, index and the names
		// ***************************************
		if (!cAudioMixer.isValid()) {
			TRACE("initialSaveMMMode: WaveoutInitialize failed\n");
			return FALSE;
		}

		// get dwLineID
		MIXERLINE mxl;
		::ZeroMemory(&mxl, sizeof(mxl));
		mxl.cbStruct = sizeof(MIXERLINE);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
		MMRESULT mmResult = cAudioMixer.GetLineInfo(&mxl, (MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE));
		if (MMSYSERR_NOERROR != mmResult) {
			OnError("initialSaveMMMode: mixerGetLineInfo");
			return FALSE;
		}

		// get dwControlID
		MIXERCONTROL mxc;
		MIXERLINECONTROLS mxlc;
		m_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
		mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mxlc.dwLineID = mxl.dwLineID;
		mxlc.dwControlType = m_dwControlType;
		mxlc.cControls = 1;
		mxlc.cbmxctrl = sizeof(MIXERCONTROL);
		mxlc.pamxctrl = &mxc;
		mmResult = cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (MMSYSERR_NOERROR != mmResult) {
			// no mixer, try MUX
			m_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
			mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = m_dwControlType;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = &mxc;
			mmResult = cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
			if (MMSYSERR_NOERROR != mmResult) {
				OnError("initialSaveMMMode: mixerGetLineControls");
				return FALSE;
			}
		}

		// store dwControlID, cMultipleItems
		m_strDstLineName = mxl.szName;
		m_strSelectControlName = mxc.szName;
		m_dwSelectControlID = mxc.dwControlID;
		m_dwMultipleItems = mxc.cMultipleItems;

		if (m_dwMultipleItems == 0)
			return FALSE;

		// ****************
		// Save mixer array
		// ****************
		if (!cAudioMixer.isValid() || m_dwMultipleItems == 0)
			return FALSE;

		BOOL bRetVal = FALSE;

		MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[m_dwMultipleItems];
		if (pmxcdSelectValue != NULL) {
			MIXERCONTROLDETAILS mxcd;
			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = m_dwSelectControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = m_dwMultipleItems;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
			mxcd.paDetails = pmxcdSelectValue;
			if (MMSYSERR_NOERROR == cAudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE)) {
				if (m_SelectArrayInitialState)
					delete []m_SelectArrayInitialState;

				m_SelectArrayInitialState=pmxcdSelectValue;
				bRetVal = TRUE;
			}
		}

		WaveoutUninitialize();

		return bRetVal;
	} else {
		return FALSE;
	}

	return TRUE;
}

//use this function only inside WaveoutGetSelectControl()
//passing MIXERLINE_COMPONENTTYPE_SRC_ANALOG to this function indicates not onl that we are only searching for this line
//but also, if we cannot find it, we should also try to find for MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT
BOOL WaveoutSearchSrcLine(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch,int feedback_skip_namesearch)
{
	BOOL retval=FALSE;

	//This code allows the manual config to override the 2 pass searching
	if (lineToSearch==MIXERLINE_COMPONENTTYPE_SRC_ANALOG) { //if searching for speakers line
		if (iFeedbackLine>=0) {
			m_dwIndex = iFeedbackLine;

			//not necessary because the validity of iFeedbackLine ==> iFeedbackLineInfo is also valid
			//iFeedbackLineInfo = pmxcdSelectText[m_dwIndex].dwParam1;

			return TRUE;
		}
	}

	//1st Pass Search (Name Search)
	if (!feedback_skip_namesearch) { //if skip the first pass
		// determine which line controls the speaker feedback source
		for (DWORD dwi = 0; dwi < m_dwMultipleItems; dwi++) {
			// get the line information
			MIXERLINE mxl;
			mxl.cbStruct = sizeof(MIXERLINE);
			mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
			MMRESULT mmResult = cAudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID);
			if (MMSYSERR_NOERROR == mmResult) {
				if ((mxl.dwComponentType == lineToSearch) || ((mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT) && (lineToSearch==MIXERLINE_COMPONENTTYPE_SRC_ANALOG))) {
					//if match
					//or if don't match, but we are searching for waveout, and that dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT

					m_strMicName = pmxcdSelectText[dwi].szName;

					if (namesearch!="") {
						if (m_strMicName==namesearch) {
							// found, dwi is the index.
							retval=TRUE;
							m_dwIndex = dwi;
							iFeedbackLine = m_dwIndex;
							iFeedbackLineInfo = pmxcdSelectText[m_dwIndex].dwParam1;
							//::MessageBox(NULL,m_strMicName,"Note",MB_OK |MB_ICONEXCLAMATION);
							break;
						}
					} else {
						retval=TRUE;
						m_dwIndex = dwi;
						break;
					}
				} //if coponent match

			} //if GetlineInfo succeeded

		} // for each items
	}

	//2nd Pass Search
	//if line not found, if recording from microphone ...do nothing
	//if line not found, if recording from speakers ...do a automatic search
	if (manual_mode)
		ManualSearch(pmxcdSelectText,lineToSearch,namesearch);
	else
		AutomaticSearch(pmxcdSelectText,lineToSearch,namesearch);

	return retval;
}

BOOL configWaveOutManual()
{
	//int retv = ::MessageBox(NULL,"Please note : This step is unecessary if the Auto Search function already managed to detect the line for recording speakers output. You should use this only if the Auto Search function fails. Proceed?","Note",MB_YESNO | MB_ICONQUESTION);
	int retv = MessageOut(NULL,IDS_STRING_UNNECESSARY ,IDS_STRING_NOTE,MB_YESNO | MB_ICONQUESTION);
	if (retv == IDNO)
		return FALSE;

	//set to undetected state to force detection
	iFeedbackLine = -1;

	int orig_recordaudio = iRecordAudio;

	manual_mode = 1;
	//Record the wave out for each line
	//This should not set any thing because
	useWaveout(TRUE,TRUE); //do not report errors, skip (1st Pass) name search
	manual_mode = 0;

	//CString anstr;
	//anstr.Format("You will now be asked several questions. A tone may or may not be heard after you click 'OK'. Please listen carefully before answering the questions.");
	//int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_OK | MB_ICONEXCLAMATION);
	int ret = MessageOut(hWndGlobal,IDS_STRING_ASKQUESTIONS ,IDS_STRING_ANALYZE,MB_OK | MB_ICONEXCLAMATION);

	for (DWORD dwi = 0; dwi < m_dwMultipleItems; dwi++) {
		//testfile=GetProgPath()+"\\testsnd.wav";
		//sndPlaySound(testfile, SND_SYNC);
		//anstr.Format("This is a sample tone. Did you hear a tone?");
		////int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_YESNO | MB_ICONQUESTION);
		//if (ret==IDNO) {
		// break;
		//}

		CString testfile("");
		testfile.Format("%s\testrec%d.wav", GetProgPath(), dwi);
		sndPlaySound(testfile, SND_SYNC);

		//anstr.Format("Testing line %d of %d. Did you hear a tone?",dwi+1,m_dwMultipleItems);
		//int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_YESNO | MB_ICONQUESTION);

		int ret = MessageOut(hWndGlobal,IDS_STRING_TESTINGLINE, IDS_STRING_ANALYZE, MB_YESNO | MB_ICONQUESTION, dwi+1,m_dwMultipleItems);
		if (ret == IDYES) {
			//anstr.Format("Line %d set for recording sound from speakers !",dwi+1);
			//int ret = ::MessageBox(hWndGlobal,anstr,"Analyzing",MB_OK | MB_ICONEXCLAMATION);
			int ret = MessageOut(hWndGlobal,IDS_STRING_SETTINGLINE, IDS_STRING_ANALYZE, MB_OK | MB_ICONEXCLAMATION, dwi+1);

			iFeedbackLine = dwi;
			iFeedbackLineInfo = storedID[iFeedbackLine]; //storedID s crated in the manual mode of useWaveout/useWave
			break;
		}
	}

	//Clean up
	for (int dwi = 0; dwi < m_dwMultipleItems; ++dwi) {
		CString testfile("");
		testfile.Format("%s\testrec%d.wav", GetProgPath(), dwi);

		DeleteFile(testfile);
	}

	if (iFeedbackLine == -1)
		MessageOut(hWndGlobal,IDS_STRING_NODETECT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

	//restore
	if (orig_recordaudio==1)
		useWavein(TRUE,FALSE); //set to record from microphone
	//else if (orig_recordaudio==2)
	// useWaveout(TRUE,FALSE); //set to record from speakers

	return TRUE;
}

BOOL ManualSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch)
{
	if (lineToSearch==MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
		//if searching for speakers line
		//ver 1.6

		if (!asdCreated) {
			asd.Create(IDD_AUTOSEARCH,NULL);
			asdCreated = 1;
		} else {
			//This line is needed to ensure the AutoSearchDialog (asd) is not shown before the searching proceeds
			//This can happen if the user forgets to close it after a previous search
			asd.ShowWindow(SW_HIDE);
		}

		if (m_dwIndex>m_dwMultipleItems) { //if still not found
			//Assume searching for MIXERLINE_COMPONENTTYPE_SRC_ANALOG means searching for speaker source line
			//int ret = ::MessageBox(NULL,"We will now proceed with the manual search for the line used for recording speakers output. Not all soundcards support this function. CamStudio will play several tones and then ask you a series of questions. Please turn on your speakers now. \n\nIt is strongly recommended that you detach your microphone from your soundcard, or at least make sure that there is no background noise around your microphone. When you are ready, click 'OK'.","Manual Search",MB_OK | MB_ICONEXCLAMATION | MB_OK);
			int ret = MessageOut(NULL,IDS_STRING_MANUALSEARCH ,IDS_STRING_MANUAL,MB_OK | MB_ICONEXCLAMATION);

			//if (ret==IDNO) return FALSE;

			asd.ShowWindow(SW_RESTORE);
			CString anstr;
			anstr.LoadString(IDS_STRING_HEARTONES);
			//anstr.Format("You will hear several tones while CamStudio is searching your system. Please wait.....");
			asd.SetVarText(anstr);
			asd.SetButtonEnable(FALSE);

			//analyze every source line
			for (DWORD dwi = 0; dwi < m_dwMultipleItems; dwi++) {
				storedID[dwi] = pmxcdSelectText[dwi].dwParam1;

				CString fmtstr;
				fmtstr.LoadString(IDS_STRING_RECLINE);

				CString anstr;
				anstr.Format(LPCTSTR(fmtstr), dwi+1, m_dwMultipleItems);
				asd.SetVarTextLine2(anstr);

				WaveoutSetSelectValue(TRUE,dwi,TRUE);
				WaveoutInternalAdjustVolume(pmxcdSelectText[dwi].dwParam1);

				// TODO: why different files? Record: testrecX.wav; Play: testsnd.wav ???
				//CString fnum;
				//fnum.Format("%d", dwi);
				CString testfile;
				//testfile = GetProgPath() + "\\testrec" + fnum + ".wav";
				testfile.Format("%s\\testrec%d.wav", GetProgPath(), dwi);

				// TODO: How is testfile used here?
				mciRecordOpen();
				mciSetWaveFormat();
				mciRecordStart();

				// TODO: Isn't testfile the file we want to play?
				CString soundpath = GetProgPath() + "\\testsnd.wav";
				sndPlaySound(soundpath, SND_SYNC);

				mciRecordStop(testfile);
				mciRecordClose();
			}

			asd.ShowWindow(SW_HIDE);
			//asd.SetButtonEnable(TRUE);
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
			MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue =new MIXERCONTROLDETAILS_BOOLEAN[m_dwMultipleItems];
			::ZeroMemory(pmxcdSelectValue, m_dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));
			pmxcdSelectValue[m_dwIndex].fValue = TRUE;
			WaveoutSetSelectArray(pmxcdSelectValue);
			delete []pmxcdSelectValue;
			//WaveoutSetSelectValue(TRUE,m_dwIndex);
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

//We do not perform autosearch on load
BOOL SafeUseWaveoutOnLoad()
{
	BOOL val = TRUE;

	if (iFeedbackLine>=0) //if iFeedbackLine already found
		useWaveout(TRUE,FALSE);
	else if (iFeedbackLine<0) {
		iRecordAudio=1;
		useWavein(TRUE,FALSE); //silence mode
		//MessageOut(NULL,IDS_STRING_NODETECTLINE,IDS_STING_NOTE,MB_OK | MB_ICONEXCLAMATION);
	}

	return val;
}

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
	if (cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
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
	if (MMSYSERR_NOERROR != cAudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
		return FALSE;

	long dwVal = mxcdVolume.dwValue;

	double fraction=0;
	long targetVal=0;
	float denom,numer;
	denom = (float) dwMaximum - dwMinimum;
	numer = (float) dwVal;

	float volumeThresholdPercent = 7.0;
	if (denom>0.00001) {
		fraction = (numer/denom)*100.0;
		if (fraction<volumeThresholdPercent) { //if volume less than 7% of maximum
			targetVal = (long) (volumeThresholdPercent/100.0 * (dwMaximum - dwMinimum)) + dwMinimum;

			MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { targetVal };
			MIXERCONTROLDETAILS mxcd;
			mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
			mxcd.dwControlID = dwVolumeControlID;
			mxcd.cChannels = 1;
			mxcd.cMultipleItems = 0;
			mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
			mxcd.paDetails = &mxcdVolume;
			if (cAudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
				return FALSE;
		}
	}

	return TRUE;
}

BOOL WaveoutGetVolumeControl()
{
	//We do not even know the iFeedbackLine, let alone its volume
	if (iFeedbackLine<0) {
		return FALSE;
	}

	if (!cAudioMixer.isValid())
		return FALSE;

	// get dwLineID
	MIXERLINE mxl;

	mxl.cbStruct = sizeof(MIXERLINE);
	mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	MMRESULT mmResult = cAudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (MMSYSERR_NOERROR != mmResult)
		return FALSE;

	//got the CD audio volume instead of Steroes mix???? for line 7, iFeedbackLine = 6, the source==6 ==> CD Audio
	MIXERLINE mxl2;
	mxl2.cbStruct = sizeof(MIXERLINE);
	mxl2.dwLineID = iFeedbackLineInfo;

	mmResult = cAudioMixer.GetLineInfo(&mxl2, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID);
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
	if (cAudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
		return FALSE;

	// store dwControlID
	m_strDstLineName = mxl.szName;
	m_strVolumeControlName = mxc.szName;
	m_dwMinimum = mxc.Bounds.dwMinimum;
	m_dwMaximum = mxc.Bounds.dwMaximum;
	m_dwVolumeControlID = mxc.dwControlID;

	return TRUE;
}

BOOL WaveoutGetVolume(DWORD &dwVal)
{
	if (!cAudioMixer.isValid())
		return FALSE;

	if (m_dwVolumeControlID == -1)
		return FALSE;

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (MMSYSERR_NOERROR != cAudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
		return FALSE;

	dwVal = mxcdVolume.dwValue;

	return TRUE;
}

BOOL WaveoutSetVolume(DWORD dwVal)
{
	if (!cAudioMixer.isValid()) {
		return FALSE;
	}

	if (m_dwVolumeControlID == -1)
		return FALSE;

	MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { dwVal };
	MIXERCONTROLDETAILS mxcd;
	mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mxcd.dwControlID = m_dwVolumeControlID;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxcd.paDetails = &mxcdVolume;
	if (cAudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
		return FALSE;

	return TRUE;
}

BOOL WaveoutVolumeInitialize()
{
	// get the number of mixer devices present in the system
	m_nNumMixers = ::mixerGetNumDevs();

	cAudioMixer.Close();
	::ZeroMemory(&m_mxcaps, sizeof(MIXERCAPS));

	m_strVolumeControlName.Empty();
	m_strDstLineName.Empty();

	// open the first mixer
	// A "mapper" for audio mixer devices does not currently exist.
	if (m_nNumMixers != 0) {
		if (MMSYSERR_NOERROR != cAudioMixer.Open(iSelectedMixer, (DWORD) hWndGlobal, NULL, MIXER_OBJECTF_MIXER | CALLBACK_WINDOW))
			return FALSE;

		if (MMSYSERR_NOERROR != cAudioMixer.GetDevCaps(&m_mxcaps, sizeof(MIXERCAPS)))
			return FALSE;
	}

	return TRUE;
}

BOOL WaveoutVolumeUninitialize()
{
	BOOL bSucc = cAudioMixer.isValid() && (MMSYSERR_NOERROR == cAudioMixer.Close());
	return bSucc;
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

void AnalyzeData(CBuffer* buffer,int wBitsPerSample)
{
	if (buffer==NULL)
		return;

	if (wBitsPerSample==8) {
		for (long i=0;i<(buffer->ByteLen);i++) {
			int value=(byte) *(buffer->ptr.b+i) - 128;
			analyzeTotal+=labs(value);
		}

		analyzeTotal/=buffer->ByteLen; //divide by the number of samples

		analyzeAggregate += analyzeTotal;
		analyzeCount +=1;
	} else if (wBitsPerSample==16) {
		for (long i=0;i<(buffer->ByteLen);i+=2) {
			long offset=i/2;
			int value=*(buffer->ptr.s+offset);
			analyzeTotal+=labs(value);
		}
		analyzeTotal/=((buffer->ByteLen)/2); //divide by the number of samples
		analyzeAggregate += analyzeTotal;
		analyzeCount +=1;
	}
}

BOOL AutomaticSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText,DWORD lineToSearch,CString namesearch)
{
	if (lineToSearch==MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
		//if searching for speakers line
		//ver 1.6
		double analyze_threshold =3.0;

		if (!asdCreated) {
			asd.Create(IDD_AUTOSEARCH,NULL);
			asdCreated = 1;
		} else {
			//This line is needed to ensure the AutoSearchDialog (asd) is not shown before the searching proceeds
			//This can happen if the user forgets to close it after a previous search
			asd.ShowWindow(SW_HIDE);
		}

		if (m_dwIndex>m_dwMultipleItems) { //if still not found
			//Assume searching for MIXERLINE_COMPONENTTYPE_SRC_ANALOG means searching for speaker source line
			//int ret = ::MessageBox(NULL,"Not all soundcards support the recording of sound from your speakers. CamStudio will attempt to find the appropriate line on your system. \n\nIt is strongly recommended that you detach your microphone from your soundcard, or at least make sure that there is no background noise around your microphone. When you are ready, click 'OK'.","Note",MB_OK | MB_ICONEXCLAMATION | MB_OK);
			int ret = MessageOut(NULL,IDS_STRING_NOTALL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			//if (ret==IDNO) return FALSE;

			asd.ShowWindow(SW_RESTORE);
			CString anstr;
			//anstr.Format("You will hear several tones while CamStudio is searching your system. Please wait.....");
			anstr.LoadString(IDS_STRING_HEARSEARCH);
			asd.SetVarText(anstr);
			asd.SetButtonEnable(FALSE);

			//analyze every source line

			maximum_line=-1;
			second_maximum_line=-1;
			maximum_value=-1;
			second_maximum_value=-1;
			for (DWORD dwi = 0; dwi < m_dwMultipleItems; dwi++) {
				CString anstr,fmtstr;
				fmtstr.LoadString(IDS_STRING_ANALINE);
				anstr.Format(LPCTSTR(fmtstr),dwi+1,m_dwMultipleItems);
				asd.SetVarTextLine2(anstr);
				//((CStatic *) GetDlgItem(asd.m_hWnd,IDC_TEXT1))->SetWindowText(anstr);

				WaveoutSetSelectValue(TRUE,dwi,TRUE);

				WaveoutInternalAdjustVolume(pmxcdSelectText[dwi].dwParam1);

				CString fnum;
				fnum.Format("%d",dwi);
				CString testfile;
				testfile=GetProgPath()+"\\testrec" + fnum + ".wav";

				mciRecordOpen();
				mciSetWaveFormat();
				mciRecordStart();

				CString soundpath=GetProgPath()+"\\testsnd.wav";
				sndPlaySound(soundpath, SND_SYNC);

				mciRecordStop(testfile);
				mciRecordClose();

				//Open file for Analysis
				CSoundFile *pFile = NULL;
				pFile = new CSoundFile(testfile);
				if (pFile->GetMode()==FILE_ERROR)
					MessageOut(NULL,IDS_STRING_ERRSND ,IDS_STRING_ANALYZE,MB_OK | MB_ICONEXCLAMATION);

				int BasicBufSize = 32768;

				if (pFile->BitsPerSample()==16)
					analyze_threshold=300.0;

				CBuffer* buf;

				analyzeTotal=0;
				analyzeAggregate=0;
				analyzeCount=0;

				while (buf = pFile->Read())
				{
					AnalyzeData(buf,pFile->BitsPerSample());
				}
				analyzeAggregate/=analyzeCount;

				if (analyzeAggregate>maximum_value) {
					second_maximum_value = maximum_value;
					second_maximum_line = maximum_line;

					maximum_value = analyzeAggregate;
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

					m_dwIndex = maximum_line;
					iFeedbackLine = m_dwIndex;
					iFeedbackLineInfo = pmxcdSelectText[m_dwIndex].dwParam1;

					CString anstr,fmtstr;
					fmtstr.LoadString(IDS_STRING_LINEDETECTED);

					//anstr.Format("CamStudio detected line %d for recording sound from speakers! \n[%.2f,%.2f]",maximum_line+1,ratio,maximum_value);
					anstr.Format(LPCTSTR(fmtstr),maximum_line+1);
					asd.SetVarTextLine2(anstr);
					//}
				}
			}

			if (m_dwIndex > m_dwMultipleItems) {
				CString anstr;
				//anstr.Format("CamStudio is unable to detected the line on your system for recording sound from your speakers. You may want to try manual configuration in Audio Options.");
				anstr.LoadString(IDS_STRING_AUTODETECTFAILS);
				asd.SetVarTextLine2(anstr);
			}
			asd.SetButtonEnable(TRUE);
		}
	}
	return TRUE;
}

void FreeWaveoutResouces()
{
	if (m_SelectArray) {
		delete []m_SelectArray;
		m_SelectArray=NULL;
	}

	if (m_SelectArrayInitialState) {
		delete []m_SelectArrayInitialState;
		m_SelectArrayInitialState=NULL;
	}
}
