// Utilities.h : header file

#pragma once

// Utilities functions

// Shows a window
BOOL ShowWnd(CWnd* pWnd)
{
	return pWnd->ShowWindow(SW_SHOW);
}

// Hides a window
BOOL HideWnd(CWnd* pWnd)
{
	return pWnd->ShowWindow(SW_HIDE);
}

// Shows a popup menu
void ShowPopupMenu(CWnd* pWnd, unsigned int uResourseID, int nSubMenuPos)
{
	CPoint aPos;
	GetCursorPos(&aPos);
	CMenu aMenu;
	aMenu.LoadMenu(uResourseID);		
	aMenu.GetSubMenu(nSubMenuPos)->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, aPos.x, aPos.y, pWnd, 0);
}