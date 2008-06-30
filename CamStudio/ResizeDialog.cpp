// ResizeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "ResizeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void ErrMsg(char format[], ...);

/////////////////////////////////////////////////////////////////////////////
// CResizeDialog dialog

CResizeDialog::CResizeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CResizeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResizeDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CResizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizeDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResizeDialog, CDialog)
	//{{AFX_MSG_MAP(CResizeDialog)
	ON_BN_CLICKED(IDRESET, OnReset)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizeDialog message handlers

void CResizeDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CResizeDialog::OnReset() 
{
	if (m_transWnd) {
	
		m_transWnd->widthPos  = 32; //100%
		m_transWnd->heightPos = 32;	
		((CSliderCtrl *) GetDlgItem(IDC_WIDTHSLIDER))->SetPos(m_transWnd->widthPos);
		((CSliderCtrl *) GetDlgItem(IDC_HEIGHTSLIDER))->SetPos(m_transWnd->heightPos);

	
		m_transWnd->RefreshWindowSize();

	}	
	
}

void CResizeDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{

	if (m_transWnd) {
	
		m_transWnd->widthPos  = ((CSliderCtrl *) GetDlgItem(IDC_WIDTHSLIDER))->GetPos();
		m_transWnd->heightPos = ((CSliderCtrl *) GetDlgItem(IDC_HEIGHTSLIDER))->GetPos();	
		m_transWnd->RefreshWindowSize();

	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CResizeDialog::PreModal(CTransparentWnd *transWnd)
{
	m_transWnd = transWnd;

}

BOOL CResizeDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	//WidthHeight	
	if (m_transWnd)
	{
		((CSliderCtrl *) GetDlgItem(IDC_WIDTHSLIDER))->SetRange(0,200);
		((CSliderCtrl *) GetDlgItem(IDC_HEIGHTSLIDER))->SetRange(0,200);
		((CSliderCtrl *) GetDlgItem(IDC_WIDTHSLIDER))->SetPos(m_transWnd->widthPos);
		((CSliderCtrl *) GetDlgItem(IDC_HEIGHTSLIDER))->SetPos(m_transWnd->heightPos);
	}	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
