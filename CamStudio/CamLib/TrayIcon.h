// CTrayIcon class
// Class to manage the tray icon processing.
// TODO: Temporpary class implementation; new one on the way
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CTrayIcon
{
public:
	CTrayIcon();	// not implemented

	virtual ~CTrayIcon();

	static const UINT m_WM_TRAY_ICON_NOTIFY_MESSAGE = (WM_APP + 50);

	void SetNotifyWnd(HWND hWnd);
	void initTrayIconData(HWND hWnd);
	void finishTrayIconData();

	void TraySetIcon(HICON hIcon);
	void TraySetIcon(UINT nResourceID);
	void TraySetIcon(LPCTSTR lpszResourceName);
	void TraySetToolTip(LPCTSTR lpszToolTip);
	void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
	BOOL TraySetMenu(UINT nResourceID,UINT nDefaultPos=0);
	BOOL TraySetMenu(HMENU hMenu,UINT nDefaultPos=0);
	BOOL TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos=0);
	BOOL TrayUpdate();
	BOOL TrayShow();
	BOOL TrayHide();
	void OnTrayLButtonDown(CPoint pt);
	void OnTrayLButtonDblClk(CPoint pt);
	void OnTrayRButtonDown(CPoint pt);
	void OnTrayRButtonDblClk(CPoint pt);
	void OnTrayMouseMove(CPoint pt);

	BOOL MinimizeToTray() const	{return m_bMinimizeToTray;}

	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);

private:
	NOTIFYICONDATA m_nid;
	BOOL m_bTrayIconVisible;
	BOOL m_bMinimizeToTray;
	BOOL m_bDefaultMenuItem;
	UINT m_nDefaultMenuItem;
	CMenu m_TrayMenu;
};

