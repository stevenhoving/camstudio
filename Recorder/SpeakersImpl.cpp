#include "stdafx.h"
#include "Recorder.h"

#include "AutoSearchDialog.h"

#include <fisterlib/sound_file.h>
#include <fisterlib/buffer.h>

#include "AudioMixer.h"
#include "MCI.h"
#include <CamLib/CStudioLib.h>
#include "resource.h"

#include <mmsystem.h>

extern HWND g_hWndGlobal;

extern CSoundFile *g_pSoundFile;

/////////////////////////////////////////////////////////////////////////////
void FreeWaveoutResouces();
BOOL useVolume(int operation, DWORD &dwVal, int silence_mode);
BOOL configWaveOutManual();
BOOL configWaveOut();
BOOL useWaveout(BOOL silence_mode, int feedback_skip_namesearch);
BOOL useWavein(BOOL silence_mode, int feedback_skip_namesearch);

BOOL WaveoutUninitialize();

namespace
{ // anonymous

BOOL WaveoutInitialize();
BOOL WaveoutGetSelectControl(DWORD lineToSearch, CString namesearch, int feedback_skip_namesearch);
BOOL WaveoutSetSelectValue(LONG lVal, DWORD dwIndex, BOOL zero_others);
BOOL WaveoutSearchSrcLine(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText, DWORD lineToSearch, CString namesearch,
                          int feedback_skip_namesearch);
BOOL WaveoutSetSelectArray(MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue);
BOOL WaveoutInternalAdjustVolume(long lineID);
BOOL WaveoutVolumeUninitialize();
BOOL WaveoutVolumeInitialize();
BOOL WaveoutSetVolume(DWORD dwVal);
BOOL WaveoutGetVolume(DWORD &dwVal);
BOOL WaveoutGetVolumeControl();

BOOL useWave(DWORD lineToSearch, CString namesearch, BOOL silence_mode, int feedback_skip_namesearch);
BOOL ManualSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText, DWORD lineToSearch, CString namesearch);
BOOL AutomaticSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText, DWORD lineToSearch, CString namesearch);
BOOL SafeUseWaveoutOnLoad();

CAutoSearchDlg g_SearchDlg;

bool g_bSearchDlgCreated = false;

CString g_strDstLineName;
CString g_strSelectControlName;
CString g_strMicName;
CString g_strVolumeControlName;
// DWORD m_dwMinimum = 0;
// DWORD m_dwMaximum = 0;
int g_dwVolumeControlID = -1;

// all thsee variables will be ready after WaveoutGetSelectControl()
DWORD g_dwControlType = 0;     // MUX or MIXER ...we are searching for these
DWORD g_dwSelectControlID = 0; // the found controlID of the MUX/MIXER
DWORD g_dwMultipleItems = 0;   // max source lines connected to the MUX/MIXER
DWORD g_dwIndex = 0;

// assume there is less than 100 lines;
// this array is used for manual search
int g_storedID[100];

// version 1.6
// =============== Capture waveout ===================
UINT g_uNumMixers = 0;

CAudioMixer g_AudioMixer;

MIXERCAPS g_sMixerCaps;
MIXERCONTROLDETAILS_BOOLEAN *g_pmcdbSelectArray = 0;
MIXERCONTROLDETAILS_BOOLEAN *g_pmcdbSelectArrayInitialState = 0;
BOOL g_usingWaveout = FALSE;

int g_manual_mode = 0;

double g_dAnalyzeTotal = 0.0;
double g_dAnalyzeAggregate = 0.0;
double g_dAnalyzeCount = 0.0;

int g_maximum_line = -1;
int g_second_maximum_line = -1;
double g_maximum_value = -1.0;
double g_second_maximum_value = -1.0;

} // namespace

/////////////////////////////////////////////////////////////////////////////

// Ver 1.6
// Mixer Routines

// Note:
// We can use the following variables after calling WaveoutGetSelectControl()
// g_dwMultipleItems ...number of MUX/MIXER source lines
// g_dwIndex ... ranging from 0..to..g_dwMultipleItems, test each value to see with is the analogue feedback from speakers
BOOL useWaveout(BOOL silence_mode, int feedback_skip_namesearch)
{
    return useWave(MIXERLINE_COMPONENTTYPE_SRC_ANALOG, "Stereo Mix", silence_mode, feedback_skip_namesearch);
}

BOOL useWavein(BOOL silence_mode, int feedback_skip_namesearch)
{
    return useWave(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, "", silence_mode, feedback_skip_namesearch);
}

BOOL configWaveOut()
{
    // set to undetected state to force detection
    cAudioFormat.m_iFeedbackLine = -1;

    int orig_recordaudio = cAudioFormat.m_iRecordAudio;

    // Automatically Configure feedback line by simply selecting it
    useWaveout(FALSE, TRUE); // report errors, skip (1st Pass) name search

    // restore
    if (orig_recordaudio == 1)
        useWavein(TRUE, FALSE); // set back to record from microphone

    if (g_bSearchDlgCreated)
        g_SearchDlg.ShowWindow(SW_HIDE);

    return TRUE;
}

BOOL WaveoutUninitialize()
{
    BOOL bSucc = (g_AudioMixer.isValid()) ? (MMSYSERR_NOERROR == g_AudioMixer.Close()) : TRUE;
    return bSucc;
}

