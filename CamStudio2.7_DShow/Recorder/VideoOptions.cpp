// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// VideoOptions.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "VideoOptions.h"
#include "RecorderView.h"
#include "vfw/VCM.h"
#include "SystemDeviceEnumerator.h"
#include "AMVfwCompressDialogs.h"
#include "StreamingConfig.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define NUMBER_OF_PROFILES 16
#define LOWER_BOUND 1
#define UPPER_BOUND 1000
// According to the Documentation, the DDV_ functiosn are global and have the AFXAPI
// calling convention so they cannot be a class member function. They access the dialog
// class through the CDataExchange::m_pDlgWnd member. Here, we limit the scope of the
// DDV_ function in a namespace and call the CVideoOptionsDlg class to perform the
// validation.
namespace {
void AFXAPI DDV_KeyFrameInterval(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	CVideoOptionsDlg * pDlg = reinterpret_cast<CVideoOptionsDlg *>(pDX->m_pDlgWnd);
	if (pDlg)
		pDlg->DDV_KeyFrameInterval(pDX, value, minVal, maxVal);
}
void AFXAPI DDV_CaptureInterval(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	CVideoOptionsDlg * pDlg = reinterpret_cast<CVideoOptionsDlg *>(pDX->m_pDlgWnd);
	if (pDlg)
		pDlg->DDV_CaptureInterval(pDX, value, minVal, maxVal);
}
void AFXAPI DDV_PlaybackRate(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	CVideoOptionsDlg * pDlg = reinterpret_cast<CVideoOptionsDlg *>(pDX->m_pDlgWnd);
	if (pDlg)
		pDlg->DDV_PlaybackRate(pDX, value, minVal, maxVal);
}

}
int m_arrCaptureRate [16] = {1000,500,250,200,125,100,50,40,25,20,10,8,5,4,2,1};
int m_arrPlaybackRate[16] = {1,2,4,5,8,10,20,25,40,50,100,125,200,250,500,1000};
int m_arrAutoSetCaptureRate[100];
int m_arrAutoSetPlaybackRate[100];
/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsDlg dialog

CVideoOptionsDlg::CVideoOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVideoOptionsDlg::IDD, pParent)
, m_iQuality(0)
, m_iKeyFrameInterval(0)
, m_iCaptureInterval(0)
, m_iPlaybackRate(0)
, m_iAdjust(0)
, m_iStaticQuality(0)
, m_iCurrentSliderPos(0)
{
	m_iQuality = cVideoOpts.m_iCompQuality;
	if (m_iQuality < 1)
		m_iQuality = 1;
	m_iStaticQuality = m_iQuality;
	m_iPlaybackRate = cVideoOpts.m_iFramesPerSecond;
	m_iKeyFrameInterval = cVideoOpts.m_iKeyFramesEvery;
	m_iCaptureInterval = cVideoOpts.m_iTimeLapse;
	InitAutoAdjustArray();
}

CVideoOptionsDlg::CVideoOptionsDlg(const sVideoOpts& cOpts, CWnd* pParent)
: CDialog(CVideoOptionsDlg::IDD, pParent)
, m_iQuality(0)
, m_iKeyFrameInterval(200)
, m_iCaptureInterval(0)
, m_iPlaybackRate(200)
, m_iAdjust(0)
, m_iStaticQuality(0)
, m_cOpts(cOpts)
, m_pCompFilter(NULL)
, m_pVidCapFilter(NULL)
{
	m_iQuality = cOpts.m_iCompQuality;
	if (m_iQuality < 1)
		m_iQuality = 1;
	m_iStaticQuality = m_iQuality;
	m_iPlaybackRate = cOpts.m_iFramesPerSecond;
	m_iKeyFrameInterval = cOpts.m_iKeyFramesEvery;
	m_iCaptureInterval = cOpts.m_iTimeLapse;
	InitAutoAdjustArray();
}

void CVideoOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPRESSORS, m_ctrlCBCompressor);
	DDX_Control(pDX, IDC_VIDCAP, m_ctrlCBVCF);
	DDX_Control(pDX, ID_ABOUT, m_ctrlBtnAbout);
	DDX_Control(pDX, ID_CONFIGURE, m_ctrlBtnCfgComp);
	DDX_Control(pDX, ID_CONFIGURE_VCF, m_ctrlBtnCfgVidCap);
	DDX_Control(pDX, IDC_QUALITY_SLIDER, m_ctrlSliderQuality);
	DDX_Control(pDX, IDC_QUALITY, m_ctrlStaticQuality);
	DDX_Control(pDX, IDC_KEYFRAMES, m_ctrlEditKeyFrameInterval);
	DDX_Control(pDX, IDC_KEYFRAMES2, m_ctrlEditCaptureInterval);
	DDX_Control(pDX, IDC_FPS, m_ctrlEdiPlaybackRate);
	DDX_Control(pDX, IDC_AUTO, m_ctrlButtonAutoAdjust);
	DDX_Control(pDX, IDC_LOCK, m_ctrlButtonLock);
	DDX_Control(pDX, IDC_CFE_INFO, m_ctrlButtonInfo);
	DDX_Control(pDX, IDC_ADJUST, m_ctrlSliderAdjust);
	DDX_Slider(pDX, IDC_QUALITY_SLIDER, m_iQuality);
	DDX_Text(pDX, IDC_KEYFRAMES, m_iKeyFrameInterval);
	::DDV_KeyFrameInterval(pDX, m_iKeyFrameInterval, 1, 1000);//200);
	DDX_Text(pDX, IDC_KEYFRAMES2, m_iCaptureInterval);
	::DDV_CaptureInterval(pDX, m_iCaptureInterval, 1, 60000);
	DDX_Text(pDX, IDC_FPS, m_iPlaybackRate);
	::DDV_PlaybackRate(pDX, m_iPlaybackRate, 1, 1000);//200);
	DDX_Slider(pDX, IDC_ADJUST, m_iAdjust);
	DDX_Text(pDX, IDC_QUALITY, m_iStaticQuality);
	//DDX_Control(pDX, IDC_SUPPORTROUNDDOWN, m_ctrlButtonRoundDown);
}

BEGIN_MESSAGE_MAP(CVideoOptionsDlg, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_ABOUT, OnAbout)
	ON_CBN_SELCHANGE(IDC_COMPRESSORS, OnSelchangeCompressors)
	ON_BN_CLICKED(ID_CONFIGURE, OnConfigure)
	ON_BN_CLICKED(IDC_AUTO, OnAuto)
	ON_BN_CLICKED(IDC_LOCK, OnLock)
	ON_BN_CLICKED(IDC_CFE_INFO, OnCaptureFrameRateInfo)
	ON_BN_CLICKED(IDOK, &CVideoOptionsDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_VIDCAP, &CVideoOptionsDlg::OnCbnSelchangeVidcap)
	ON_BN_CLICKED(ID_CONFIGURE_VCF, &CVideoOptionsDlg::OnBnClickedConfigureVcf)
END_MESSAGE_MAP()

void CVideoOptionsDlg::RefreshCompressorButtons()
{
	CAMVfwCompressDialogs vfwCompDialogs(this->m_hWnd);
	if(SUCCEEDED(vfwCompDialogs.CreateInstance(CLSID_VideoCompressorCategory)))
	{
		m_ctrlBtnAbout.EnableWindow(vfwCompDialogs.HasAboutDialog(m_pCompFilter));
		m_ctrlBtnCfgComp.EnableWindow(vfwCompDialogs.HasConfigDialog(m_pCompFilter));
	}
}

void CVideoOptionsDlg::RefreshVidCapConfigButton()
{
	CAMVfwCompressDialogs vfwCompDialogs(this->m_hWnd);
	if(SUCCEEDED(vfwCompDialogs.CreateInstance(CLSID_VideoCompressorCategory)))
	{
			m_ctrlBtnCfgVidCap.EnableWindow(vfwCompDialogs.HasConfigDialog(m_pVidCapFilter));
	}
}

