#pragma once

#include <CamLib/TrayIcon.h>
#include "UpdateDialog.h"
#include <iostream>
#include <sstream>
#include <string>

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

    // Attributes
public:
    CBitmap &Logo()
    {
        return m_bmLogo;
    }

    // Operations
public:
    void UpdateViewtype();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    //}}AFX_VIRTUAL

    // Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected: // control bar embedded members
    CStatusBar m_wndStatusBar;
    CToolBar m_wndToolBar;
    CBitmap m_ToolbarBitmap256;
    CBitmap m_ToolbarBitmapMask;
    CImageList m_ilToolBar;
    CBitmap m_bmLogo;
    // CBitmap        m_bmNotification;
    CUpdateDialog m_bDialog;

    // Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnViewCompactview();
    afx_msg void OnUpdateViewCompactview(CCmdUI *pCmdUI);
    afx_msg void OnViewButtonsview();
    afx_msg void OnUpdateViewButtonsview(CCmdUI *pCmdUI);
    afx_msg void OnViewNormalview();
    afx_msg void OnUpdateViewNormalview(CCmdUI *pCmdUI);
    afx_msg void OnViewtype();
    //}}AFX_MSG
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnXNote(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnMotionDetector(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
private:
    CTrayIcon m_TrayIcon;
    static const UINT WM_USER_XNOTE;
    static const UINT WM_USER_MOTIONDETECTOR;
    void CheckForNewVersion();
    void Parse(int result[3], const std::string &input);
    bool LessThanVersion(const std::string &a, const std::string &b);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

extern int maxxScreen;
extern int maxyScreen;
extern int minxScreen;
extern int minyScreen;
