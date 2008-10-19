// TransRateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "TransRateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int iRrefreshRate;

/////////////////////////////////////////////////////////////////////////////
// CTransRateDlg dialog

CTransRateDlg::CTransRateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTransRateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransRateDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_myparent = NULL;
}

void CTransRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransRateDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTransRateDlg, CDialog)
	//{{AFX_MSG_MAP(CTransRateDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransRateDlg message handlers

void CTransRateDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	int valRate = ((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->GetPos();

	CString valstr;
	valstr.Format("%d fps",valRate);
	((CSliderCtrl *) GetDlgItem(IDC_RATETEXT))->SetWindowText(valstr);

	if (m_myparent) {

		m_myparent->AdjustRefreshRate(valRate);

		//this line should not be put before m_myparent->AdjustRefreshRate(valRate);
		m_myparent->refreshRate = valRate;
		iRrefreshRate = valRate;

	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTransRateDlg::PreModal( CVideoWnd * parent)
{
	m_myparent = parent;

}

BOOL CTransRateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_myparent)
		return TRUE;

	// TODO: Add extra initialization here
	((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->EnableWindow(TRUE);
	((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->SetRange(1,60);
	((CSliderCtrl *) GetDlgItem(IDC_RATESLIDER))->SetPos(m_myparent->refreshRate);

	CString valstr;
	valstr.Format("%d fps",m_myparent->refreshRate);
	((CSliderCtrl *) GetDlgItem(IDC_RATETEXT))->SetWindowText(valstr);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTransRateDlg::OnCancel()
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}

void CTransRateDlg::OnOK()
{
	// TODO: Add extra cleanup here

	CDialog::OnOK();
}

