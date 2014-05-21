// CAudioFormatDlg.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "AudioOptionsDlg.h"
#include "RecorderView.h"
#include "vfw/ACM.h"
#include "CStudioLib.h"
#include "SystemDeviceEnumerator.h"
#include "AudioCompressorFormats.h"
#include "AMVfwCompressDialogs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if !defined(AVE_FORMAT_MPEGLAYER3)
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#endif



/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg dialog

CAudioFormatDlg::CAudioFormatDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAudioFormatDlg::IDD, pParent),
m_pAudioCapFilter(NULL),
m_pAudioCompFilter(NULL)
{
}

CAudioFormatDlg::CAudioFormatDlg(const sAudioFormat& cFmt, CWnd* pParent)
: CDialog(CAudioFormatDlg::IDD, pParent),
m_pAudioCapFilter(NULL),
m_pAudioCompFilter(NULL),
m_vectorCompressionFormats()
{
}

void CAudioFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUDIOCOMPRESSOR, m_ctrlCBAudioCompressors);
	DDX_Control(pDX, IDC_INPUTDEVICE, m_ctrlCBInputDevice);
	DDX_Control(pDX, IDC_SUPPORTEDFORMAT, m_ctrlCBSupportedFormats);
	DDX_Control(pDX, ID_CONFIGUREAUDIO, m_ctrlBTNConfigure);
	DDX_Control(pDX, IDC_BITSPERSAMPLE, m_ctrlEditBitsPerSample);
	DDX_Control(pDX, IDC_CHANNELS, m_ctrlEditChannels);
	DDX_Control(pDX, IDC_SAMPLESPERSEC, m_ctrlSamplesPerSec);
	DDX_Control(pDX, IDC_AVGBYTESPERSEC, m_ctrlAvgBytesPerSec);
}

BEGIN_MESSAGE_MAP(CAudioFormatDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CAudioFormatDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CAudioFormatDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_AUDIOCOMPRESSOR, &CAudioFormatDlg::OnCbnSelchangeAudiocompressor)
	ON_CBN_SELCHANGE(IDC_SUPPORTEDFORMAT, &CAudioFormatDlg::OnCbnSelchangeSupportedformat)
	ON_CBN_SELCHANGE(IDC_INPUTDEVICE, &CAudioFormatDlg::OnCbnSelchangeInputdevice)
	ON_BN_CLICKED(IDVOLUME, &CAudioFormatDlg::OnBnClickedVolume)
	ON_BN_CLICKED(ID_CONFIGUREAUDIO, &CAudioFormatDlg::OnBnClickedConfigure)
END_MESSAGE_MAP()


void CAudioFormatDlg::UpdateCompressFormatInterface()
{
	WAVEFORMATEX *pCurrentFormat =  GetCurrentFormat();
	if(pCurrentFormat)
	{
		CString sData = GetManufacturerFromFormatTag(pCurrentFormat->wFormatTag);
		sData.Format("%d", pCurrentFormat->wBitsPerSample);
		m_ctrlEditBitsPerSample.SetWindowTextA(sData);
		sData.Format("%d (%s)", pCurrentFormat->nChannels , pCurrentFormat->nChannels == 1 ? "mono" : "stereo");
		m_ctrlEditChannels.SetWindowTextA(sData);
		sData.Format("%d", pCurrentFormat->nSamplesPerSec);
		m_ctrlSamplesPerSec.SetWindowTextA(sData);
		sData.Format("%d", pCurrentFormat->nAvgBytesPerSec);
		m_ctrlAvgBytesPerSec.SetWindowTextA(sData);
	}
	else
	{
		m_ctrlEditBitsPerSample.SetWindowTextA("");
		m_ctrlEditChannels.SetWindowTextA("");
		m_ctrlSamplesPerSec.SetWindowTextA("");
		m_ctrlAvgBytesPerSec.SetWindowTextA("");
	}
}

