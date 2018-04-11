// With much experimentation, I've painstakingly restored this file which was missing
// from the originally released 2.0 source code. I hope everyone appreciates the effort
// I have put into making this coveted feature available to all and of course enjoys
// having access to the feature itself. To the future of CamStudio! ~Jake P.

#include "stdafx.h"
#include "Recorder.h"
#include "AudioSpeakers.h"
#include "AudioVolume.h"

#include <mmsystem.h>
#include <vfw.h>
#include <windowsx.h>
#include <cstdio>
#include <filesystem>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD g_dwIndex;

#define SETVOLUME 0
#define GETVOLUME 1
#define GETVOLUMEINFO 2

extern BOOL useVolume(int operation, DWORD &dwVal, int silence_mode);

extern BOOL configWaveOut();
extern BOOL configWaveOutManual();

/////////////////////////////////////////////////////////////////////////////
// CAudioSpeakersDlg dialog

CAudioSpeakersDlg::CAudioSpeakersDlg(CWnd *pParent /*=nullptr*/)
    : CDialog(CAudioSpeakersDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CAudioSpeakersDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

void CAudioSpeakersDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAudioSpeakersDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_VOLUMESLIDER, m_ctrlSliderVolume);
    DDX_Control(pDX, IDC_STATICVOLUME, m_ctrlStaticVolume);
    DDX_Control(pDX, IDC_STATICINFO, m_ctrlStaticLineInfo);
    DDX_Control(pDX, IDC_SOUNDDEVICE, m_ctrlCBSoundDevice);
}

BEGIN_MESSAGE_MAP(CAudioSpeakersDlg, CDialog)
//{{AFX_MSG_MAP(CAudioSpeakersDlg)
ON_BN_CLICKED(IDVOLUME, OnVolume)
ON_BN_CLICKED(ID_AUTOCONFIG, OnAutoconfig)
ON_CBN_SELCHANGE(IDC_SOUNDDEVICE, OnSelchangeSounddevice)
ON_BN_CLICKED(ID_MANUALCONFIG, OnManualconfig)
ON_WM_HSCROLL()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg message handlers

void CAudioSpeakersDlg::OnOK()
{
    // Set the volume
    if (-1 < cAudioFormat.m_iFeedbackLine)
    {
        DWORD volume = m_ctrlSliderVolume.GetPos();
        useVolume(SETVOLUME, volume, TRUE);
    }

    CDialog::OnOK();
}

BOOL CAudioSpeakersDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Get the line index
    BOOL success = (0 <= cAudioFormat.m_iFeedbackLine);
    DWORD volume = 0;
    CString line;
    if (success)
    {
        line.Format(_T("%d"), cAudioFormat.m_iFeedbackLine + 1);
        if (!useVolume(GETVOLUME, volume, TRUE))
        {
            success = false;
        }
    }

    if (!success)
    {
        VERIFY(line.LoadString(IDS_STRING_LINEUNDETECTED));
    }

    m_ctrlSliderVolume.EnableWindow(success);
    m_ctrlSliderVolume.SetPos(volume);
    m_ctrlStaticVolume.EnableWindow(success);
    m_ctrlStaticLineInfo.SetWindowText(line);
    // END Get the line index

    // Generate device list
    m_ctrlCBSoundDevice.ResetContent();

    cAudioFormat.m_iMixerDevices = waveOutGetNumDevs();
    for (int i = 0; i < cAudioFormat.m_iMixerDevices; i++)
    {
        WAVEOUTCAPS wocaps;
        MMRESULT mmr_s = waveOutGetDevCaps(i, &wocaps, sizeof(WAVEOUTCAPS));
        if (mmr_s == MMSYSERR_NOERROR)
        {
            m_ctrlCBSoundDevice.AddString(wocaps.szPname);
        }
    }

    // Select the device combo box
    int deviceIsSelected = 0;
    int selectedDevice = WAVE_MAPPER;
    for (int i = 0; i < cAudioFormat.m_iMixerDevices; i++)
    {
        if (cAudioFormat.m_iSelectedMixer == i)
        {
            m_ctrlCBSoundDevice.SetCurSel(i);
            selectedDevice = i;
            deviceIsSelected = 1;
        }
    }

    if (!deviceIsSelected)
    {
        if (cAudioFormat.m_iMixerDevices > 0)
        {
            cAudioFormat.m_iSelectedMixer = 0;
        }
        m_ctrlCBSoundDevice.SetCurSel(0);
    }

    return TRUE;
}

void CAudioSpeakersDlg::OnVolume()
{
    OnAudioVolume(m_hWnd);
}

void CAudioSpeakersDlg::OnAutoconfig()
{
    configWaveOut();
    OnInitDialog();
}

void CAudioSpeakersDlg::OnSelchangeSounddevice()
{
    cAudioFormat.m_iSelectedMixer = m_ctrlCBSoundDevice.GetCurSel();
    cAudioFormat.m_iFeedbackLine = -1;
    OnInitDialog();
}

void CAudioSpeakersDlg::OnManualconfig()
{
    configWaveOutManual();
    OnInitDialog();
}

void CAudioSpeakersDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
