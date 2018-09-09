#pragma once

#include "settings/video_settings.h"

#include <filesystem>
#include <memory>
#include <string>

class CRecorderDoc;

class mouse_capture_ui;
class window_select_ui;
class video_settings_model;
class settings_model;
class mouse_hook;
class shortcut_controller;
class capture_thread;

class CRecorderView : public CView
{
protected:
    CRecorderView();
    DECLARE_DYNCREATE(CRecorderView)

public:
    ~CRecorderView() override;
    CRecorderDoc *GetDocument();

    bool GetRecordState();
    bool GetPausedState();

    // CView
    void OnDraw(CDC *pDC) override;

#ifdef _DEBUG
    void AssertValid() const override;
    void Dump(CDumpContext &dc) const override;
#endif

private:
    std::string generate_temp_filename();
    void restore_window();
    std::filesystem::path generate_auto_filename();

    void set_shortcuts();

protected:
    void OnRecord();
    void OnStop();
    void OnCancel();

    afx_msg void OnRegionRubber();
    afx_msg void OnRegionPanregion();
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
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
    afx_msg void OnPause();
    afx_msg void OnUpdatePause(CCmdUI *pCmdUI);
    afx_msg void OnUpdateStop(CCmdUI *pCmdUI);
    afx_msg void OnOptionsKeyboardshortcuts();
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg BOOL OnEraseBkgnd(CDC *pDC);
    afx_msg void OnRegionWindow();
    afx_msg void OnUpdateRegionWindow(CCmdUI *pCmdUI);
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnOptionsProgramsettings();

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

    std::unique_ptr<shortcut_controller> shortcut_controller_;

    // state machinery for the ongoing recording...
    bool is_recording{false};
    bool is_paused{false};

    // state machinery for indicating that a new recording will be started when the shortcut is pressed.
    bool allow_new_record_start_key_{ true };

    std::string temp_video_filepath_{};

    HBITMAP g_hLogoBM{nullptr};

    double zoom_{1};
    CPoint zoomed_at_;
    DWORD zoom_when_{0}; // FIXME: I hope it is unlikely zoom start at 47 day boundary ever happen by accident
    int zoom_direction_{-1}; // zoomed out

    void DisplayRecordingStatistics(CDC &srcDC);
    void DisplayBackground(CDC &srcDC);
    void DisplayRecordingMsg(CDC &srcDC);

    bool RunViewer(const CString &strNewFile);
    void DisplayAutopanInfo(CRect rc);
};

#ifndef _DEBUG // debug version in vscapView.cpp
inline CRecorderDoc *CRecorderView::GetDocument()
{
    return (CRecorderDoc *)m_pDocument;
}
#endif
