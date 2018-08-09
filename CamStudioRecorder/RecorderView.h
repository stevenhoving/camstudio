#pragma once

#include "BasicMessageDlg.h"
#include "capture_thread.h"

#include <memory>
#include <string>

#define TEMPFILETAGINDICATOR "~temp"

class CRecorderDoc;

class mouse_capture_ui;
class window_select_ui;

class video_settings_model;
class settings_model;

class mouse_hook;

class CRecorderView : public CView
{
protected: // create from serialization only
    CRecorderView();
    DECLARE_DYNCREATE(CRecorderView)

public:
    CRecorderDoc *GetDocument();

    void SaveSettings();
    void LoadSettings();


public:
    virtual void OnDraw(CDC *pDC); // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);


private:
    std::string generate_temp_filename(video_container::type container);
    void restore_window();


    std::filesystem::path generate_auto_filename();
protected:
    virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);


    // Implementation
public:
    ~CRecorderView() override;

    bool GetRecordState();
    bool GetPausedState();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    //{{AFX_MSG(CRecorderView)
    afx_msg void OnRegionRubber();
    afx_msg void OnRegionPanregion();
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnRecord();
    afx_msg void OnStop();
    afx_msg void OnUpdateRegionPanregion(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRegionRubber(CCmdUI *pCmdUI);
    afx_msg void OnVideoSettings();
    afx_msg void OnOptionsCursoroptions();
    afx_msg void OnUpdateRecord(CCmdUI *pCmdUI);
    afx_msg void OnRegionFullscreen();
    afx_msg void OnUpdateRegionFullscreen(CCmdUI *pCmdUI);
    afx_msg void OnRegionSelectScreen();
    afx_msg void OnUpdateRegionSelectScreen(CCmdUI *pCmdUI);
    afx_msg void OnRegionAllScreens();
    afx_msg void OnUpdateRegionAllScreens(CCmdUI *pCmdUI);
    afx_msg void OnHelpWebsite();
    afx_msg void OnHelpHelp();
    afx_msg void OnPause();
    afx_msg void OnUpdatePause(CCmdUI *pCmdUI);
    afx_msg void OnUpdateStop(CCmdUI *pCmdUI);
    afx_msg void OnHelpFaq();
    afx_msg void OnOptionsKeyboardshortcuts();
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg void OnRegionWindow();
    afx_msg void OnUpdateRegionWindow(CCmdUI *pCmdUI);
    afx_msg void OnCaptureChanged(CWnd *pWnd);

    afx_msg LRESULT OnRecordStart(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRecordInterrupted(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRecordPaused(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUserGeneric(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
public:
    DECLARE_EVENTSINK_MAP()

public:
    // TODO: should be private
    static UINT WM_USER_RECORDINTERRUPTED;
    static UINT WM_USER_RECORDPAUSED;
    static UINT WM_USER_SAVECURSOR;
    static UINT WM_USER_GENERIC;
    static UINT WM_USER_RECORDSTART;

private:
    std::unique_ptr<capture_thread> capture_thread_;
    std::unique_ptr<video_settings_model> video_settings_model_;
    std::unique_ptr<settings_model> settings_model_;

    std::unique_ptr<mouse_capture_ui> mouse_capture_ui_;
    std::unique_ptr<window_select_ui> window_select_ui_;

    std::unique_ptr<mouse_hook> mouse_hook_;

    double zoom_{1};
    CPoint zoomed_at_;
    DWORD zoom_when_{0}; // FIXME: I hope it is unlikely zoom start at 47 day boundary ever happen by accident
    int zoom_direction_{-1}; // zoomed out

    void DisplayRecordingStatistics(CDC &srcDC);
    void DisplayBackground(CDC &srcDC);
    void DisplayRecordingMsg(CDC &srcDC);
    bool SaveAppSettings();

    bool RunViewer(const CString &strNewFile);
    void DisplayAutopanInfo(CRect rc);
    // dialog controls
public:
    afx_msg void OnOptionsProgramsettings();
};

#ifndef _DEBUG // debug version in vscapView.cpp
inline CRecorderDoc *CRecorderView::GetDocument()
{
    return (CRecorderDoc *)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// External Variables
extern HWND g_hWndGlobal;
extern bool g_bRecordState;
extern CRect g_rcUse;
extern CRect g_old_rcClip;
