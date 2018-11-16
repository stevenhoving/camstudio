// Class to manage the tray icon processing.
// TODO: Temporpary class implementation; new one on the way....

#pragma once

class CTrayIcon
{
public:
    CTrayIcon();

    virtual ~CTrayIcon();

    static const UINT m_WM_TRAY_ICON_NOTIFY_MESSAGE = (WM_APP + 50);

    void SetNotifyWnd(HWND hWnd);
    void initTrayIconData(HWND hWnd);
    void finishTrayIconData();

    void SetIcon(HICON hIcon);
    void SetIcon(UINT nResourceID);
    void SetIcon(const TCHAR * lpszResourceName);
    void SetToolTip(const TCHAR *lpszToolTip);
    //void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
    BOOL SetMenu(UINT nResourceID, UINT nDefaultPos = 0);
    BOOL SetMenu(HMENU hMenu, UINT nDefaultPos = 0);
    BOOL SetMenu(const TCHAR * lpszMenuName, UINT nDefaultPos = 0);
    BOOL TrayUpdate();
    BOOL Show();
    BOOL Hide();
    BOOL MinimizeToTray() const;
    void OnTrayLButtonDown(CPoint pt);
    void OnTrayLButtonDblClk(CPoint pt);
    void OnTrayRButtonDown(CPoint pt);
    void OnTrayRButtonDblClk(CPoint pt);
    void OnTrayMouseMove(CPoint pt);

    afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);

private:
    NOTIFYICONDATA m_nid;
    BOOL m_bTrayIconVisible{FALSE};
    BOOL m_bMinimizeToTray{TRUE};
    BOOL m_bDefaultMenuItem{0};
    UINT m_nDefaultMenuItem{0};
    CMenu m_TrayMenu;
};