BOOL finalRestoreMMMode()
{
    BOOL bResult = (0 < waveInGetNumDevs()) && (0 < waveOutGetNumDevs()) && (0 < mixerGetNumDevs());

    // Safety code
    if (!bResult)
    {
        // Do not proceed with mixer code unless soundcard with mic/speaker is detected
        return bResult;
    }

    // select the waveout as recording source
    bResult = WaveoutInitialize();
    if (!bResult)
    {
        return bResult;
    }

    // ***************************************
    // get the Control ID, index and the names
    // ***************************************
    bResult = g_AudioMixer.isValid();
    if (!bResult)
        return bResult;

    // get dwLineID
    MIXERLINE mxl;
    ::ZeroMemory(&mxl, sizeof(MIXERLINE));
    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
    MMRESULT mmResult = g_AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
    bResult = (MMSYSERR_NOERROR != mmResult);
    if (!bResult)
        return bResult;

    // get dwControlID
    MIXERCONTROL mxc;
    MIXERLINECONTROLS mxlc;
    g_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mxlc.dwLineID = mxl.dwLineID;
    mxlc.dwControlType = g_dwControlType;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
    mxlc.pamxctrl = &mxc;
    bResult = (MMSYSERR_NOERROR !=
               g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE));
    if (!bResult)
    {
        // no mixer, try MUX
        g_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
        mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
        mxlc.dwLineID = mxl.dwLineID;
        mxlc.dwControlType = g_dwControlType;
        mxlc.cControls = 1;
        mxlc.cbmxctrl = sizeof(MIXERCONTROL);
        mxlc.pamxctrl = &mxc;
        bResult = (MMSYSERR_NOERROR !=
                   g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE));
        if (!bResult)
        {
            return bResult;
        }
    }

    // store dwControlID, cMultipleItems
    g_strDstLineName = mxl.szName;
    g_strSelectControlName = mxc.szName;
    g_dwSelectControlID = mxc.dwControlID;
    g_dwMultipleItems = mxc.cMultipleItems;
    bResult = (0 < g_dwMultipleItems);
    if (!bResult)
        return bResult;

    // *******************
    // Restore mixer array
    // *******************
    bResult = g_AudioMixer.isValid(); // Q: did this change since we checked above?
    if (!bResult)
        return bResult;

    //    BOOL bRetVal = FALSE;

    // get all the values first
    MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = g_pmcdbSelectArrayInitialState;
    bResult = (nullptr != pmxcdSelectValue);
    if (bResult)
    {
        MIXERCONTROLDETAILS mxcd;
        {
            ASSERT(g_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER || g_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

            mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
            mxcd.dwControlID = g_dwSelectControlID;
            mxcd.cChannels = 1;
            mxcd.cMultipleItems = g_dwMultipleItems;
            mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
            mxcd.paDetails = pmxcdSelectValue;
            bResult = (MMSYSERR_NOERROR ==
                       g_AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE));
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
    // Safety code
    if (!::waveInGetNumDevs() || !::waveOutGetNumDevs() || !::mixerGetNumDevs())
    {
        // Do not proceed with mixer code unless soundcard with mic/speaker is detected
        TRACE("initialSaveMMMode: no devices!\n");
        return bResult;
    }

    // select the waveout as recording source
    if (WaveoutInitialize())
    {
        // ***************************************
        // get the Control ID, index and the names
        // ***************************************
        if (!g_AudioMixer.isValid())
        {
            TRACE("initialSaveMMMode: WaveoutInitialize failed\n");
            return bResult;
        }

        // get dwLineID
        MIXERLINE mxl;
        ::ZeroMemory(&mxl, sizeof(mxl));
        mxl.cbStruct = sizeof(MIXERLINE);
        mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        MMRESULT mmResult = g_AudioMixer.GetLineInfo(&mxl, (MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE));
        if (mmResult != MMSYSERR_NOERROR)
        {
            ::OnError("initialSaveMMMode: mixerGetLineInfo");
            return bResult;
        }

        // get dwControlID
        MIXERCONTROL mxc;
        MIXERLINECONTROLS mxlc;
        g_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
        mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
        mxlc.dwLineID = mxl.dwLineID;
        mxlc.dwControlType = g_dwControlType;
        mxlc.cControls = 1;
        mxlc.cbmxctrl = sizeof(MIXERCONTROL);
        mxlc.pamxctrl = &mxc;
        mmResult = g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
        if (mmResult != MMSYSERR_NOERROR)
        {
            // no mixer, try MUX
            g_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
            mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
            mxlc.dwLineID = mxl.dwLineID;
            mxlc.dwControlType = g_dwControlType;
            mxlc.cControls = 1;
            mxlc.cbmxctrl = sizeof(MIXERCONTROL);
            mxlc.pamxctrl = &mxc;
            mmResult = g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
            if (mmResult != MMSYSERR_NOERROR)
            {
                OnError("initialSaveMMMode: mixerGetLineControls");
                return bResult;
            }
        }

        // store dwControlID, cMultipleItems
        g_strDstLineName = mxl.szName;
        g_strSelectControlName = mxc.szName;
        g_dwSelectControlID = mxc.dwControlID;
        g_dwMultipleItems = mxc.cMultipleItems;

        // ****************
        // Save mixer array
        // ****************
        if (!g_AudioMixer.isValid() || g_dwMultipleItems == 0)
        {
            return bResult;
        }

        MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[g_dwMultipleItems];

        MIXERCONTROLDETAILS mxcd;
        ::ZeroMemory(&mxcd, sizeof(MIXERCONTROLDETAILS));
        mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
        mxcd.dwControlID = g_dwSelectControlID;
        mxcd.cChannels = 1;
        mxcd.cMultipleItems = g_dwMultipleItems;
        mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        mxcd.paDetails = pmxcdSelectValue;

        mmResult = g_AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
        if (mmResult == MMSYSERR_NOERROR)
        {
            if (g_pmcdbSelectArrayInitialState)
            {
                delete[] g_pmcdbSelectArrayInitialState;
            }

            g_pmcdbSelectArrayInitialState = pmxcdSelectValue;
            bResult = TRUE;
        }

        WaveoutUninitialize();
    }

    return bResult;
}

BOOL configWaveOutManual()
{
    // int retv = ::MessageBox(nullptr,"Please note : This step is unecessary if the Auto Search function already managed
    // to detect the line for recording speakers output. You should use this only if the Auto Search function fails.
    // Proceed?","Note",MB_YESNO | MB_ICONQUESTION);
    int retv = MessageOut(nullptr, IDS_STRING_UNNECESSARY, IDS_STRING_NOTE, MB_YESNO | MB_ICONQUESTION);
    if (retv == IDNO)
    {
        return FALSE;
    }

    // set to undetected state to force detection
    cAudioFormat.m_iFeedbackLine = -1;

    int orig_recordaudio = cAudioFormat.m_iRecordAudio;

    g_manual_mode = 1;
    // Record the wave out for each line
    // This should not set any thing because
    useWaveout(TRUE, TRUE); // do not report errors, skip (1st Pass) name search
    g_manual_mode = 0;

    // CString anstr;
    // anstr.Format("You will now be asked several questions. A tone may or may not be heard after you click 'OK'.
    // Please listen carefully before answering the questions."); int ret =
    // ::MessageBox(g_hWndGlobal,anstr,"Analyzing",MB_OK | MB_ICONEXCLAMATION);
    MessageOut(g_hWndGlobal, IDS_STRING_ASKQUESTIONS, IDS_STRING_ANALYZE, MB_OK | MB_ICONEXCLAMATION);

    for (int dwi = 0; dwi < (int)g_dwMultipleItems; dwi++)
    {
        // testfile=GetProgPath()+"\\testsnd.wav";
        // sndPlaySound(testfile, SND_SYNC);
        // anstr.Format("This is a sample tone. Did you hear a tone?");
        ////int ret = ::MessageBox(g_hWndGlobal,anstr,"Analyzing",MB_YESNO | MB_ICONQUESTION);
        // if (ret==IDNO) {
        // break;
        //}

        CString testfile("");
        testfile.Format("%s\testrec%d.wav", GetMyVideoPath().GetString(), dwi);
        sndPlaySound(testfile, SND_SYNC);

        // anstr.Format("Testing line %d of %d. Did you hear a tone?",dwi+1,g_dwMultipleItems);
        // int ret = ::MessageBox(g_hWndGlobal,anstr,"Analyzing",MB_YESNO | MB_ICONQUESTION);

        int ret = MessageOut(g_hWndGlobal, IDS_STRING_TESTINGLINE, IDS_STRING_ANALYZE, MB_YESNO | MB_ICONQUESTION,
                             dwi + 1, (long)g_dwMultipleItems);
        if (ret == IDYES)
        {
            // anstr.Format("Line %d set for recording sound from speakers !",dwi+1);
            // int ret = ::MessageBox(g_hWndGlobal,anstr,"Analyzing",MB_OK | MB_ICONEXCLAMATION);
            MessageOut(g_hWndGlobal, IDS_STRING_SETTINGLINE, IDS_STRING_ANALYZE, MB_OK | MB_ICONEXCLAMATION, dwi + 1);

            cAudioFormat.m_iFeedbackLine = dwi;
            cAudioFormat.m_iFeedbackLineInfo =
                g_storedID[cAudioFormat.m_iFeedbackLine]; // storedID s crated in the manual mode of useWaveout/useWave
            break;
        }
    }

    // Clean up
    for (DWORD dwi = 0; dwi < g_dwMultipleItems; ++dwi)
    {
        CString testfile("");
        testfile.Format("%s\testrec%d.wav", GetMyVideoPath().GetString(), dwi);

        DeleteFile(testfile);
    }

    if (cAudioFormat.m_iFeedbackLine == -1)
    {
        MessageOut(g_hWndGlobal, IDS_STRING_NODETECT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
    }

    // restore
    if (orig_recordaudio == 1)
    {
        useWavein(TRUE, FALSE); // set to record from microphone
    }
    // else if (orig_recordaudio==2)
    // useWaveout(TRUE,FALSE); //set to record from speakers

    return TRUE;
}

BOOL onLoadSettings(int iRecordAudio)
{
    // Safety code
    if ((waveInGetNumDevs() == 0) || (waveOutGetNumDevs() == 0) || (mixerGetNumDevs() == 0))
    {
        // Do not proceed with mixer code unless soundcard with mic/speaker is detected
        return FALSE;
    }

    if (iRecordAudio == 1)
    {
        useWavein(TRUE, FALSE); // silence mode
    }
    else if (iRecordAudio == 2)
    {
        // useWaveout(TRUE,FALSE); //silence mode
        SafeUseWaveoutOnLoad();
    }

    return TRUE;
}

#define SETVOLUME 0
#define GETVOLUME 1
#define GETVOLUMEINFO 2

BOOL useVolume(int operation, DWORD &dwVal, int silence_mode)
{
    if (WaveoutVolumeInitialize())
    {
        // get the Control ID, index and the names
        if (WaveoutGetVolumeControl())
        {
            if (operation == SETVOLUME)
            {
                if (WaveoutSetVolume(dwVal))
                {
                }
                else
                {
                    if (!silence_mode)
                    {
                        //::MessageBox(nullptr,"WaveoutSetVolume() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
                        MessageOut(nullptr, IDS_STRING_WSETVOL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
                    }

                    return FALSE;
                }
            }
            else if (operation == GETVOLUME)
            {
                if (WaveoutGetVolume(dwVal))
                {
                }
                else
                {
                    if (!silence_mode)
                    {
                        //::MessageBox(nullptr,"WaveoutGetVolume() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
                        MessageOut(nullptr, IDS_STRING_WGETVOL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
                    }

                    return FALSE;
                }
            }
        }
        else
        {
            if (!silence_mode)
            {
                //::MessageBox(nullptr,"WaveoutGetVolumeControl() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
                MessageOut(nullptr, IDS_STRING_WGETVOLCTRL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            }

            return FALSE;
        }

        WaveoutVolumeUninitialize();
    }
    else
    {
        if (!silence_mode)
        {
            //::MessageBox(nullptr,"WaveoutVolumeInitialize() failed.","Note",MB_OK |MB_ICONEXCLAMATION);
            MessageOut(nullptr, IDS_STRING_WVOLINIT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        }
        return FALSE;
    }

    return TRUE;
}

void AnalyzeData(const CBuffer &buffer, int wBitsPerSample)
{
    // if (buffer == nullptr)
    //    return;

    if (wBitsPerSample == 8)
    {
        for (long i = 0; i < (buffer.ByteLen); i++)
        {
            int value = (byte)(*(buffer.ptr.b + i)) - 128;
            g_dAnalyzeTotal += labs(value);
        }

        g_dAnalyzeTotal /= buffer.ByteLen; // divide by the number of samples

        g_dAnalyzeAggregate += g_dAnalyzeTotal;
        g_dAnalyzeCount += 1;
    }
    else if (wBitsPerSample == 16)
    {
        for (long i = 0; i < (buffer.ByteLen); i += 2)
        {
            long offset = i / 2;
            int value = *(buffer.ptr.s + offset);
            g_dAnalyzeTotal += labs(value);
        }
        g_dAnalyzeTotal /= ((buffer.ByteLen) / 2); // divide by the number of samples
        g_dAnalyzeAggregate += g_dAnalyzeTotal;
        g_dAnalyzeCount += 1;
    }
}

void FreeWaveoutResouces()
{
    delete[] g_pmcdbSelectArray;
    g_pmcdbSelectArray = nullptr;

    delete[] g_pmcdbSelectArrayInitialState;
    g_pmcdbSelectArrayInitialState = nullptr;
}

namespace
{ // anonymous
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
    if (g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
        return FALSE;

    // store dwControlID
    long dwMinimum = mxc.Bounds.dwMinimum;
    long dwMaximum = mxc.Bounds.dwMaximum;
    long dwVolumeControlID = mxc.dwControlID;

    if (dwVolumeControlID == -1)
    {
        return FALSE;
    }

    // Get Volume
    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
    MIXERCONTROLDETAILS mxcd;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = dwVolumeControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;

    const auto mmr = g_AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
    if (mmr != MMSYSERR_NOERROR)
    {
        return FALSE;
    }

    float denom = (float)dwMaximum - dwMinimum;
    float numer = (float)mxcdVolume.dwValue;

    if (denom > 0.00001)
    {
        static const float volumeThresholdPercent = 7.0;

        double fraction = (numer / denom) * 100.0;
        if (fraction < volumeThresholdPercent)
        {
            // if volume less than 7% of maximum
            long targetVal = (long)(volumeThresholdPercent / 100.0 * (dwMaximum - dwMinimum)) + dwMinimum;

            MIXERCONTROLDETAILS_UNSIGNED mxcdVolumeTemp = {targetVal};
            MIXERCONTROLDETAILS mxcd_temp;
            mxcd_temp.cbStruct = sizeof(MIXERCONTROLDETAILS);
            mxcd_temp.dwControlID = dwVolumeControlID;
            mxcd_temp.cChannels = 1;
            mxcd_temp.cMultipleItems = 0;
            mxcd_temp.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
            mxcd_temp.paDetails = &mxcdVolumeTemp;
            if (g_AudioMixer.SetControlDetails(&mxcd_temp, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

// We do not perform auto search on load
BOOL SafeUseWaveoutOnLoad()
{
    BOOL val = TRUE;

    if (cAudioFormat.m_iFeedbackLine >= 0) // if iFeedbackLine already found
    {
        useWaveout(TRUE, FALSE);
    }
    else if (cAudioFormat.m_iFeedbackLine < 0)
    {
        cAudioFormat.m_iRecordAudio = MICROPHONE;
        useWavein(TRUE, FALSE); // silence mode
        // MessageOut(nullptr,IDS_STRING_NODETECTLINE,IDS_STING_NOTE,MB_OK | MB_ICONEXCLAMATION);
    }

    return val;
}

BOOL AutomaticSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText, DWORD lineToSearch, CString namesearch)
{
    if (lineToSearch == MIXERLINE_COMPONENTTYPE_SRC_ANALOG)
    {
        // if searching for speakers line
        // ver 1.6
        double analyze_threshold = 3.0;

        if (!g_bSearchDlgCreated)
        {
            g_SearchDlg.Create(IDD_AUTOSEARCH, nullptr);
            g_bSearchDlgCreated = true;
        }
        else
        {
            // This line is needed to ensure the AutoSearchDialog (g_SearchDlg) is not shown before the searching proceeds
            // This can happen if the user forgets to close it after a previous search
            g_SearchDlg.ShowWindow(SW_HIDE);
        }

        if (g_dwIndex > g_dwMultipleItems)
        { // if still not found
            // Assume searching for MIXERLINE_COMPONENTTYPE_SRC_ANALOG means searching for speaker source line
            // int ret = ::MessageBox(nullptr,"Not all soundcards support the recording of sound from your speakers.
            // CamStudio will attempt to find the appropriate line on your system. \n\nIt is strongly recommended that
            // you detach your microphone from your soundcard, or at least make sure that there is no background noise
            // around your microphone. When you are ready, click 'OK'.","Note",MB_OK | MB_ICONEXCLAMATION | MB_OK);
            MessageOut(nullptr, IDS_STRING_NOTALL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            // if (ret==IDNO) return FALSE;

            g_SearchDlg.ShowWindow(SW_RESTORE);
            CString anstr;
            // anstr.Format("You will hear several tones while CamStudio is searching your system. Please wait.....");
            anstr.LoadString(IDS_STRING_HEARSEARCH);
            g_SearchDlg.SetVarText(anstr);
            g_SearchDlg.SetButtonEnable(FALSE);

            // analyze every source line

            g_maximum_line = -1;
            g_second_maximum_line = -1;
            g_maximum_value = -1;
            g_second_maximum_value = -1;
            for (int dwi = 0; dwi < (int)g_dwMultipleItems; dwi++)
            {
                CString anstrx, fmtstrx;
                fmtstrx.LoadString(IDS_STRING_ANALINE);
                anstrx.Format(LPCTSTR(fmtstrx), dwi + 1, g_dwMultipleItems);
                g_SearchDlg.SetVarTextLine2(anstrx);
                //((CStatic *) GetDlgItem(g_SearchDlg.m_hWnd,IDC_TEXT1))->SetWindowText(anstr);

                WaveoutSetSelectValue(TRUE, dwi, TRUE);

                WaveoutInternalAdjustVolume(pmxcdSelectText[dwi].dwParam1);

                CString fnum;
                fnum.Format("%d", dwi);
                CString testfile;
                testfile = GetMyVideoPath() + "\\testrec" + fnum + ".wav";

                mciRecordOpen(g_hWndGlobal);
                mciSetWaveFormat();
                mciRecordStart();

                CString soundpath = GetMyVideoPath() + "\\testsnd.wav";
                sndPlaySound(soundpath, SND_SYNC);

                mciRecordStop(g_hWndGlobal, testfile);
                mciRecordClose();

                // Open file for Analysis
                auto pFile = new CSoundFile(testfile.GetString());
                if (pFile->GetMode() == FILE_ERROR)
                {
                    MessageOut(nullptr, IDS_STRING_ERRSND, IDS_STRING_ANALYZE, MB_OK | MB_ICONEXCLAMATION);
                }

                // int BasicBufSize = 32768;

                if (pFile->BitsPerSample() == 16)
                {
                    analyze_threshold = 300.0;
                }

                g_dAnalyzeTotal = 0;
                g_dAnalyzeAggregate = 0;
                g_dAnalyzeCount = 0;
                CBuffer *buf = pFile->Read();
                while (buf)
                {
                    AnalyzeData(*buf, pFile->BitsPerSample());
                    buf = pFile->Read();
                }
                g_dAnalyzeAggregate /= g_dAnalyzeCount;

                if (g_dAnalyzeAggregate > g_maximum_value)
                {
                    g_second_maximum_value = g_maximum_value;
                    g_second_maximum_line = g_maximum_line;

                    g_maximum_value = g_dAnalyzeAggregate;
                    g_maximum_line = dwi;
                }

                delete pFile;
                g_pSoundFile = nullptr;

                DeleteFile(testfile);
            }

            if ((g_second_maximum_value > 0) && (g_maximum_value > 0) && (g_maximum_value > g_second_maximum_value))
            {
                if (g_second_maximum_value < 0.00000001)
                {
                    g_second_maximum_value = 0.00000001; // prevent denominator being 0
                }

                double ratio;
                ratio = g_maximum_value / g_second_maximum_value;

                // The criteria (of line detection) assumes there is only one recording line and the rest of the lines
                // return silence
                if (ratio > 5.0)
                {
                    // if (g_maximum_value>analyze_threshold) {

                    g_dwIndex = g_maximum_line;
                    cAudioFormat.m_iFeedbackLine = g_dwIndex;
                    cAudioFormat.m_iFeedbackLineInfo = pmxcdSelectText[g_dwIndex].dwParam1;

                    CString anstrx, fmtstrx;
                    fmtstrx.LoadString(IDS_STRING_LINEDETECTED);

                    // anstr.Format("CamStudio detected line %d for recording sound from speakers!
                    // \n[%.2f,%.2f]",maximum_line+1,ratio,g_maximum_value);
                    anstrx.Format(LPCTSTR(fmtstrx), g_maximum_line + 1);
                    g_SearchDlg.SetVarTextLine2(anstrx);
                    //}
                }
            }

            if (g_dwIndex > g_dwMultipleItems)
            {
                CString anstrx;
                // anstr.Format("CamStudio is unable to detected the line on your system for recording sound from your
                // speakers. You may want to try manual configuration in Audio Options.");
                anstrx.LoadString(IDS_STRING_AUTODETECTFAILS);
                g_SearchDlg.SetVarTextLine2(anstrx);
            }
            g_SearchDlg.SetButtonEnable(TRUE);
        }
    }
    return TRUE;
}

BOOL ManualSearch(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText, DWORD lineToSearch, CString namesearch)
{
    if (lineToSearch == MIXERLINE_COMPONENTTYPE_SRC_ANALOG)
    {
        // if searching for speakers line
        // ver 1.6

        if (!g_bSearchDlgCreated)
        {
            g_SearchDlg.Create(IDD_AUTOSEARCH, nullptr);
            g_bSearchDlgCreated = true;
        }
        else
        {
            // This line is needed to ensure the AutoSearchDialog (g_SearchDlg) is not shown before the searching proceeds
            // This can happen if the user forgets to close it after a previous search
            g_SearchDlg.ShowWindow(SW_HIDE);
        }

        if (g_dwIndex > g_dwMultipleItems)
        { // if still not found
            // Assume searching for MIXERLINE_COMPONENTTYPE_SRC_ANALOG means searching for speaker source line
            // int ret = ::MessageBox(nullptr,"We will now proceed with the manual search for the line used for recording
            // speakers output. Not all soundcards support this function. CamStudio will play several tones and then ask
            // you a series of questions. Please turn on your speakers now. \n\nIt is strongly recommended that you
            // detach your microphone from your soundcard, or at least make sure that there is no background noise
            // around your microphone. When you are ready, click 'OK'.","Manual Search",MB_OK | MB_ICONEXCLAMATION |
            // MB_OK);
            MessageOut(nullptr, IDS_STRING_MANUALSEARCH, IDS_STRING_MANUAL, MB_OK | MB_ICONEXCLAMATION);

            // if (ret==IDNO) return FALSE;

            g_SearchDlg.ShowWindow(SW_RESTORE);
            CString anstr;
            anstr.LoadString(IDS_STRING_HEARTONES);
            // anstr.Format("You will hear several tones while CamStudio is searching your system. Please wait.....");
            g_SearchDlg.SetVarText(anstr);
            g_SearchDlg.SetButtonEnable(FALSE);

            // analyze every source line
            for (DWORD dwi = 0; dwi < g_dwMultipleItems; dwi++)
            {
                g_storedID[dwi] = pmxcdSelectText[dwi].dwParam1;

                CString fmtstr;
                fmtstr.LoadString(IDS_STRING_RECLINE);

                CString anstrx;
                anstrx.Format(LPCTSTR(fmtstr), dwi + 1, g_dwMultipleItems);
                g_SearchDlg.SetVarTextLine2(anstrx);

                WaveoutSetSelectValue(TRUE, dwi, TRUE);
                WaveoutInternalAdjustVolume(pmxcdSelectText[dwi].dwParam1);

                // TODO: why different files? Record: testrecX.wav; Play: testsnd.wav ???
                // CString fnum;
                // fnum.Format("%d", dwi);
                CString testfile;
                // testfile = GetProgPath() + "\\testrec" + fnum + ".wav";
                testfile.Format("%s\\testrec%d.wav", GetMyVideoPath().GetString(), dwi);

                // TODO: How is testfile used here?
                mciRecordOpen(g_hWndGlobal);
                mciSetWaveFormat();
                mciRecordStart();

                // TODO: Isn't testfile the file we want to play?
                CString soundpath = GetMyVideoPath() + "\\testsnd.wav";
                sndPlaySound(soundpath, SND_SYNC);

                mciRecordStop(g_hWndGlobal, testfile);
                mciRecordClose();
            }

            g_SearchDlg.ShowWindow(SW_HIDE);
            // SearchDlg.SetButtonEnable(TRUE);
        }
    }

    return TRUE;
}

BOOL useWave(DWORD lineToSearch, CString namesearch, BOOL silence_mode, int feedback_skip_namesearch)
{
    // select the waveout as recording source
    if (WaveoutInitialize())
    {
        // get the Control ID, index and the names
        if (WaveoutGetSelectControl(lineToSearch, namesearch, feedback_skip_namesearch))
        {
            // if (WaveoutSaveSelectArray()) {
            MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[g_dwMultipleItems];
            ::ZeroMemory(pmxcdSelectValue, g_dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));
            pmxcdSelectValue[g_dwIndex].fValue = TRUE;
            WaveoutSetSelectArray(pmxcdSelectValue);
            delete[] pmxcdSelectValue;
            // WaveoutSetSelectValue(TRUE,g_dwIndex);
            g_usingWaveout = TRUE;

            //}
            // else {
            // if (!silence_mode)
            // ::MessageBox(nullptr,"WaveoutSaveSelectArray() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
            // return FALSE;
            //}
        }
        else
        {
            if (!silence_mode)
                //::MessageBox(nullptr,"WaveoutGetSelectControl() failed.","Note",MB_OK | MB_ICONEXCLAMATION);
                MessageOut(nullptr, IDS_STRING_GETSELCTRL, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
        }

        if (WaveoutUninitialize())
        {
        }
        else
        {
            //::MessageBox(nullptr,"WaveoutUnInitialize() failed.","Note",MB_OK |MB_ICONEXCLAMATION);
            MessageOut(nullptr, IDS_STRING_UNINIT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
        }
    }
    else
    {
        if (!silence_mode)
            //::MessageBox(nullptr,"WaveoutInitialize() failed.","Note",MB_OK |MB_ICONEXCLAMATION);
            MessageOut(nullptr, IDS_STRING_INIT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }

    return TRUE;
}

BOOL WaveoutSetSelectArray(MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue)
{
    if (!g_AudioMixer.isValid() || g_dwMultipleItems == 0)
        return FALSE;

    BOOL bRetVal = FALSE;

    if (pmxcdSelectValue != nullptr)
    {
        MIXERCONTROLDETAILS mxcd;
        {
            ASSERT(g_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER || g_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

            mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
            mxcd.dwControlID = g_dwSelectControlID;
            mxcd.cChannels = 1;
            mxcd.cMultipleItems = g_dwMultipleItems;
            mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
            mxcd.paDetails = pmxcdSelectValue;
            if (g_AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) ==
                MMSYSERR_NOERROR)
                bRetVal = TRUE;
        }

        // delete []pmxcdSelectValue;
    }

    return bRetVal;
}

// use this function only inside WaveoutGetSelectControl()
// passing MIXERLINE_COMPONENTTYPE_SRC_ANALOG to this function indicates not only that we are
// only searching for this line but also, if we cannot find it, we should also try to find for
// MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT
BOOL WaveoutSearchSrcLine(MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText, DWORD lineToSearch, CString namesearch,
                          int feedback_skip_namesearch)
{
    BOOL bResult = FALSE;

    // This code allows the manual config to override the 2 pass searching

    // if searching for speakers line
    if (lineToSearch == MIXERLINE_COMPONENTTYPE_SRC_ANALOG)
    {
        bResult = (0 <= cAudioFormat.m_iFeedbackLine);
        if (bResult)
        {
            g_dwIndex = cAudioFormat.m_iFeedbackLine;

            // not necessary because the validity of iFeedbackLine ==> iFeedbackLineInfo
            // is also valid
            // iFeedbackLineInfo = pmxcdSelectText[g_dwIndex].dwParam1;
            return bResult;
        }
    }

    // 1st Pass Search (Name Search)
    if (!feedback_skip_namesearch) // if skip the first pass
    {
        // determine which line controls the speaker feedback source
        for (DWORD dwi = 0; !bResult && dwi < g_dwMultipleItems; dwi++)
        {
            // get the line information
            MIXERLINE mxl;
            mxl.cbStruct = sizeof(MIXERLINE);
            mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
            MMRESULT mmResult = g_AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID);
            if (MMSYSERR_NOERROR == mmResult)
            {
                if ((mxl.dwComponentType == lineToSearch) ||
                    ((mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT) &&
                     (lineToSearch == MIXERLINE_COMPONENTTYPE_SRC_ANALOG)))
                {
                    // if match
                    // or if don't match, but we are searching for waveout,
                    // and that dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT

                    g_strMicName = pmxcdSelectText[dwi].szName;

                    if (namesearch != "")
                    {
                        if (g_strMicName == namesearch)
                        {
                            // found, dwi is the index.
                            bResult = TRUE;
                            g_dwIndex = dwi;
                            cAudioFormat.m_iFeedbackLine = g_dwIndex;
                            cAudioFormat.m_iFeedbackLineInfo = pmxcdSelectText[g_dwIndex].dwParam1;
                            //::MessageBox(nullptr,g_strMicName,"Note",MB_OK |MB_ICONEXCLAMATION);
                        }
                    }
                    else
                    {
                        bResult = TRUE;
                        g_dwIndex = dwi;
                    }
                }
            }
        }
    }

    // 2nd Pass Search
    // if line not found, if recording from microphone ...do nothing
    // if line not found, if recording from speakers ...do a automatic search
    if (g_manual_mode)
        ManualSearch(pmxcdSelectText, lineToSearch, namesearch);
    else
        AutomaticSearch(pmxcdSelectText, lineToSearch, namesearch);

    return bResult;
}

BOOL WaveoutSetSelectValue(LONG lVal, DWORD dwIndex, BOOL zero_others)
{
    if (!g_AudioMixer.isValid() || g_dwMultipleItems == 0 || dwIndex >= g_dwMultipleItems)
        return FALSE;

    BOOL bRetVal = FALSE;

    // get all the values first
    MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[g_dwMultipleItems];
    if (pmxcdSelectValue != nullptr)
    {
        MIXERCONTROLDETAILS mxcd;
        mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
        mxcd.dwControlID = g_dwSelectControlID;
        mxcd.cChannels = 1;
        mxcd.cMultipleItems = g_dwMultipleItems;
        mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        mxcd.paDetails = pmxcdSelectValue;
        if (MMSYSERR_NOERROR ==
            g_AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
        {
            ASSERT(g_dwControlType == MIXERCONTROL_CONTROLTYPE_MIXER || g_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX);

            // MUX restricts the line selection to one source line at a time.
            if ((zero_others) || (lVal && g_dwControlType == MIXERCONTROL_CONTROLTYPE_MUX))
                ::ZeroMemory(pmxcdSelectValue, g_dwMultipleItems * sizeof(MIXERCONTROLDETAILS_BOOLEAN));

            // set the Microphone value
            pmxcdSelectValue[dwIndex].fValue = lVal;

            mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
            mxcd.dwControlID = g_dwSelectControlID;
            mxcd.cChannels = 1;
            mxcd.cMultipleItems = g_dwMultipleItems;
            mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
            mxcd.paDetails = pmxcdSelectValue;
            if (MMSYSERR_NOERROR ==
                g_AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE))
                bRetVal = TRUE;
        }

        delete[] pmxcdSelectValue;
    }

    return bRetVal;
}

// BOOL WaveoutGetSelectValue(LONG &lVal,DWORD g_dwIndex);
// BOOL WaveoutGetSelectValue(LONG &lVal,DWORD g_dwIndex)
//{
//    if (!g_AudioMixer.isValid()
//        || g_dwMultipleItems == 0
//        || g_dwIndex >= g_dwMultipleItems)
//        return FALSE;
//
//    BOOL bRetVal = FALSE;
//
//    MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[g_dwMultipleItems];
//    if (pmxcdSelectValue != nullptr) {
//        MIXERCONTROLDETAILS mxcd;
//        mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
//        mxcd.dwControlID = g_dwSelectControlID;
//        mxcd.cChannels = 1;
//        mxcd.cMultipleItems = g_dwMultipleItems;
//        mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
//        mxcd.paDetails = pmxcdSelectValue;
//        if (MMSYSERR_NOERROR == g_AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER |
//        MIXER_GETCONTROLDETAILSF_VALUE)) {
//            lVal = pmxcdSelectValue[g_dwIndex].fValue;
//            bRetVal = TRUE;
//        }
//
//        delete []pmxcdSelectValue;
//    }
//
//    return bRetVal;
//}
//

// The value return by this function is important
// it (is returned to the useWave function) and indicates whether a control and its source line is found
BOOL WaveoutGetSelectControl(DWORD lineToSearch, CString namesearch, int feedback_skip_namesearch)
{
    if (!g_AudioMixer.isValid())
    {
        TRACE("WaveoutGetSelectControl: nullptr m_hMixer\n");
        return FALSE;
    }

    // get dwLineID
    MIXERLINE mxl;
    ::ZeroMemory(&mxl, sizeof(mxl));
    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
    MMRESULT mmResult = g_AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (MMSYSERR_NOERROR != mmResult)
    {
        OnError("WaveoutGetSelectControl: mixerGetLineInfo");
        return FALSE;
    }

    // get dwControlID
    g_dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER;
    MIXERCONTROL mxc;
    ::ZeroMemory(&mxc, sizeof(mxc));
    mxc.cbStruct = sizeof(mxc);

    MIXERLINECONTROLS mxlc;
    ::ZeroMemory(&mxlc, sizeof(mxlc));
    mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
    mxlc.dwLineID = mxl.dwLineID;
    mxlc.dwControlType = g_dwControlType;
    mxlc.cControls = 1;
    mxlc.cbmxctrl = sizeof(MIXERCONTROL);
    mxlc.pamxctrl = &mxc;
    if (MMSYSERR_NOERROR != g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE))
    {
        // no mixer, try MUX
        g_dwControlType = MIXERCONTROL_CONTROLTYPE_MUX;
        // mxlc.cbStruct = sizeof(MIXERLINECONTROLS);
        mxlc.dwLineID = mxl.dwLineID;
        mxlc.dwControlType = g_dwControlType;
        mxlc.cControls = 1;
        mxlc.cbmxctrl = sizeof(MIXERCONTROL);
        mxlc.pamxctrl = &mxc;
        if (MMSYSERR_NOERROR !=
            g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE))
        {
            OnError("WaveoutGetSelectControl: mixerGetLineControls");
            return FALSE;
        }
    }

    // store dwControlID, cMultipleItems
    g_strDstLineName = mxl.szName;
    g_strSelectControlName = mxc.szName;
    g_dwSelectControlID = mxc.dwControlID;
    g_dwMultipleItems = mxc.cMultipleItems;
    if (g_dwMultipleItems == 0)
    {
        TRACE("WaveoutGetSelectControl: g_dwMultipleItems == 0\n");
        return FALSE;
    }

    // get the index of the Select control
    MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText = new MIXERCONTROLDETAILS_LISTTEXT[g_dwMultipleItems];
    if (pmxcdSelectText != nullptr)
    {
        MIXERCONTROLDETAILS mxcd;
        mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
        mxcd.dwControlID = g_dwSelectControlID;
        mxcd.cChannels = 1;
        mxcd.cMultipleItems = g_dwMultipleItems;
        mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
        mxcd.paDetails = pmxcdSelectText;
        if (MMSYSERR_NOERROR ==
            g_AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT))
        {
            //// determine which controls the speaker feedback source line
            // for (DWORD dwi = 0; dwi < g_dwMultipleItems; dwi++)
            //{
            // // get the line information
            // MIXERLINE mxl;
            // mxl.cbStruct = sizeof(MIXERLINE);
            // mxl.dwLineID = pmxcdSelectText[dwi].dwParam1;
            // if (::mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(m_hMixer), &mxl, MIXER_OBJECTF_HMIXER |
            // MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR
            // && mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_ANALOG) {
            // // found, dwi is the index.
            // g_dwIndex = dwi;
            // g_strMicName = pmxcdSelectText[dwi].szName;
            // break;
            // }
            //}

            WaveoutSearchSrcLine(pmxcdSelectText, lineToSearch, namesearch, feedback_skip_namesearch);
        }

        delete[] pmxcdSelectText;
    }
    else
    {
        OnError("WaveoutGetSelectControl: pmxcdSelectText");
    }

    return (g_dwIndex < g_dwMultipleItems);
}

BOOL WaveoutInitialize()
{
    // get the number of mixer devices present in the system
    g_uNumMixers = ::mixerGetNumDevs();

    if (g_AudioMixer.isValid())
        g_AudioMixer.Close();

    ::ZeroMemory(&g_sMixerCaps, sizeof(MIXERCAPS));
    g_strDstLineName.Empty();
    g_strSelectControlName.Empty();
    g_strMicName.Empty();
    g_dwMultipleItems = 0;

    //#undef max // The max macro conflicts with the following function.
    // g_dwIndex = numeric_limits<DWORD>::max();
    g_dwIndex = 100000;

    // open the first mixer
    // A "mapper" for audio mixer devices does not currently exist.
    if (g_uNumMixers != 0)
    {
        if (MMSYSERR_NOERROR != g_AudioMixer.Open(cAudioFormat.m_iSelectedMixer, (DWORD_PTR)g_hWndGlobal, 0,
                                                MIXER_OBJECTF_MIXER | CALLBACK_WINDOW))
        {
            OnError("WaveoutInitialize");
            return FALSE;
        }

        if (MMSYSERR_NOERROR != g_AudioMixer.GetDevCaps(&g_sMixerCaps, sizeof(MIXERCAPS)))
        {
            OnError("WaveoutInitialize");
            return FALSE;
        }
    }

    return TRUE;
}

BOOL WaveoutVolumeUninitialize()
{
    BOOL bSucc = g_AudioMixer.isValid() && (MMSYSERR_NOERROR == g_AudioMixer.Close());
    return bSucc;
}

BOOL WaveoutVolumeInitialize()
{
    // get the number of mixer devices present in the system
    g_uNumMixers = ::mixerGetNumDevs();

    g_AudioMixer.Close();
    ::ZeroMemory(&g_sMixerCaps, sizeof(MIXERCAPS));

    g_strVolumeControlName.Empty();
    g_strDstLineName.Empty();

    // open the first mixer
    // A "mapper" for audio mixer devices does not currently exist.
    if (g_uNumMixers != 0)
    {
        if (MMSYSERR_NOERROR != g_AudioMixer.Open(cAudioFormat.m_iSelectedMixer, (DWORD_PTR)g_hWndGlobal, 0,
                                                MIXER_OBJECTF_MIXER | CALLBACK_WINDOW))
            return FALSE;

        if (MMSYSERR_NOERROR != g_AudioMixer.GetDevCaps(&g_sMixerCaps, sizeof(MIXERCAPS)))
            return FALSE;
    }

    return TRUE;
}

BOOL WaveoutSetVolume(DWORD dwVal)
{
    if (!g_AudioMixer.isValid())
    {
        return FALSE;
    }

    if (g_dwVolumeControlID == -1)
        return FALSE;

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = {dwVal};
    MIXERCONTROLDETAILS mxcd;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = g_dwVolumeControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    if (g_AudioMixer.SetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
        return FALSE;

    return TRUE;
}

BOOL WaveoutGetVolume(DWORD &dwVal)
{
    if (!g_AudioMixer.isValid())
        return FALSE;

    if (g_dwVolumeControlID == -1)
        return FALSE;

    MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
    MIXERCONTROLDETAILS mxcd;
    mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mxcd.dwControlID = g_dwVolumeControlID;
    mxcd.cChannels = 1;
    mxcd.cMultipleItems = 0;
    mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
    mxcd.paDetails = &mxcdVolume;
    if (MMSYSERR_NOERROR != g_AudioMixer.GetControlDetails(&mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
        return FALSE;

    dwVal = mxcdVolume.dwValue;

    return TRUE;
}

BOOL WaveoutGetVolumeControl()
{
    // We do not even know the iFeedbackLine, let alone its volume
    if (cAudioFormat.m_iFeedbackLine < 0)
    {
        return FALSE;
    }

    if (!g_AudioMixer.isValid())
        return FALSE;

    // get dwLineID
    MIXERLINE mxl;
    mxl.cbStruct = sizeof(MIXERLINE);
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
    MMRESULT mmResult = g_AudioMixer.GetLineInfo(&mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
    if (MMSYSERR_NOERROR != mmResult)
        return FALSE;

    // got the CD audio volume instead of Stereo mix????
    // for line 7, iFeedbackLine = 6, the source==6 ==> CD Audio
    MIXERLINE mxl2;
    mxl2.cbStruct = sizeof(MIXERLINE);
    mxl2.dwLineID = cAudioFormat.m_iFeedbackLineInfo;
    mmResult = g_AudioMixer.GetLineInfo(&mxl2, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID);
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
    if (g_AudioMixer.GetLineControls(&mxlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
        return FALSE;

    // store dwControlID
    g_strDstLineName = mxl.szName;
    g_strVolumeControlName = mxc.szName;
    // m_dwMinimum = mxc.Bounds.dwMinimum;
    // m_dwMaximum = mxc.Bounds.dwMaximum;
    g_dwVolumeControlID = mxc.dwControlID;

    return TRUE;
}

} // namespace
