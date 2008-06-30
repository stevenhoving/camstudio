// MouseHook.cpp : Defines the entry point for the DLL application.
//


#include "stdafx.h"
#define _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
#include "Hook.h"
#include <stdio.h>
#include <mmsystem.h>

#pragma data_seg(".SHARE")
HWND hWndServer = NULL;
UINT nMsg = 0;
HHOOK hook = NULL;
unsigned long oldKeyMouseTime = 0;
#pragma data_seg()
#pragma comment(linker, "/section:.SHARE,rws")

HINSTANCE hInst;


UINT WM_USER_RECORDINTERRUPTED;
UINT WM_USER_SAVECURSOR;
UINT WM_USER_GENERIC;
UINT WM_USER_RECORDSTART;


static LRESULT CALLBACK hookproc(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  Reason, 
                       LPVOID Reserved
					 )
{
 switch(Reason)
    { /* reason */
     case DLL_PROCESS_ATTACH:
	hInst = hInstance;
	WM_USER_RECORDINTERRUPTED = RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
			WM_USER_SAVECURSOR = RegisterWindowMessage(WM_USER_SAVECURSOR_MSG);
			WM_USER_GENERIC = RegisterWindowMessage(WM_USER_GENERIC_MSG);
			WM_USER_RECORDSTART = RegisterWindowMessage(WM_USER_RECORDSTART_MSG);
	return TRUE;
     case DLL_PROCESS_DETACH:
	if(hWndServer != NULL)
	   UninstallMyHook(hWndServer);
	return TRUE;
    } /* reason */
    return TRUE;
}


__declspec(dllexport) BOOL InstallMyHook(HWND hWnd, UINT message_to_call)
    {
     if(hWndServer != NULL)
	return FALSE; // already hooked!
     hook = SetWindowsHookEx(WH_GETMESSAGE,
			    (HOOKPROC)hookproc,
			    hInst,
			    0);
     if(hook != NULL)
	{ /* success */
	 hWndServer = hWnd;
	 nMsg = message_to_call;
	 
	 return TRUE;
	} /* success */
     return FALSE; // failed to set hook
    } // setMyHook


__declspec(dllexport) BOOL UninstallMyHook(HWND hWnd)
    {
     if(hWnd != hWndServer || hWnd == NULL)
	return FALSE;
     BOOL unhooked = UnhookWindowsHookEx(hook);
     if(unhooked)
	hWndServer = NULL;
     return unhooked;
    } // clearMyHook



static LRESULT CALLBACK hookproc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode < 0)
	{ /* pass it on */
	 CallNextHookEx(hook, nCode, wParam, lParam);
	 return 0;
	} /* pass it on */
     
	 
	 LPMSG msg = (LPMSG)lParam;     
	

	 //ver 1.2
	 if(msg->message == WM_MOUSEMOVE || msg->message == WM_NCMOUSEMOVE || msg->message == WM_LBUTTONDOWN || (msg->message == WM_LBUTTONUP)) {

		 unsigned long currentKeyMouseTime = timeGetTime();
		 unsigned long difftime = currentKeyMouseTime - oldKeyMouseTime;
		 if (difftime>20) {	//up to 50 frames per second	 

			HCURSOR hcur= GetCursor();		 		 
			PostMessage(hWndServer, WM_USER_SAVECURSOR , (unsigned int) hcur, msg->message);
			oldKeyMouseTime = currentKeyMouseTime;
		 
		 }
		 

	 } 	 	 
	 
     return CallNextHookEx(hook, nCode, wParam, lParam);    
    
} 
