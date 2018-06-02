#include "stdafx.h"
#include "Recorder.h"
#include "MainFrm.h"
#include "RecorderView.h"
#include <CamLib/console.h>
#include <afxdatarecovery.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool g_bRecordState; // in RecorderView

int maxxScreen;
int maxyScreen;
int minxScreen;
int minyScreen;

static HMENU hMenu = nullptr;

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
//}}AFX_MSG_MAP
ON_MESSAGE(CTrayIcon::m_WM_TRAY_ICON_NOTIFY_MESSAGE, OnTrayNotify)
ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
//#ifdef _DEBUG
    console::create();
//#endif
    // TODO: add member initialization code here
    m_bmLogo.LoadBitmap(IDB_BITMAP3);
}

CMainFrame::~CMainFrame()
{
    // ver 1.2
    m_ToolbarBitmap256.DeleteObject();
    m_ToolbarBitmapMask.DeleteObject();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE("Failed to create toolbar\n");
        return -1; // fail to create
    }

    if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
    {
        TRACE("Failed to create status bar\n");
        return -1; // fail to create
    }

    // ver 1.2
    m_ToolbarBitmap256.LoadBitmap(IDB_TOOLBAR256);
    m_ToolbarBitmapMask.LoadBitmap(IDB_TOOLBARMASK);

    HDC hScreenDC = ::GetDC(nullptr);
    int numbits = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
    // maxxScreen = ::GetDeviceCaps(hScreenDC, HORZRES);
    maxxScreen = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
    // maxyScreen = ::GetDeviceCaps(hScreenDC, VERTRES);
    maxyScreen = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
    minxScreen = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
    minyScreen = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
    ::ReleaseDC(nullptr, hScreenDC);

    if (numbits > 8)
    {
        // m_ilToolBar.Create(21,21, ILC_COLOR8 | ILC_MASK, 4, 4);
        m_ilToolBar.Create(21, 21, ILC_COLOR16 | ILC_MASK, 4, 4);
        // m_ilToolBar.Create(32,32, ILC_COLOR16 | ILC_MASK, 4, 4);
        m_ilToolBar.Add(&m_ToolbarBitmap256, &m_ToolbarBitmapMask);

        // VC6
        // m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar);

        // VC5
        ::SendMessage(m_wndToolBar.GetToolBarCtrl().m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)m_ilToolBar.GetSafeHandle());
    }

    m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

    // TODO: Remove this if you don't want tool tips or a resizeable toolbar
    m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    // m_wndToolBar.SetButtonText(1,"Record");
    // TODO: Delete these three lines if you don't want the toolbar to
    // be dockable
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    // ver 1.2
    m_TrayIcon.SetNotifyWnd(*this);
    m_TrayIcon.TraySetIcon(IDR_MAINFRAME);
    m_TrayIcon.TraySetToolTip(_T("CamStudio"));
    m_TrayIcon.TraySetMenu(IDR_TRAYMENU);
    m_TrayIcon.TrayShow();

    SetWindowText(_T("CamStudio"));
    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: Modify the Window class or styles here by modifying

    // the CREATESTRUCT cs
    cs.x = 200;
    cs.y = 200;

    // use logo to set width and heights
    BITMAP bitmap;
    m_bmLogo.GetBitmap(&bitmap);

    cs.cx = bitmap.bmWidth - 26;  // Otherwise size won't be correct
    cs.cy = bitmap.bmHeight + 10; // Otherwise size won't be correct
    // add width of borders
    cs.cx += (::GetSystemMetrics(SM_CXFRAME) * 2) + ::GetSystemMetrics(SM_CXMENUSIZE);
    // add height of Caption + menu + toolbar + status + borders
    cs.cy += ::GetSystemMetrics(SM_CYCAPTION) + (::GetSystemMetrics(SM_CYMENU) * 2) // assume statusbar same height
             + ::GetSystemMetrics(SM_CXMENUSIZE) + (::GetSystemMetrics(SM_CYFRAME) * 2);

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

