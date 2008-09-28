#pragma once

class CTrayIIcon
{
public:
	CTrayIIcon(void);
	~CTrayIIcon(void);
};

#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 50)

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
void initTrayIconData(HWND hWndGlobal);
void finishTrayIconData();
