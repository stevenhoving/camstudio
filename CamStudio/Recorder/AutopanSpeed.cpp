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
// CAutopanSpeed dialog

CAutopanSpeed::CAutopanSpeed(CWnd* pParent /*=NULL*/)
: CDialog(CAutopanSpeed::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutopanSpeed)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAutopanSpeed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutopanSpeed)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PANSLIDER, m_ctrlSliderPanSpeed);
	DDX_Control(pDX, IDC_MAXSPEED, m_ctrlStaticMaxSpeed);
}

BEGIN_MESSAGE_MAP(CAutopanSpeed, CDialog)
	//{{AFX_MSG_MAP(CAutopanSpeed)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutopanSpeed message handlers

BOOL CAutopanSpeed::OnInitDialog()
{
	CDialog::OnInitDialog();

	// extra initialization
	// set the slider limits and current position
	const int MAXPANSPEED = 200;
	ASSERT(iMaxPan <= MAXPANSPEED);
	m_ctrlSliderPanSpeed.SetRange(1, MAXPANSPEED, TRUE);
	m_ctrlSliderPanSpeed.SetPos(iMaxPan);

	// update the test speed display
	CString maxpanspeedstr;
	maxpanspeedstr.Format("%d", iMaxPan);
	m_ctrlStaticMaxSpeed.SetWindowText(maxpanspeedstr);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAutopanSpeed::OnOK()
{
	// read the slider position and set the max pan speed
	iMaxPan = m_ctrlSliderPanSpeed.GetPos();
	CDialog::OnOK();
}

void CAutopanSpeed::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// update the text display
	CString maxpanspeedstr;
	maxpanspeedstr.Format("%d", nPos);
	m_ctrlStaticMaxSpeed.SetWindowText(maxpanspeedstr);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