WAVEFORMATEX* CAudioFormatDlg::GetCurrentFormat() const
{
	WAVEFORMATEX *pCurrentFormat = NULL;
	int nCurrItem = m_ctrlCBSupportedFormats.GetCurSel();
	if(nCurrItem >= 0 && nCurrItem <= m_ctrlCBSupportedFormats.GetCount())
	{
		pCurrentFormat =  m_vectorCompressionFormats.at(nCurrItem);
	}
	return pCurrentFormat;
}
const CString CAudioFormatDlg::GetCurrentCaptureDevice() const
{
	if(m_ctrlCBInputDevice.GetCurSel() < 0)
		return "";
	CString sCurrentDevice;
	m_ctrlCBInputDevice.GetLBText(m_ctrlCBInputDevice.GetCurSel(), sCurrentDevice);
	return sCurrentDevice;
}
const CString CAudioFormatDlg::GetCurrentCompressor() const
{
	if(m_ctrlCBAudioCompressors.GetCurSel() < 0)
		return "";
	CString sCurrentCompressor;
	m_ctrlCBAudioCompressors.GetLBText(m_ctrlCBAudioCompressors.GetCurSel() , sCurrentCompressor);
	return sCurrentCompressor;
}
const int CAudioFormatDlg::GetCurrentCompression() const
{
	return m_ctrlCBSupportedFormats.GetCurSel();
}
void CAudioFormatDlg::ReInitializeCaptureFilter()
{
	if(m_pAudioCapFilter)
		delete m_pAudioCapFilter;
	m_pAudioCapFilter = new CAudioCaptureFilter(GetCurrentCaptureDevice());
}
void CAudioFormatDlg::ReInitializeCompressorFilter()
{
	if(m_pAudioCompFilter)
		delete m_pAudioCompFilter;
	m_pAudioCompFilter = new CAudioCompressorFilter(GetCurrentCompressor());
}
CString CAudioFormatDlg::GetManufacturerFromFormatTag(WORD wFormatTAg)
{
	CString sManufacturer;
	switch(wFormatTAg)
	{
	case WAVE_FORMAT_PCM:
		sManufacturer = "";
		break;
	case WAVE_FORMAT_UNKNOWN:
	case WAVE_FORMAT_ADPCM:
	case WAVE_FORMAT_IEEE_FLOAT:
	case WAVE_FORMAT_ALAW:
	case WAVE_FORMAT_MULAW:
	case WAVE_FORMAT_DTS:
	case WAVE_FORMAT_DRM:
	case WAVE_FORMAT_WMAVOICE9:
	case WAVE_FORMAT_WMAVOICE10:
	case WAVE_FORMAT_GSM610:
	case WAVE_FORMAT_MSNAUDIO:
	case WAVE_FORMAT_MSG723:
	case WAVE_FORMAT_MPEG:
	case WAVE_FORMAT_DSAT_DISPLAY:
	case WAVE_FORMAT_MSRT24:
	case WAVE_FORMAT_MSAUDIO1:
	case WAVE_FORMAT_WMAUDIO2:
	case WAVE_FORMAT_WMAUDIO3:
	case WAVE_FORMAT_WMAUDIO_LOSSLESS:
	case WAVE_FORMAT_WMASPDIF:
	case WAVE_FORMAT_MPEG_ADTS_AAC:
	case WAVE_FORMAT_MPEG_RAW_AAC:
	case WAVE_FORMAT_MPEG_LOAS:
	case WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC:
	case WAVE_FORMAT_NOKIA_MPEG_RAW_AAC:
	case WAVE_FORMAT_VODAFONE_MPEG_ADTS_AAC:
	case WAVE_FORMAT_VODAFONE_MPEG_RAW_AAC:
	case WAVE_FORMAT_MPEG_HEAAC:
		sManufacturer = "Microsoft Corporation";
			break;
	case WAVE_FORMAT_VSELP:
		sManufacturer = "Compaq Computer Corp.";
		break;
	case WAVE_FORMAT_IBM_CVSD:
		sManufacturer = "IBM Corporation";
		break;
	case WAVE_FORMAT_OKI_ADPCM:
		sManufacturer = "OKI";
		break;
	case WAVE_FORMAT_IMA_ADPCM:
		sManufacturer = "Intel Corporation";
		break;
	case WAVE_FORMAT_MEDIASPACE_ADPCM:
		sManufacturer = "Videologic";
		break;
	case WAVE_FORMAT_SIERRA_ADPCM:
		sManufacturer = "Sierra Semiconductor Corp";
		break;
	case WAVE_FORMAT_G723_ADPCM:
	case WAVE_FORMAT_ANTEX_ADPCME:
	case WAVE_FORMAT_G721_ADPCM:
	case WAVE_FORMAT_G728_CELP:
		sManufacturer = "Antex Electronics Corporation";
		break;
	case WAVE_FORMAT_RT24:
	case WAVE_FORMAT_PAC:
		sManufacturer = "InSoft, Inc.";
		break;
	case WAVE_FORMAT_MPEGLAYER3:
		sManufacturer = "ISO/MPEG Layer3 Format Tag";
		break;
	case WAVE_FORMAT_LUCENT_G723:
		sManufacturer = "Lucent Technologies";
		break;
	case WAVE_FORMAT_CIRRUS:
		sManufacturer = "Cirrus Logic";
		break;
	case WAVE_FORMAT_ESPCM:
	case WAVE_FORMAT_ESST_AC3:
		sManufacturer = "ESS Technology, Inc.";
		break;
	case WAVE_FORMAT_VOXWARE:
	case WAVE_FORMAT_VOXWARE_BYTE_ALIGNED:
	case WAVE_FORMAT_VOXWARE_AC8:
	case WAVE_FORMAT_VOXWARE_AC10:
	case WAVE_FORMAT_VOXWARE_AC16:
	case WAVE_FORMAT_VOXWARE_AC20:
	case WAVE_FORMAT_VOXWARE_RT24:
	case WAVE_FORMAT_VOXWARE_RT29:
	case WAVE_FORMAT_VOXWARE_RT29HW:
	case WAVE_FORMAT_VOXWARE_VR12:
	case WAVE_FORMAT_VOXWARE_VR18:
	case WAVE_FORMAT_VOXWARE_TQ40:
	case WAVE_FORMAT_VOXWARE_TQ60:
		sManufacturer = "Voxware Inc";
		break;
	case WAVE_FORMAT_CANOPUS_ATRAC:
		sManufacturer ="Canopus, co., Ltd.";
		break;
	case WAVE_FORMAT_G726_ADPCM:
	case WAVE_FORMAT_G722_ADPCM:
		sManufacturer = "APICOM";
		break;
	case WAVE_FORMAT_SOFTSOUND:
		sManufacturer = "Softsound, Ltd.";
		break;
	case WAVE_FORMAT_G729A:
	case WAVE_FORMAT_VME_VMPCM:
	case WAVE_FORMAT_TPC:
	case WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS:
		sManufacturer = "AT&T Labs, Inc.";
		break;
	case WAVE_FORMAT_MVI_MVI2:
		sManufacturer = "Motion Pixels";
		break;
	case WAVE_FORMAT_DF_G726:
	case WAVE_FORMAT_DF_GSM610:
		sManufacturer = "DataFusion Systems (Pty) (Ltd)";
		break;
	case WAVE_FORMAT_ISIAUDIO:
		sManufacturer = "Iterated Systems, Inc.";
		break;
	case WAVE_FORMAT_ONLIVE:
		sManufacturer = "OnLive! Technologies, Inc.";
		break;
	case WAVE_FORMAT_SBC24:
		sManufacturer = "Siemens Business Communications Sys";
		break;
	case WAVE_FORMAT_DOLBY_AC3_SPDIF:
		sManufacturer = "Sonic Foundry";
		break;
	case WAVE_FORMAT_MEDIASONIC_G723:
		sManufacturer = "MediaSonic";
		break;
	case WAVE_FORMAT_PROSODY_8KBPS:
		sManufacturer = "Aculab plc";
		break;
	case WAVE_FORMAT_ZYXEL_ADPCM:
		sManufacturer = "ZyXEL Communications, Inc.";
		break;
	case WAVE_FORMAT_PHILIPS_LPCBB:
		sManufacturer = "Philips Speech Processing";
		break;
	case WAVE_FORMAT_PACKED:
		sManufacturer = "Studer Professional Audio AG";
		break;
	case WAVE_FORMAT_MALDEN_PHONYTALK:
		sManufacturer = "Malden Electronics Ltd.";
		break;
	case WAVE_FORMAT_RAW_AAC1:
		sManufacturer = "";
		break;
	case WAVE_FORMAT_RHETOREX_ADPCM:
		sManufacturer = "Rhetorex Inc.";
		break;
	case WAVE_FORMAT_IRAT:
		sManufacturer = "BeCubed Software Inc.";
		break;
	case WAVE_FORMAT_VIVO_G723:
	case WAVE_FORMAT_VIVO_SIREN:
		sManufacturer = "Vivo Software";
		break;
	case WAVE_FORMAT_DIGITAL_G723:
		sManufacturer = "Digital Equipment Corporation";
		break;
	case WAVE_FORMAT_SANYO_LD_ADPCM:
		sManufacturer = "Sanyo Electric Co., Ltd.";
		break;
	case WAVE_FORMAT_SIPROLAB_ACEPLNET:
	case WAVE_FORMAT_SIPROLAB_ACELP4800:
	case WAVE_FORMAT_SIPROLAB_ACELP8V3:
	case WAVE_FORMAT_SIPROLAB_G729:
	case WAVE_FORMAT_SIPROLAB_G729A:
	case WAVE_FORMAT_SIPROLAB_KELVIN:
		sManufacturer = "Sipro Lab Telecom Inc.";
		break;
	case WAVE_FORMAT_G726ADPCM:
		sManufacturer = "Dictaphone Corporation";
		break;
	case WAVE_FORMAT_QUALCOMM_PUREVOICE:
	case WAVE_FORMAT_QUALCOMM_HALFRATE:
		sManufacturer = "Qualcomm, Inc.";
		break;
	case WAVE_FORMAT_TUBGSM:
		sManufacturer = "Ring Zero Systems, Inc.";
		break;
	case WAVE_FORMAT_UNISYS_NAP_ADPCM:
	case WAVE_FORMAT_UNISYS_NAP_ULAW:
	case WAVE_FORMAT_UNISYS_NAP_ALAW:
	case WAVE_FORMAT_UNISYS_NAP_16K:
		sManufacturer = "Unisys Corp.";
		break;
	case WAVE_FORMAT_CREATIVE_ADPCM:
	case WAVE_FORMAT_CREATIVE_FASTSPEECH8:
	case WAVE_FORMAT_CREATIVE_FASTSPEECH10:
		sManufacturer = "Creative Labs, Inc";
		break;
	case WAVE_FORMAT_UHER_ADPCM:
		sManufacturer = "UHER informatic GmbH";
		break;
	case WAVE_FORMAT_QUARTERDECK:
		sManufacturer = "Quarterdeck Corporation";
		break;
	case WAVE_FORMAT_ILINK_VC:
		sManufacturer = "I-link Worldwide";
		break;
	case WAVE_FORMAT_RAW_SPORT:
		sManufacturer = "Aureal Semiconductor";
		break;
	case WAVE_FORMAT_GENERIC_PASSTHRU:
		sManufacturer = "";
		break;
	case WAVE_FORMAT_IPI_HSX:
	case WAVE_FORMAT_IPI_RPELP:
		sManufacturer = "Interactive Products, Inc.";
		break;
	case WAVE_FORMAT_CS2:
		sManufacturer = "Consistent Software";
		break;
	case WAVE_FORMAT_SONY_SCX:
		sManufacturer = "Sony Corp.";
		break;
	case WAVE_FORMAT_FM_TOWNS_SND:
		sManufacturer = "Fujitsu Corp.";
		break;
	case WAVE_FORMAT_BTV_DIGITAL:
		sManufacturer = "Brooktree Corporation";
		break;
	case WAVE_FORMAT_QDESIGN_MUSIC:
		sManufacturer = "QDesign Corporation";
		break;
	case WAVE_FORMAT_OLIGSM:
	case WAVE_FORMAT_OLIADPCM:
	case WAVE_FORMAT_OLICELP:
	case WAVE_FORMAT_OLISBC:
	case WAVE_FORMAT_OLIOPR:
		sManufacturer = "Ing C. Olivetti & C., S.p.A.";
		break;
	case WAVE_FORMAT_LH_CODEC:
		sManufacturer = "Lernout & Hauspie";
		break;
	case WAVE_FORMAT_NORRIS:
		sManufacturer = "Norris Communications, Inc.";
		break;
	case WAVE_FORMAT_DVM:
		sManufacturer = "FAST Multimedia AG";
		break;
	default:
		sManufacturer = "";
		break;
		}
	return sManufacturer;
}

