// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
// 
//
// FixedRegion.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "FixedRegion.h"

#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern int captureleft;
extern int capturetop;
extern int fixedcapture;


extern int maxxScreen;
extern int maxyScreen;
extern int capturewidth;
extern int captureheight;

extern HWND hMouseCaptureWnd;
extern int DefineMode;
extern int MouseCaptureMode;
extern HWND hWnd_FixedRegion;

extern RECT   rcUse; 

extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);
extern int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);

extern int supportMouseDrag;

/////////////////////////////////////////////////////////////////////////////
// CFixedRegion dialog


CFixedRegion::CFixedRegion(CWnd* pParent /*=NULL*/)
	: CDialog(CFixedRegion::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixedRegion)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFixedRegion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixedRegion)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFixedRegion, CDialog)
	//{{AFX_MSG_MAP(CFixedRegion)
	ON_BN_CLICKED(IDSELECT, OnSelect)
	ON_BN_CLICKED(IDC_FIXEDTOPLEFT, OnFixedtopleft)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_REGIONUPDATE, OnRegionUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedRegion message handlers

void CFixedRegion::OnOK() 
{
	// TODO: Add extra validation here
	CString widthstr;
	CString heightstr;

	int width;
	int height;
	
	((CEdit *) GetDlgItem(IDC_WIDTH))->GetWindowText(widthstr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->GetWindowText(heightstr);

	sscanf(LPCTSTR(widthstr),"%d",&width);
	sscanf(LPCTSTR(heightstr),"%d",&height);


	if (width<=0) {

		//CString msgstr;
		//msgstr.Format("The width must be greater than 0");
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_WIDTHGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;


	}

	if (width>maxxScreen) {

		//CString msgstr;
		//msgstr.Format("The width must be smaller than the screen width (%d)",maxxScreen);
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOutINT(this->m_hWnd,IDS_STRING_WIDTHSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxxScreen);
		return;


	}

	if (height<=0) {

		//CString msgstr;
		//msgstr.Format("The height must be greater than 0");
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_HEIGHTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);		
		return;

	}

	if (height>maxyScreen) {

		//CString msgstr;
		//msgstr.Format("The height must be smaller than the screen height (%d)",maxyScreen);
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

		MessageOutINT(this->m_hWnd,IDS_STRING_HEIGHTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
		return;


	}



	//version 1.5
	int fval = ((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->GetCheck();

	if (fval) { //Bypass all the following checks if the Fixed Top-Left setting is not turned on

		CString xstr;
		CString ystr;

		int xval;
		int yval;
		
		((CEdit *) GetDlgItem(IDC_X))->GetWindowText(xstr);
		((CEdit *) GetDlgItem(IDC_Y))->GetWindowText(ystr);

		sscanf(LPCTSTR(xstr),"%d",&xval);
		sscanf(LPCTSTR(ystr),"%d",&yval);
		

		if (xval<=0) {

			//CString msgstr;
			//msgstr.Format("The left value must be greater than 0");
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

			MessageOut(this->m_hWnd,IDS_STRING_LEFTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;


		}

		if (xval>maxxScreen) {

			//CString msgstr;
			//msgstr.Format("The left value must be smaller than the screen width (%d)",maxxScreen);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOutINT(this->m_hWnd,IDS_STRING_LEFTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxxScreen);
			return;


		}

		if (yval<=0) {

			//CString msgstr;
			//msgstr.Format("The top value must be greater than 0");
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_TOPGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			

			return;

		}

		if (yval>maxyScreen) {

			//CString msgstr;
			//msgstr.Format("The top value must be smaller than the screen height (%d)",maxyScreen);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

			MessageOutINT(this->m_hWnd,IDS_STRING_TOPSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
			
			
			return;


		}


		
		if (xval+width>maxxScreen) {

			
			width=maxxScreen-xval;
			
			if (width<=0) {
				xval=100;
				width=320;
				
			}		

			//CString msgstr;
			//msgstr.Format("Value exceed screen width. The width is adjusted to %d",width);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOutINT(this->m_hWnd,IDS_STRING_VALUEEXCEEDWIDTH, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,width);
		
		}
		

		if (yval+height>maxyScreen) {
			
			height=maxyScreen-yval;
			
			if (height<=0) {
			
				yval=100;
				height=240;
				
			}		

			//CString msgstr;
			//msgstr.Format("Value exceed screen height. The height is adjusted to %d",height);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);		

			MessageOutINT(this->m_hWnd,IDS_STRING_VALUEEXCEEDHEIGHT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,height);

		}


		captureleft= xval;
		capturetop= yval;

	}

	fixedcapture = fval;	

	///////// 



	capturewidth= width;
	captureheight= height;

	//ver 1.8
	if (((CButton *) GetDlgItem(IDC_SUPPORTMOUSEDRAG))->GetCheck())
		supportMouseDrag = 1;
	else
		supportMouseDrag = 0;

		
	CDialog::OnOK();
}

BOOL CFixedRegion::OnInitDialog() 
{
	CDialog::OnInitDialog();


	// TODO: Add extra initialization here


	//version 1.5
	CString xstr;
	CString ystr;
		
	xstr.Format("%d",captureleft);
	ystr.Format("%d",capturetop);	

	((CEdit *) GetDlgItem(IDC_X))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_X))->SetWindowText(xstr);
	((CEdit *) GetDlgItem(IDC_Y))->SetWindowText(ystr);

	
	if (fixedcapture) {

		((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->SetCheck(TRUE);
		

	}
	else {

		((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->SetCheck(FALSE);
		((CEdit *) GetDlgItem(IDC_X))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(FALSE);

	}
	///////////////////////////////


	CString widthstr;
	CString heightstr;
	
	widthstr.Format("%d",capturewidth);
	heightstr.Format("%d",captureheight);
	
	((CEdit *) GetDlgItem(IDC_WIDTH))->SetWindowText(widthstr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->SetWindowText(heightstr);

	((CStatic *) GetDlgItem(IDC_MSG))->SetWindowText("");

	if (supportMouseDrag) {
		((CButton *) GetDlgItem(IDC_SUPPORTMOUSEDRAG))->SetCheck(TRUE);		
	}
	else {
		((CButton *) GetDlgItem(IDC_SUPPORTMOUSEDRAG))->SetCheck(FALSE);
	}


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFixedRegion::OnSelect() 
{
	// TODO: Add your control notification handler code here
	((CStatic *) GetDlgItem(IDC_MSG))->SetWindowText("Click and drag to define a rectangle");
	
	MouseCaptureMode = 1; //set temporarily to 1
	DefineMode = 1;
	hWnd_FixedRegion = m_hWnd;
	::ShowWindow(hMouseCaptureWnd,SW_MAXIMIZE);
	::UpdateWindow(hMouseCaptureWnd);	

	((CStatic *) GetDlgItem(IDC_MSG))->SetWindowText("");
	
}


LRESULT CFixedRegion::OnRegionUpdate (WPARAM wParam, LPARAM lParam) {


	CString widthstr;
	CString heightstr;

	int width,height;

	width=rcUse.right-rcUse.left+1;
	height=rcUse.bottom-rcUse.top+1;
	
	widthstr.Format("%d",width);
	heightstr.Format("%d",height);
	
	((CEdit *) GetDlgItem(IDC_WIDTH))->SetWindowText(widthstr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->SetWindowText(heightstr);


	//version 1.5
	int fixtl = ((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->GetCheck();
	if (fixtl) {
	//if (fixedcapture) {


		CString xstr;
		CString ystr;

		int x,y;

		x=rcUse.left;
		y=rcUse.top;
	
		xstr.Format("%d",x);
		ystr.Format("%d",y);

		((CEdit *) GetDlgItem(IDC_X))->SetWindowText(xstr);
		((CEdit *) GetDlgItem(IDC_Y))->SetWindowText(ystr);
	
	}


	return 0;     
}

void CFixedRegion::OnFixedtopleft() 
{
	// TODO: Add your control notification handler code here
	int fixtl = ((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->GetCheck();
	if (fixtl) {

		((CEdit *) GetDlgItem(IDC_X))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(TRUE);

	}
	else {

		((CEdit *) GetDlgItem(IDC_X))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(FALSE);

	}

	
}
