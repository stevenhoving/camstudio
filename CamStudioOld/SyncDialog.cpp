// SyncDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "SyncDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int shiftType;
extern int timeshift;
int shiftTypeLocal = 0;
int timeShiftLocal = 0;

/////////////////////////////////////////////////////////////////////////////
// CSyncDialog dialog


CSyncDialog::CSyncDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSyncDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyncDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSyncDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyncDialog, CDialog)
	//{{AFX_MSG_MAP(CSyncDialog)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncDialog message handlers

BOOL CSyncDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	shiftTypeLocal = shiftType;
	timeShiftLocal = timeshift;

	UpdateGui();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSyncDialog::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(0);

	shiftTypeLocal = 1;


	
}

void CSyncDialog::OnRadio3() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(1);

	shiftTypeLocal = 2;

	
}

void CSyncDialog::OnRadio4() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(0);

	shiftTypeLocal = 0;

	
}

void CSyncDialog::OnOK() 
{
	// TODO: Add extra validation here	
	shiftType = shiftTypeLocal;
	timeshift = timeShiftLocal;
	
	CDialog::OnOK();
}


void CSyncDialog::UpdateGui() 
{

	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(shiftType == 0);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(shiftType == 1);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(shiftType == 2);

	UDACCEL acc[2];
    acc[0].nSec = 2; 
    acc[0].nInc = 10; 

	acc[1].nSec = 4; 
    acc[1].nInc = 50; 

 


	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetBuddy(GetDlgItem(IDC_EDIT1));
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetRange(0,5000);
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetPos(timeshift);			
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetAccel(2,acc);		



}

void CSyncDialog::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	CString timeshStr("");
	((CEdit *) GetDlgItem(IDC_EDIT1))->GetWindowText(timeshStr);
	sscanf(LPCTSTR(timeshStr),"%d",&timeShiftLocal);
	
	
}