void CAudioFormatDlg::LoadAudioDevices()
{
	m_ctrlCBInputDevice.ResetContent();

	CSystemDeviceEnumerator sdEnum;
	std::vector<CString> vectorAudioDev;

	HRESULT hr = sdEnum.EnumerateAudioDevices(vectorAudioDev);
	if(SUCCEEDED(hr) && vectorAudioDev.size() > 0)
	{
		int nSelected = -1;
		int nIndex = 0;
		for (std::vector<CString>::iterator it = vectorAudioDev.begin(); it != vectorAudioDev.end(); it++)
		{
			CString sName(*it);
			m_ctrlCBInputDevice.AddString(sName);
			if(sName.CompareNoCase(cAudioFormat.m_sCaptureDevice.c_str())==0)
			{
				nSelected = nIndex;
			}
			nIndex++;
		}
		if(nSelected == -1)
		{
			m_ctrlCBInputDevice.SetCurSel(-1);
		}
		else
		{
			m_ctrlCBInputDevice.SetCurSel(nSelected);
			ReInitializeCaptureFilter();
		}
	}
	else
	{
		m_ctrlCBInputDevice.AddString("None Available");
		m_ctrlCBInputDevice.SetCurSel(0);
		m_ctrlBTNConfigure.EnableWindow(0);
	}
}

