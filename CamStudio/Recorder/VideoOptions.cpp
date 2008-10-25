// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// VideoOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "VideoOptions.h"

#include <stdio.h>
#include <mmsystem.h>
#include <vfw.h>
#include <windowsx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ICINFO * pCompressorInfo;
extern int num_compressor;
extern int selected_compressor;
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
{
	//{{AFX_DATA_INIT(CVideoOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CVideoOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoOptionsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
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

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsDlg message handlers

void CVideoOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	int quality;
	CString qualitystr;

	quality = ((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->GetPos();
	qualitystr.Format("%d",quality);
	((CStatic *) GetDlgItem(IDC_QUALITY))->SetWindowText(qualitystr);

	if (bAutoAdjust)
		UpdateAdjustSliderVal();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVideoOptionsDlg::OnOK()
{
	// TODO: Add extra validation here
	CString fps;
	CString keyframes;
	CString timelp;
	CString qualitystr;

	((CEdit *) GetDlgItem(IDC_FPS))->GetWindowText(fps);
	((CEdit *) GetDlgItem(IDC_KEYFRAMES))->GetWindowText(keyframes);
	((CEdit *) GetDlgItem(IDC_KEYFRAMES2))->GetWindowText(timelp);
	((CStatic *) GetDlgItem(IDC_QUALITY))->GetWindowText(qualitystr);

	int own_timelapse;
	int own_frames_per_second;
	int own_keyFramesEvery;
	int quality;

	//Can into local variables first
	sscanf_s(LPCTSTR(fps),"%d",&own_frames_per_second);
	sscanf_s(LPCTSTR(keyframes),"%d",&own_keyFramesEvery);
	sscanf_s(LPCTSTR(timelp),"%d",&own_timelapse);
	sscanf_s(LPCTSTR(qualitystr),"%d",&quality);

	if (own_timelapse<0) {
		//MessageBox("Timelapse for each frame cannot be less than 0 milliseconds.","Note",MB_OK);
		MessageOut(this->m_hWnd,IDS_STRING_TIMELAPSELESS ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (own_timelapse>7200000) {
		//MessageBox("Timelapse for each frame cannot be more than 7200000 milliseconds (2 hours).","Note",MB_OK);
		MessageOut(this->m_hWnd,IDS_STRING_TIMELAPSEMORE ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if ((own_keyFramesEvery<1) || (own_keyFramesEvery>200)) {
		//char tempstr[300];
		//sprintf(tempstr,"Key frames cannot be set for every %d frames. Please enter a value in the range 1 to 200.",own_keyFramesEvery);
		//MessageBox(tempstr,"Note",MB_OK);

		MessageOut(this->m_hWnd,IDS_STRING_KEYFRAMES1, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,own_keyFramesEvery);

		return;
	}

	if ((own_frames_per_second<1) || (own_frames_per_second>200)) {
		//char tempstr[300];
		//sprintf(tempstr,"Playback Rate cannot be set to %d frames per second. Please enter a value in the range 1 to 200.",own_frames_per_second);
		//MessageBox(tempstr,"Note",MB_OK);

		MessageOut(this->m_hWnd,IDS_STRING_PLAYBACKRATE, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,own_frames_per_second);
		return;
	}

	//Verification Passed..Setting Global Values
	iTimeLapse = own_timelapse;
	iFramesPerSecond =own_frames_per_second;
	iKeyFramesEvery =own_keyFramesEvery;

	iCompQuality = quality * 100;

	int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
	if (sel != CB_ERR) {
		dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		strCodec = CString(pCompressorInfo[sel].szDescription);
		selected_compressor = sel;
	}
	//else
	// selected_compressor = -1;

	//Ver 1.2
	bAutoAdjust = ((CButton *) GetDlgItem(IDC_AUTO))->GetCheck();
	iValueAdjust = ((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->GetPos();

	CDialog::OnOK();
}

BOOL CVideoOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	CString fps;
	CString keyframes;
	CString timelp;
	CString qualitystr;
	int quality;

	quality = iCompQuality/100;

	fps.Format("%d",iFramesPerSecond);
	keyframes.Format("%d",iKeyFramesEvery);
	timelp.Format("%d",iTimeLapse);
	qualitystr.Format("%d",quality);

	((CEdit *) GetDlgItem(IDC_FPS))->SetWindowText(fps);
	((CEdit *) GetDlgItem(IDC_KEYFRAMES))->SetWindowText(keyframes);
	((CEdit *) GetDlgItem(IDC_KEYFRAMES2))->SetWindowText(timelp);
	((CStatic *) GetDlgItem(IDC_QUALITY))->SetWindowText(qualitystr);

	((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->SetRange(1,100,TRUE);
	((CSliderCtrl *) GetDlgItem(IDC_QUALITY_SLIDER))->SetPos(quality);

	//Ver 1.2
	bAutoAdjust = bAutoAdjust;
	iValueAdjust = iValueAdjust;

	((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->SetTicFreq( 10 );
	((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->SetRange(1,100,TRUE);

	((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->SetPos(iValueAdjust);

	((CButton *) GetDlgItem(IDC_AUTO))->SetCheck(bAutoAdjust);
	RefreshAutoOptions();

	if (num_compressor>0) {

		int sel = -1;
		for (int i=0; i<num_compressor;i++) {

			CString cname(pCompressorInfo[i].szDescription);
			((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->AddString(cname);

			if (dwCompfccHandler == pCompressorInfo[i].fccHandler) {

				sel = i;

			}

		}

		if (sel == -1)
		{
			sel = 0;
			dwCompfccHandler = pCompressorInfo[sel].fccHandler;
		}

		((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->SetCurSel(sel);

		RefreshCompressorButtons();

	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoOptionsDlg::RefreshCompressorButtons()
{

	int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
	if (sel != CB_ERR) {

		HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		if (hic) {

			if (ICQueryAbout(hic))
				((CButton *) GetDlgItem(ID_ABOUT))->EnableWindow(TRUE);
			else
				((CButton *) GetDlgItem(ID_ABOUT))->EnableWindow(FALSE);

			if (ICQueryConfigure(hic))
				((CButton *) GetDlgItem(ID_CONFIGURE))->EnableWindow(TRUE);
			else
				((CButton *) GetDlgItem(ID_CONFIGURE))->EnableWindow(FALSE);

			ICClose(hic);
		}

	}

}

void CVideoOptionsDlg::OnAbout()
{
	// TODO: Add your control notification handler code here
	int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
	if (sel != CB_ERR) {

		HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		if (hic) {

			ICAbout(hic,m_hWnd);
			ICClose(hic);
		}

	}

}

void CVideoOptionsDlg::OnSelchangeCompressors()
{
	// TODO: Add your control notification handler code here
	RefreshCompressorButtons();

}

//Ver 1.2
//Note : Because the program stores the state for only one compressor (in the pVideoCompressParams),
//if the user chooses "Configure" for compressor A, then chooses another compressor B and presses "Configure" again
//the previous state info for compressor A will be lost.
void CVideoOptionsDlg::OnConfigure()
{
	// TODO: Add your control notification handler code here
	int sel = ((CComboBox *) GetDlgItem(IDC_COMPRESSORS))->GetCurSel();
	if (sel != CB_ERR) {

		//Still unable to handle DIVX state (results in error)
		//if (pCompressorInfo[sel].fccHandler==mmioFOURCC('D', 'I', 'V', 'X')) return;

		HIC hic = ICOpen(pCompressorInfo[sel].fccType, pCompressorInfo[sel].fccHandler, ICMODE_QUERY);
		if (hic) {

			//Set our current Video Compress State Info into the hic, which will update the ICConfigure Dialog
			SetVideoCompressState (hic , pCompressorInfo[sel].fccHandler);

			ICConfigure(hic,m_hWnd);

			//Get Video Compress State Info from the hic after adjustment with the ICConfigure dialog
			//This will set the external pVideoCompressParams variable which is used by AVICOMPRESSOPTIONS
			//(This means the external variable pVideoCompressParams will be changed even if user press "Cancel")
			GetVideoCompressState (hic , pCompressorInfo[sel].fccHandler);

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
	BOOL val = ((CButton *) GetDlgItem(IDC_AUTO))->GetCheck();
	if (val)
		bAutoAdjust=1;
	else
		bAutoAdjust=0;

	RefreshAutoOptions();

}

void CVideoOptionsDlg::RefreshAutoOptions()
{

	if (bAutoAdjust) {

		((CEdit *) GetDlgItem(IDC_FPS))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_KEYFRAMES))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_KEYFRAMES2))->EnableWindow(FALSE);
		((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->EnableWindow(TRUE);

		UpdateAdjustSliderVal();

	}
	else {

		((CEdit *) GetDlgItem(IDC_FPS))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_KEYFRAMES))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_KEYFRAMES2))->EnableWindow(TRUE);
		((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->EnableWindow(FALSE);

	}

}

void CVideoOptionsDlg::UpdateAdjustSliderVal()
{
	int framerate;
	int delayms;
	int val = ((CSliderCtrl *) GetDlgItem(IDC_ADJUST))->GetPos();
	AutoSetRate( val, framerate, delayms);

	CString frameratestr;
	CString delaymsstr;

	frameratestr.Format("%d",framerate);
	delaymsstr.Format("%d",delayms);

	((CEdit *) GetDlgItem(IDC_FPS))->SetWindowText(frameratestr);
	((CEdit *) GetDlgItem(IDC_KEYFRAMES))->SetWindowText(frameratestr);
	((CEdit *) GetDlgItem(IDC_KEYFRAMES2))->SetWindowText(delaymsstr);

}

