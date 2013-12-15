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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	//{{AFX_DATA_INIT(CVideoOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iQuality = cVideoOpts.m_iCompQuality/100;
	if (m_iQuality < 1)
		m_iQuality = 1;
	m_iStaticQuality = m_iQuality;
	m_iPlaybackRate = cVideoOpts.m_iFramesPerSecond;
	//if ((m_iPlaybackRate < 1) || (60000 < m_iPlaybackRate))
	//	m_iPlaybackRate = (m_iPlaybackRate < 1) ? 1 : 60000;
	m_iKeyFrameInterval = cVideoOpts.m_iKeyFramesEvery;
	//if ((m_iKeyFrameInterval < 1) || (200 < m_iKeyFrameInterval))
	//	m_iKeyFrameInterval = (m_iKeyFrameInterval < 1) ? 1 : 200;
	m_iCaptureInterval = cVideoOpts.m_iTimeLapse;
	//if ((m_iCaptureInterval < 1) || (200 < m_iCaptureInterval))
	//	m_iCaptureInterval = (m_iCaptureInterval < 1) ? 1 : 200;
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
{
	m_iQuality = cOpts.m_iCompQuality/100;
	if (m_iQuality < 1)
		m_iQuality = 1;
	m_iStaticQuality = m_iQuality;
	m_iPlaybackRate = cOpts.m_iFramesPerSecond;
	//if ((m_iPlaybackRate < 1) || (60000 < m_iPlaybackRate))
	//	m_iPlaybackRate = (m_iPlaybackRate < 1) ? 1 : 60000;
	m_iKeyFrameInterval = cOpts.m_iKeyFramesEvery;
	//if ((m_iKeyFrameInterval < 1) || (200 < m_iKeyFrameInterval))
	//	m_iKeyFrameInterval = (m_iKeyFrameInterval < 1) ? 1 : 200;
	m_iCaptureInterval = cOpts.m_iTimeLapse;
	//if ((m_iCaptureInterval < 1) || (200 < m_iCaptureInterval))
	//	m_iCaptureInterval = (m_iCaptureInterval < 1) ? 1 : 200;
	InitAutoAdjustArray();
}

void CVideoOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoOptionsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMPRESSORS, m_ctrlCBCompressor);
	DDX_Control(pDX, ID_ABOUT, m_ctrlButtonAbout);
	DDX_Control(pDX, ID_CONFIGURE, m_ctrlButtonConfigure);
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
	DDX_Control(pDX, IDC_SUPPORTROUNDDOWN, m_ctrlButtonRoundDown);
}

