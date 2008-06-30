// MciHelpDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "MciHelpDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMciHelpDialog dialog


CMciHelpDialog::CMciHelpDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMciHelpDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMciHelpDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMciHelpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMciHelpDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMciHelpDialog, CDialog)
	//{{AFX_MSG_MAP(CMciHelpDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMciHelpDialog message handlers

void CMciHelpDialog::OnOK() 
{
	// TODO: Add extra validation here
	ShowWindow(SW_HIDE);
	
	//CDialog::OnOK();
}

void CMciHelpDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	ShowWindow(SW_HIDE);
	
	//CDialog::OnCancel();
}
