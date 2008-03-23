// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "playplus.h"

#include "MainFrm.h"
#include <vfw.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND mainWnd;
extern BOOL playerAlive;
extern HDRAWDIB*  ghdd;    
extern int  giFirstVideo;   
extern HWND viewWnd;


extern int maxxScreen;
extern int maxyScreen;

extern int fileModified;

extern CStatusBar* statusbarCtrl;

#define PLAYER  0
#define DUBBER  1
extern int pmode;

CMenu PlayerMenu;
int menu_loaded = 0;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_SIZE()
	ON_WM_INITMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,                     // playing , recording or stopped
	ID_SEPARATOR,						// time indicator
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;



	if (pmode==DUBBER) {

		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}

	}
	else if (pmode==PLAYER) {

		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_PLAYERTOOLBAR))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // fail to create
		}

	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	statusbarCtrl = &m_wndStatusBar;

	
	/*
	CRect rect;
	
	int sliderheight = 20;
	int offset = 0; 
	
	m_wndToolBar.SetButtonInfo (6, ID_SLIDER, TBBS_SEPARATOR, 200);
	m_wndToolBar.GetItemRect (6, &rect);
    
	offset = rect.Height() - sliderheight; 
	offset /=2;
	
	rect.top = rect.top + offset ;
	rect.bottom = rect.bottom - offset ;

	CString msx;
	msx.Format("offset %d, recttop %d, rectbottom %d, rectleft %d, rectright %d", offset, rect.top, rect.bottom, rect.left, rect.right);
	MessageBox(msx,"note",MB_OK);

	
	if (!m_wndSliderCtrl.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP |// dwStyle
						TBS_HORZ |
						//TBS_AUTOTICKS |
						TBS_NOTICKS |
						TBS_BOTH,
						rect,					// rect
						&m_wndToolBar,		// CWnd* pParentWnd
						ID_SLIDER))		// UINT  nID
	{
		return -1;
	}
	*/
	
		
	
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	//Disable Floating
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	
	//DockControlBar(&m_wndToolBar);

	if (pmode == PLAYER) {
		SetWindowText("Player");
	}
	else
		SetWindowText("Dubber");

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	
	cs.x = 100;	
	cs.y = 100;
	cs.cx=320;   
	cs.cy=320;    

	cs.style &= ~FWS_ADDTOTITLE;
	

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


BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	BOOL retval = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	
	mainWnd = m_hWnd;	


	
	//::SetWindowPos(mainWnd, NULL, 0, 0, 100, 100,0 );			
	//SetWindowPos( &wndTop, 0, 0, 100, 100, SWP_SHOWWINDOW);  

	return retval;
	
}

void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	playerAlive = FALSE;

	if ((fileModified == 1) && (pmode == DUBBER)) {

		int ret = MessageBox("Do you want to save changes ?","Note",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (ret == IDYES) {
			SendMessage(WM_COMMAND,ID_FILE_SAVEAS,0);
			return;
		}
		else if (ret == IDCANCEL) 
			return;

	}
	

	CFrameWnd::OnClose();
}


void CMainFrame::ResizeToMovie(RECT movierect) 
{
	
	// TODO: Add your command handler code here
	CRect clientrect;
	CRect windowrect;
	CRect toolbarrect;
	CRect statusbarrect;

	GetWindowRect( &windowrect);
	GetClientRect( &clientrect);
	m_wndToolBar.GetWindowRect(&toolbarrect);
	
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->GetWindowRect(&statusbarrect);

	int borderHeight = GetSystemMetrics(SM_CYBORDER);
	int borderWidth = GetSystemMetrics(SM_CXBORDER);
	int captionHeight = GetSystemMetrics(SM_CYCAPTION);
	
	//Dock Toolbar
	DockControlBar(&m_wndToolBar);

	//Menu Height
	int menuY = GetSystemMetrics(SM_CYMENU);
	
	CWnd* mainWindow = AfxGetMainWnd( );
	AdjustWindowRectEx(&movierect, GetWindowLong(mainWindow->m_hWnd, GWL_STYLE), TRUE, GetWindowLong(mainWindow->m_hWnd, GWL_EXSTYLE));
	
	int compactcx = movierect.right-movierect.left + 1 + 4;
	int compactcy = movierect.bottom-movierect.top + 1 + toolbarrect.Height() +statusbarrect.Height() + 4;

	//compactcx += 2;
	//compactcy += 2;

	//if (compactcx > 600) compactcx=600;
	//if (compactcy > 480) compactcy=480;

	if (compactcx > maxxScreen  - windowrect.left)
		windowrect.left = maxxScreen - compactcx;

	if (compactcy > maxyScreen  - windowrect.top)
		windowrect.top = maxyScreen - compactcy - 20; //20 to compensate for taskbar height

	if (windowrect.left<=0) windowrect.left = 1;
	if (windowrect.top<=0) windowrect.top = 1;
		
	SetWindowPos( &wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);  

	
	
	
}

void CMainFrame::OffsetRectZero(CRect& winrect) 
{

	int x = winrect.left;
	int y = winrect.top;

	winrect.left = 0;
	winrect.top = 0;
	winrect.right = winrect.right - x;
	winrect.bottom = winrect.bottom - x;

}



void CMainFrame::AdjustTimeBar(CRect clientrect)
{

	m_wndToolBar.AdjustTimeBar(clientrect);

}

BOOL CMainFrame::OnQueryNewPalette() 
{
	// TODO: Add your message handler code here and/or call default

	
	BOOL val;

	if (giFirstVideo >= 0) {
		HDC hdc = ::GetDC(viewWnd);
		
		//
		// Realize the palette of the first video stream
		//
		if (val = DrawDibRealize(ghdd[giFirstVideo], hdc, FALSE))
			::InvalidateRect(viewWnd,NULL,TRUE);

		::ReleaseDC(viewWnd,hdc);
		return val;
	}
	 
	
	return CFrameWnd::OnQueryNewPalette();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect clientrect;	
	GetClientRect( &clientrect);

	AdjustTimeBar(clientrect);

	// TODO: Add your message handler code here
	if (pmode==DUBBER) {

	}
	else if (pmode==PLAYER) {

		if  (!menu_loaded) {
		
			PlayerMenu.LoadMenu(IDR_MENUPLAYER);
			menu_loaded = 1;

			//HMENU hMenu = ::GetMenu(m_hWnd);	
			//::SetMenu( m_hWnd, PlayerMenu.m_hMenu );				
			SetMenu(&PlayerMenu);				

		}		

	}
	
	
}


void CMainFrame::OnInitMenu(CMenu* pMenu) 
{
	CFrameWnd::OnInitMenu(pMenu);
	
	
	
}

void CMainFrame::EnableSlideBarButton(BOOL setToOn, int nIndex)
{

	m_wndToolBar.EnableButton(setToOn, nIndex);
	//m_wndToolBar.EnableButton(TRUE, ID_FILE_REWIND);
	//m_wndToolBar.EnableButton(TRUE, 4);

}
