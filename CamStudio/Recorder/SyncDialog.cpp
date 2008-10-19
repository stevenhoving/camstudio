// SyncDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "SyncDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int shiftTypeLocal = 0;
int timeShiftLocal = 0;

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg dialog

CSyncDlg::CSyncDlg(CWnd* pParent /*=NULL*/)
: CDialog(CSyncDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyncDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyncDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSyncDlg, CDialog)
	//{{AFX_MSG_MAP(CSyncDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg message handlers

BOOL CSyncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	shiftTypeLocal = iShiftType;
	timeShiftLocal = iTimeShift;

	UpdateGui();

	// TODO: Add extra initialization here

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSyncDlg::OnRadio1()
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(0);

	shiftTypeLocal = 1;

}

void CSyncDlg::OnRadio3()
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(1);

	shiftTypeLocal = 2;

}

void CSyncDlg::OnRadio4()
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(0);

	shiftTypeLocal = 0;

}

void CSyncDlg::OnOK()
{
	// TODO: Add extra validation here
	iShiftType = shiftTypeLocal;
	iTimeShift = timeShiftLocal;

	CDialog::OnOK();
}

void CSyncDlg::UpdateGui()
{

	((CButton *) GetDlgItem(IDC_RADIO4))->SetCheck(iShiftType == 0);
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(iShiftType == 1);
	((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(iShiftType == 2);

	UDACCEL acc[2];
	acc[0].nSec = 2;
	acc[0].nInc = 10;

	acc[1].nSec = 4;
	acc[1].nInc = 50;

	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetBuddy(GetDlgItem(IDC_EDIT1));
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetRange(0,5000);
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetPos(iTimeShift);
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetAccel(2,acc);

}

void CSyncDlg::OnChangeEdit1()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.

	// TODO: Add your control notification handler code here
	CString timeshStr("");
	((CEdit *) GetDlgItem(IDC_EDIT1))->GetWindowText(timeshStr);
	sscanf_s(LPCTSTR(timeshStr),"%d",&timeShiftLocal);

}
