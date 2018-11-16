#pragma once

#include "TrayIcon.h"
#include <iostream>
#include <sstream>
#include <string>

class CMainFrame : public CFrameWnd
{
protected:
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

public:
    CBitmap &Logo()
    {
        return logo_bmp_;
    }

    BOOL PreCreateWindow(CREATESTRUCT &cs) override;

public:
    ~CMainFrame() override;
#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext &dc) const override;
#endif

protected:
    afx_msg auto OnCreate(LPCREATESTRUCT lpCreateStruct) -> int;
    afx_msg void OnClose();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg auto OnTrayNotify(WPARAM wParam, LPARAM lParam) -> LRESULT;
    DECLARE_MESSAGE_MAP()
private:
    CStatusBar wnd_status_bar_;
    CToolBar wnd_toolbar_;
    CBitmap toolbar_bmp_256_;
    CBitmap toolbar_bmp_mask_;
    CImageList image_list_toolbar_;
    CBitmap logo_bmp_;
    CTrayIcon tray_icon_;
};
