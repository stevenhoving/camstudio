// CTrayIcon class
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TrayIcon.h"

//Tray Icon
//NOTIFYICONDATA IconData;

CTrayIcon::CTrayIcon()
:	m_bTrayIconVisible(FALSE),
	m_bMinimizeToTray(TRUE),
	m_bDefaultMenuItem(0),
	m_nDefaultMenuItem(0)
{
	::ZeroMemory(&m_nid, sizeof(m_nid));
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.uID = 1;
	m_nid.uFlags = NIF_MESSAGE;
	m_nid.hWnd = 0;
	m_nid.uCallbackMessage = m_WM_TRAY_ICON_NOTIFY_MESSAGE;
}

CTrayIcon::~CTrayIcon()
{
	//ver 1.2
	finishTrayIconData();
}

void CTrayIcon::SetNotifyWnd(HWND hWnd)
{
	m_nid.hWnd = hWnd;
}

void CTrayIcon::initTrayIconData(HWND hWnd)
{
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = 0;
	m_nid.uID = 1;
	m_nid.uCallbackMessage = m_WM_TRAY_ICON_NOTIFY_MESSAGE;
	m_nid.hIcon = 0;
	m_nid.szTip[0] = 0;
	m_nid.uFlags = NIF_MESSAGE;
	m_nid.hWnd = hWnd;

	m_bTrayIconVisible = FALSE;
	m_bDefaultMenuItem = 0;
	m_bMinimizeToTray = TRUE;
}

void CTrayIcon::finishTrayIconData()
{
	if (m_nid.hWnd && (m_nid.uID > 0) && m_bTrayIconVisible) {
		Shell_NotifyIcon(NIM_DELETE,&m_nid);
	}
}

void CTrayIcon::TraySetIcon(HICON hIcon)
{
	ASSERT(hIcon);

	m_nid.hIcon = hIcon;
	m_nid.uFlags |= NIF_ICON;
}

void CTrayIcon::TraySetIcon(UINT nResourceID)
{
	ASSERT(nResourceID>0);
	HICON hIcon = AfxGetApp()->LoadIcon(nResourceID);
	if (hIcon) {
		m_nid.hIcon = hIcon;
		m_nid.uFlags |= NIF_ICON;
	} else {
		TRACE("FAILED TO LOAD ICON\n");
	}
}

void CTrayIcon::TraySetIcon(LPCTSTR lpszResourceName)
{
	HICON hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
	if (hIcon) {
		m_nid.hIcon = hIcon;
		m_nid.uFlags |= NIF_ICON;
	} else {
		TRACE("FAILED TO LOAD ICON\n");
	}
}

void CTrayIcon::TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT((strlen(lpszToolTip) > 0) && (strlen(lpszToolTip) < 64));

	strcpy_s(m_nid.szTip, lpszToolTip);
	m_nid.uFlags |= NIF_TIP;
}

BOOL CTrayIcon::TrayShow()
{
	BOOL bSuccess = FALSE;
	if (!m_bTrayIconVisible) {
		bSuccess = Shell_NotifyIcon(NIM_ADD,&m_nid);
		if (bSuccess) {
			m_bTrayIconVisible= TRUE;
		}
	} else {
		TRACE("ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

BOOL CTrayIcon::TrayHide()
{
	BOOL bSuccess = FALSE;
	if (m_bTrayIconVisible) {
		bSuccess = Shell_NotifyIcon(NIM_DELETE,&m_nid);
		if (bSuccess) {
			m_bTrayIconVisible= FALSE;
		}
	} else {
		TRACE("ICON ALREADY HIDDEN");
	}
	return bSuccess;
}

BOOL CTrayIcon::TrayUpdate()
{
	BOOL bSuccess = FALSE;
	if (m_bTrayIconVisible) {
		bSuccess = Shell_NotifyIcon(NIM_MODIFY,&m_nid);
	} else {
		TRACE("ICON NOT VISIBLE");
	}
	return bSuccess;
}

BOOL CTrayIcon::TraySetMenu(UINT nResourceID,UINT /*nDefaultPos*/)
{
	BOOL bSuccess;
	bSuccess = m_TrayMenu.LoadMenu(nResourceID);
	return bSuccess;
}

BOOL CTrayIcon::TraySetMenu(LPCTSTR lpszMenuName,UINT /*nDefaultPos*/)
{
	BOOL bSuccess;
	bSuccess = m_TrayMenu.LoadMenu(lpszMenuName);
	return bSuccess;
}

BOOL CTrayIcon::TraySetMenu(HMENU hMenu,UINT /*nDefaultPos*/)
{
	m_TrayMenu.Attach(hMenu);
	return TRUE;
}

void CTrayIcon::OnTrayRButtonDown(CPoint pt)
{
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
	TrackPopupMenu(m_TrayMenu.GetSubMenu(0)->m_hMenu,TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,0,AfxGetMainWnd()->m_hWnd,NULL);

	PostMessage(AfxGetMainWnd()->m_hWnd,WM_NULL, 0, 0);
}

void CTrayIcon::OnTrayLButtonDown(CPoint /*pt*/)
{
}

void CTrayIcon::OnTrayLButtonDblClk(CPoint /*pt*/)
{
	::SetForegroundWindow( AfxGetMainWnd()->m_hWnd);
	AfxGetMainWnd()->ShowWindow(SW_RESTORE);
}

void CTrayIcon::OnTrayRButtonDblClk(CPoint /*pt*/)
{
}

void CTrayIcon::OnTrayMouseMove(CPoint /*pt*/)
{
}

LRESULT CTrayIcon::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0L;	// no processed
	UINT uID = (UINT) wParam;
	if (uID != 1)
		return lResult;

	UINT uMsg = (UINT) lParam;
	CPoint pt;
	GetCursorPos(&pt);

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		OnTrayMouseMove(pt);
		break;
	case WM_LBUTTONDOWN:
		OnTrayLButtonDown(pt);
		break;
	case WM_LBUTTONDBLCLK:
		OnTrayLButtonDblClk(pt);
		break;

	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		OnTrayRButtonDown(pt);
		break;
	case WM_RBUTTONDBLCLK:
		OnTrayRButtonDblClk(pt);
		break;
	}

	return 0;
}

///////////////////////////////////////////////////
// Ver 1.2
///////////////////////////////////////////////////
//Functions for implementing Tray Icons
///////////////////////////////////////////////////

