// AutoSearchDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "AutoSearchDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoSearchDialog dialog

CAutoSearchDialog::CAutoSearchDialog(CWnd* pParent /*=NULL*/)
: CDialog(CAutoSearchDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoSearchDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CAutoSearchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoSearchDialog)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TEXT1, m_ctrlStaticText1);
	DDX_Control(pDX, IDC_TEXT2, m_ctrlStaticText2);
	DDX_Control(pDX, IDC_CLOSE, m_ctrlButtonClose);
}

BEGIN_MESSAGE_MAP(CAutoSearchDialog, CDialog)
	//{{AFX_MSG_MAP(CAutoSearchDialog)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseDialog)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoSearchDialog message handlers

void CAutoSearchDialog::SetVarText(CString textstr)
{
	m_ctrlStaticText1.SetWindowText(textstr);
}

void CAutoSearchDialog::SetVarTextLine2(CString textstr)
{
	m_ctrlStaticText2.SetWindowText(textstr);
}

void CAutoSearchDialog::SetButtonEnable(BOOL enable)
{
	m_ctrlButtonClose.EnableWindow(enable);
}

void CAutoSearchDialog::OnCloseDialog()
{
	// TODO: This needs to change.
	// The dialog should not persist for the duration fo the program.
	ShowWindow(SW_HIDE);
}

void CAutoSearchDialog::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);

	//CDialog::OnClose();
}
