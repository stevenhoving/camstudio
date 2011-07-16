// EditTransparency.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "EditTransparency.h"
#include "TransparentWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTransparencyDlg dialog

// The standard ctor is private and inaccessible by design
CEditTransparencyDlg::CEditTransparencyDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEditTransparencyDlg::IDD, pParent)
, m_rbEnableTrans(m_bEnableTransOld)
, m_bEnableTransOld(false)
, m_riLevel(m_iLevelOld)
, m_iLevelOld(50)
{
	//{{AFX_DATA_INIT(CEditTransparencyDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

}

/////////////////////////////////////////////////////////////////////////////
// CEditTransparencyDlg ctor
// TODO: The ctor here is clumsy but better than it was.
// The original version used pointers in order to provide feedback through
// the the variables and InvalidateTransparency(). The proper way to have
// done this would have been to pass the values in InvalidateTransparency
// call.
/////////////////////////////////////////////////////////////////////////////
CEditTransparencyDlg::CEditTransparencyDlg(bool& bEnable, int& iLevel, CTransparentWnd* pParent)
: CDialog(CEditTransparencyDlg::IDD, pParent)
, m_rbEnableTrans(bEnable)
, m_bEnableTransOld(bEnable)
, m_riLevel(iLevel)
, m_iLevelOld(iLevel)
, m_pTransparentWnd(pParent)
{
}

void CEditTransparencyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditTransparencyDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK1, m_bEnableTranparency);
	DDX_Control(pDX, IDC_TRANSSLIDER, m_ctrlSliderTransparency);
	DDX_Control(pDX, IDC_TRANSTEXT, m_ctrlStaticTransparency);
}

BEGIN_MESSAGE_MAP(CEditTransparencyDlg, CDialog)
	//{{AFX_MSG_MAP(CEditTransparencyDlg)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTransparencyDlg message handlers

void CEditTransparencyDlg::OnCheck1()
{
	// TODO: Add your control notification handler code here
	m_rbEnableTrans = m_bEnableTranparency.GetCheck();

	m_ctrlSliderTransparency.EnableWindow(m_rbEnableTrans);
	m_pTransparentWnd->InvalidateTransparency();
}

void CEditTransparencyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	m_riLevel = m_ctrlSliderTransparency.GetPos();

	CString valstr;
	valstr.Format("%d", m_riLevel);
	m_ctrlStaticTransparency.SetWindowText(valstr);

	m_pTransparentWnd->InvalidateTransparency();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CEditTransparencyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ctrlSliderTransparency.EnableWindow(m_rbEnableTrans);
	m_ctrlSliderTransparency.SetRange(0, 100);
	m_ctrlSliderTransparency.SetPos(m_riLevel);

	m_bEnableTranparency.SetCheck(m_rbEnableTrans);

	CString valstr;
	valstr.Format("%d", m_riLevel);
	m_ctrlStaticTransparency.SetWindowText(valstr);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditTransparencyDlg::OnCancel()
{
	m_rbEnableTrans = m_bEnableTransOld;
	m_riLevel = m_iLevelOld;

	m_pTransparentWnd->InvalidateTransparency();

	CDialog::OnCancel();
}