void CVideoOptionsDlg::LoadVideoCompressors()
{
	m_ctrlCBCompressor.ResetContent();
	CSystemDeviceEnumerator sdEnum;
	std::vector<CString> vectorVidComp;

	HRESULT hr = sdEnum.EnumerateVideoCompressors(vectorVidComp);
	if(SUCCEEDED(hr) && vectorVidComp.size() > 0)
	{
		int nSelectedComp = -1;
		int nIndex = 0;
		for (std::vector<CString>::iterator it = vectorVidComp.begin(); it != vectorVidComp.end(); it++)
		{
			CString sName(*it);
			m_ctrlCBCompressor.AddString(sName);
			if(m_cOpts.m_sCompressorName.compare(sName) == 0)
			{
				nSelectedComp = nIndex;
			}
			nIndex++;
		}
		if(nSelectedComp < 0)
			m_ctrlCBCompressor.SetCurSel(0);
		else
			m_ctrlCBCompressor.SetCurSel(nSelectedComp);
	}
	ReInitializeVidCompFilter();
	RefreshCompressorButtons();
}

void CVideoOptionsDlg::LoadVideoCaptureFilters()
{
	m_ctrlCBVCF.ResetContent();
	CSystemDeviceEnumerator sdEnum;
	std::vector<CString> vectorVidCap;

	HRESULT hr = sdEnum.EnumerateVideoCaptureDevices(vectorVidCap);
	if(SUCCEEDED(hr) && vectorVidCap.size() > 0)
	{
		int nSelectedComp = -1;
		int nIndex = 0;
		for (std::vector<CString>::iterator it = vectorVidCap.begin(); it != vectorVidCap.end(); it++)
		{
			CString sName(*it);
			m_ctrlCBVCF.AddString(sName);
			if(m_cOpts.m_sCaptureFilterName.compare(sName) == 0)
			{
				nSelectedComp = nIndex;
			}
			nIndex++;
		}
		if(nSelectedComp < 0)
			m_ctrlCBVCF.SetCurSel(0);
		else
			m_ctrlCBVCF.SetCurSel(nSelectedComp);
	}
	else
	{
		m_ctrlCBVCF.AddString("None Available");
		m_ctrlCBVCF.SetCurSel(0);
	}
	ReInitializeVidCapFilter();
	RefreshVidCapConfigButton();
}
void CVideoOptionsDlg::AutoSetRateWithLock(int val, int& framerate, int& delayms)
{
	int nStep = std::ceil((float)UPPER_BOUND / NUMBER_OF_PROFILES);
	for(int i = 0, ii = LOWER_BOUND; i <= UPPER_BOUND; i++,ii+=nStep)
	{
		if(val >= ii && val <= (ii+nStep))
		{
			delayms = m_arrCaptureRate[i];
			framerate = m_arrPlaybackRate[i];
			break;
		}
	}

}
void CVideoOptionsDlg::InitAutoAdjustArray()
{
	int framerate = 0;
	int delayms = 0;
	for(int i = 0; i< 100;i++)
	{
		AutoSetRate(i+1, framerate, delayms);
		m_arrAutoSetCaptureRate[i] = delayms;
		m_arrAutoSetPlaybackRate[i] = framerate;
	}
}
//void AutoSetRate(int val,int& framerate,int& delayms);
void CVideoOptionsDlg::AutoSetRate(int val, int& framerate, int& delayms)
{
	if (val <= 17) {
		//fps more than 1 per second
		framerate = 200 - ((val - 1) * 10); //framerate 200 to 40;
		//1 corr to 200, 17 corr to 40

		delayms = 1000/framerate;

	} else if (val <= 56) {
		//fps more than 1 per second
		framerate = (57 - val); //framerate 39 to 1;
		//18 corr to 39, 56 corr to 1

		delayms = 1000/framerate;
	} else if (val <= 86) { //assume iTimeLapse
		framerate = 20;
		delayms = (val-56) * 1000;

		//57 corr to 1000, 86 corr to 30000 (20 seconds)
	} else if (val<=99) { //assume iTimeLapse
		framerate = 20;
		delayms = (val-86) * 2000 + 30000;

		//87 corr to 30000, 99 corr to 56000 (56 seconds)
	} else {
		//val=100, iTimeLapse
		framerate = 20;
		delayms = 60000;

		//100 corr to 60000
	}
}

