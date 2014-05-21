// MouseMsgHandlerImpl.h : header file

#pragma once

#include "TrayIcon.h"
#include "TrayIconMouseMsgHandler.h"
#include "Utilities.h"

// Left mouse button double click message handler
class CLeftMouseDblClickMsgHandler: public CTrayIconMouseMsgHandler
{
public:

	CLeftMouseDblClickMsgHandler() : CTrayIconMouseMsgHandler(WM_LBUTTONDBLCLK){}

	void MouseMsgHandler()
	{
		ShowWnd(AfxGetMainWnd());
	}
};

// Right mouse button click message handler
class CRightMouseClickMsgHandler: public CTrayIconMouseMsgHandler
{
public:

	CRightMouseClickMsgHandler() : CTrayIconMouseMsgHandler(WM_RBUTTONDOWN){}

	void MouseMsgHandler()
	{
		ShowPopupMenu(AfxGetMainWnd(), IDR_TRAYMENU, 0);
	}
};

// Mouse hover message handler
class CMouseHoverMsgHandler: public CTrayIconMouseMsgHandler
{
public:

	CMouseHoverMsgHandler(CTrayIcon* pTrayIcon) : CTrayIconMouseMsgHandler(WM_MOUSEFIRST)
	{
		m_pTrayIcon = pTrayIcon;
	}

	void SetTrayIcon(CTrayIcon* pTrayIcon)
	{
		m_pTrayIcon = pTrayIcon;
	}

	void MouseMsgHandler()
	{
		m_pTrayIcon->HoverIcon();
	}

private:

	CTrayIcon* m_pTrayIcon;
};