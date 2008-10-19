// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// FixedRegion.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "FixedRegion.h"
#include "MouseCaptureWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int iDefineMode;

/////////////////////////////////////////////////////////////////////////////
// CFixedRegionDlg dialog
IMPLEMENT_DYNAMIC(CFixedRegionDlg, CDialog)

CFixedRegionDlg::CFixedRegionDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFixedRegionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFixedRegionDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CFixedRegionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFixedRegionDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_MSG, m_ctrlStaticMsg);	
	DDX_Control(pDX, IDC_WIDTH, m_ctrlEditWidth);	
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFixedRegionDlg, CDialog)
	//{{AFX_MSG_MAP(CFixedRegionDlg)
	ON_BN_CLICKED(IDSELECT, OnSelect)
	ON_BN_CLICKED(IDC_FIXEDTOPLEFT, OnFixedtopleft)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_REGIONUPDATE, OnRegionUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedRegionDlg message handlers

void CFixedRegionDlg::OnOK()
{
	// TODO: Add extra validation here
	CString widthstr;
	CString heightstr;
	m_ctrlEditWidth.GetWindowText(widthstr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->GetWindowText(heightstr);

	int width;
	int height;
	sscanf_s(LPCTSTR(widthstr),"%d", &width);
	sscanf_s(LPCTSTR(heightstr),"%d", &height);

	if (width <= 0)
	{
		//CString msgstr;
		//msgstr.Format("The width must be greater than 0");
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd, IDS_STRING_WIDTHGREATER, IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (maxxScreen < width)
	{
		//CString msgstr;
		//msgstr.Format("The width must be smaller than the screen width (%d)",maxxScreen);
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd, IDS_STRING_WIDTHSMALLER, IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION, maxxScreen);
		return;
	}

	if (height <= 0)
	{
		//CString msgstr;
		//msgstr.Format("The height must be greater than 0");
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_HEIGHTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (maxyScreen < height)
	{
		//CString msgstr;
		//msgstr.Format("The height must be smaller than the screen height (%d)",maxyScreen);
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_HEIGHTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
		return;
	}

	//version 1.5
	int fval = ((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->GetCheck();
	if (fval)
	{
		//Bypass all the following checks if the Fixed Top-Left setting is not turned on
		CString xstr;
		CString ystr;
		((CEdit *) GetDlgItem(IDC_X))->GetWindowText(xstr);
		((CEdit *) GetDlgItem(IDC_Y))->GetWindowText(ystr);

		int xval;
		int yval;
		sscanf_s(LPCTSTR(xstr),"%d", &xval);
		sscanf_s(LPCTSTR(ystr),"%d", &yval);

		if (xval <= 0)
		{
			//CString msgstr;
			//msgstr.Format("The left value must be greater than 0");
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_LEFTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (maxxScreen < xval)
		{
			//CString msgstr;
			//msgstr.Format("The left value must be smaller than the screen width (%d)",maxxScreen);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_LEFTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxxScreen);
			return;
		}

		if (yval <= 0)
		{
			//CString msgstr;
			//msgstr.Format("The top value must be greater than 0");
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_TOPGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (maxyScreen < yval)
		{
			//CString msgstr;
			//msgstr.Format("The top value must be smaller than the screen height (%d)",maxyScreen);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_TOPSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
			return;
		}

		if (maxxScreen < (xval + width))
		{
			width = maxxScreen - xval;
			if (width <= 0)
			{
				xval = 100;
				width = 320;
			}

			//CString msgstr;
			//msgstr.Format("Value exceed screen width. The width is adjusted to %d",width);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDWIDTH, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,width);
		}

		if (maxyScreen < (yval + height))
		{
			height = maxyScreen - yval;
			if (height <= 0)
			{
				yval = 100;
				height = 240;
			}

			//CString msgstr;
			//msgstr.Format("Value exceed screen height. The height is adjusted to %d",height);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

			MessageOut(this->m_hWnd,IDS_STRING_VALUEEXCEEDHEIGHT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION,height);
		}

		iCaptureLeft = xval;
		iCaptureTop = yval;
	}

	bFixedCapture = fval;

	/////////

	iCaptureWidth = width;
	iCaptureHeight = height;

	//ver 1.8
	if (((CButton *) GetDlgItem(IDC_SUPPORTMOUSEDRAG))->GetCheck())
		bSupportMouseDrag = 1;
	else
		bSupportMouseDrag = 0;

	CDialog::OnOK();
}

BOOL CFixedRegionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	//version 1.5
	CString xstr;
	CString ystr;
	xstr.Format("%d",iCaptureLeft);
	ystr.Format("%d",iCaptureTop);

	((CEdit *) GetDlgItem(IDC_X))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(TRUE);
	((CEdit *) GetDlgItem(IDC_X))->SetWindowText(xstr);
	((CEdit *) GetDlgItem(IDC_Y))->SetWindowText(ystr);

	if (bFixedCapture)
	{
		((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->SetCheck(TRUE);
	}
	else
	{
		((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->SetCheck(FALSE);
		((CEdit *) GetDlgItem(IDC_X))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(FALSE);
	}
	///////////////////////////////

	CString widthstr;
	CString heightstr;
	widthstr.Format("%d",iCaptureWidth);
	heightstr.Format("%d",iCaptureHeight);

	m_ctrlEditWidth.SetWindowText(widthstr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->SetWindowText(heightstr);
	m_ctrlStaticMsg.SetWindowText("");

	if (bSupportMouseDrag)
	{
		((CButton *) GetDlgItem(IDC_SUPPORTMOUSEDRAG))->SetCheck(TRUE);
	}
	else
	{
		((CButton *) GetDlgItem(IDC_SUPPORTMOUSEDRAG))->SetCheck(FALSE);
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFixedRegionDlg::OnSelect()
{
	// TODO: Add your control notification handler code here
	m_ctrlStaticMsg.SetWindowText("Click and drag to define a rectangle");

	iMouseCaptureMode = 1; //set temporarily to 1
	iDefineMode = 1;
	hFixedRegionWnd = m_hWnd;
	::ShowWindow(hMouseCaptureWnd, SW_MAXIMIZE);
	::UpdateWindow(hMouseCaptureWnd);
	m_ctrlStaticMsg.SetWindowText("");
}

LRESULT CFixedRegionDlg::OnRegionUpdate (WPARAM wParam, LPARAM lParam) {

	int width = rcUse.right - rcUse.left + 1;
	int height = rcUse.bottom - rcUse.top + 1;

	CString widthstr;
	CString heightstr;
	widthstr.Format("%d",width);
	heightstr.Format("%d",height);

	m_ctrlEditWidth.SetWindowText(widthstr);
	((CEdit *) GetDlgItem(IDC_HEIGHT))->SetWindowText(heightstr);

	//version 1.5
	int fixtl = ((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->GetCheck();
	if (fixtl)
	{
		CString xstr;
		CString ystr;
		xstr.Format("%d", rcUse.left);
		ystr.Format("%d", rcUse.top);
		((CEdit *) GetDlgItem(IDC_X))->SetWindowText(xstr);
		((CEdit *) GetDlgItem(IDC_Y))->SetWindowText(ystr);
	}

	return 0;
}

void CFixedRegionDlg::OnFixedtopleft()
{
	// TODO: Add your control notification handler code here
	int fixtl = ((CButton *) GetDlgItem(IDC_FIXEDTOPLEFT))->GetCheck();
	if (fixtl)
	{
		((CEdit *) GetDlgItem(IDC_X))->EnableWindow(TRUE);
		((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(TRUE);
	}
	else
	{
		((CEdit *) GetDlgItem(IDC_X))->EnableWindow(FALSE);
		((CEdit *) GetDlgItem(IDC_Y))->EnableWindow(FALSE);
	}
}
