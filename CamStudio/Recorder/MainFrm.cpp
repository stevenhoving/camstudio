// MainFrm.cpp : implementation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "MainFrm.h"
#include "RecorderView.h"
#include "addons\Camstudio4XNote.h"

/*
TODO: Break AVI 2 GB boundary
================================
AVI File Size Limits
http://neuron2.net/LVG/filesize.html
Extended AVI recording with OpenDML 
This is an extension to the AVI file format that allows AVI files to be of any size. 
To create and work with OpenDML AVIs, you need software that supports the OpenDML standard (such as VirtualDub).

http://www.the-labs.com/Video/odmlff2-avidef.pdf

Google:  +msdn +AVI +2.0

MSDN: Avi 2.0

See also AviSynth

AVI overview:   http://www.jmcgowan.com/avi.html


avi.h declarec Class CAVI
CAVI use PAVISTREAM


PAVISTREAM is defines in vfw.h
typedef       IAVIStream FAR* PAVISTREAM;

DECLARE_INTERFACE_(IAVIStream, IUnknown)

http://vjforums.com/archive/index.php/t-13937.html
* The source file to write the opendml avi's is located in VirtualDub/source/AVIOutputFile.cpp and 
its all custom code that follows the openDML spec, without using any VFW methods for opening the file, 
writing chunks, etc.
* The VFW api has methods like AVIStreamWrite / AVIStreamWriteHeader / AVIStreamCreate for writing avi 
streams, and none of this is used by the virtualdub code, although I did find that it does use VFW and 
the AVIStreamRead method for opening some .avi's.

Avi writing using directshow: http://www.codeguru.com/forum/archive/index.php/t-372248.html


*/


/*
using namespace std;

// For Marshalling memory
// compile with: /clr
using namespace System;
using namespace System::Runtime::InteropServices;
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool bRecordState;	// in RecorderView

int maxxScreen;
int maxyScreen;
int minxScreen;
int minyScreen;

static HMENU hMenu = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

// Do not change applied RegisterWindowMessage names because otherwise the communication link between sender and receiver application is broken.
const UINT CMainFrame::WM_USER_XNOTE = ::RegisterWindowMessage("XNote");
const UINT CMainFrame::WM_USER_MOTIONDETECTOR = ::RegisterWindowMessage("MotionAlerter");
const UINT CMainFrame::WM_USER_STOPRECORD = ::RegisterWindowMessage("StopRecord");

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_COMPACTVIEW, OnViewCompactview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMPACTVIEW, OnUpdateViewCompactview)
	ON_COMMAND(ID_VIEW_BUTTONSVIEW, OnViewButtonsview)
	ON_COMMAND(ID_VIEW_NORMALVIEW, OnViewNormalview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BUTTONSVIEW, OnUpdateViewButtonsview)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NORMALVIEW, OnUpdateViewNormalview)
	ON_COMMAND(ID_VIEWTYPE, OnViewtype)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CTrayIcon::m_WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)
	ON_REGISTERED_MESSAGE(CMainFrame::WM_USER_XNOTE, OnXNote)
	ON_REGISTERED_MESSAGE(CMainFrame::WM_USER_MOTIONDETECTOR, OnMotionDetector)
	ON_REGISTERED_MESSAGE(CMainFrame::WM_USER_STOPRECORD, OnStopRecord)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// String send by MotionAlerter as message identifier is : "MotionAlerter"
//
// CMainFrame::OnMotionDetector
// Handles XNote supporing WindowsMessages. Xnote is a stopwatch application.  (http://www.xnotestopwatch.com/)
// MotionDetector is based on the MotionDetection example from Andrew ....
// Allows that external program can work with Camstudio and can instruct CamStudio when to start recording, to pause and to terminate recording.
//
// wParam: loword: id (in  xnote.ini under [General] "id=" key)
//         hiword: action (1 - start, 2 - stop, 3 - snap, 4 - reset, 5 - release pause)
// lParam: time in ms  (Near future, as we are able to use camera as timer. Gige Camera required)
// BTW. Curently only action 5 (release pause, restart recording) is supported
//////////////////////////////////////////////////////////
LRESULT CMainFrame::OnMotionDetector(UINT wParam, LONG lParam )
{
	//TRACE("## CMainFrame::OnMotionDetector (d)    wParam=[%d] HI[%d], LO[%d]\n",wParam, HIWORD(wParam), LOWORD(wParam) );
	dynamic_cast<CRecorderView *>(m_pViewActive)->XNoteProcessWinMessage( HIWORD(wParam), XNOTE_TRIGGER_MOTIONDETECTOR , XNOTE_SOURCE_MOTIONDETECTOR , NULL );

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnXNote
// Handles XNote WindowsMessages. Xnote is a stopwatch application.  (http://www.xnotestopwatch.com/)
// Allows that external program can work with Camstudio and can instruct CamStudio when to start recording, to pause and to terminate recording.
//
// wParam: loword: id (in  xnote.ini under [General] "id=" key)
//         hiword: action (1 - start, 2 - stop, 3 - snap, 4 - reset)
// lParam: time in ms
//////////////////////////////////////////////////////////
LRESULT CMainFrame::OnXNote(UINT wParam, LONG lParam)
{
	// Xnote sends source and action info with wParam
	int nHiMod256 = HIWORD(wParam) % 256 ;
	//TRACE("## CMainFrame::OnXNote (u)    wParam=[%u] HI[%u], LO[%u] nHiMod256=[%d]\n",wParam, HIWORD(wParam), LOWORD(wParam), nHiMod256 );
	dynamic_cast<CRecorderView *>(m_pViewActive)->XNoteProcessWinMessage( nHiMod256, LOWORD(wParam), XNOTE_SOURCE_XNOTESTOPWATCH , (ULONG)lParam );

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Initiates recording stop from the recorder view
//
// CMainFrame::OnStopRecord
// wParam: unused
// lParam: unsued
//////////////////////////////////////////////////////////
LRESULT CMainFrame::OnStopRecord(UINT /*wParam*/, LONG /*lParam*/ )
{
 	dynamic_cast<CRecorderView *>(m_pViewActive)->PostMessage(CRecorderView::WM_USER_RECORDINTERRUPTED,0,0);
 
	return 0;
}

