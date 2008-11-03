// CAudioSpeakersDlg.cpp : implementation file
//

// With much experimentation, I've painstakingly restored this file which was missing
// from the originally released 2.0 source code. I hope everyone appreciates the effort
// I have put into making this coveted feature available to all and of course enjoys
// having access to the feature itself. To the future of CamStudio! ~Jake P.

#include "stdafx.h"
#include "Recorder.h"

#include <mmsystem.h>
#include <vfw.h>
#include <windowsx.h>

#include "AudioSpeakers.h"

#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD m_dwIndex;

#define SETVOLUME 0
#define GETVOLUME 1
#define GETVOLUMEINFO 2

extern BOOL useVolume(int operation,DWORD &dwVal,int silence_mode);

extern BOOL configWaveOut();
extern BOOL configWaveOutManual();

/////////////////////////////////////////////////////////////////////////////
// CAudioSpeakersDlg dialog

CAudioSpeakersDlg::CAudioSpeakersDlg(CWnd *pParent /*=NULL*/)
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
	if (-1 < iFeedbackLine)
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
	CString line;
	BOOL success = (iFeedbackLine > -1);
	DWORD volume = 0;

	if (success) {
		line.Format(_T("%d"), iFeedbackLine + 1);
		if (!useVolume(GETVOLUME, volume, TRUE)) {
			success = false;
		}
	}

	if (!success) {
		VERIFY(line.LoadString(IDS_STRING_LINEUNDETECTED));
	}

	m_ctrlSliderVolume.EnableWindow(success);
	m_ctrlSliderVolume.SetPos(volume);
	m_ctrlStaticVolume.EnableWindow(success);
	m_ctrlStaticLineInfo.SetWindowText(line);
	// END Get the line index

	//Generate device list
	m_ctrlCBSoundDevice.ResetContent( );

	iNumberOfMixerDevices = waveOutGetNumDevs();
	for (int i = 0; i < iNumberOfMixerDevices; i++) {
		WAVEOUTCAPS wocaps;
		MMRESULT mmr_s = waveOutGetDevCaps(i,&wocaps,sizeof(WAVEOUTCAPS));
		if (mmr_s == MMSYSERR_NOERROR) {
			m_ctrlCBSoundDevice.AddString(wocaps.szPname);
		}
	}

	//Select the device combo box
	int deviceIsSelected = 0;
	int selectedDevice = WAVE_MAPPER;
	for (int i = 0; i < iNumberOfMixerDevices; i++) {
		if (iSelectedMixer == i) {
			m_ctrlCBSoundDevice.SetCurSel(i);
			selectedDevice = i;
			deviceIsSelected = 1;
		}
	}

	if (!deviceIsSelected) {
		if (iNumberOfMixerDevices > 0) {
			iSelectedMixer = 0;
		}
		m_ctrlCBSoundDevice.SetCurSel(0);
	}

	return TRUE;
}

void CAudioSpeakersDlg::OnVolume()
{
	// Ver 1.1
	if (waveInGetNumDevs() == 0) {
		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
		//MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd, IDS_STRING_NOINPUT1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	TCHAR dirx[_MAX_PATH];
	VERIFY(::GetWindowsDirectory(dirx, _MAX_PATH));
	CString Windir(dirx);
	//Test Windows\sndvol32.exe
	CString AppDir = Windir;
	CString SubDir = "";
	CString exeFileName("\\sndvol32.exe");
	CString testLaunchPath = AppDir + SubDir + exeFileName;
	CString launchPath("");
	if (launchPath == "") {
		//Verify sndvol32.exe exists
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs, OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath = testLaunchPath;
		}
	}

	//Test Windows\system32\sndvol32.exe
	//AppDir = Windir;	// unmodified
	SubDir = "\\system32";
	testLaunchPath = AppDir + SubDir + exeFileName;
	if (launchPath == "") {
		//Verify sndvol32.exe exists
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs, OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath = testLaunchPath;

			//need CloseHandle ?
			//BOOL ret = CloseHandle((HANDLE) hdir);
			//if (!ret) MessageBox("Close handle Fails","Note",MB_OK | MB_ICONEXCLAMATION);
		}
	}

	//Test Windows\system\sndvol32.exe
	// AppDir = Windir;			// unmodified
	// SubDir = "\\system32";	// unmodified
	testLaunchPath = AppDir + SubDir + exeFileName;
	if (launchPath == "") {
		//Verify sndvol32.exe exists
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath=testLaunchPath;
		}
	}

	if (launchPath != "") { //launch Volume Control
		//not sure
		launchPath = launchPath + _T(" /d ");
		launchPath.AppendFormat(_T("%d"), m_ctrlCBSoundDevice.GetCurSel());

		if (WinExec(launchPath, SW_SHOW) != 0) {
		} else {
			//MessageBox("Error launching Volume Control!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_ERRVOLCTRL1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CAudioSpeakersDlg::OnAutoconfig()
{
	configWaveOut();
	OnInitDialog();
}

void CAudioSpeakersDlg::OnSelchangeSounddevice()
{
	iSelectedMixer = m_ctrlCBSoundDevice.GetCurSel();
	iFeedbackLine = -1;
	OnInitDialog();
}

void CAudioSpeakersDlg::OnManualconfig()
{
	configWaveOutManual();
	OnInitDialog();
}

void CAudioSpeakersDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
