// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// AutopanSpeed.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "AutopanSpeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutopanSpeedDlg dialog

CAutopanSpeedDlg::CAutopanSpeedDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutopanSpeedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutopanSpeedDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAutopanSpeedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutopanSpeedDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PANSLIDER, m_ctrlSliderPanSpeed);
	DDX_Control(pDX, IDC_MAXSPEED, m_ctrlStaticMaxSpeed);
}

BEGIN_MESSAGE_MAP(CAutopanSpeedDlg, CDialog)
	//{{AFX_MSG_MAP(CAutopanSpeedDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutopanSpeedDlg message handlers

BOOL CAutopanSpeedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// extra initialization
	// set the slider limits and current position
	const int MAXPANSPEED = 200;
	ASSERT(cProgramOpts.m_iMaxPan <= MAXPANSPEED);
	m_ctrlSliderPanSpeed.SetRange(1, MAXPANSPEED, TRUE);
	m_ctrlSliderPanSpeed.SetPos(cProgramOpts.m_iMaxPan);

	// update the test speed display
	CString maxpanspeedstr;
	maxpanspeedstr.Format("%d", cProgramOpts.m_iMaxPan);
	m_ctrlStaticMaxSpeed.SetWindowText(maxpanspeedstr);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAutopanSpeedDlg::OnOK()
{
	// read the slider position and set the max pan speed
	cProgramOpts.m_iMaxPan = m_ctrlSliderPanSpeed.GetPos();
	CDialog::OnOK();
}

void CAutopanSpeedDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// update the text display
	// FIXME: how to check that we are scrolling our slider?
	// it looks like it is generic message is handled here
	if (SB_THUMBTRACK == nSBCode) { // this (temporarily) fixes reset of the counter to 0 when slider is releazed
		CString maxpanspeedstr;
		maxpanspeedstr.Format("%d", nPos);
		m_ctrlStaticMaxSpeed.SetWindowText(maxpanspeedstr);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
