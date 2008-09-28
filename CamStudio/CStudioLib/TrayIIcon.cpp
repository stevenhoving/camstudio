#include "StdAfx.h"
#include "TrayIIcon.h"

CTrayIIcon::CTrayIIcon(void)
{
}

CTrayIIcon::~CTrayIIcon(void)
{
}

///////////////////////////////////////////////////
// Ver 1.2
///////////////////////////////////////////////////
//Functions for implementing Tray Icons
///////////////////////////////////////////////////

//Tray Icon
NOTIFYICONDATA IconData;
BOOL bTrayIconVisible = FALSE;
BOOL bMinimizeToTray = TRUE;
BOOL DefaultMenuItem = 0;
UINT nDefaultMenuItem = 0;
CMenu TrayMenu;

void TraySetIcon(HICON hIcon)
{
	ASSERT(hIcon);

	IconData.hIcon = hIcon;
	IconData.uFlags |= NIF_ICON;
}

void TraySetIcon(UINT nResourceID)
{
	ASSERT(nResourceID>0);
	HICON hIcon = AfxGetApp()->LoadIcon(nResourceID);
	if (hIcon) {
		IconData.hIcon = hIcon;
		IconData.uFlags |= NIF_ICON;
	} else {
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void TraySetIcon(LPCTSTR lpszResourceName)
{
	HICON hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
	if (hIcon) {
		IconData.hIcon = hIcon;
		IconData.uFlags |= NIF_ICON;
	} else {
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT((strlen(lpszToolTip) > 0) && (strlen(lpszToolTip) < 64));

	strcpy(IconData.szTip, lpszToolTip);
	IconData.uFlags |= NIF_TIP;
}

void initTrayIconData(HWND hWndGlobal)
{
	IconData.cbSize = sizeof(NOTIFYICONDATA);
	IconData.hWnd = 0;
	IconData.uID = 1;
	IconData.uCallbackMessage = WM_TRAY_ICON_NOTIFY_MESSAGE;
	IconData.hIcon = 0;
	IconData.szTip[0] = 0;
	IconData.uFlags = NIF_MESSAGE;
	IconData.hWnd = hWndGlobal;

	bTrayIconVisible = FALSE;
	DefaultMenuItem = 0;
	bMinimizeToTray = TRUE;
}

void finishTrayIconData()
{
	if (IconData.hWnd && (IconData.uID > 0) && bTrayIconVisible) {
		Shell_NotifyIcon(NIM_DELETE,&IconData);
	}
}

BOOL TrayShow()
{
	BOOL bSuccess = FALSE;
	if (!bTrayIconVisible) {
		bSuccess = Shell_NotifyIcon(NIM_ADD,&IconData);
		if (bSuccess) {
			bTrayIconVisible= TRUE;
		}
	} else {
		TRACE0("ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

BOOL TrayHide()
{
	BOOL bSuccess = FALSE;
	if (bTrayIconVisible) {
		bSuccess = Shell_NotifyIcon(NIM_DELETE,&IconData);
		if (bSuccess) {
			bTrayIconVisible= FALSE;
		}
	} else {
		TRACE0("ICON ALREADY HIDDEN");
	}
	return bSuccess;
}

BOOL TrayUpdate()
{
	BOOL bSuccess = FALSE;
	if (bTrayIconVisible) {
		bSuccess = Shell_NotifyIcon(NIM_MODIFY,&IconData);
	} else {
		TRACE0("ICON NOT VISIBLE");
	}
	return bSuccess;
}

BOOL TraySetMenu(UINT nResourceID,UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = TrayMenu.LoadMenu(nResourceID);
	return bSuccess;
}

BOOL TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = TrayMenu.LoadMenu(lpszMenuName);
	return bSuccess;
}

BOOL TraySetMenu(HMENU hMenu,UINT nDefaultPos)
{
	TrayMenu.Attach(hMenu);
	return TRUE;
}

void OnTrayRButtonDown(CPoint pt)
{
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
	TrackPopupMenu(TrayMenu.GetSubMenu(0)->m_hMenu,TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,0,AfxGetMainWnd()->m_hWnd,NULL);

	PostMessage(AfxGetMainWnd()->m_hWnd,WM_NULL, 0, 0);
}

void OnTrayLButtonDown(CPoint pt)
{
}

void OnTrayLButtonDblClk(CPoint pt)
{
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);
}

void OnTrayRButtonDblClk(CPoint pt)
{
}

void OnTrayMouseMove(CPoint pt)
{
}