void CVideoOptionsDlg::RefreshAutoOptions()
{
	m_ctrlEdiPlaybackRate.EnableWindow(!m_cOpts.m_bAutoAdjust);
	m_ctrlEditKeyFrameInterval.EnableWindow(!m_cOpts.m_bAutoAdjust);
	m_ctrlEditCaptureInterval.EnableWindow(!m_cOpts.m_bAutoAdjust);
	m_ctrlSliderAdjust.EnableWindow(m_cOpts.m_bAutoAdjust);
}

void CVideoOptionsDlg::UpdateAdjustSliderVal()
{
	UpdateData();
	if(m_cOpts.m_bAutoAdjust && m_cOpts.m_bLock)
	{
		AutoSetRateWithLock(m_iAdjust, m_iPlaybackRate, m_iCaptureInterval);
	}
	else
	{
		AutoSetRate(m_iAdjust, m_iPlaybackRate, m_iCaptureInterval);
		//m_iKeyFrameInterval = m_iPlaybackRate;
	}
	UpdateData(FALSE);
}
void CVideoOptionsDlg::DDV_KeyFrameInterval(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	if ((value < minVal) || (maxVal < value)) {
		if (pDX->m_bSaveAndValidate) {
			// "Key frames cannot be set for every %d frames. Please enter a value in the range 1 to 200."
			MessageOut(*(pDX->m_pDlgWnd), IDS_STRING_KEYFRAMES1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, value);
			pDX->PrepareEditCtrl(IDC_KEYFRAMES);
			pDX->Fail();
		} else {
			m_iKeyFrameInterval = (value < minVal) ? minVal : maxVal;
		}
	}
}

