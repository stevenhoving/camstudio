// PresetTime.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "PresetTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int presettime;
extern int recordpreset;


/////////////////////////////////////////////////////////////////////////////
// CPresetTime dialog


CPresetTime::CPresetTime(CWnd* pParent /*=NULL*/)
	: CDialog(CPresetTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPresetTime)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPresetTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPresetTime)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPresetTime, CDialog)
	//{{AFX_MSG_MAP(CPresetTime)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern void ErrMsg(char frmt[], ...);

/////////////////////////////////////////////////////////////////////////////
// CPresetTime message handlers

void CPresetTime::OnOK() 
{
	
	int val = ((CButton *) GetDlgItem(IDC_CHECK1))->GetCheck();	
	if (val)
		recordpreset = 1;
	else
		recordpreset = 0;

	int val2 = ((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->GetPos();	
	presettime = val2;


	//ErrMsg("%d",presettime);
	
	CDialog::OnOK();
}

BOOL CPresetTime::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(recordpreset);	
	
	UDACCEL acc[4];
    acc[0].nSec = 1; 
    acc[0].nInc = 1; 

	acc[1].nSec = 2; 
    acc[1].nInc = 5; 

	acc[2].nSec = 3; 
    acc[2].nInc = 20; 

	acc[2].nSec = 4; 
    acc[2].nInc = 100; 
	
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetAccel(4,acc);	
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetRange(1,7200);	
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetPos(presettime);	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
