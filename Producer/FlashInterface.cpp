// FlashInterface.cpp : implementation file
//

#include "stdafx.h"
#include "playplus.h"
#include "FlashInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString loadingPath;
extern double percentLoadedThreshold;

extern int FrameOffsetX;
extern int FrameOffsetY;

extern int swfbk_red;
extern int swfbk_green;
extern int swfbk_blue;

int local_swfbk_red;
int local_swfbk_green;
int local_swfbk_blue;

extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);
extern void MsgC(const char fmt[], ...);

/////////////////////////////////////////////////////////////////////////////
// CFlashInterface property page

IMPLEMENT_DYNCREATE(CFlashInterface, CPropertyPage)

CFlashInterface::CFlashInterface() : CPropertyPage(CFlashInterface::IDD)
{
	//{{AFX_DATA_INIT(CFlashInterface)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CFlashInterface::~CFlashInterface()
{
}

void CFlashInterface::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlashInterface)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlashInterface, CPropertyPage)
	//{{AFX_MSG_MAP(CFlashInterface)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTONCHOOSE, OnButtonchoose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlashInterface message handlers

BOOL CFlashInterface::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	int valset = int(percentLoadedThreshold * 100.0);
	if (valset > 100) valset = 100;
	if (valset < 10) valset = 10;

	UDACCEL acc[2];
    acc[0].nSec = 1; 
    acc[0].nInc = 5; 

	acc[1].nSec = 3; 
    acc[1].nInc = 10; 

	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetBuddy(GetDlgItem(IDC_PRELOADPERCENT));
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetRange(10,100);
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetPos(valset);			
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetAccel(2,acc);		
	
	// TODO: Add extra initialization here
	((CEdit *)GetDlgItem(IDC_PRELOADBITMAP))->SetWindowText(loadingPath);


	local_swfbk_red = swfbk_red;
	local_swfbk_green = swfbk_green;
	local_swfbk_blue = swfbk_blue;	

	CString foxStr,foyStr;
	foxStr.Format("%d",FrameOffsetX);
	foyStr.Format("%d",FrameOffsetY);	
	((CEdit *) GetDlgItem(IDC_FRAMEOFFSETX))->SetWindowText(foxStr);
	((CEdit *) GetDlgItem(IDC_FRAMEOFFSETY))->SetWindowText(foyStr);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFlashInterface::OnOK() 
{

	CString foxStr,foyStr;	
	((CEdit *) GetDlgItem(IDC_FRAMEOFFSETX))->GetWindowText(foxStr);
	((CEdit *) GetDlgItem(IDC_FRAMEOFFSETY))->GetWindowText(foyStr);

	int lfox,lfoy;
	sscanf(LPCTSTR(foxStr),"%d",&lfox);
	sscanf(LPCTSTR(foyStr),"%d",&lfoy);

	int foxValid = 1;
	int foyValid = 1;
	if (lfox < 0) 
	{
		MessageOut(this->m_hWnd,IDS_FOXERRSTR, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		foxValid = 0;

		//Question : how to return from a property sheet? 
		//return only return this page .. and not the others
		
	
	}

	if (lfoy < 0) 
	{
		MessageOut(this->m_hWnd,IDS_FOYERRSTR, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		foyValid = 0;
	}


	if (lfox > 200) 
	{
		MessageOut(this->m_hWnd,IDS_FOXERRLESS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		foxValid = 0;
	}

	if (lfoy > 200) 
	{
		MessageOut(this->m_hWnd,IDS_FOYERRLESS, IDS_NOTE, MB_OK | MB_ICONEXCLAMATION);
		foyValid = 0;
	}

	if (foxValid)		
		FrameOffsetX = lfox ;
	
	if (foyValid)	
		FrameOffsetY = lfoy ;
		
	swfbk_red = local_swfbk_red;
	swfbk_green = local_swfbk_green;
	swfbk_blue = local_swfbk_blue;

	//MsgC("swfbk_red %d",swfbk_red);
	//MsgC("swfbk_green %d",swfbk_green);
	//MsgC("swfbk_blue %d",swfbk_blue);



	((CEdit *)GetDlgItem(IDC_PRELOADBITMAP))->GetWindowText(loadingPath);

	int spinpos = ((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->GetPos();			
	percentLoadedThreshold = ((double) spinpos) / 100.0;
	
	CPropertyPage::OnOK();
}


void CFlashInterface::OnButtonchoose() 
{
	
	static char BASED_CODE szFilter[] =	"Windows Bitamp Files (*.bmp)|*.bmp||";	
	char szTitle[] = "Select bitmap to display when SWF is loading"		;
	
	CFileDialog fdlg(FALSE,"*.bmp","*.bmp",OFN_LONGNAMES,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;	
	
			
	CString m_newfile;	
	
	if(fdlg.DoModal() == IDOK)
	{
		m_newfile = fdlg.GetPathName();						
		((CEdit *)GetDlgItem(IDC_PRELOADBITMAP))->SetWindowText(m_newfile);
			
	}
	else {				
		
		return;

	}
	
}


void CFlashInterface::OnButton1() 
{
	// TODO: Add your control notification handler code here
	COLORREF localColor = RGB(local_swfbk_red,local_swfbk_green,local_swfbk_blue);
	CColorDialog colordlg(localColor,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (colordlg.DoModal()==IDOK)
	{
		localColor = colordlg.GetColor();
		local_swfbk_red = GetRValue(localColor);
		local_swfbk_green = GetGValue(localColor);
		local_swfbk_blue  = GetBValue(localColor);

	}

	
}



		

