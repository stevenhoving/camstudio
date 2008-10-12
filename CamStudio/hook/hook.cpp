/////////////////////////////////////////////////////////////////////////////
// hook.cpp
// Mouse button/move event hook DLL.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#define _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
#include "Hook.h"
#include <stdio.h>
#include <mmsystem.h>

#pragma data_seg(".SHARE")
HWND hWndServer = NULL;
UINT nMsg = 0;
HHOOK hook = NULL;
DWORD oldKeyMouseTime = 0L;
#pragma data_seg()
#pragma comment(linker, "/section:.SHARE,rws")

HINSTANCE hInst = 0;
UINT WM_USER_RECORDINTERRUPTED;
UINT WM_USER_SAVECURSOR;
UINT WM_USER_GENERIC;
UINT WM_USER_RECORDSTART;

namespace {	// annonymous

/////////////////////////////////////////////////////////////////////////////
// hookproc
// monitor mouse button/movement events.
// every delta time between mouse button/move events greater than 20
// milliseconds post a WM_USER_SAVECURSOR message to the server window.
// Always pass message to next hook.
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK hookproc(UINT nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hookproc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	// TODO: A UINT can never be negative
	//if (nCode < 0)
	//{
	//	// Pass it on
	//	::CallNextHookEx(hook, nCode, wParam, lParam);
	//	return 0;
	//}

	// Version 1.2
	LPMSG msg = (LPMSG)lParam;
	if (msg->message == WM_MOUSEMOVE || msg->message == WM_NCMOUSEMOVE || msg->message == WM_LBUTTONDOWN || (msg->message == WM_LBUTTONUP))
	{
		DWORD currentKeyMouseTime = ::timeGetTime();
		DWORD difftime = currentKeyMouseTime - oldKeyMouseTime;
		if (20 < difftime)
		{
			// Up to 50 frames per second
			HCURSOR hcur = ::GetCursor();
			::PostMessage(hWndServer, WM_USER_SAVECURSOR, (unsigned int)hcur, msg->message);
			oldKeyMouseTime = currentKeyMouseTime;
		}
	}

	return ::CallNextHookEx(hook, nCode, wParam, lParam);
}

}	// namespace annonymous

/////////////////////////////////////////////////////////////////////////////
// DllMain
// Entry point for hook.dll.
/////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD  Reason, LPVOID /*Reserved*/)
{
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		hInst = hInstance;
		WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
		WM_USER_SAVECURSOR = ::RegisterWindowMessage(WM_USER_SAVECURSOR_MSG);
		WM_USER_GENERIC = ::RegisterWindowMessage(WM_USER_GENERIC_MSG);
		WM_USER_RECORDSTART = ::RegisterWindowMessage(WM_USER_RECORDSTART_MSG);
		break;
	case DLL_PROCESS_DETACH:
		if (hWndServer != NULL)
		{
			UninstallMyHook(hWndServer);
		}

		break;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// InstallMyHook
// Installs hookproc into the hook chain for all desktop threads, sets
// the notification window to hWnd using message_to_call. 
/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) BOOL InstallMyHook(HWND hWnd, UINT message_to_call)
{
	BOOL bHooked = (0 == hWndServer);
	if (!bHooked)
	{
		return bHooked;	// already hooked
	}

	hook = ::SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)hookproc, hInst, 0);
	bHooked = (0 != hook);
	hWndServer = (bHooked) ? hWnd : 0;
	nMsg = (bHooked) ? message_to_call : 0;
	return bHooked;
}

/////////////////////////////////////////////////////////////////////////////
// UninstallMyHook
// Removes hookproc fromn hook chain for window hWnd
/////////////////////////////////////////////////////////////////////////////
__declspec(dllexport) BOOL UninstallMyHook(HWND hWnd)
{
	BOOL bUnhooked = (hWnd && (hWndServer == hWnd));	// valid arguments
	if (!bUnhooked)
	{
		return bUnhooked;
	}

	bUnhooked = ::UnhookWindowsHookEx(hook);
	if (bUnhooked)
	{
		hWndServer = 0;
	}

	return bUnhooked;
}

