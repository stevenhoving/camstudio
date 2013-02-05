// CamWindow.h	- include file for CamStudio Library window api's
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMWINDOW_H
#define CAMWINDOW_H

#pragma once

// CamWindow.cpp
// Window
void SetTitleBar(CString title);
RECT FrameWindow(HWND hWnd, int maxxScreen, int maxyScreen, RECT rcClip);

#endif	// CAMWINDOW_H
