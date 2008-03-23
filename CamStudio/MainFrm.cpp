// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "vscap.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recordstate;
int viewtype = 0;
static HMENU hMenu = NULL;

extern BOOL bMinimizeToTray;
extern BOOL TrayShow();
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

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
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator

};



/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
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
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	
	//ver 1.2
	m_ToolbarBitmap256.LoadBitmap( IDB_TOOLBAR256 );    
	m_ToolbarBitmapMask.LoadBitmap( IDB_TOOLBARMASK );    	
		
	HDC hScreenDC = ::GetDC(NULL);	
	int numbits = ::GetDeviceCaps(hScreenDC, BITSPIXEL );		
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
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	//m_wndToolBar.SetButtonText(1,"Record");
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	SetWindowText("CamStudio");

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	
	//  the CREATESTRUCT cs
	cs.x = 200;	
	cs.y = 200;
	cs.cx=280;   
	
	//cs.cy=280;    
	cs.cy=292;    

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
	if (recordstate==1) {

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

	GetWindowRect( &windowrect);
	GetClientRect( &clientrect);
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

	int compactcx = 280;
	//int compactcy = 280;	
	int compactcy = 292;	

	compactcx=windowrect.Width();
	compactcy=windowrect.Height() - clientrect.Height() + toolbarrect.Height() - borderHeight;
	viewtype = 1;

	SetWindowPos( &wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);  
	
}

void CMainFrame::OnUpdateViewCompactview(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(viewtype==1);
	
}


void CMainFrame::OnViewButtonsview() 
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
	int captionHeight = GetSystemMetrics(SM_CYCAPTION);
	
	//Dock Toolbar
	DockControlBar(&m_wndToolBar);
	
	
	//Hide menu
	hMenu = ::GetMenu(m_hWnd);	
	::SetMenu( m_hWnd, NULL );
	

	//Hide Status Bar
	pStatus->ShowWindow(SW_HIDE);
	
	int compactcx = 280;
	//int compactcy = 280;	
	int compactcy = 292;	

	compactcx=windowrect.Width();
	compactcy=captionHeight + toolbarrect.Height() + borderHeight + borderHeight;
	viewtype = 2;					
	
	SetWindowPos( &wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);  


	
}

void CMainFrame::OnViewNormalview() 
{
	// TODO: Add your command handler code here
	CRect clientrect;
	CRect windowrect;
	CRect toolbarrect;
	CRect statusbarrect;

	GetWindowRect( &windowrect);
	GetClientRect( &clientrect);

	//Show Status Bar
	CStatusBar* pStatus = (CStatusBar*) AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	pStatus->ShowWindow(SW_SHOW);

	//Dock Toolbar
	DockControlBar(&m_wndToolBar);	

	//Show Menu
	if ( hMenu != NULL )	
		::SetMenu( m_hWnd, hMenu );
	
	int compactcx = 280;
	//int compactcy = 280;
	int compactcy = 292;
	viewtype = 0;

	SetWindowPos( &wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);  

	
}

void CMainFrame::OnUpdateViewButtonsview(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(viewtype==2);
	
}

void CMainFrame::OnUpdateViewNormalview(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(viewtype==0);
	
}

void CMainFrame::OnViewtype() 
{
	// TODO: Add your command handler code here

	
	viewtype++;
	if (viewtype>2)
		viewtype=0;	

	UpdateViewtype() ;
		
	
}

void CMainFrame::UpdateViewtype() 
{

	
	if (viewtype==0) 
		OnViewNormalview();
	else if (viewtype==1) 
		OnViewCompactview();
	else if (viewtype==2) 
		OnViewButtonsview();	


}



void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	
	if(bMinimizeToTray)
	{
		if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
		
			//if( TrayShow())
				this->ShowWindow(SW_HIDE);		
				
		}
		else
			CFrameWnd::OnSysCommand(nID, lParam);	
	}
	else
		CFrameWnd::OnSysCommand(nID, lParam);
}


