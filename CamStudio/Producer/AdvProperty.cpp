// AdvProperty.cpp : implementation file
//

#include "stdafx.h"
#include "playplus.h"
#include "AdvProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int allowChaining;
extern int freecharacter;

extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);
extern void MsgC(const char fmt[], ...);

/////////////////////////////////////////////////////////////////////////////
// CAdvProperty property page

IMPLEMENT_DYNCREATE(CAdvProperty, CPropertyPage)

CAdvProperty::CAdvProperty() : CPropertyPage(CAdvProperty::IDD)
{
	//{{AFX_DATA_INIT(CAdvProperty)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CAdvProperty::~CAdvProperty()
{
}

void CAdvProperty::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvProperty)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvProperty, CPropertyPage)
	//{{AFX_MSG_MAP(CAdvProperty)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvProperty message handlers

void CAdvProperty::OnOK() 
{

	int val1 = ((CButton *) GetDlgItem(IDC_FREECHARACTER))->GetCheck();
	int val2 = ((CButton *) GetDlgItem(IDC_ALLOWCHAINING))->GetCheck();

	if (val1)
		freecharacter = 1;
	else
		freecharacter = 0;

	if (val2)
		allowChaining = 1;
	else
		allowChaining = 0;

	
	CPropertyPage::OnOK();
}

BOOL CAdvProperty::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	((CButton *) GetDlgItem(IDC_FREECHARACTER))->SetCheck(freecharacter != 0);
	((CButton *) GetDlgItem(IDC_ALLOWCHAINING))->SetCheck(allowChaining != 0);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