static UINT indicators[] =
{
	ID_SEPARATOR	// status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bmLogo.LoadBitmap(IDB_BITMAP3);
}

CMainFrame::~CMainFrame()
{
	//ver 1.2
	m_ToolbarBitmap256.DeleteObject();
	m_ToolbarBitmapMask.DeleteObject();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.Create(this)
		|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE("Failed to create toolbar\n");
		return -1; // fail to create
	}

	if (!m_wndStatusBar.Create(this)
		|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE("Failed to create status bar\n");
		return -1; // fail to create
	}

	//ver 1.2
	m_ToolbarBitmap256.LoadBitmap(IDB_TOOLBAR256);
	m_ToolbarBitmapMask.LoadBitmap(IDB_TOOLBARMASK);

	HDC hScreenDC = ::GetDC(NULL);
	int numbits = ::GetDeviceCaps(hScreenDC, BITSPIXEL );
	//maxxScreen = ::GetDeviceCaps(hScreenDC, HORZRES);
	maxxScreen = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
	//maxyScreen = ::GetDeviceCaps(hScreenDC, VERTRES);
	maxyScreen = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
	minxScreen = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
	minyScreen = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
	::ReleaseDC(NULL,hScreenDC);

	if (numbits>8) {
		//m_ilToolBar.Create(21,21, ILC_COLOR8 | ILC_MASK, 4, 4);
		m_ilToolBar.Create(21,21, ILC_COLOR16 | ILC_MASK, 4, 4);
		//m_ilToolBar.Create(32,32, ILC_COLOR16 | ILC_MASK, 4, 4);
		m_ilToolBar.Add(&m_ToolbarBitmap256,&m_ToolbarBitmapMask);

		//VC6
		//m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar);

		//VC5
		::SendMessage(m_wndToolBar.GetToolBarCtrl().m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)m_ilToolBar.GetSafeHandle());
	}

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle()
		| CBRS_TOOLTIPS
		| CBRS_FLYBY
		| CBRS_SIZE_DYNAMIC);

	//m_wndToolBar.SetButtonText(1,"Record");
	// TODO: Delete these three lines if you don't want the toolbar to
	// be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//ver 1.2
	m_TrayIcon.SetNotifyWnd(*this);
	m_TrayIcon.TraySetIcon(IDR_MAINFRAME);
	m_TrayIcon.TraySetToolTip("CamStudio");
	m_TrayIcon.TraySetMenu(IDR_TRAYMENU);
	m_TrayIcon.TrayShow();

#ifdef CAMSTUDIO4XNOTE 
	SetWindowText("CamStudio4Xnote by Janhgm - Custom Build");
#else
	SetWindowText("CamStudio - Custom Build");
#endif
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying

	// the CREATESTRUCT cs
	cs.x = 200;
	cs.y = 200;
	cs.cx = 280 - 10;
	cs.cy = 292 + 19;

	// use logo to set widht and heights
	BITMAP bitmap;
	m_bmLogo.GetBitmap(&bitmap);
	cs.cx = bitmap.bmWidth - 10; // Otherwise size won't be correct
	cs.cy = bitmap.bmHeight + 19; // Otherwise size won't be correct
	// add width of borders
	cs.cx += (::GetSystemMetrics(SM_CXFRAME) * 2)
		+ ::GetSystemMetrics(SM_CXMENUSIZE)
;
	// add height of Caption + menu + toolbar + status + borders
	cs.cy += ::GetSystemMetrics(SM_CYCAPTION)
		+ (::GetSystemMetrics(SM_CYMENU) * 2)	// assume statusbar same hieght
		+ ::GetSystemMetrics(SM_CXMENUSIZE)
		+ (::GetSystemMetrics(SM_CYFRAME) * 2)
;

	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~WS_MAXIMIZEBOX;

	cs.lpszClass = _T("CamStudio");

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (bRecordState)
	{
		//MessageBox("Please stop video recording before exiting program.","Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(this->m_hWnd,IDS_STRING_STOPBEFOREEXIT ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CFrameWnd::OnClose();
}