void CMainFrame::Dump(CDumpContext &dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnClose()
{
    if (g_bRecordState)
    {
        MessageOut(this->m_hWnd, IDS_STRING_STOPBEFOREEXIT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    // Workarround for CFrameWnd::OnClose();
    if (m_lpfnCloseProc != NULL)
    {
        // if there is a close proc, then defer to it, and return
        // after calling it so the frame itself does not close.
        (*m_lpfnCloseProc)(this);
        return;
    }

    // Note: only queries the active document
    CDocument* pDocument = GetActiveDocument();
    if (pDocument != NULL && !pDocument->CanCloseFrame(this))
    {
        // document can't close right now -- don't close it
        return;
    }
    CWinApp* pApp = AfxGetApp();
    if (pApp != NULL && pApp->m_pMainWnd == this)
    {
        CDataRecoveryHandler *pHandler = pApp->GetDataRecoveryHandler();
        if ((pHandler != NULL) && (pHandler->GetShutdownByRestartManager()))
        {
            // If the application is being shut down by the Restart Manager, do
            // a final autosave.  This will mark all the documents as not dirty,
            // so the SaveAllModified call below won't prompt for save.
            pHandler->AutosaveAllDocumentInfo();
            pHandler->SaveOpenDocumentList();
        }

        // attempt to save all documents
        if (pDocument == NULL && !pApp->SaveAllModified())
            return;     // don't close it

        if ((pHandler != NULL) && (!pHandler->GetShutdownByRestartManager()))
        {
            // If the application is not being shut down by the Restart Manager,
            // delete any autosaved documents since everything is now fully saved.
            pHandler->DeleteAllAutosavedFiles();
        }

        // hide the application's windows before closing all the documents
        pApp->HideApplication();

        // close all documents first
        pApp->CloseAllDocuments(FALSE);

        // don't exit if there are outstanding component objects
        if (!AfxOleCanExitApp())
        {
            // take user out of control of the app
            AfxOleSetUserCtrl(FALSE);

            // don't destroy the main window and close down just yet
            //  (there are outstanding component (OLE) objects)
            return;
        }

        // there are cases where destroying the documents may destroy the
        //  main window of the application.
        if (!afxContextIsDLL && pApp->m_pMainWnd == NULL)
        {
            AfxPostQuitMessage(0);
            return;
        }
    }

    // detect the case that this is the last frame on the document and
    // shut down with OnCloseDocument instead.
    if (pDocument != NULL && pDocument->m_bAutoDelete)
    {
        BOOL bOtherFrame = FALSE;
        POSITION pos = pDocument->GetFirstViewPosition();
        while (pos != NULL)
        {
            CView* pView = pDocument->GetNextView(pos);
            ENSURE_VALID(pView);
            if (pView->GetParentFrame() != this)
            {
                bOtherFrame = TRUE;
                break;
            }
        }
        if (!bOtherFrame)
        {
            pDocument->OnCloseDocument();
            return;
        }

        // allow the document to cleanup before the window is destroyed
        pDocument->PreCloseFrame(this);
    }

    // then destroy the window
    DestroyWindow();
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

    // Dock Toolbar
    DockControlBar(&m_wndToolBar);

    // Hide Menu
    if (hMenu != nullptr)
        ::SetMenu(m_hWnd, hMenu);

    // Hide Status Bar
    CStatusBar *pStatus = (CStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    pStatus->ShowWindow(SW_HIDE);

    int compactcx = windowrect.Width();
    int compactcy = windowrect.Height() - clientrect.Height() + toolbarrect.Height() - borderHeight - borderHeight;
    cProgramOpts.m_iViewType = VIEW_COMPACT;

    SetWindowPos(&wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);
}

void CMainFrame::OnUpdateViewCompactview(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(cProgramOpts.m_iViewType == VIEW_COMPACT);
}

void CMainFrame::OnViewButtonsview()
{
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

    // Dock Toolbar
    DockControlBar(&m_wndToolBar);

    // Hide menu
    CMenu *pOldMenu = GetMenu();
    hMenu = pOldMenu->Detach();
    SetMenu(0);

    // Hide Status Bar
    m_wndStatusBar.ShowWindow(SW_HIDE);

    int compactcx = windowrect.Width();
    int compactcy = captionHeight + toolbarrect.Height() + borderHeight + borderHeight;
    cProgramOpts.m_iViewType = VIEW_BUTTON;

    SetWindowPos(&wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);
}

void CMainFrame::OnViewNormalview()
{
    // TODO: Add your command handler code here
    CRect windowrect;
    GetWindowRect(&windowrect);

    CRect rectToolbar;
    m_wndToolBar.GetWindowRect(&rectToolbar);

    // Show Status Bar
    CRect rectStatusbar;
    m_wndStatusBar.GetWindowRect(&rectStatusbar);
    m_wndStatusBar.ShowWindow(SW_SHOW);

    // Dock Toolbar
    DockControlBar(&m_wndToolBar);

    // Show Menu
    if (hMenu)
    {
        SetMenu(CMenu::FromHandle(hMenu));
    }

    // use logo to set widht and heights
    BITMAP bitmap;
    m_bmLogo.GetBitmap(&bitmap);
    int compactcx = bitmap.bmWidth - 26;
    // add width of borders
    compactcx += (::GetSystemMetrics(SM_CXFRAME) * 2) + ::GetSystemMetrics(SM_CXMENUSIZE);

    int compactcy = bitmap.bmHeight - 9;
    // add height of Caption + menu + status + borders
    compactcy += ::GetSystemMetrics(SM_CYCAPTION) + ::GetSystemMetrics(SM_CYMENU) + rectToolbar.Height() +
                 rectStatusbar.Height() + (::GetSystemMetrics(SM_CYFRAME) * 2);

    cProgramOpts.m_iViewType = VIEW_NORMAL;

    SetWindowPos(&wndTop, windowrect.left, windowrect.top, compactcx, compactcy, SWP_SHOWWINDOW);
}

void CMainFrame::OnUpdateViewButtonsview(CCmdUI *pCmdUI)
{
    // TODO: Add your command update UI handler code here
    pCmdUI->SetCheck(cProgramOpts.m_iViewType == VIEW_BUTTON);
}

void CMainFrame::OnUpdateViewNormalview(CCmdUI *pCmdUI)
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
    if (m_TrayIcon.MinimizeToTray())
    {
        if ((nID & 0xFFF0) == SC_MINIMIZE)
        {
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
