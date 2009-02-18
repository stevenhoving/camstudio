// CTroubleShootDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "TroubleShoot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int TroubleShootVal;

/////////////////////////////////////////////////////////////////////////////
// CTroubleShootDlg dialog

CTroubleShootDlg::CTroubleShootDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTroubleShootDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTroubleShootDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTroubleShootDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTroubleShootDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK1, m_ctrlButtonBehavior1);
	DDX_Control(pDX, IDC_CHECK2, m_ctrlButtonBehavior2);
}

BEGIN_MESSAGE_MAP(CTroubleShootDlg, CDialog)
	//{{AFX_MSG_MAP(CTroubleShootDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTroubleShootDlg message handlers

BOOL CTroubleShootDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ctrlButtonBehavior2.SetCheck(cVideoOpts.m_bRestrictVideoCodecs);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTroubleShootDlg::OnOK()
{
	// TODO: Add extra validation here
	TroubleShootVal = 0;

	int check = m_ctrlButtonBehavior1.GetCheck();
	if (check) {
		// "CamStudio will now exit and set your system to record with microphone. Proceed ?"
		int ret = MessageOut(*this, IDS_STRING_EXITSET, IDS_STRING_NOTE,MB_YESNOCANCEL | MB_ICONQUESTION);
		if (ret == IDCANCEL)
		{
			return;
		}
		TroubleShootVal = (ret == IDYES) ? 1 : 0;
	}

	int check2 = m_ctrlButtonBehavior2.GetCheck();
	cVideoOpts.m_bRestrictVideoCodecs = (check2) ? true : false;

	CDialog::OnOK();
}

