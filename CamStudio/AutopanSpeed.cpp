// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
// 
//
// AutopanSpeed.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "AutopanSpeed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int maxpan;

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
	
	// TODO: Add extra initialization here
	CString maxpanspeedstr;

	((CSliderCtrl *) GetDlgItem(IDC_PANSLIDER))->SetRange(1,200,TRUE);
	((CSliderCtrl *) GetDlgItem(IDC_PANSLIDER))->SetPos(maxpan);
	
	maxpanspeedstr.Format("%d",maxpan);
	((CStatic *) GetDlgItem(IDC_MAXSPEED))->SetWindowText(maxpanspeedstr);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutopanSpeed::OnOK() 
{
	// TODO: Add extra validation here
	int maxpanspeed;
	CString maxpanspeedstr;

	((CStatic *) GetDlgItem(IDC_MAXSPEED))->GetWindowText(maxpanspeedstr);
	sscanf(LPCTSTR(maxpanspeedstr),"%d",&maxpanspeed);

	maxpan = maxpanspeed;
	
	CDialog::OnOK();
}

void CAutopanSpeed::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int maxpanspeed;
	CString maxpanspeedstr;
	
	maxpanspeed = ((CSliderCtrl *) GetDlgItem(IDC_PANSLIDER))->GetPos();
	maxpanspeedstr.Format("%d",maxpanspeed);
	((CStatic *) GetDlgItem(IDC_MAXSPEED))->SetWindowText(maxpanspeedstr);
	
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