void CVideoOptionsDlg::DDV_CaptureInterval(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	if (value < minVal) {
		if (pDX->m_bSaveAndValidate) {
			if (!m_ctrlButtonAutoAdjust.GetCheck()) {
				//"Timelapse for each frame cannot be less than 0 milliseconds."
				MessageOut(*(pDX->m_pDlgWnd), IDS_STRING_TIMELAPSELESS, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
				pDX->PrepareEditCtrl(IDC_KEYFRAMES2);
				pDX->Fail();
			}
		}
		m_iCaptureInterval = minVal;
		return;
	}

	if (maxVal < value) {
		if (pDX->m_bSaveAndValidate) {
			if (!m_ctrlButtonAutoAdjust.GetCheck()) {
				//MessageBox("Timelapse for each frame cannot be more than 7200000 milliseconds (2 hours).","Note",MB_OK);
				MessageOut(*(pDX->m_pDlgWnd), IDS_STRING_TIMELAPSEMORE, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
				pDX->PrepareEditCtrl(IDC_KEYFRAMES2);
				pDX->Fail();
			}
		}
		m_iCaptureInterval = maxVal;
		return;
	}
}

void CVideoOptionsDlg::DDV_PlaybackRate(CDataExchange* pDX, int value, int minVal, int maxVal)
{
	if ((value < minVal) || (maxVal < value)) {
		if (pDX->m_bSaveAndValidate) {
			if (!m_ctrlButtonAutoAdjust.GetCheck()) {
				// "Playback Rate cannot be set to %d frames per second. Please enter a value in the range 1 to 200."
				MessageOut(*(pDX->m_pDlgWnd), IDS_STRING_PLAYBACKRATE, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, value);
				pDX->PrepareEditCtrl(IDC_FPS);
				pDX->Fail();
			}
		}
		m_iPlaybackRate = (value < minVal) ? minVal : maxVal;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsDlg message handlers

BOOL CVideoOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_ctrlSliderQuality.SetRange(1, 100, TRUE);
	AdjustSliderRange();
	GetCurrentSliderPos();
	m_ctrlButtonAutoAdjust.SetCheck(m_cOpts.m_bAutoAdjust);
	m_ctrlButtonLock.SetCheck(m_cOpts.m_bLock);
	m_bInfo.LoadBitmap(IDB_BITMAP10);
	HBITMAP hBitmap= (HBITMAP) m_bInfo.GetSafeHandle();
	m_ctrlButtonInfo.SetBitmap(hBitmap);
	RefreshAutoOptions();
	AdjustSliderRange();
	LoadVideoCaptureFilters();
	LoadVideoCompressors();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoOptionsDlg::GetCurrentSliderPos()
{
	if(m_cOpts.m_bAutoAdjust && m_cOpts.m_bLock)
	{
		m_ctrlSliderAdjust.SetPos(m_iCurrentSliderPos = CurrentLockPosition());
	}
	else
	{
		m_ctrlSliderAdjust.SetPos(m_iCurrentSliderPos = CurrentAutoAdjustPosition());
	}
}
void CVideoOptionsDlg::AdjustSliderRange()
{
	m_iCurrentSliderPos = m_ctrlSliderAdjust.GetPos();
	if(m_cOpts.m_bAutoAdjust && m_cOpts.m_bLock)
	{
		if(m_ctrlSliderAdjust.GetRangeMax() == 100)
			m_iCurrentSliderPos = m_iCurrentSliderPos * 10;
		m_ctrlSliderAdjust.SetRange(1, 1000, TRUE);
		m_ctrlSliderAdjust.SetTicFreq(63);
		m_ctrlSliderAdjust.SetPos(m_iCurrentSliderPos);
	}
	else
	{
		if(m_ctrlSliderAdjust.GetRangeMax() == 1000)
		{
			if(m_iCurrentSliderPos >= 10)
				m_iCurrentSliderPos = abs(m_iCurrentSliderPos / 10);
		}
		m_ctrlSliderAdjust.SetRange(1, 100, TRUE);
		m_ctrlSliderAdjust.SetTicFreq(10);
		m_ctrlSliderAdjust.SetPos(m_iCurrentSliderPos);
	}
}
void CVideoOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateData();
	m_iStaticQuality = m_iQuality;
	UpdateData(FALSE);
	if (m_cOpts.m_bAutoAdjust)
		UpdateAdjustSliderVal();	// TODO: This is not affected by the quality value!
}
int CVideoOptionsDlg::CurrentLockPosition()
{
	int pos = 0;//m_iCurrentSliderPos;
	for(int i = 0; i < sizeof(m_arrCaptureRate)/sizeof(m_arrCaptureRate[0]);i++)
	{
		if(m_iCaptureInterval == m_arrCaptureRate[i] || (m_iCaptureInterval <= m_arrCaptureRate[i] && m_iCaptureInterval >= m_arrCaptureRate[i+1]))
		{
			pos= (i+1) * 63;
			break;
		}
	}
	return pos;
}
int CVideoOptionsDlg::CurrentAutoAdjustPosition()
{
	int pos = 0;//m_iCurrentSliderPos;
	for(int i = 0; i < sizeof(m_arrAutoSetCaptureRate)/sizeof(m_arrAutoSetCaptureRate[0]);i++)
	{
		if(m_iCaptureInterval == m_arrAutoSetCaptureRate[i])
			return i+1;
		if(m_iCaptureInterval <= m_arrAutoSetCaptureRate[i] && m_iCaptureInterval >= m_arrAutoSetCaptureRate[i +1])
			return i+1;
	}
	return pos;
}
void CVideoOptionsDlg::OnOK()
{
	if (!UpdateData()) {
		return;
	}
	if(m_ctrlButtonLock.GetCheck())
	{
		if((m_iCaptureInterval * m_iPlaybackRate) != 1000)
		{
			MessageBox("Capture and Playback Rate lock is enabled.\n Values must be multipliers of 1000", "CamStudio", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}
	m_cOpts.m_sCaptureFilterName = GetCurrentVideoDevice();
	m_cOpts.m_sCompressorName = GetCurrentVideoCompressor();
	m_cOpts.m_iCompQuality = m_iQuality;
	m_cOpts.m_iFramesPerSecond = m_iPlaybackRate;
	m_cOpts.m_iKeyFramesEvery = m_iKeyFrameInterval;
	m_cOpts.m_iTimeLapse = m_iCaptureInterval;
	m_cOpts.m_bAutoAdjust = m_ctrlButtonAutoAdjust.GetCheck() ? true : false;
	m_cOpts.m_bLock = m_ctrlButtonLock.GetCheck() ? true : false;
	m_cOpts.m_iValueAdjust = m_iAdjust;
	m_cOpts.m_iSelectedCompressor = -1;
	CDialog::OnOK();
}
void CVideoOptionsDlg::ReInitializeVidCapFilter()
{
	if(m_pVidCapFilter)
		delete m_pVidCapFilter;
	m_pVidCapFilter = new CScreenCaptureFilter(GetCurrentVideoDevice());
}
void CVideoOptionsDlg::ReInitializeVidCompFilter()
{
	if(m_pCompFilter)
		delete m_pCompFilter;
	m_pCompFilter = new CVideoCompressorFilter(GetCurrentVideoCompressor());
}
const CString CVideoOptionsDlg::GetCurrentVideoDevice() const
{
	CString sVideoDevice;
	if(m_ctrlCBVCF.GetCurSel() >=0)
	{
		m_ctrlCBVCF.GetLBText(m_ctrlCBVCF.GetCurSel(), sVideoDevice);
	}
	return sVideoDevice;
}
const CString CVideoOptionsDlg::GetCurrentVideoCompressor() const
{
	CString sCompressor;
	if(m_ctrlCBCompressor.GetCurSel() >= 0)
	{
		m_ctrlCBCompressor.GetLBText(m_ctrlCBCompressor.GetCurSel(), sCompressor);
	}
	return sCompressor;
}
void CVideoOptionsDlg::OnAbout()
{
	CAMVfwCompressDialogs vfwCompDialog(this->m_hWnd);
	if(SUCCEEDED(vfwCompDialog.CreateInstance(CLSID_VideoCompressorCategory)))
	{
		vfwCompDialog.DisplayAboutDialog(m_pCompFilter);
	}
}

void CVideoOptionsDlg::OnSelchangeCompressors()
{
	ReInitializeVidCompFilter();
	RefreshCompressorButtons();
}

void CVideoOptionsDlg::OnConfigure()
{
	CAMVfwCompressDialogs vfwCompDialog(this->m_hWnd);
	if(SUCCEEDED(vfwCompDialog.CreateInstance(CLSID_VideoCompressorCategory)))
	{
		vfwCompDialog.DisplayConfigDialog(m_pCompFilter);
	}
}

void CVideoOptionsDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CVideoOptionsDlg::OnBnClickedOk()
{
	OnOK();
}

void CVideoOptionsDlg::OnCbnSelchangeVidcap()
{
	ReInitializeVidCapFilter();
	RefreshVidCapConfigButton();
}


void CVideoOptionsDlg::OnBnClickedConfigureVcf()
{
	CAMVfwCompressDialogs vfwCompDialog(this->m_hWnd);
	HRESULT hr = vfwCompDialog.CreateInstance(CLSID_VideoInputDeviceCategory);
	if(SUCCEEDED(hr))
	{
		// // VHScrCap hack
		if(m_pVidCapFilter->FilterName().CompareNoCase("VHScrCap") == 0)
			MessageBox("Can't configure VHScrCap. Use ConfigureVHScrCapDlg32.exe found in VHScrCap installation directory.", "VHScrCap", MB_ICONEXCLAMATION);
		else
			vfwCompDialog.DisplayConfigDialog(m_pVidCapFilter);
	}
}
void CVideoOptionsDlg::OnAuto()
{
	// TODO: Add your control notification handler code here
	m_cOpts.m_bAutoAdjust = m_ctrlButtonAutoAdjust.GetCheck() ? true : false;

	RefreshAutoOptions();
	AdjustSliderRange();
}
void CVideoOptionsDlg::OnLock()
{
	if(m_ctrlButtonLock.GetCheck() == false)
	{
		if(MessageBox("Please note by deselecting this option you increase the risk \nthat the audio and video will desynchronize. Are you sure?", 
			"CamStudio", MB_YESNO | MB_ICONEXCLAMATION ) == IDYES)
		{
			m_ctrlButtonLock.SetCheck(false);
		}
		else
		{
			m_ctrlButtonLock.SetCheck(true);
		}
	}
	m_cOpts.m_bLock = m_ctrlButtonLock.GetCheck() ? true : false;
	RefreshAutoOptions();
	AdjustSliderRange();
	
}

void CVideoOptionsDlg::OnCaptureFrameRateInfo()
{
	MessageBox("Note that the lower the 'Capture Frame' rate figure, the greater the CPU usage will be (i.e. the slower your PC will run) and the larger your video's final filesize will be.", 
		"CamStudio",MB_OK | MB_ICONEXCLAMATION);
}