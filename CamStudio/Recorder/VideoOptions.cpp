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
#include "VCM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ICINFO * pCompressorInfo;
extern int num_compressor;
extern CString strCodec;

//Video Compress Parameters
extern LPVOID pVideoCompressParams;

extern void FreeVideoCompressParams();
extern BOOL AllocVideoCompressParams(DWORD paramsSize);
extern void GetVideoCompressState (HIC hic, DWORD fccHandler);
extern void SetVideoCompressState (HIC hic , DWORD fccHandler);

extern void AutoSetRate(int val, int& framerate, int& delayms);

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
	m_iStaticQuality = m_iQuality;
	m_iPlaybackRate = cVideoOpts.m_iFramesPerSecond;
	m_iKeyFrameInterval = cVideoOpts.m_iKeyFramesEvery;
	m_iCaptureInterval = cVideoOpts.m_iTimeLapse;

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
	DDV_MinMaxInt(pDX, m_iKeyFrameInterval, 1, 200);
	DDX_Text(pDX, IDC_KEYFRAMES2, m_iCaptureInterval);
	DDV_MinMaxInt(pDX, m_iCaptureInterval, 1, 7200000);
	DDX_Text(pDX, IDC_FPS, m_iPlaybackRate);
	DDV_MinMaxInt(pDX, m_iPlaybackRate, 1, 200);
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
		CHIC chic;
		if (chic.Open(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY)) {
			m_ctrlButtonAbout.EnableWindow(ICERR_UNSUPPORTED != chic.QueryAbout());
			m_ctrlButtonConfigure.EnableWindow(ICERR_UNSUPPORTED != chic.QueryConfigure());
		}

		//HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		//if (hic) {
		//	m_ctrlButtonAbout.EnableWindow(ICQueryAbout(hic));
		//	m_ctrlButtonConfigure.EnableWindow(ICQueryConfigure(hic));
		//	ICClose(hic);
		//}
	}
}

void CVideoOptionsDlg::RefreshAutoOptions()
{
	m_ctrlEdiPlaybackRate.EnableWindow(!cVideoOpts.m_bAutoAdjust);
	m_ctrlEditKeyFrameInterval.EnableWindow(!cVideoOpts.m_bAutoAdjust);
	m_ctrlEditCaptureInterval.EnableWindow(!cVideoOpts.m_bAutoAdjust);
	m_ctrlSliderAdjust.EnableWindow(cVideoOpts.m_bAutoAdjust);
	if (cVideoOpts.m_bAutoAdjust) {
		UpdateAdjustSliderVal();
	}
}

void CVideoOptionsDlg::UpdateAdjustSliderVal()
{
	int framerate;
	int delayms;
	int val = m_ctrlSliderAdjust.GetPos();
	AutoSetRate(val, framerate, delayms);

	CString frameratestr;
	CString delaymsstr;
	frameratestr.Format("%d",framerate);
	delaymsstr.Format("%d",delayms);

	m_ctrlEdiPlaybackRate.SetWindowText(frameratestr);
	m_ctrlEditKeyFrameInterval.SetWindowText(frameratestr);
	m_ctrlEditCaptureInterval.SetWindowText(delaymsstr);
}

// LoadICList
// Load the Compressor ComboBox with available compressors 
int CVideoOptionsDlg::LoadICList()
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsDlg message handlers

BOOL CVideoOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ctrlSliderQuality.SetRange(1,100,TRUE);
	m_ctrlSliderAdjust.SetTicFreq( 10 );
	m_ctrlSliderAdjust.SetRange(1, 100,TRUE);
	m_ctrlButtonAutoAdjust.SetCheck(cVideoOpts.m_bAutoAdjust);

	RefreshAutoOptions();

	if (num_compressor > 0) {
		int sel = -1;
		for (int i = 0; i < num_compressor; i++) {
			CString cname(pCompressorInfo[i].szDescription);
			m_ctrlCBCompressor.AddString(cname);
			if (cVideoOpts.m_dwCompfccHandler == pCompressorInfo[i].fccHandler) {
				sel = i;
			}
		}

		if (sel == -1) {
			sel = 0;
			cVideoOpts.m_dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		}

		m_ctrlCBCompressor.SetCurSel(sel);

		RefreshCompressorButtons();
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData();
	m_iStaticQuality = m_iQuality;
	UpdateData(FALSE);
	if (cVideoOpts.m_bAutoAdjust)
		UpdateAdjustSliderVal();	// TODO: This is not affected by the quality value!

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVideoOptionsDlg::OnOK()
{
	if (!UpdateData()) {
		return;
	}
	cVideoOpts.m_iTimeLapse = m_iCaptureInterval;
	cVideoOpts.m_iFramesPerSecond = m_iPlaybackRate;
	cVideoOpts.m_iKeyFramesEvery = m_iKeyFrameInterval;
	cVideoOpts.m_iCompQuality = m_iQuality * 100;
	cVideoOpts.m_bAutoAdjust = m_ctrlButtonAutoAdjust.GetCheck();
	cVideoOpts.m_iValueAdjust = m_iAdjust;
	cVideoOpts.m_iSelectedCompressor = -1;
	int sel = m_ctrlCBCompressor.GetCurSel();
	if (sel != CB_ERR) {
		cVideoOpts.m_dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		strCodec = CString(pCompressorInfo[sel].szDescription);
		cVideoOpts.m_iSelectedCompressor = sel;
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

		HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		if (hic) {
			//Set our current Video Compress State Info into the hic, which will update the ICConfigure Dialog
			SetVideoCompressState(hic, pCompressorInfo[sel].fccHandler);

			ICConfigure(hic, m_hWnd);

			//Get Video Compress State Info from the hic after adjustment with the ICConfigure dialog
			//This will set the external pVideoCompressParams variable which is used by AVICOMPRESSOPTIONS
			//(This means the external variable pVideoCompressParams will be changed even if user press "Cancel")
			GetVideoCompressState(hic, pCompressorInfo[sel].fccHandler);

			ICClose(hic);
		}
	}
}

void CVideoOptionsDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CVideoOptionsDlg::OnAuto()
{
	// TODO: Add your control notification handler code here
	BOOL val = m_ctrlButtonAutoAdjust.GetCheck();
	cVideoOpts.m_bAutoAdjust = val;

	RefreshAutoOptions();
}
