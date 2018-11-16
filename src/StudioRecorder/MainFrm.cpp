#include "stdafx.h"
#include "MainFrm.h"
#include "Recorder.h"
#include "RecorderView.h"
#include <CamLib/console.h>
#include <afxdatarecovery.h>

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_MESSAGE(CTrayIcon::m_WM_TRAY_ICON_NOTIFY_MESSAGE, &CMainFrame::OnTrayNotify)
    ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

static UINT indicators[] = {
    ID_SEPARATOR // status line indicator
};

CMainFrame::CMainFrame()
{
#ifdef _DEBUG
    console::create();
#endif

    logo_bmp_.LoadBitmap(IDB_BITMAP3);
}

CMainFrame::~CMainFrame()
{
    toolbar_bmp_256_.DeleteObject();
    toolbar_bmp_mask_.DeleteObject();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!wnd_toolbar_.Create(this) || !wnd_toolbar_.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE("Failed to create toolbar\n");
        return -1;
    }

    if (!wnd_status_bar_.Create(this) || !wnd_status_bar_.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
    {
        TRACE("Failed to create status bar\n");
        return -1;
    }

    toolbar_bmp_256_.LoadBitmap(IDB_TOOLBAR256);
    toolbar_bmp_mask_.LoadBitmap(IDB_TOOLBARMASK);


    image_list_toolbar_.Create(21, 21, ILC_COLOR16 | ILC_MASK, 4, 4);
    //m_ilToolBar.Create(32,32, ILC_COLOR16 | ILC_MASK, 4, 4);
    image_list_toolbar_.Add(&toolbar_bmp_256_, &toolbar_bmp_mask_);

    // VC6
    // m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar);

    // VC5
    ::SendMessage(wnd_toolbar_.GetToolBarCtrl().m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)image_list_toolbar_.GetSafeHandle());

    wnd_toolbar_.ModifyStyle(0, TBSTYLE_FLAT);
    wnd_toolbar_.SetBarStyle(wnd_toolbar_.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    tray_icon_.SetNotifyWnd(*this);
    tray_icon_.SetIcon(IDR_MAINFRAME);
    tray_icon_.SetToolTip(_T("CamStudio"));
    tray_icon_.SetMenu(IDR_TRAYMENU);
    tray_icon_.Show();

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
    logo_bmp_.GetBitmap(&bitmap);

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

void CMainFrame::OnClose()
{
    auto view = dynamic_cast<CRecorderView *>(GetActiveView());
    if (view)
    {
        view->shutdown();
    }

    CFrameWnd::OnClose();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (tray_icon_.MinimizeToTray())
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
    return tray_icon_.OnTrayNotify(wParam, lParam);
}
