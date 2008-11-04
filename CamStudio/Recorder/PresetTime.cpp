// PresetTime.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "PresetTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPresetTimeDlg dialog

CPresetTimeDlg::CPresetTimeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPresetTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPresetTimeDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CPresetTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPresetTimeDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPIN1, m_ctrlSpinTime);
	DDX_Control(pDX, IDC_CHECK1, m_ctrlButtonTime);
}

BEGIN_MESSAGE_MAP(CPresetTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CPresetTimeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPresetTimeDlg message handlers

void CPresetTimeDlg::OnOK()
{
	int val = m_ctrlButtonTime.GetCheck();
	cProgramOpts.m_bRecordPreset = (val) ? true : false;

	int val2 = m_ctrlSpinTime.GetPos();
	cProgramOpts.m_iPresetTime = val2;

	//ErrMsg("%d",iPresetTime);

	CDialog::OnOK();
}

BOOL CPresetTimeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ctrlButtonTime.SetCheck(cProgramOpts.m_bRecordPreset);

	UDACCEL acc[4];
	acc[0].nSec = 1;
	acc[0].nInc = 1;

	acc[1].nSec = 2;
	acc[1].nInc = 5;

	acc[2].nSec = 3;
	acc[2].nInc = 20;

	acc[2].nSec = 4;
	acc[2].nInc = 100;

	m_ctrlSpinTime.SetAccel(4, acc);
	m_ctrlSpinTime.SetRange(1, 7200);
	m_ctrlSpinTime.SetPos(cProgramOpts.m_iPresetTime);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
