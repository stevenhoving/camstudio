// CAudioFormatDlg.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "AudioFormat.h"
#include "CStudioLib.h"
#include <windowsx.h> // for memory functions GlobalXXX

//External Variables

extern LPWAVEFORMATEX pwfx;

extern void BuildRecordingFormat();
extern void AllocCompressFormat();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined(AVE_FORMAT_MPEGLAYER3)
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#endif

//Local Variables

//ver 1.8

/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg dialog

CAudioFormatDlg::CAudioFormatDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAudioFormatDlg::IDD, pParent)
, m_pwfx(0)
, m_cbwfx(0)
, m_iAudioBitsPerSample(0)
, m_iAudioNumChannels(0)
, m_iAudioSamplesPerSeconds(0)
, m_bAudioCompression(TRUE)
, m_iNumFormat(0)
, m_iNumDevice(0)
, m_iInterleavePeriod(0)
{
	//{{AFX_DATA_INIT(CAudioFormatDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAudioFormatDlg::CAudioFormatDlg(const sAudioFormat& cFmt, CWnd* pParent)
: CDialog(CAudioFormatDlg::IDD, pParent)
, m_cFmt(cFmt)
, m_pwfx(0)
, m_cbwfx(0)
, m_iAudioBitsPerSample(cFmt.m_iBitsPerSample)
, m_iAudioNumChannels(cFmt.m_iNumChannels)
, m_iAudioSamplesPerSeconds(cFmt.m_iSamplesPerSeconds)
, m_bAudioCompression(cFmt.m_bCompression)
, m_iNumFormat(0)
, m_iNumDevice(0)
, m_iInterleavePeriod(cFmt.m_iInterleavePeriod)
{
}

void CAudioFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAudioFormatDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_IFACTOR, m_ctrlEditFactor);
	DDX_Control(pDX, IDC_INTERLEAVE, m_ctrlButtonInterleave);
	DDX_Control(pDX, IDC_INTERLEAVEFRAMES, m_ctrlButtonInterleaveFrames);	
	DDX_Control(pDX, IDC_INTERLEAVESECONDS, m_ctrlButtonInterleaveSeconds);
	DDX_Control(pDX, IDC_RECORDFORMAT, m_ctrlCBRecordFormat);
	DDX_Control(pDX, IDC_INPUTDEVICE, m_ctrlCBInputDevice);
	DDX_Control(pDX, IDC_SYSTEMRECORD, m_ctrlButtonSystemRecord);
	DDX_Control(pDX, IDC_COMPRESSEDFORMATTAG, m_ctrlEditCompressedFormatTag);
	DDX_Control(pDX, IDC_COMPRESSEDFORMAT, m_ctrlEditCompressedFormat);
	DDX_Control(pDX, IDC_CHOOSE_COMPRESSED_FORMAT, m_ctrlButtonChooseCompressedFormat);
	DDX_Text(pDX, IDC_IFACTOR, m_iInterleavePeriod);
	DDV_MinMaxInt(pDX, m_iInterleavePeriod, 0, 1000);
}