BEGIN_MESSAGE_MAP(CVideoOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoOptionsDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_ABOUT, OnAbout)
	ON_CBN_SELCHANGE(IDC_COMPRESSORS, OnSelchangeCompressors)
	ON_BN_CLICKED(ID_CONFIGURE, OnConfigure)
	ON_BN_CLICKED(IDC_AUTO, OnAuto)
	ON_BN_CLICKED(IDC_LOCK, OnLock)
	ON_BN_CLICKED(IDC_CFE_INFO, OnCaptureFrameRateInfo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SUPPORTROUNDDOWN, &CVideoOptionsDlg::OnBnClickedSupportrounddown)
	ON_BN_CLICKED(IDOK, &CVideoOptionsDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void CVideoOptionsDlg::RefreshCompressorButtons()
{
	int sel = m_ctrlCBCompressor.GetCurSel();
	if (sel != CB_ERR) {
		BOOL bEnableAbout = FALSE;
		BOOL bEnableCfg = FALSE;
		CHIC chic;
		if (chic.Open(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY)) {
			//bEnableAbout = (ICERR_UNSUPPORTED != chic.QueryAbout());
			//bEnableCfg = (ICERR_UNSUPPORTED != chic.QueryConfigure());
			bEnableAbout = chic.QueryAbout() ? TRUE : FALSE;
			bEnableCfg = chic.QueryConfigure() ? TRUE : FALSE;
		}
		m_ctrlButtonAbout.EnableWindow(bEnableAbout);
		m_ctrlButtonConfigure.EnableWindow(bEnableCfg);

		//HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		//if (hic) {
		//	m_ctrlButtonAbout.EnableWindow(ICQueryAbout(hic));
		//	m_ctrlButtonConfigure.EnableWindow(ICQueryConfigure(hic));
		//	ICClose(hic);
		//}
	}
}
void CVideoOptionsDlg::AutoSetRateWithLock(int val, int& framerate, int& delayms)
{
	if(val >= 1 && val <=63)
	{
		// 1000
		delayms = m_arrCaptureRate[0];
		// 1
		framerate = m_arrPlaybackRate[0];
	}
	else if(val > 63 && val <= 126)
	{
		// 500
		delayms = m_arrCaptureRate[1];
		// 2
		framerate = m_arrPlaybackRate[1];
	}
	else if(val > 126 && val <= 189)
	{
		//250
		delayms = m_arrCaptureRate[2];
		//4
		framerate = m_arrPlaybackRate[2];
	}
	else if(val > 189 && val <= 252)
	{
		//200
		delayms = m_arrCaptureRate[3];
		//5
		framerate = m_arrPlaybackRate[3];
	}
	else if(val > 252 && val <= 315)
	{
		//125
		delayms = m_arrCaptureRate[4];
		//8
		framerate = m_arrPlaybackRate[4];
	}
	else if(val > 315 && val <= 378)
	{
		//100
		delayms = m_arrCaptureRate[5];
		//10
		framerate = m_arrPlaybackRate[5];
	}
	else if(val > 378 && val <= 441)
	{
		//50
		delayms = m_arrCaptureRate[6];
		//20
		framerate = m_arrPlaybackRate[6];
	}
	else if(val > 441 && val <= 504)
	{
		//40
		delayms = m_arrCaptureRate[7];
		//25
		framerate = m_arrPlaybackRate[7];
	}
	else if(val > 504 && val <= 567)
	{
		//25
		delayms = m_arrCaptureRate[8];
		//40
		framerate = m_arrPlaybackRate[8];
	}
	else if(val > 567 && val <= 630)
	{
		//20
		delayms = m_arrCaptureRate[9];
		//50
		framerate = m_arrPlaybackRate[9];
	}
	else if(val > 630 && val <= 693)
	{
		//10
		delayms = m_arrCaptureRate[10];
		//100
		framerate = m_arrPlaybackRate[10];
	}
	else if( val > 693 && val <= 756)
	{
		//8
		delayms = m_arrCaptureRate[11];
		//125
		framerate = m_arrPlaybackRate[11];
	}
	else if(val > 756 && val <= 819)
	{
		//5
		delayms = m_arrCaptureRate[12];
		//200
		framerate = m_arrPlaybackRate[12];
	}
	else if(val > 819 && val <= 882)
	{
		//4
		delayms = m_arrCaptureRate[13];
		//250
		framerate = m_arrPlaybackRate[13];
	}
	else if(val > 882 && val <= 945)
	{
		//2
		delayms = m_arrCaptureRate[14];
		//500
		framerate = m_arrPlaybackRate[14];
	}
	else if(val > 945 && val <=1000)
	{
		//1
		delayms = m_arrCaptureRate[15];
		//10000
		framerate = m_arrPlaybackRate[15];
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

		////Automatically Adjust the Quality for MSVC (MS Video 1) if the framerate is too high
		//int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
		//if (pCompressorInfo[sel].fccHandler==mmioFOURCC('M', 'S', 'V', 'C')) {
		//	int cQuality = ((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->GetPos();
		//	if (cQuality<80) {
		//		((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->SetPos(80);
		//	}
		//}
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
	//if (m_cOpts.m_bAutoAdjust) {
	//	UpdateAdjustSliderVal();
	//}
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

// LoadICList
// Load the Compressor ComboBox with available compressors
int CVideoOptionsDlg::LoadICList()
{
	m_ctrlCBCompressor.ResetContent();
	CHIC::TRACEFOURCC(m_cOpts.m_dwCompfccHandler);
	if (0 < num_compressor) {
		int sel = -1;
		for (int i = 0; i < num_compressor; i++) {
			m_ctrlCBCompressor.AddString(CString(pCompressorInfo[i].szDescription));
			CHIC::TRACEFOURCC(pCompressorInfo[i].fccHandler, CString(pCompressorInfo[i].szDescription));
			if (m_cOpts.m_dwCompfccHandler == pCompressorInfo[i].fccHandler) {
				sel = i;
			}
		}

		if (sel == -1) {
			sel = 0;
			m_cOpts.m_dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		}

		m_ctrlCBCompressor.SetCurSel(sel);
	}

	return num_compressor;
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
	m_ctrlButtonRoundDown.SetCheck(m_cOpts.m_bRoundDown);
	m_bInfo.LoadBitmap(IDB_BITMAP10);
	HBITMAP hBitmap= (HBITMAP) m_bInfo.GetSafeHandle();
	m_ctrlButtonInfo.SetBitmap(hBitmap);
	RefreshAutoOptions();
	LoadICList();
	RefreshCompressorButtons();
	AdjustSliderRange();
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
	if(m_ctrlButtonLock.GetCheck() == true)
	{
		if((m_iCaptureInterval * m_iPlaybackRate) != 1000)
		{
			MessageBox("Capture and Playback Rate lock is enabled.\n Values must be multipliers of 1000", "CamStudio", MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	}
	m_cOpts.m_iCompQuality = m_iQuality * 100;
	m_cOpts.m_iFramesPerSecond = m_iPlaybackRate;
	m_cOpts.m_iKeyFramesEvery = m_iKeyFrameInterval;
	m_cOpts.m_iTimeLapse = m_iCaptureInterval;
	m_cOpts.m_bAutoAdjust = m_ctrlButtonAutoAdjust.GetCheck() ? true : false;
	m_cOpts.m_bLock = m_ctrlButtonLock.GetCheck() ? true : false;
	m_cOpts.m_iValueAdjust = m_iAdjust;
	m_cOpts.m_iSelectedCompressor = -1;
	int sel = m_ctrlCBCompressor.GetCurSel();
	if (sel != CB_ERR) {
		m_cOpts.m_dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		CHIC::TRACEFOURCC(m_cOpts.m_dwCompfccHandler);
		strCodec = CString(pCompressorInfo[sel].szDescription);
		m_cOpts.m_iSelectedCompressor = sel;
	}
	m_cOpts.m_bRoundDown = m_ctrlButtonRoundDown.GetCheck() ? true : false;
	CDialog::OnOK();	// call base;

}

void CVideoOptionsDlg::OnAbout()
{
	int sel = m_ctrlCBCompressor.GetCurSel();
	if (sel != CB_ERR) {
		CHIC chic;
		if (chic.Open(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY)) {
			VERIFY(ICERR_OK == chic.About(m_hWnd));
		}
	}
}

void CVideoOptionsDlg::OnSelchangeCompressors()
{
	RefreshCompressorButtons();
}

//Ver 1.2
// Note : Because the program stores the state for only one compressor (in the
// pVideoCompressParams), if the user chooses "Configure" for compressor A, then
// chooses another compressor B and presses "Configure" again the previous state
// info for compressor A will be lost.
void CVideoOptionsDlg::OnConfigure()
{
	int sel = m_ctrlCBCompressor.GetCurSel();
	if (sel != CB_ERR) {
		// Still unable to handle DIVX state (results in error)
		//if (pCompressorInfo[sel].fccHandler == mmioFOURCC('D', 'I', 'V', 'X'))
		//	return;

//TODO, How long is this code stil experimental and soo in use ???
#define EXPERIMENTAL_CODE
#ifdef EXPERIMENTAL_CODE
		CHIC chic;
		if (chic.Open(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY)) {
			// Set our current Video Compress State Info into the hic,
			// which will update the ICConfigure Dialog
			SetVideoCompressState(chic, chic.Handler());
			chic.Configure(m_hWnd);

			// Get Video Compress State Info from the hic after adjustment with the
			// ICConfigure dialog. This will set the external pVideoCompressParams
			// variable which is used by AVICOMPRESSOPTIONS (This means the external
			// variable pVideoCompressParams will be changed even if user press "Cancel")
			DWORD dwSize = chic.GetStateSize();
			LRESULT lResult = chic.GetState(m_cOpts.State(dwSize), dwSize);

			// if (lResult != dwSize)  ==> C4244 Warning, type mismatch
			if ( lResult - dwSize != 0 ) // Save
			{
				m_cOpts.State(0L);
			}
		}
#else
		HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		if (hic) {
			// Set our current Video Compress State Info into the hic, which will
			// update the ICConfigure Dialog
			SetVideoCompressState(hic, pCompressorInfo[sel].fccHandler);

			ICConfigure(hic, m_hWnd);

			// Get Video Compress State Info from the hic after adjustment with the
			// ICConfigure dialog. This will set the external pVideoCompressParams
			// variable which is used by AVICOMPRESSOPTIONS (This means the external
			// variable pVideoCompressParams will be changed even if user press "Cancel")
			GetVideoCompressState(hic, pCompressorInfo[sel].fccHandler);

			ICClose(hic);
		}
#endif
#undef EXPERIMENTAL_CODE
	}
}

void CVideoOptionsDlg::OnCancel()
{
	CDialog::OnCancel();
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
	MessageBox("Note that the lower the 'Capture Frame' rate figure, the greater\ the CPU usage will be (i.e. the slower your PC will run) and the larger\ your video's final filesize will be.", 
		"CamStudio",MB_OK | MB_ICONEXCLAMATION);
}



void CVideoOptionsDlg::OnBnClickedSupportrounddown()
{
}


void CVideoOptionsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();

}