void CAudioFormatDlg::LoadAudioCompressors()
{
	m_ctrlCBAudioCompressors.ResetContent();
	CSystemDeviceEnumerator acEnum;
	std::vector<CString> vectorAudComp;
	HRESULT hr = acEnum.EnumerateAudioCompressors(vectorAudComp);
	if(SUCCEEDED(hr) && vectorAudComp.size() > 0)
	{
		int nSelected = -1;
		int nIndex = 0;
		for (std::vector<CString>::iterator it = vectorAudComp.begin(); it != vectorAudComp.end(); it++)
		{
			CString sName(*it);
			m_ctrlCBAudioCompressors.AddString(sName);
			if(sName.CompareNoCase(cAudioFormat.m_sCompressor.c_str())==0)
			{
				nSelected = nIndex;
			}
			nIndex++;
		}
		if(nSelected == -1)
		{
			m_ctrlCBAudioCompressors.SetCurSel(-1);
		}
		else
		{
			m_ctrlCBAudioCompressors.SetCurSel(nSelected);
			ReInitializeCompressorFilter();
		}
	}
	else
	{
		m_ctrlCBAudioCompressors.AddString("None Available");
		m_ctrlCBAudioCompressors.SetCurSel(0);
	}
}

void CAudioFormatDlg::LoadCompressionFormats()
{
	if(!m_pAudioCompFilter)
		return;
	m_vectorCompressionFormats.clear();
	m_ctrlCBSupportedFormats.ResetContent();
	CAudioCompressorFormats compFormats(m_pAudioCompFilter->FilterName());
	HRESULT hr = compFormats.GetSupportedFormats(m_vectorCompressionFormats);
	
	if(SUCCEEDED(hr) && m_vectorCompressionFormats.size() > 0)
	{
		for(std::vector<WAVEFORMATEX*>::iterator it = m_vectorCompressionFormats.begin(); it != m_vectorCompressionFormats.end();it++)
		{
			WAVEFORMATEX * pFormat(*it);
			CString sFormat;
			sFormat.Format(" %d Hertz, %s, %d Bits %d Bytes/Sec",
											pFormat->nSamplesPerSec, 
											(pFormat->nChannels == 1) ? "mono" : "stereo", 
											pFormat->wBitsPerSample,
											pFormat->nAvgBytesPerSec);
			m_ctrlCBSupportedFormats.AddString(sFormat);
		}
	}
	m_ctrlCBSupportedFormats.SetCurSel(cAudioFormat.m_iCurrentCompression);
}

