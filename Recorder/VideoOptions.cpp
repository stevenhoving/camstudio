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
{
	//{{AFX_DATA_INIT(CVideoOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_iQuality = cVideoOpts.m_iCompQuality/100;
	if (m_iQuality < 1)
		m_iQuality = 1;
	m_iStaticQuality = m_iQuality;
	m_iPlaybackRate = cVideoOpts.m_iFramesPerSecond;
	if ((m_iPlaybackRate < 1) || (60000 < m_iPlaybackRate))
		m_iPlaybackRate = (m_iPlaybackRate < 1) ? 1 : 60000;
	m_iKeyFrameInterval = cVideoOpts.m_iKeyFramesEvery;
	if ((m_iKeyFrameInterval < 1) || (200 < m_iKeyFrameInterval))
		m_iKeyFrameInterval = (m_iKeyFrameInterval < 1) ? 1 : 200;
	m_iCaptureInterval = cVideoOpts.m_iTimeLapse;
	if ((m_iCaptureInterval < 1) || (200 < m_iCaptureInterval))
		m_iCaptureInterval = (m_iCaptureInterval < 1) ? 1 : 200;
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
	if ((m_iPlaybackRate < 1) || (60000 < m_iPlaybackRate))
		m_iPlaybackRate = (m_iPlaybackRate < 1) ? 1 : 60000;
	m_iKeyFrameInterval = cOpts.m_iKeyFramesEvery;
	if ((m_iKeyFrameInterval < 1) || (200 < m_iKeyFrameInterval))
		m_iKeyFrameInterval = (m_iKeyFrameInterval < 1) ? 1 : 200;
	m_iCaptureInterval = cOpts.m_iTimeLapse;
	if ((m_iCaptureInterval < 1) || (200 < m_iCaptureInterval))
		m_iCaptureInterval = (m_iCaptureInterval < 1) ? 1 : 200;
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
	DDX_Control(pDX, IDC_ADJUST, m_ctrlSliderAdjust);
	DDX_Slider(pDX, IDC_QUALITY_SLIDER, m_iQuality);
	DDX_Text(pDX, IDC_KEYFRAMES, m_iKeyFrameInterval);
	::DDV_KeyFrameInterval(pDX, m_iKeyFrameInterval, 1, 200);
	DDX_Text(pDX, IDC_KEYFRAMES2, m_iCaptureInterval);
	::DDV_CaptureInterval(pDX, m_iCaptureInterval, 1, 60000);
	DDX_Text(pDX, IDC_FPS, m_iPlaybackRate);
	::DDV_PlaybackRate(pDX, m_iPlaybackRate, 1, 200);
	DDX_Slider(pDX, IDC_ADJUST, m_iAdjust);
	DDX_Text(pDX, IDC_QUALITY, m_iStaticQuality);
}

BEGIN_MESSAGE_MAP(CVideoOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoOptionsDlg)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(ID_ABOUT, OnAbout)
	ON_CBN_SELCHANGE(IDC_COMPRESSORS, OnSelchangeCompressors)
	ON_BN_CLICKED(ID_CONFIGURE, OnConfigure)
	ON_BN_CLICKED(IDC_AUTO, OnAuto)
	//}}AFX_MSG_MAP
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
	if (m_cOpts.m_bAutoAdjust) {
		UpdateAdjustSliderVal();
	}
}

void CVideoOptionsDlg::UpdateAdjustSliderVal()
{
	UpdateData();
	AutoSetRate(m_iAdjust, m_iPlaybackRate, m_iCaptureInterval);
	m_iKeyFrameInterval = m_iPlaybackRate;
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
	m_ctrlSliderAdjust.SetRange(1, 100, TRUE);
	m_ctrlSliderAdjust.SetTicFreq(10);
	m_ctrlSliderAdjust.SetPos(m_cOpts.m_iValueAdjust);
	m_ctrlButtonAutoAdjust.SetCheck(m_cOpts.m_bAutoAdjust);

	RefreshAutoOptions();
	LoadICList();
	RefreshCompressorButtons();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
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

void CVideoOptionsDlg::OnOK()
{
	if (!UpdateData()) {
		return;
	}

	m_cOpts.m_iCompQuality = m_iQuality * 100;
	m_cOpts.m_iFramesPerSecond = m_iPlaybackRate;
	m_cOpts.m_iKeyFramesEvery = m_iKeyFrameInterval;
	m_cOpts.m_iTimeLapse = m_iCaptureInterval;
	m_cOpts.m_bAutoAdjust = m_ctrlButtonAutoAdjust.GetCheck() ? true : false;
	m_cOpts.m_iValueAdjust = m_iAdjust;
	m_cOpts.m_iSelectedCompressor = -1;
	int sel = m_ctrlCBCompressor.GetCurSel();
	if (sel != CB_ERR) {
		m_cOpts.m_dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		CHIC::TRACEFOURCC(m_cOpts.m_dwCompfccHandler);
		strCodec = CString(pCompressorInfo[sel].szDescription);
		m_cOpts.m_iSelectedCompressor = sel;
	}
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
}

