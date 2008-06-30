// TransRateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "TransRateDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_refreshRate;

/////////////////////////////////////////////////////////////////////////////
// CTransRateDialog dialog


CTransRateDialog::CTransRateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTransRateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransRateDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_myparent = NULL;
}


void CTransRateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransRateDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransRateDialog, CDialog)
	//{{AFX_MSG_MAP(CTransRateDialog)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransRateDialog message handlers


void CTransRateDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	int valRate =  ((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->GetPos();

	CString valstr;
	valstr.Format("%d fps",valRate);
	((CSliderCtrl *) GetDlgItem(IDC_RATETEXT))->SetWindowText(valstr);

	if (m_myparent) {			
		
		m_myparent->AdjustRefreshRate(valRate);
		
		//this line should not be put before m_myparent->AdjustRefreshRate(valRate);
		m_myparent->refreshRate = valRate;
		g_refreshRate = valRate;

	}	

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CTransRateDialog::PreModal( CVideoWnd * parent) 
{	
	m_myparent = parent;	

}

BOOL CTransRateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_myparent)
		return TRUE;
	
	// TODO: Add extra initialization here
	((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->EnableWindow(TRUE);
	((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->SetRange(1,60);
	((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->SetPos(m_myparent->refreshRate);
	
	CString  valstr;
	valstr.Format("%d fps",m_myparent->refreshRate);
	((CSliderCtrl *) GetDlgItem(IDC_RATETEXT))->SetWindowText(valstr);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransRateDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CTransRateDialog::OnOK() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnOK();
}



