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
	// TRACE ( _T("## Why are m_itop and m_ileft defined here as 1,1 instead of 0,0. Not correct I beieve\n") );

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
	DDV_MinMaxInt(pDX, m_iLeft, minxScreen, maxxScreen);
	DDX_Text(pDX, IDC_Y, m_iTop);
	DDV_MinMaxInt(pDX, m_iTop, minyScreen, maxyScreen);
	DDX_Text(pDX, IDC_WIDTH, m_iWidth);
	DDV_MinMaxInt(pDX, m_iWidth, 0, abs(maxxScreen-minxScreen));
	DDX_Text(pDX, IDC_HEIGHT, m_iHeight);
	DDV_MinMaxInt(pDX, m_iHeight, 0, abs(maxyScreen-minyScreen));
}

BEGIN_MESSAGE_MAP(CFixedRegionDlg, CDialog)
	//{{AFX_MSG_MAP(CFixedRegionDlg)
	ON_BN_CLICKED(IDSELECT, OnSelect)
	ON_BN_CLICKED(IDC_FIXEDTOPLEFT, OnFixedtopleft)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_APP_REGIONUPDATE, OnRegionUpdate)
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_EN_CHANGE(IDC_Y, &CFixedRegionDlg::OnEnChangeY)
	ON_EN_CHANGE(IDC_HEIGHT, &CFixedRegionDlg::OnEnChangeHeight)
	ON_BN_CLICKED(IDOK, &CFixedRegionDlg::OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedRegionDlg message handlers

void CFixedRegionDlg::OnOK()
{
	if (!UpdateData()) {
		return;
	}

	// Not Correct, one to high because MinXY is zero an MaxXY is width/height no of pixels.
	int maxWidth = abs(maxxScreen - minxScreen);   // Assuming number first pixel is one not zero.
	int maxHeight = abs(maxyScreen - minyScreen);
	// TRACE(_T("## CFixedRegionDlg::OnOK / maxWidth=[%d], maxHeight=[%d]\n"), maxWidth, maxHeight );
	
	if (m_iWidth < 0)
	{
		MessageOut(m_hWnd, IDS_STRING_WIDTHGREATER, IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (maxWidth < m_iWidth)
	{
		MessageOut(m_hWnd, IDS_STRING_WIDTHSMALLER, IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION, maxxScreen);
		return;
	}

	if (m_iHeight < 0)
	{
		MessageOut(m_hWnd,IDS_STRING_HEIGHTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	if (maxHeight < m_iHeight)
	{
		MessageOut(m_hWnd,IDS_STRING_HEIGHTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
		return;
	}

	//version 1.5
	int fval = m_ctrlButtonFixTopLeft.GetCheck();
	if (fval)
	{
		if (m_iLeft < minxScreen)
		{
			MessageOut(m_hWnd,IDS_STRING_LEFTGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (maxxScreen < m_iLeft)
		{
			MessageOut(this->m_hWnd,IDS_STRING_LEFTSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxxScreen);
			return;
		}

		if (m_iTop < minyScreen)
		{
			MessageOut(m_hWnd,IDS_STRING_TOPGREATER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (maxyScreen < m_iTop)
		{
			MessageOut(m_hWnd,IDS_STRING_TOPSMALLER ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION,maxyScreen);
			return;
		}

		if (maxWidth < (m_iLeft + m_iWidth))
		{
			m_iWidth = maxxScreen - m_iLeft;
			if (m_iWidth <= 0)
			{
				//TODO -- where did these constants came from? Get rid of 'em, put 'em in an ini or #define them somewhere
				//Answer: See struct sRegionOpts in Profile.h. An area of 240x320 is defined there.
				m_iLeft = minxScreen + 100;
				m_iWidth = 320;
			}
			MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDWIDTH, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, m_iWidth);
		}

		if (maxHeight < (m_iTop + m_iHeight))
		{
			m_iHeight = maxyScreen - m_iTop;
			if (m_iHeight <= 0)
			{
				//TODO -- where did these constants come from? Get rid of 'em, put 'em in an ini or #define them somewhere
				//Answer: See struct sRegionOpts in Profile.h. An area of 240x320 is defined there.
				m_iTop = minyScreen + 100;
				m_iHeight = 240;
			}
			MessageOut(m_hWnd,IDS_STRING_VALUEEXCEEDHEIGHT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, m_iHeight);
		}
	}

	// Technical pixel coordinates are applicable.  Top-Left = 0:0 not 1:1 What user often think it is.
	// TRACE(_T("## CFixedRegionDlg::OnOK / L=%d, T=%d, W=%d, H=%d\n"), m_iLeft, m_iTop, m_iWidth, m_iHeight );

	cRegionOpts.m_iLeft = m_iLeft;
	cRegionOpts.m_iTop = m_iTop;
	cRegionOpts.m_bFixed = fval ? true : false;
	cRegionOpts.m_iWidth = m_iWidth ;
	cRegionOpts.m_iHeight = m_iHeight ;
	cRegionOpts.m_bMouseDrag = m_ctrlButtonMouseDrag.GetCheck() ? true : false;

	CDialog::OnOK();
}

BOOL CFixedRegionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iLeft = cRegionOpts.m_iLeft;
	m_iTop = cRegionOpts.m_iTop;
	m_iWidth = cRegionOpts.m_iWidth;
	m_iHeight = cRegionOpts.m_iHeight;
	UpdateData(FALSE);

	// TRACE(_T("## CFixedRegionDlg::OnInitDialog / L=%d, T=%d, W=%d, H=%d\n"), m_iLeft, m_iTop, m_iWidth, m_iHeight );

	m_ctrlEditPosX.EnableWindow(TRUE);
	m_ctrlEditPosY.EnableWindow(TRUE);
	m_ctrlButtonFixTopLeft.SetCheck(cRegionOpts.m_bFixed);
	m_ctrlEditPosX.EnableWindow(cRegionOpts.m_bFixed);
	m_ctrlEditPosY.EnableWindow(cRegionOpts.m_bFixed);
	m_ctrlButtonMouseDrag.SetCheck(cRegionOpts.m_bMouseDrag);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFixedRegionDlg::OnSelect()
{
	m_ctrlStaticMsg.SetWindowText(_T("Click and drag to define a rectangle"));

	cRegionOpts.m_iCaptureMode = CAPTURE_VARIABLE; //set temporarily to variable region
	iDefineMode = 1;
	hFixedRegionWnd = m_hWnd;
	::ShowWindow(hMouseCaptureWnd, SW_SHOW);
	::UpdateWindow(hMouseCaptureWnd);
	m_ctrlStaticMsg.SetWindowText(_T(""));
}

// OnRegionUpdate
// message handler for WM_APP_REGIONUPDATE
// TODO: why doesn't this message send position values?
LRESULT CFixedRegionDlg::OnRegionUpdate(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	CRect rectUse(rcUse);
	if (!m_ctrlButtonFixTopLeft.GetCheck()) {
		m_iLeft		= rectUse.left;
		m_iTop		= rectUse.top;
	}
	m_iWidth	= rectUse.Width();
	m_iHeight	= rectUse.Height();

	//TRACE(_T("## CFixedRegionDlg::OnRegionUpdate / L=%d, T=%d, W=%d, H=%d\n"), m_iLeft, m_iTop, m_iWidth, m_iHeight );

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

LRESULT CFixedRegionDlg::OnDisplayChange(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	//TODO: add handling if a display is turned on / off while trying to select region
	// -- this probably isn't as important as catching this same message when recording but
	//it should eventually be handled.

	return 0;
}

void CFixedRegionDlg::OnEnChangeY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CFixedRegionDlg::OnEnChangeHeight()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CFixedRegionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	OnOK();
}