/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg message handlers

BOOL CAudioFormatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadAudioDevices();
	LoadAudioCompressors();
	LoadCompressionFormats();
	UpdateCompressFormatInterface();
	return TRUE; 
}

void CAudioFormatDlg::OnOK()
{
	m_cFmt.m_sCaptureDevice = GetCurrentCaptureDevice();
	m_cFmt.m_sCompressor = GetCurrentCompressor();
	m_cFmt.m_iCurrentCompression = GetCurrentCompression();
	WAVEFORMATEX *pCurrentFormat = GetCurrentFormat();
	if(pCurrentFormat)
	{
		m_cFmt.m_iFormatTag = pCurrentFormat->wFormatTag;
		m_cFmt.m_iNumChannels = pCurrentFormat->nChannels;
		m_cFmt.m_iSamplesPerSeconds = pCurrentFormat->nSamplesPerSec;
		m_cFmt.m_iAvgBytesPerSec = pCurrentFormat->nAvgBytesPerSec;
		m_cFmt.m_iBlockAlign = pCurrentFormat->nBlockAlign;
		m_cFmt.m_iBitsPerSample = pCurrentFormat->wBitsPerSample;
		m_cFmt.m_iCbSize = pCurrentFormat->cbSize;
		/*
		// just copy the value of m_iRecordAudio from current config
		// just not to overwrite it when new values
		// are copied from m_cFmt
		*/
		m_cFmt.m_iRecordAudio = cAudioFormat.m_iRecordAudio;
	}

	CDialog::OnOK();
}

