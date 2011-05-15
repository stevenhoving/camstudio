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

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg dialog

CSyncDlg::CSyncDlg(int iType, int iShift, CWnd* pParent /*=NULL*/)
: CDialog(CSyncDlg::IDD, pParent)
, m_iTimeShift(iShift)
, m_iShiftType(iType)
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
	DDX_Text(pDX, IDC_EDIT1, m_iTimeShift);
}

BEGIN_MESSAGE_MAP(CSyncDlg, CDialog)
	//{{AFX_MSG_MAP(CSyncDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnVideoFirst)
	ON_BN_CLICKED(IDC_RADIO3, OnAudioFirst)
	ON_BN_CLICKED(IDC_RADIO4, OnNoShift)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyncDlg message handlers

BOOL CSyncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlButtonNoSync.SetCheck(m_iShiftType == NOSYNCH);
	m_ctrlButtonInvalid.SetCheck(m_iShiftType == VIDEOFIRST);
	m_ctrlButtonAudioFirst.SetCheck(m_iShiftType == AUDIOFIRST);

	// TODO: Is this really necessary? 2-level increase
	UDACCEL acc[2];
	acc[0].nSec = 2;
	acc[0].nInc = 10;

	acc[1].nSec = 4;
	acc[1].nInc = 50;

	//m_ctrlSpinTimeShift.SetBuddy(&m_ctrlEditTimeShift);	// TODO: redundant: done in resource editor
	m_ctrlSpinTimeShift.SetRange(0, 5000);
	m_ctrlSpinTimeShift.SetPos(m_iTimeShift);
	m_ctrlSpinTimeShift.SetAccel(2, acc);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// No shift
void CSyncDlg::OnNoShift()
{
	// TODO: Add your control notification handler code here
	m_ctrlButtonNoSync.SetCheck(TRUE);
	m_ctrlButtonInvalid.SetCheck(FALSE);
	m_ctrlButtonAudioFirst.SetCheck(FALSE);

	m_iShiftType = NOSYNCH;
}

// Video First (INVALID)
void CSyncDlg::OnVideoFirst()
{
	m_ctrlButtonNoSync.SetCheck(FALSE);
	m_ctrlButtonInvalid.SetCheck(TRUE);
	m_ctrlButtonAudioFirst.SetCheck(FALSE);

	m_iShiftType = VIDEOFIRST;
}

// Audio First
void CSyncDlg::OnAudioFirst()
{
	m_ctrlButtonNoSync.SetCheck(FALSE);
	m_ctrlButtonInvalid.SetCheck(FALSE);
	m_ctrlButtonAudioFirst.SetCheck(TRUE);

	m_iShiftType = AUDIOFIRST;
}

void CSyncDlg::OnOK()
{
	CDialog::OnOK();
}
