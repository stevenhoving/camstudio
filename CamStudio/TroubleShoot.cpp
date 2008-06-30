// TroubleShoot.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "TroubleShoot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int restrictVideoCodecs;
extern int TroubleShootVal; 
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);

/////////////////////////////////////////////////////////////////////////////
// TroubleShoot dialog


TroubleShoot::TroubleShoot(CWnd* pParent /*=NULL*/)
	: CDialog(TroubleShoot::IDD, pParent)
{
	//{{AFX_DATA_INIT(TroubleShoot)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TroubleShoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TroubleShoot)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TroubleShoot, CDialog)
	//{{AFX_MSG_MAP(TroubleShoot)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TroubleShoot message handlers

void TroubleShoot::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	
}

void TroubleShoot::OnOK() 
{
	// TODO: Add extra validation here

	TroubleShootVal = 0;

	int check = ((CButton *) GetDlgItem(IDC_CHECK1))->GetCheck();
	int ret=IDNO;
	if (check) {
		//ret = MessageBox("CamStudio will now exit and set your system to record with microphone. Proceed ?","Note",MB_YESNOCANCEL | MB_ICONQUESTION); 
		ret = MessageOut(this->m_hWnd,IDS_STRING_EXITSET ,IDS_STRING_NOTE,MB_YESNOCANCEL | MB_ICONQUESTION);


		if (ret==IDYES)
			TroubleShootVal += 1;
		else if (ret==IDCANCEL)
			return;

	}

	int check2 = ((CButton *) GetDlgItem(IDC_CHECK2))->GetCheck();
	int ret2=IDNO;
	if (check2) {

		restrictVideoCodecs = 1;
	}
	else
		restrictVideoCodecs = 0;

	CDialog::OnOK();
}


void TroubleShoot::OnCheck2() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL TroubleShoot::OnInitDialog() 
{

	if (restrictVideoCodecs)
		((CButton *) GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
	else
		((CButton *) GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
	
		
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
