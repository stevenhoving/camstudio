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
, m_myparent(dynamic_cast<CVideoWnd *>(pParent))
{
	//{{AFX_DATA_INIT(CTransRateDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTransRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransRateDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RATESLIDER, m_ctrlSliderTransRate);
	DDX_Control(pDX, IDC_RATETEXT, m_ctrlStaticFrameRate);
}

BEGIN_MESSAGE_MAP(CTransRateDlg, CDialog)
	//{{AFX_MSG_MAP(CTransRateDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransRateDlg message handlers

BOOL CTransRateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (!m_myparent)
		return TRUE;

	m_ctrlSliderTransRate.EnableWindow(TRUE);
	m_ctrlSliderTransRate.SetRange(1,60);
	m_ctrlSliderTransRate.SetPos(m_myparent->m_iRefreshRate);

	CString valstr;
	valstr.Format(_T("%d fps"),m_myparent->m_iRefreshRate);
	m_ctrlStaticFrameRate.SetWindowText(valstr);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTransRateDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CTransRateDlg::OnOK()
{
	CDialog::OnOK();
}

void CTransRateDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int valRate = m_ctrlSliderTransRate.GetPos();

	CString valstr;
	valstr.Format(TEXT("%d fps"), valRate);
	m_ctrlStaticFrameRate.SetWindowText(valstr);

	if (m_myparent) {
		m_myparent->AdjustRefreshRate(valRate);
		//this line should not be put before m_myparent->AdjustRefreshRate(valRate);
		m_myparent->m_iRefreshRate = valRate;
		iRrefreshRate = valRate;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