BEGIN_MESSAGE_MAP(CAudioFormatDlg, CDialog)
	//{{AFX_MSG_MAP(CAudioFormatDlg)
	ON_BN_CLICKED(IDC_CHOOSE_COMPRESSED_FORMAT, OnChoose)
	ON_CBN_SELCHANGE(IDC_RECORDFORMAT, OnSelchangeRecordformat)
	ON_BN_CLICKED(IDC_INTERLEAVE, OnInterleave)
	ON_BN_CLICKED(IDVOLUME, OnVolume)
	ON_CBN_SELCHANGE(IDC_INPUTDEVICE, OnSelchangeInputdevice)
	ON_BN_CLICKED(IDC_INTERLEAVEFRAMES, OnInterleaveframes)
	ON_BN_CLICKED(IDC_INTERLEAVESECONDS, OnInterleaveseconds)
	ON_BN_CLICKED(IDC_SYSTEMRECORD, OnSystemrecord)
	ON_BN_CLICKED(IDC_BUTTON1, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg message handlers

BOOL CAudioFormatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Interleave
	m_ctrlButtonInterleave.SetCheck(cAudioFormat.m_bInterleaveFrames);
	m_iInterleavePeriod = cAudioFormat.m_iInterleaveFactor;

	m_ctrlEditFactor.EnableWindow(cAudioFormat.m_bInterleaveFrames);
	m_ctrlButtonInterleaveFrames.EnableWindow(cAudioFormat.m_bInterleaveFrames);
	m_ctrlButtonInterleaveSeconds.EnableWindow(cAudioFormat.m_bInterleaveFrames);

	m_ctrlButtonInterleaveFrames.SetCheck(cAudioFormat.m_iInterleavePeriod == FRAMES);
	m_ctrlButtonInterleaveSeconds.SetCheck(cAudioFormat.m_iInterleavePeriod == MILLISECONDS);

	m_iAudioBitsPerSample = cAudioFormat.m_iBitsPerSample;
	m_iAudioNumChannels = cAudioFormat.m_iNumChannels;
	m_iAudioSamplesPerSeconds = cAudioFormat.m_iSamplesPerSeconds;

	m_bAudioCompression = cAudioFormat.m_bCompression;

	//Ver 1.2

	//Generate device list
	m_iNumDevice = 0;
	m_devicemap[m_iNumDevice] = WAVE_MAPPER;
	m_iNumDevice++;

	m_ctrlCBInputDevice.ResetContent( );
	m_ctrlCBInputDevice.AddString("Default input device");

	int numdevs = waveInGetNumDevs();
	for (int i = 0; i < numdevs; i++) {
		WAVEINCAPS wicaps;
		MMRESULT mmr = waveInGetDevCaps(i,&wicaps,sizeof(WAVEINCAPS));
		if (mmr == MMSYSERR_NOERROR) {
			m_ctrlCBInputDevice.AddString(wicaps.szPname);
			m_devicemap[m_iNumDevice] = i;
			m_iNumDevice ++;
		}
	}

	//Select the device combo box
	int deviceIsSelected= 0;
	int selectedDevice = WAVE_MAPPER;
	for (int i = 0; i < m_iNumDevice; ++i) {
		if (cAudioFormat.m_uDeviceID == m_devicemap[i]) {
			m_ctrlCBInputDevice.SetCurSel(i);
			selectedDevice = m_devicemap[i];
			deviceIsSelected = 1;
		}
	}
	if (!deviceIsSelected) {
		if (m_iNumDevice) {
			m_ctrlCBInputDevice.SetCurSel(0);
		}
	}

	//Ver 1.2
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( cAudioFormat.m_uDeviceID , &pwic, sizeof(pwic) );

	m_iNumFormat = 0; //counter, number of format

	// This code works on the assumption (when filling in values for the user - interfaces)
	// that the m_Format and pwfx formats (external variables) are already chosen correctly
	// and compatibile with each other
	int devID = m_ctrlCBInputDevice.GetCurSel();
	if (devID < m_iNumDevice) {
		UpdateDeviceData(selectedDevice, cAudioFormat.m_dwWaveinSelected, pwfx);
	}

	//ver 1.8
	m_ctrlButtonSystemRecord.SetCheck(bUseMCI);

	UpdateData(FALSE);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAudioFormatDlg::OnOK()
{
	UpdateData();
	cAudioFormat.m_iInterleaveFactor = m_iInterleavePeriod;
	cAudioFormat.m_bInterleaveFrames = m_ctrlButtonInterleave.GetCheck();
	cAudioFormat.m_iInterleavePeriod = (m_ctrlButtonInterleaveFrames.GetCheck()) ? FRAMES : MILLISECONDS;

	//The Recording format, Compressed format and device must be valid before
	//data from the Audio Options Dialog can be updated to the external variables
	if (0 < m_iNumFormat) {
		int sel = m_ctrlCBRecordFormat.GetCurSel();
		if (0 <= sel) {
			if (m_pwfx) {
				//Ver 1.2
				int getdevice = m_ctrlCBInputDevice.GetCurSel();
				if (getdevice < m_iNumDevice) {
					if (pwfx == NULL) {
						AllocCompressFormat(); //Allocate external format in order to return values
					}

					if (m_cbwfx <= cAudioFormat.m_dwCbwFX) { //All checks cleared, update external values
						// Updating to external variables
						m_cFmt.m_uDeviceID			= m_devicemap[getdevice];
						m_cFmt.m_bCompression		= m_bAudioCompression;
						m_cFmt.m_iBitsPerSample		= m_iAudioBitsPerSample;
						m_cFmt.m_iNumChannels		= m_iAudioNumChannels;
						m_cFmt.m_iSamplesPerSeconds = m_iAudioSamplesPerSeconds;

						// Update the external pwfx (compressed format);
						cAudioFormat.m_dwWaveinSelected	= m_formatmap[sel];
						cAudioFormat.m_dwCbwFX = m_cbwfx;
						//::memcpy((void *)pwfx, (void *)m_pwfx, m_cbwfx);
						::memcpy(pwfx, m_pwfx, m_cbwfx);

						BuildRecordingFormat();
					}
				}
			}
		}
	}

	if (m_pwfx) {
		GlobalFreePtr(m_pwfx);
		m_pwfx = NULL;
	}

	//ver 1.8
	int val = m_ctrlButtonSystemRecord.GetCheck();
	bUseMCI = (val) ? 1 : 0;

	CDialog::OnOK();
}

void CAudioFormatDlg::OnChoose()
{
	if (m_pwfx == NULL) {
		SuggestLocalCompressFormat();
	}

	// initialize the ACMFORMATCHOOSE members
	ACMFORMATCHOOSE acmfc;
	memset(&acmfc, 0, sizeof(acmfc));

	acmfc.cbStruct = sizeof(acmfc);
	acmfc.hwndOwner = m_hWnd;
	acmfc.pwfx = m_pwfx;

	//if (initial_audiosetup)
	// acmfc.fdwStyle = 0;
	//else
	acmfc.fdwStyle = ACMFORMATCHOOSE_STYLEF_INITTOWFXSTRUCT;

	acmfc.cbwfx = m_cbwfx;
	acmfc.pszTitle = TEXT("Audio Compression Format");
	acmfc.szFormatTag[0] = '\0';
	acmfc.szFormat[0] = '\0';
	acmfc.pszName = NULL;
	acmfc.cchName = 0;
	acmfc.fdwEnum = 0;
	acmfc.pwfxEnum = NULL;
	acmfc.hInstance = NULL;
	acmfc.pszTemplateName = NULL;
	acmfc.lCustData = 0L;
	acmfc.pfnHook = NULL;

	/////////////////////////////
	// Valid formats for saving
	/////////////////////////////
	BuildLocalRecordingFormat();

	acmfc.pwfxEnum = &m_FormatLocal;
	acmfc.fdwEnum = ACM_FORMATENUMF_SUGGEST;

	MMRESULT mmresult = acmFormatChoose(&acmfc);
	if (MMSYSERR_NOERROR != mmresult) {
		if (ACMERR_CANCELED != mmresult) {
			CString msgstr;
			CString tstr;
			tstr.LoadString(IDS_STRING_NOTE);
			msgstr.Format("FormatChoose() failed with error = %u!", mmresult);
			MessageBox(msgstr,tstr, MB_OK | MB_ICONEXCLAMATION);
		}

		return;
	}

	TRACE("\nWAVEFORMAT:");
	TRACE("\nwFormatTag = %d",m_pwfx->wFormatTag);
	TRACE("\nnChannels = %d",m_pwfx->nChannels);
	TRACE("\nnSamplesPerSec = %d",m_pwfx->nSamplesPerSec);
	TRACE("\nnAvgBytesPerSec = %d",m_pwfx->nAvgBytesPerSec);
	TRACE("\nnBlockAlign = %d",m_pwfx->nBlockAlign);
	TRACE("\ncbSize = %d",m_pwfx->cbSize);

	UpdateLocalCompressFormatInterface();
}

void CAudioFormatDlg::UpdateLocalCompressFormatInterface()
{
	if (m_bAudioCompression==0) {
		m_ctrlEditCompressedFormatTag.SetWindowText("None Available");
		m_ctrlEditCompressedFormat.SetWindowText(" ");
		return;
	}

	char pszFormat[200];
	char pszFormatTag[200];
	BOOL res = GetFormatDescription(m_pwfx, pszFormatTag, pszFormat);
	if (res) {
		m_ctrlEditCompressedFormatTag.SetWindowText(pszFormatTag);
		m_ctrlEditCompressedFormat.SetWindowText(pszFormat);
	}
}

void CAudioFormatDlg::OnSelchangeRecordformat()
{
	if (m_iNumFormat <= 0) {
		return; //no format to choose from
	}

	int sel = m_ctrlCBRecordFormat.GetCurSel();
	if (sel < 0) {
		return;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_1M08) {
		m_iAudioBitsPerSample = 8;
		m_iAudioNumChannels = 1;
		m_iAudioSamplesPerSeconds = 11025;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_1M16) {
		m_iAudioBitsPerSample = 16;
		m_iAudioNumChannels = 1;
		m_iAudioSamplesPerSeconds = 11025;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_1S08) {
		m_iAudioBitsPerSample = 8;
		m_iAudioNumChannels = 2;
		m_iAudioSamplesPerSeconds = 11025;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_1S16) {
		m_iAudioBitsPerSample = 16;
		m_iAudioNumChannels = 2;
		m_iAudioSamplesPerSeconds = 11025;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_2M08) {
		m_iAudioBitsPerSample = 8;
		m_iAudioNumChannels = 1;
		m_iAudioSamplesPerSeconds = 22050;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_2M16) {
		m_iAudioBitsPerSample = 16;
		m_iAudioNumChannels = 1;
		m_iAudioSamplesPerSeconds = 22050;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_2S08) {
		m_iAudioBitsPerSample = 8;
		m_iAudioNumChannels = 2;
		m_iAudioSamplesPerSeconds = 22050;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_2S16) {
		m_iAudioBitsPerSample = 16;
		m_iAudioNumChannels = 2;
		m_iAudioSamplesPerSeconds = 22050;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_4M08) {
		m_iAudioBitsPerSample = 8;
		m_iAudioNumChannels = 1;
		m_iAudioSamplesPerSeconds = 44100;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_4M16) {
		m_iAudioBitsPerSample = 16;
		m_iAudioNumChannels = 1;
		m_iAudioSamplesPerSeconds = 44100;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_4S08) {
		m_iAudioBitsPerSample = 8;
		m_iAudioNumChannels = 2;
		m_iAudioSamplesPerSeconds = 44100;
	}

	if (m_formatmap[sel] == WAVE_FORMAT_4S16) {
		m_iAudioBitsPerSample = 16;
		m_iAudioNumChannels = 2;
		m_iAudioSamplesPerSeconds = 44100;
	}

	BuildLocalRecordingFormat();
	SuggestLocalCompressFormat();
	UpdateLocalCompressFormatInterface();
}

void CAudioFormatDlg::OnInterleave()
{
	BOOL binteleave = m_ctrlButtonInterleave.GetCheck();
	if (binteleave) {
		m_ctrlEditFactor.EnableWindow(TRUE);
		m_ctrlButtonInterleaveFrames.EnableWindow(TRUE);
		m_ctrlButtonInterleaveSeconds.EnableWindow(TRUE);
	} else {
		m_ctrlEditFactor.EnableWindow(FALSE);
		m_ctrlButtonInterleaveFrames.EnableWindow(FALSE);
		m_ctrlButtonInterleaveSeconds.EnableWindow(FALSE);
	}
}

void CAudioFormatDlg::OnCancel()
{
	if (m_pwfx) {
		GlobalFreePtr(m_pwfx);
		m_pwfx = NULL;
	}

	CDialog::OnCancel();
}

void CAudioFormatDlg::OnVolume()
{
	// Ver 1.1
	if (waveInGetNumDevs() == 0) {
		//CString msgstr;
		//msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
		//MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_NOINPUT1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}


	char dirx[300];
	GetWindowsDirectory(dirx,300);
	CString Windir(dirx);

	//Test Windows\sndvol32.exe
	CString exeFileName("\\sndvol32.exe");
	CString AppDir = Windir;
	CString SubDir = "";
	CString testLaunchPath = AppDir + SubDir + exeFileName;
	CString launchPath("");
	if (launchPath == "") {
		//Verify sndvol32.exe exists
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath=testLaunchPath;
		}
	}

	if (launchPath == "") {
		//Test Windows\system32\sndvol32.exe
		//Verify sndvol32.exe exists
		SubDir = "\\system32";
		testLaunchPath = AppDir + SubDir + exeFileName;
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath = testLaunchPath;

			//need CloseHandle ?
			//BOOL ret = CloseHandle((HANDLE) hdir);
			//if (!ret) MessageBox("Close handle Fails","Note",MB_OK | MB_ICONEXCLAMATION);
		}
	}

	if (launchPath == "") {
		//Test Windows\system\sndvol32.exe
		//Verify sndvol32.exe exists
		SubDir = "\\system32";
		testLaunchPath = AppDir + SubDir + exeFileName;
		OFSTRUCT ofs;
		HFILE hdir = OpenFile(testLaunchPath, &ofs,OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath = testLaunchPath;
		}
	}

	if (launchPath != "") { //launch Volume Control
		//not sure
		launchPath = launchPath + " /r /rec /record";

		if (WinExec(launchPath,SW_SHOW) != 0) {
		} else {
			//MessageBox("Error launching Volume Control!","Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_ERRVOLCTRL1 ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		}
	}
}

void CAudioFormatDlg::OnSelchangeInputdevice()
{
	int devID = m_ctrlCBInputDevice.GetCurSel();
	if (devID < m_iNumDevice) {
		UpdateDeviceData(m_devicemap[devID],cAudioFormat.m_dwWaveinSelected,NULL);
	}
}

// ver 1.2
// =====================================
// UpdateDeviceData
//
// Update the user - interface based on the device data
//
// If the third parameter (compressed format) is not null, we assume it is compatibile with the 2nd parameter (recording format)
//
// =====================================
void CAudioFormatDlg::UpdateDeviceData(UINT deviceID, DWORD curr_sel_rec_format, LPWAVEFORMATEX curr_sel_pwfx)
{
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( deviceID , &pwic, sizeof(pwic) );

	int selected_cindex=-1; //selected index of combo box
	m_iNumFormat=0; //counter, number of format

	//Reset Recording Format Combo Box and format map
	m_ctrlCBRecordFormat.ResetContent();
	m_iNumFormat = 0;

	//This code works on the assumption (when filling in values for the interfaces)
	//that the m_Format and pwfx formats (external variables) are already chosen correctly and compatibile with each other
	if ((pwic.dwFormats) & WAVE_FORMAT_1M08) {
		m_ctrlCBRecordFormat.AddString("11.025 kHz, mono, 8-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_1M08;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_1M16) {
		m_ctrlCBRecordFormat.AddString("11.025 kHz, mono, 16-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_1M16;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_1S08) {
		m_ctrlCBRecordFormat.AddString("11.025 kHz, stereo, 8-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_1S08;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_1S16) {
		m_ctrlCBRecordFormat.AddString("11.025 kHz, stereo, 16-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_1S16;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_2M08) {
		m_ctrlCBRecordFormat.AddString("22.05 kHz, mono, 8-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_2M08;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_2M16) {
		m_ctrlCBRecordFormat.AddString("22.05 kHz, mono, 16-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_2M16;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_2S08) {
		m_ctrlCBRecordFormat.AddString("22.05 kHz, stereo, 8-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_2S08;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_2S16) {
		m_ctrlCBRecordFormat.AddString("22.05 kHz, stereo, 16-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_2S16;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4M08) {
		m_ctrlCBRecordFormat.AddString("44.1 kHz, mono, 8-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_4M08;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4M16) {
		m_ctrlCBRecordFormat.AddString("44.1 kHz, mono, 16-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_4M16;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4S08) {
		m_ctrlCBRecordFormat.AddString("44.1 kHz, stereo, 8-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_4S08;
		m_iNumFormat++;
	}

	if ((pwic.dwFormats) & WAVE_FORMAT_4S16) {
		m_ctrlCBRecordFormat.AddString("44.1 kHz, stereo, 16-bit");
		m_formatmap[m_iNumFormat]=WAVE_FORMAT_4S16;
		m_iNumFormat++;
	}

	if (m_iNumFormat<=0) {
		m_ctrlCBRecordFormat.AddString("None Available");
		m_ctrlCBRecordFormat.SetCurSel(0);
		m_ctrlEditCompressedFormatTag.SetWindowText("None Available");
		m_ctrlEditCompressedFormat.SetWindowText(" ");
		m_ctrlButtonChooseCompressedFormat.EnableWindow(FALSE);
		//For this case, where no recording format, compressed format is available
		//is handled by OnOk (no external formats is updated) when the user press the OK button.
		return;
	} else {
		m_ctrlButtonChooseCompressedFormat.EnableWindow(TRUE);
	}

	for (int k=0;k<m_iNumFormat;k++) {
		if (curr_sel_rec_format == m_formatmap[k])
			selected_cindex=k;
	}

	//If can reach here ==> m_iNumFormat > 0
	if ((selected_cindex==-1) && (m_iNumFormat>0)) { //selected recording format not found
		//force selection to one that is compatible
		selected_cindex=0;

		m_ctrlCBRecordFormat.SetCurSel(selected_cindex);

		//force selection of compress format
		OnSelchangeRecordformat();
	} else {
		//Compressed or Save format
		AllocLocalCompressFormat();
		if (curr_sel_pwfx == NULL) {
			SuggestLocalCompressFormat();
		} else {
			ASSERT(m_pwfx);
			ASSERT(curr_sel_pwfx);
			memcpy(m_pwfx, curr_sel_pwfx, cAudioFormat.m_dwCbwFX);
		}

		UpdateLocalCompressFormatInterface();

		//will this following line call OnSelchangeRecordformat() ?
		if (selected_cindex >= 0) {
			m_ctrlCBRecordFormat.SetCurSel(selected_cindex);
		}
	}
}

void CAudioFormatDlg::OnInterleaveframes()
{
	m_ctrlButtonInterleaveFrames.SetCheck(TRUE);
	m_ctrlButtonInterleaveSeconds.SetCheck(FALSE);
}

void CAudioFormatDlg::OnInterleaveseconds()
{
	m_ctrlButtonInterleaveFrames.SetCheck(FALSE);
	m_ctrlButtonInterleaveSeconds.SetCheck(TRUE);
}

void CAudioFormatDlg::OnSystemrecord()
{
	// TODO: Add your control notification handler code here
}

void CAudioFormatDlg::OnHelp()
{
	CString progdir = GetProgPath();
	CString helppath = progdir + "\\help.htm#Helpmci";
	Openlink(helppath);
}

BOOL CAudioFormatDlg::Openlink (CString link)
{
	// As a last resort try ShellExecuting the URL, may even work on Navigator!
	BOOL bSuccess = OpenUsingShellExecute (link);
	if (!bSuccess) {
		bSuccess = OpenUsingRegisteredClass (link);
	}

	return bSuccess;
}

BOOL CAudioFormatDlg::OpenUsingShellExecute (CString link)
{
	LPCTSTR mode = _T ("open");
	//HINSTANCE hRun = ShellExecute (GetParent ()->GetSafeHwnd (), mode, m_sActualLink, NULL, NULL, SW_SHOW);
	HINSTANCE hRun = ShellExecute (GetSafeHwnd (), mode, link, NULL, NULL, SW_SHOW);
	if ((int) hRun <= HINSTANCE_ERROR) {
		TRACE ("Failed to invoke URL using ShellExecute\n");
		return FALSE;
	}
	return TRUE;
}

BOOL CAudioFormatDlg::OpenUsingRegisteredClass (CString link)
{
	TCHAR key[MAX_PATH + MAX_PATH];

	if (GetRegKey (HKEY_CLASSES_ROOT, _T (".htm"), key) == ERROR_SUCCESS) {
		LPCTSTR mode = _T ("\\shell\\open\\command");
		strcat(key, mode);
		if (GetRegKey (HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS) {
			LPTSTR pos = strstr(key, _T ("\"%1\""));
			if (pos == NULL) {
				// No quotes found

				pos = strstr(key, _T ("%1")); // Check for %1, without quotes

				if (pos == NULL) // No parameter at all...
					pos = key + _tcslen (key) - 1;
				else
					*pos = _T ('\0'); // Remove the parameter
			} else
				*pos = _T ('\0'); // Remove the parameter

			strcat_s(pos, strlen(pos) + 2, _T (" "));
			strcat_s(pos, strlen(pos) + strlen(link) + 1, link);
			HINSTANCE result = (HINSTANCE) WinExec (key, SW_SHOW);
			if ((int) result <= HINSTANCE_ERROR) {
				CString str;
				switch ((int) result)
				{
				case 0:
					str = _T ("The operating system is out\nof memory or resources.");
					break;
				case SE_ERR_PNF:
					str = _T ("The specified path was not found.");
					break;
				case SE_ERR_FNF:
					str = _T ("The specified file was not found.");
					break;
				case ERROR_BAD_FORMAT:
					str = _T ("The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image).");
					break;
				case SE_ERR_ACCESSDENIED:
					str = _T ("The operating system denied\naccess to the specified file.");
					break;
				case SE_ERR_ASSOCINCOMPLETE:
					str = _T ("The filename association is\nincomplete or invalid.");
					break;
				case SE_ERR_DDEBUSY:
					str = _T ("The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed.");
					break;
				case SE_ERR_DDEFAIL:
					str = _T ("The DDE transaction failed.");
					break;
				case SE_ERR_DDETIMEOUT:
					str = _T ("The DDE transaction could not\nbe completed because the request timed out.");
					break;
				case SE_ERR_DLLNOTFOUND:
					str = _T ("The specified dynamic-link library was not found.");
					break;
				case SE_ERR_NOASSOC:
					str = _T ("There is no application associated\nwith the given filename extension.");
					break;
				case SE_ERR_OOM:
					str = _T ("There was not enough memory to complete the operation.");
					break;
				case SE_ERR_SHARE:
					str = _T ("A sharing violation occurred.");
					break;
				default:
					str.Format (_T ("Unknown Error (%d) occurred."), (int) result);
				}
				str = _T ("Unable to open hyperlink:\n\n") + str;
				AfxMessageBox (str, MB_ICONEXCLAMATION | MB_OK);
			} else {
				return TRUE;
			}
		}
	}
	return FALSE;
}

LONG CAudioFormatDlg::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx (key, subkey, 0, KEY_QUERY_VALUE, &hkey);
	if (retval == ERROR_SUCCESS) {
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue (hkey, NULL, data, &datasize);
		strcpy_s(retdata, strlen(retdata) + strlen(data) + 1, data);
		RegCloseKey (hkey);
	}

	return retval;
}

void CAudioFormatDlg::AllocLocalCompressFormat()
{
	if (m_pwfx) {
		//Do nothing....already allocated
		return;
	}
	MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &m_cbwfx);
	if (MMSYSERR_NOERROR != mmresult) {
		CString msgstr;
		CString tstr;
		tstr.LoadString(IDS_STRING_NOTE);
		msgstr.Format("Metrics failed mmresult=%u!", mmresult);
		::MessageBox(NULL,msgstr,tstr, MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (m_cbwfx < cAudioFormat.m_dwCbwFX)
		m_cbwfx = cAudioFormat.m_dwCbwFX;

	m_pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, m_cbwfx);
	if (NULL == m_pwfx) {
		CString msgstr;
		CString tstr;
		tstr.LoadString(IDS_STRING_NOTE);
		msgstr.Format("GlobalAllocPtr(%lu) failed!", m_cbwfx);
		::MessageBox(NULL,msgstr,tstr, MB_OK | MB_ICONEXCLAMATION);
		return;
	}
}

void CAudioFormatDlg::BuildLocalRecordingFormat()
{
	m_FormatLocal.wFormatTag = WAVE_FORMAT_PCM;
	m_FormatLocal.wBitsPerSample = m_iAudioBitsPerSample;
	m_FormatLocal.nSamplesPerSec = m_iAudioSamplesPerSeconds;
	m_FormatLocal.nChannels = m_iAudioNumChannels;
	m_FormatLocal.nBlockAlign = m_FormatLocal.nChannels * (m_FormatLocal.wBitsPerSample/8);
	m_FormatLocal.nAvgBytesPerSec = m_FormatLocal.nSamplesPerSec * m_FormatLocal.nBlockAlign;
	m_FormatLocal.cbSize = 0;
}

void CAudioFormatDlg::SuggestLocalCompressFormat()
{
	m_bAudioCompression = TRUE;
	AllocLocalCompressFormat();

	//1st try MPEGLAYER3
	BuildLocalRecordingFormat();
	MMRESULT mmr = MMSYSERR_ERROR;
	if ((m_FormatLocal.nSamplesPerSec == 22050) && (m_FormatLocal.nChannels == 2) && (m_FormatLocal.wBitsPerSample <= 16)) {
		m_pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_FormatLocal, m_pwfx, m_cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);
	}

	if (mmr != 0) {
		//ver 1.6, use PCM if MP3 not available

		//Then try ADPCM
		//BuildLocalRecordingFormat();
		//m_pwfx->wFormatTag = WAVE_FORMAT_ADPCM;
		//mmr = acmFormatSuggest(NULL, &m_FormatLocal, m_pwfx, m_cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);
		// if (mmr != 0) {
			//Use the PCM as default
			BuildLocalRecordingFormat();
			m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
			mmr = acmFormatSuggest(NULL, &m_FormatLocal, m_pwfx, m_cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);
			if (mmr != 0) {
				m_bAudioCompression = FALSE;
			}
		//}
	}
}

BOOL CAudioFormatDlg::GetFormatDescription(LPWAVEFORMATEX pwformat, LPTSTR pszFormatTag, LPTSTR pszFormat)
{
	MMRESULT mmr;

	// Retrieve the descriptive name for the FormatTag in pwformat.
	if (NULL != pszFormatTag) {
		ACMFORMATTAGDETAILS aftd;

		memset(&aftd, 0, sizeof(aftd));

		// Fill in the required members FormatTAG query.
		aftd.cbStruct = sizeof(aftd);
		aftd.dwFormatTag = pwformat->wFormatTag;

		// Ask ACM to find first available driver that supports the specified Format tag.
		mmr = acmFormatTagDetails(NULL, &aftd, ACM_FORMATTAGDETAILSF_FORMATTAG);
		if (MMSYSERR_NOERROR != mmr) {
			return (FALSE);
		}

		// Copy the Format tag name into the calling application's
		// buffer.
		lstrcpy(pszFormatTag, aftd.szFormatTag);
	}

	CString formatstr;
	CString str_samples_per_second;
	CString str_bits_per_sample;
	CString str_avg_bytes_per_second;
	CString str_nchannels;

	str_samples_per_second.Format("%d Hz",pwformat->nSamplesPerSec);
	str_bits_per_sample.Format("%d Bit",pwformat->wBitsPerSample);
	str_avg_bytes_per_second.Format("%d Bytes/sec",pwformat->nAvgBytesPerSec);
	if (pwformat->nChannels==1)
		str_nchannels.Format("Mono");
	else
		str_nchannels.Format("Stereo");

	formatstr = str_samples_per_second + ", ";
	if ((pwformat->wBitsPerSample)>0) formatstr = formatstr + str_bits_per_sample + ", ";
	formatstr = formatstr + str_nchannels + " " + str_avg_bytes_per_second;
	lstrcpy(pszFormat, LPCTSTR(formatstr));

	return (TRUE);
}