void CAudioFormatDlg::OnSelectCompression()
{
	
}

void CAudioFormatDlg::OnSelchangeRecordformat()
{
	LoadCompressionFormats();
}

void CAudioFormatDlg::OnCancel()
{
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

	TCHAR dirx[300];
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
		HFILE hdir = OpenFile(testLaunchPath, &ofs, OF_EXIST);
		if (hdir != HFILE_ERROR) {
			launchPath=testLaunchPath;
		}
		CloseHandle((HANDLE)hdir);
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
		}
		CloseHandle((HANDLE)hdir);
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
		CloseHandle((HANDLE)hdir);
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
	// Sound mixer moved in Windows Vista! check new exe name only if windows version matches
	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx((LPOSVERSIONINFO) &osinfo))
	{
		if (osinfo.dwMajorVersion >= 6) //Vista
		{
			testLaunchPath = AppDir + SubDir + "\\SndVol.exe";
			OFSTRUCT ofs;
			HFILE hdir = OpenFile(testLaunchPath, &ofs, OF_EXIST);
			if (hdir != HFILE_ERROR) {
				launchPath=testLaunchPath;
			}
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
	//int devID = m_ctrlCBInputDevice.GetCurSel();
	//CString sData;
	//m_ctrlCBInputDevice.GetLBText(devID, sData);
	//m_cFmt.m_sCaptureDevice = sData;
	//if(m_pAudioCapFilter)
	//	delete m_pAudioCapFilter;
	//m_pAudioCapFilter = new CAudioCaptureFilter(GetCurrentCaptureDevice());
	ReInitializeCaptureFilter();
	CAMVfwCompressDialogs vfwCompDialog(this->m_hWnd);
	HRESULT hr = vfwCompDialog.CreateInstance(CLSID_VideoInputDeviceCategory);
	//m_ctrlBTNConfigure.EnableWindow(0);
	if(SUCCEEDED(hr))
	{
		m_ctrlBTNConfigure.EnableWindow((m_pAudioCapFilter->GetFilter() != NULL && vfwCompDialog.HasConfigDialog(m_pAudioCapFilter) == true));
	}
	//CAudioCaptureSource dev(sData);
}

void CAudioFormatDlg::OnBnClickedAcconfig()
{
	//CString s;
	//m_ctrlCBRecordFormat.GetLBText(m_ctrlCBRecordFormat.GetCurSel(),s);
	//CAudioCaptureSource acSrce(s);
	// TODO: Add your control notification handler code here 
}



void CAudioFormatDlg::OnEnChangeCompressedformattag()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CAudioFormatDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CAudioFormatDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}


void CAudioFormatDlg::OnCbnSelchangeAudiocompressor()
{
	//CString sData;
	//m_ctrlCBAudioCompressors.GetLBText(m_ctrlCBAudioCompressors.GetCurSel(), sData);
	//m_cFmt.m_sCompressor = sData;
	ReInitializeCompressorFilter();
	LoadCompressionFormats();
	UpdateCompressFormatInterface();
}


void CAudioFormatDlg::OnCbnSelchangeSupportedformat()
{
	UpdateCompressFormatInterface();
}


void CAudioFormatDlg::OnCbnSelchangeInputdevice()
{
	ReInitializeCaptureFilter();
}


void CAudioFormatDlg::OnBnClickedVolume()
{
	// TODO: Add your control notification handler code here
}


void CAudioFormatDlg::OnBnClickedConfigure()
{
	//CString sFilter;
	//m_ctrlCBInputDevice.GetLBText(m_ctrlCBInputDevice.GetCurSel(), sFilter);
	
	CAMVfwCompressDialogs vfwCompDialog(this->m_hWnd);
	HRESULT hr = vfwCompDialog.CreateInstance(CLSID_VideoInputDeviceCategory);
	if(SUCCEEDED(hr))
	{
		if(m_pAudioCapFilter)
			vfwCompDialog.DisplayConfigDialog(m_pAudioCapFilter);
	}
}
