// EditTransparency.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "EditTransparency.h"
#include "TransparentWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTransparency dialog


CEditTransparency::CEditTransparency(CWnd* pParent /*=NULL*/)
	: CDialog(CEditTransparency::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditTransparency)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_backup_enableTrans = 0;
	m_backup_valTrans = 50;
}


void CEditTransparency::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditTransparency)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditTransparency, CDialog)
	//{{AFX_MSG_MAP(CEditTransparency)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTransparency message handlers

void CEditTransparency::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	*m_enableTrans = ((CButton *) GetDlgItem(IDC_CHECK1))->GetCheck();

	if (*m_enableTrans) {

				
		((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->EnableWindow(TRUE);

	}
	else
		((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->EnableWindow(FALSE);


	if (m_myparent)
		((CTransparentWnd *) m_myparent)->InvalidateTransparency();
	
}

void CEditTransparency::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	*m_valTrans = ((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->GetPos();

	CString valstr;
	valstr.Format("%d",*m_valTrans);
	((CSliderCtrl *) GetDlgItem(IDC_TRANSTEXT))->SetWindowText(valstr);

	if (m_myparent)
		((CTransparentWnd *) m_myparent)->InvalidateTransparency();
	

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CEditTransparency::PreModal(int *enableTrans, int *valTrans, CWnd * parent) 
{
	m_enableTrans = enableTrans;	
	m_valTrans = valTrans;
	m_myparent = parent;

	m_backup_enableTrans = *enableTrans;
	m_backup_valTrans = *valTrans;

}

BOOL CEditTransparency::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->EnableWindow(TRUE);
	((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->SetRange(0,100);
	((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->SetPos(*m_valTrans);

	((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(*m_enableTrans);

	if (*m_enableTrans)
		((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->EnableWindow(TRUE);
	else
		((CSliderCtrl *) GetDlgItem(IDC_TRANSSLIDER))->EnableWindow(FALSE);


	CString valstr;
	valstr.Format("%d",*m_valTrans);
	((CSliderCtrl *) GetDlgItem(IDC_TRANSTEXT))->SetWindowText(valstr);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditTransparency::OnCancel() 
{
	// TODO: Add extra cleanup here
	*m_enableTrans = m_backup_enableTrans;
	*m_valTrans = m_backup_valTrans ;

	if (m_myparent)
		((CTransparentWnd *) m_myparent)->InvalidateTransparency();

	
	CDialog::OnCancel();
}
