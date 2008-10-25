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
	DDX_Control(pDX, IDC_RADIO4, m_ctrlButtonNoSync);
	DDX_Control(pDX, IDC_RADIO3, m_ctrlButtonAudioFirst);	
	DDX_Control(pDX, IDC_RADIO1, m_ctrlButtonInvalid);
	DDX_Control(pDX, IDC_SPIN1, m_ctrlSpinTimeShift);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEditTimeShift);
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
	m_ctrlButtonNoSync.SetCheck(0);
	m_ctrlButtonInvalid.SetCheck(1);
	m_ctrlButtonAudioFirst.SetCheck(0);

	shiftTypeLocal = 1;

}

void CSyncDlg::OnRadio3()
{
	// TODO: Add your control notification handler code here
	m_ctrlButtonNoSync.SetCheck(0);
	m_ctrlButtonInvalid.SetCheck(0);
	m_ctrlButtonAudioFirst.SetCheck(1);

	shiftTypeLocal = 2;

}

void CSyncDlg::OnRadio4()
{
	// TODO: Add your control notification handler code here
	m_ctrlButtonNoSync.SetCheck(1);
	m_ctrlButtonInvalid.SetCheck(0);
	m_ctrlButtonAudioFirst.SetCheck(0);

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
	m_ctrlButtonNoSync.SetCheck(iShiftType == 0);
	m_ctrlButtonInvalid.SetCheck(iShiftType == 1);
	m_ctrlButtonAudioFirst.SetCheck(iShiftType == 2);

	// TODO: Is this really necessary?
	UDACCEL acc[2];
	acc[0].nSec = 2;
	acc[0].nInc = 10;

	acc[1].nSec = 4;
	acc[1].nInc = 50;

	//m_ctrlSpinTimeShift.SetBuddy(&m_ctrlEditTimeShift);	// TODO: redundant: done in resource editor
	m_ctrlSpinTimeShift.SetRange(0,5000);
	m_ctrlSpinTimeShift.SetPos(iTimeShift);
	m_ctrlSpinTimeShift.SetAccel(2, acc);
}

void CSyncDlg::OnChangeEdit1()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.

	// TODO: Add your control notification handler code here
	CString timeshStr("");
	m_ctrlEditTimeShift.GetWindowText(timeshStr);
	sscanf_s(LPCTSTR(timeshStr),"%d",&timeShiftLocal);

}
