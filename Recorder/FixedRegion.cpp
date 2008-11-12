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
#include "MainFrm.h"			// for maxxScreen, maxyScreen
#include "RecorderView.h"
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
, m_iLeft(1)
, m_iTop(1)
, m_iWidth(1)
, m_iHeight(1)
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
	DDX_Control(pDX, IDC_HEIGHT, m_ctrlEditHeight);	
	DDX_Control(pDX, IDC_X, m_ctrlEditPosX);
	DDX_Control(pDX, IDC_Y, m_ctrlEditPosY);
	DDX_Control(pDX, IDC_SUPPORTMOUSEDRAG, m_ctrlButtonMouseDrag);
	DDX_Control(pDX, IDC_FIXEDTOPLEFT, m_ctrlButtonFixTopLeft);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_X, m_iLeft);
	DDV_MinMaxInt(pDX, m_iLeft, 0, maxxScreen);
	DDX_Text(pDX, IDC_Y, m_iTop);
	DDV_MinMaxInt(pDX, m_iTop, 0, maxyScreen);
	DDX_Text(pDX, IDC_WIDTH, m_iWidth);
	DDV_MinMaxInt(pDX, m_iWidth, 0, maxxScreen);
	DDX_Text(pDX, IDC_HEIGHT, m_iHeight);
	DDV_MinMaxInt(pDX, m_iHeight, 0, maxyScreen);
}

BEGIN_MESSAGE_MAP(CFixedRegionDlg, CDialog)
	//{{AFX_MSG_MAP(CFixedRegionDlg)
	ON_BN_CLICKED(IDSELECT, OnSelect)
	ON_BN_CLICKED(IDC_FIXEDTOPLEFT, OnFixedtopleft)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP_REGIONUPDATE, OnRegionUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedRegionDlg message handlers

void CFixedRegionDlg::OnOK()
{
	if (!UpdateData()) {
		return;
	}

	if (m_iWidth <= 0)
	{
		//CString msgstr;
		//msgstr.Format("The width must be greater than 0");
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd, IDS_STRING_WIDTHGREATER, IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (maxxScreen < m_iWidth)
	{
		//CString msgstr;
		//msgstr.Format("The width must be smaller than the screen width (%d)",maxxScreen);
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd, IDS_STRING_WIDTHSMALLER, IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION, maxxScreen);
		return;
	}

	if (m_iHeight <= 0)
	{
		//CString msgstr;
		//msgstr.Format("The height must be greater than 0");
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_STRING_HEIGHTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (maxyScreen < m_iHeight)
	{
		//CString msgstr;
		//msgstr.Format("The height must be smaller than the screen height (%d)",maxyScreen);
		//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_STRING_HEIGHTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
		return;
	}

	//version 1.5
	int fval = m_ctrlButtonFixTopLeft.GetCheck();
	if (fval)
	{
		if (m_iLeft <= 0)
		{
			//CString msgstr;
			//msgstr.Format("The left value must be greater than 0");
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_LEFTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (maxxScreen < m_iLeft)
		{
			//CString msgstr;
			//msgstr.Format("The left value must be smaller than the screen width (%d)",maxxScreen);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(this->m_hWnd,IDS_STRING_LEFTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxxScreen);
			return;
		}

		if (m_iTop <= 0)
		{
			//CString msgstr;
			//msgstr.Format("The top value must be greater than 0");
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_TOPGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (maxyScreen < m_iTop)
		{
			//CString msgstr;
			//msgstr.Format("The top value must be smaller than the screen height (%d)",maxyScreen);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd,IDS_STRING_TOPSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
			return;
		}

		if (maxxScreen < (m_iLeft + m_iWidth))
		{
			m_iWidth = maxxScreen - m_iLeft;
			if (m_iWidth <= 0)
			{
				m_iLeft = 100;
				m_iWidth = 320;
			}

			//CString msgstr;
			//msgstr.Format("Value exceed screen width. The width is adjusted to %d",width);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);
			MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDWIDTH, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, m_iWidth);
		}

		if (maxyScreen < (m_iTop + m_iHeight))
		{
			m_iHeight = maxyScreen - m_iTop;
			if (m_iHeight <= 0)
			{
				m_iTop = 100;
				m_iHeight = 240;
			}

			//CString msgstr;
			//msgstr.Format("Value exceed screen height. The height is adjusted to %d",height);
			//MessageBox(msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

			MessageOut(m_hWnd,IDS_STRING_VALUEEXCEEDHEIGHT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, m_iHeight);
		}
	}

	cRegionOpts.m_iCaptureLeft = m_iLeft;
	cRegionOpts.m_iCaptureTop = m_iTop;
	cRegionOpts.m_bFixedCapture = fval ? true : false;
	cRegionOpts.m_iCaptureWidth = m_iWidth;
	cRegionOpts.m_iCaptureHeight = m_iHeight;
	cRegionOpts.m_bSupportMouseDrag = m_ctrlButtonMouseDrag.GetCheck() ? true : false;

	CDialog::OnOK();
}

BOOL CFixedRegionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iLeft = cRegionOpts.m_iCaptureLeft;
	m_iTop = cRegionOpts.m_iCaptureTop;
	m_iWidth = cRegionOpts.m_iCaptureWidth;
	m_iHeight = cRegionOpts.m_iCaptureHeight;
	UpdateData(FALSE);

	m_ctrlEditPosX.EnableWindow(TRUE);
	m_ctrlEditPosY.EnableWindow(TRUE);
	m_ctrlButtonFixTopLeft.SetCheck(cRegionOpts.m_bFixedCapture);
	m_ctrlEditPosX.EnableWindow(cRegionOpts.m_bFixedCapture);
	m_ctrlEditPosY.EnableWindow(cRegionOpts.m_bFixedCapture);
	m_ctrlButtonMouseDrag.SetCheck(cRegionOpts.m_bSupportMouseDrag);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFixedRegionDlg::OnSelect()
{
	m_ctrlStaticMsg.SetWindowText(_T("Click and drag to define a rectangle"));

	cRegionOpts.m_iMouseCaptureMode = CAPTURE_VARIABLE; //set temporarily to variable region
	iDefineMode = 1;
	hFixedRegionWnd = m_hWnd;
	::ShowWindow(hMouseCaptureWnd, SW_MAXIMIZE);
	::UpdateWindow(hMouseCaptureWnd);
	m_ctrlStaticMsg.SetWindowText(_T(""));
}

// OnRegionUpdate
// message handler for WM_APP_REGIONUPDATE
// TODO: why doesn't this message send position values?
LRESULT CFixedRegionDlg::OnRegionUpdate(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CRect rectUse(rcUse);
	if (m_ctrlButtonFixTopLeft.GetCheck()) {
		m_iLeft		= rectUse.left;
		m_iTop		= rectUse.top;
	}
	m_iWidth	= rectUse.Width();
	m_iHeight	= rectUse.Height();

	UpdateData(FALSE);

	return 0;
}

void CFixedRegionDlg::OnFixedtopleft()
{
	// TODO: Add your control notification handler code here
	int fixtl = m_ctrlButtonFixTopLeft.GetCheck();
	m_ctrlEditPosX.EnableWindow(fixtl);
	m_ctrlEditPosY.EnableWindow(fixtl);
}
