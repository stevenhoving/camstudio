// AudioSpeakers.cpp : implementation file
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
// AudioSpeakers dialog

AudioSpeakers::AudioSpeakers(CWnd *pParent /*=NULL*/)
: CDialog(AudioSpeakers::IDD, pParent)
{
	//{{AFX_DATA_INIT(AudioSpeakers)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void AudioSpeakers::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AudioSpeakers)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AudioSpeakers, CDialog)
	//{{AFX_MSG_MAP(AudioSpeakers)
	ON_BN_CLICKED(IDVOLUME, OnVolume)
	ON_BN_CLICKED(ID_AUTOCONFIG, OnAutoconfig)
	ON_CBN_SELCHANGE(IDC_SOUNDDEVICE, OnSelchangeSounddevice)
	ON_BN_CLICKED(ID_MANUALCONFIG, OnManualconfig)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AudioFormat message handlers

void AudioSpeakers::OnOK()
{
	// Set the volume
	if (iFeedbackLine > -1)
	{
		DWORD volume = (unsigned long)((CSliderCtrl *) (GetDlgItem(IDC_VOLUMESLIDER)))->GetPos( );
		useVolume(SETVOLUME, volume, TRUE);
	}

	CDialog::OnOK();
}

BOOL AudioSpeakers::OnInitDialog()
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
		line.LoadString(IDS_STRING_LINEUNDETECTED);
	}

	((CSliderCtrl *) (GetDlgItem(IDC_VOLUMESLIDER)))->EnableWindow(success);
	((CStatic *) (GetDlgItem(IDC_STATICVOLUME)))->EnableWindow(success);
	((CSliderCtrl *) (GetDlgItem(IDC_VOLUMESLIDER)))->SetPos(volume);
	((CStatic *)(GetDlgItem(IDC_STATICINFO)))->SetWindowText(line);
	// END Get the line index

	//Generate device list
	((CComboBox *) (GetDlgItem(IDC_SOUNDDEVICE)))->ResetContent( );

	iNumberOfMixerDevices = waveOutGetNumDevs();
	for (int i = 0; i < iNumberOfMixerDevices; i++) {
		WAVEOUTCAPS wocaps;
		MMRESULT mmr_s = waveOutGetDevCaps(i,&wocaps,sizeof(WAVEOUTCAPS));
		if (mmr_s == MMSYSERR_NOERROR) {
			((CComboBox *) (GetDlgItem(IDC_SOUNDDEVICE)))->AddString(wocaps.szPname);
		}
	}

	//Select the device combo box
	int deviceIsSelected = 0;
	int selectedDevice = WAVE_MAPPER;
	for (int i = 0; i < iNumberOfMixerDevices; i++) {
		if (iSelectedMixer == i) {
			((CComboBox *) (GetDlgItem(IDC_SOUNDDEVICE)))->SetCurSel(i);
			selectedDevice = i;
			deviceIsSelected = 1;
		}
	}

	if (!deviceIsSelected) {
		if (iNumberOfMixerDevices > 0) {
			iSelectedMixer = 0;
		}
		((CComboBox *) (GetDlgItem(IDC_SOUNDDEVICE)))->SetCurSel(0);
	}

	return TRUE;
}

void AudioSpeakers::OnVolume()
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
	GetWindowsDirectory(dirx, _MAX_PATH);
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
		launchPath.AppendFormat(_T("%d"), (((CComboBox *)(GetDlgItem(IDC_SOUNDDEVICE)))->GetCurSel()));

		if (WinExec(launchPath,SW_SHOW) != 0) {
		} else {
			//MessageBox("Error launching Volume Control!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_ERRVOLCTRL1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void AudioSpeakers::OnAutoconfig()
{
	configWaveOut();
	OnInitDialog();
}

void AudioSpeakers::OnSelchangeSounddevice()
{
	iSelectedMixer = ((CComboBox *)(GetDlgItem(IDC_SOUNDDEVICE)))->GetCurSel();
	iFeedbackLine = -1;
	OnInitDialog();
}

void AudioSpeakers::OnManualconfig()
{
	configWaveOutManual();
	OnInitDialog();
}

void AudioSpeakers::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}