void CMainFrame::OnViewCompactview()
{
	// TODO: Add your command handler code here
	CRect clientrect;
	CRect windowrect;
	CRect toolbarrect;
	CRect statusbarrect;

	GetWindowRect(&windowrect);
	GetClientRect(&clientrect);
	m_wndToolBar.GetWindowRect(&toolbarrect);

	int borderHeight = GetSystemMetrics(SM_CYBORDER);

	//Dock Toolbar
	DockControlBar(&m_wndToolBar);

	//Hide Menu
	if ( hMenu != NULL )
		::SetMenu( m_hWnd, hMenu );

	//Hide Status Bar
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->ShowWindow(SW_HIDE);

	int compactcx = windowrect.Width();
	int compactcy = windowrect.Height() - clientrect.Height() + toolbarrect.Height() - borderHeight - borderHeight;
	cProgramOpts.m_iViewType = VIEW_COMPACT;

	SetWindowPos( &wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);
}

void CMainFrame::OnUpdateViewCompactview(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(cProgramOpts.m_iViewType == VIEW_COMPACT);
}

void CMainFrame::OnViewButtonsview()
{
	// TODO: Add your command handler code here

	CRect windowrect;
	CRect clientrect;
	CRect toolbarrect;
	CRect statusbarrect;

	GetWindowRect(&windowrect);
	GetClientRect(&clientrect);
	m_wndToolBar.GetWindowRect(&toolbarrect);
	m_wndStatusBar.GetWindowRect(&statusbarrect);

	int borderHeight = GetSystemMetrics(SM_CYBORDER);
	int captionHeight = GetSystemMetrics(SM_CYCAPTION);

	//Dock Toolbar
	DockControlBar(&m_wndToolBar);

	//Hide menu
	CMenu *pOldMenu = GetMenu();
	hMenu = pOldMenu->Detach();
	SetMenu(0);

	//Hide Status Bar
	m_wndStatusBar.ShowWindow(SW_HIDE);

	int compactcx = windowrect.Width();
	int compactcy = captionHeight + toolbarrect.Height() + borderHeight + borderHeight;
	cProgramOpts.m_iViewType = VIEW_BUTTON;

	SetWindowPos( &wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);
}

void CMainFrame::OnViewNormalview()
{
	// TODO: Add your command handler code here
	CRect windowrect;
	GetWindowRect(&windowrect);

	CRect rectToolbar;
	m_wndToolBar.GetWindowRect(&rectToolbar);

	//Show Status Bar
	CRect rectStatusbar;
	m_wndStatusBar.GetWindowRect(&rectStatusbar);
	m_wndStatusBar.ShowWindow(SW_SHOW);

	//Dock Toolbar
	DockControlBar(&m_wndToolBar);

	//Show Menu
	if (hMenu) {
		SetMenu(CMenu::FromHandle(hMenu));
	}

	// use logo to set widht and heights
	BITMAP bitmap;
	m_bmLogo.GetBitmap(&bitmap);
	int compactcx = bitmap.bmWidth - 10;
	// add width of borders
	compactcx += (::GetSystemMetrics(SM_CXFRAME) * 2)
		+ ::GetSystemMetrics(SM_CXMENUSIZE)
;

	int compactcy = bitmap.bmHeight;
	// add height of Caption + menu + status + borders
	compactcy += ::GetSystemMetrics(SM_CYCAPTION)
		+ ::GetSystemMetrics(SM_CYMENU)
		+ rectToolbar.Height()
		+ rectStatusbar.Height()
		+ (::GetSystemMetrics(SM_CYFRAME) * 2);

	cProgramOpts.m_iViewType = VIEW_NORMAL;

	SetWindowPos(&wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);
}

void CMainFrame::OnUpdateViewButtonsview(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(cProgramOpts.m_iViewType == VIEW_BUTTON);
}

void CMainFrame::OnUpdateViewNormalview(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(cProgramOpts.m_iViewType == VIEW_NORMAL);
}

void CMainFrame::OnViewtype()
{
	// TODO: Add your command handler code here
	cProgramOpts.m_iViewType++;
	if (cProgramOpts.m_iViewType > VIEW_BUTTON)
		cProgramOpts.m_iViewType = VIEW_NORMAL;

	UpdateViewtype();
}

void CMainFrame::UpdateViewtype()
{
	if (cProgramOpts.m_iViewType == VIEW_NORMAL)
		OnViewNormalview();
	else if (cProgramOpts.m_iViewType == VIEW_COMPACT)
		OnViewCompactview();
	else if (cProgramOpts.m_iViewType == VIEW_BUTTON)
		OnViewButtonsview();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (m_TrayIcon.MinimizeToTray()) {
		if ((nID & 0xFFF0) == SC_MINIMIZE) {
			ShowWindow(SW_HIDE);
			return;
		}
	}
	CFrameWnd::OnSysCommand(nID, lParam);
}
LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	return m_TrayIcon.OnTrayNotify(wParam, lParam);
}

CView *CMainFrame::GetViewActive()
{
 	return dynamic_cast<CRecorderView *>(m_pViewActive);
}