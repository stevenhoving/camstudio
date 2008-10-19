// ResizeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "ResizeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog

CResizeDlg::CResizeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CResizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResizeDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResizeDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CResizeDlg, CDialog)
	//{{AFX_MSG_MAP(CResizeDlg)
	ON_BN_CLICKED(IDRESET, OnReset)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg message handlers

void CResizeDlg::OnOK()
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

void CResizeDlg::OnReset()
{
	if (m_transWnd) {

		m_transWnd->widthPos = 32; //100%
		m_transWnd->heightPos = 32;
		((CSliderCtrl *) GetDlgItem(IDC_WIDTHSLIDER))->SetPos(m_transWnd->widthPos);
		((CSliderCtrl *) GetDlgItem(IDC_HEIGHTSLIDER))->SetPos(m_transWnd->heightPos);

		m_transWnd->RefreshWindowSize();

	}

}

void CResizeDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	if (m_transWnd) {

		m_transWnd->widthPos = ((CSliderCtrl *) GetDlgItem(IDC_WIDTHSLIDER))->GetPos();
		m_transWnd->heightPos = ((CSliderCtrl *) GetDlgItem(IDC_HEIGHTSLIDER))->GetPos();
		m_transWnd->RefreshWindowSize();

	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CResizeDlg::PreModal(CTransparentWnd *transWnd)
{
	m_transWnd = transWnd;

}

BOOL CResizeDlg::OnInitDialog()
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

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
