// MouseHook.cpp : Defines the entry point for the DLL application.
//


#include "stdafx.h"
#define _COMPILING_44E531B1_14D3_11d5_A025_006067718D00
#include "Hookkey.h"
#include <stdio.h>
#include <mmsystem.h>

#pragma data_seg(".SHAREKEY")
HWND hWndServerKey = NULL;
UINT nMsgKey = 0;
HHOOK hookKey = NULL;
unsigned long oldKeyTime = 0;
int previousKey = 0;
#pragma data_seg()
#pragma comment(linker, "/section:.SHAREKEY,rws")

HINSTANCE hInst;

UINT WM_USER_KEYSTART;

void ErrMsg(char frmt[], ...);

int ctrlDown = 0;
int altDown = 0;
int shiftDown = 0;


static LRESULT CALLBACK hookkeyproc(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  Reason, 
                       LPVOID Reserved
					 )
{
 switch(Reason)
    { /* reason */
     case DLL_PROCESS_ATTACH:
	hInst = hInstance;
			WM_USER_KEYSTART = RegisterWindowMessage(WM_USER_KEYSTART_MSG);
	return TRUE;
     case DLL_PROCESS_DETACH:
	if(hWndServerKey != NULL)
	   UninstallMyKeyHook(hWndServerKey);
	return TRUE;
    } /* reason */
    return TRUE;
}


__declspec(dllexport) BOOL InstallMyKeyHook(HWND hWnd, UINT message_to_call)
    {
     if(hWndServerKey != NULL)
	return FALSE; // already hooked!
     //hook = SetWindowsHookEx(WH_GETMESSAGE,
	 hookKey = SetWindowsHookEx(WH_KEYBOARD,
			    (HOOKPROC)hookkeyproc,
			    hInst,
			    0);
     if(hookKey != NULL)
	{ /* success */
	 hWndServerKey = hWnd;
	 nMsgKey = message_to_call;
	 
	 return TRUE;
	} /* success */
     return FALSE; // failed to set hook
    } // setMyHook


__declspec(dllexport) BOOL UninstallMyKeyHook(HWND hWnd)
    {
     if(hWnd != hWndServerKey || hWnd == NULL)
	return FALSE;
     BOOL unhooked = UnhookWindowsHookEx(hookKey);
     if(unhooked)
	hWndServerKey = NULL;
     return unhooked;
    } // clearMyHook



static LRESULT CALLBACK hookkeyproc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode < 0)
	{ /* pass it on */
	 CallNextHookEx(hookKey, nCode, wParam, lParam);
	 return 0;
	} /* pass it on */
     


		DWORD keyflags = lParam;
		DWORD transitionFLAG = 0x80000000;
		DWORD repeatFLAG = 0x40000000;

		DWORD val = keyflags & transitionFLAG;

			

		if (wParam==VK_SHIFT) 
		{
			if (val==0)
				shiftDown = 1;
			else
				shiftDown = 0;
		}

		if (wParam==VK_MENU) 
		{
			if (val==0)
				altDown = 1;
			else
				altDown = 0;
		}

		if (wParam==VK_CONTROL) 
		{
			if (val==0)
				ctrlDown = 1;
			else
				ctrlDown = 0;
		}


		DWORD specialKeys = ctrlDown * 4 + shiftDown * 2 + altDown * 1;


	
		//Can this code prevent too many messages from being sent to vscap	and cause it to crash ?		
		unsigned long currentKeyTime = timeGetTime();
		unsigned long difftime = currentKeyTime - oldKeyTime;
		
		//ver 1.8
		if (difftime>150) {				
		
			//Repeating keys..
			if ((previousKey == wParam) && (difftime<300))
				return CallNextHookEx(hookKey, nCode, wParam, lParam);   		
			
			previousKey = wParam;
			
			//if (lParam>0) {
				
				
				//ShowWindow( hWndServerKey, SW_SHOW); 
			
				//PostMessage(hWndServerKey, WM_USER_KEYSTART , wParam, lParam);
				PostMessage(hWndServerKey, WM_USER_KEYSTART , wParam, specialKeys);
				oldKeyTime = currentKeyTime;
			//}

		}
			  
	
		 
	 
     return CallNextHookEx(hookKey, nCode, wParam, lParam);    
    
} 



void ErrMsg(char frmt[], ...) {
  
  DWORD written;
  char buf[5000];
  va_list val;
  
  va_start(val, frmt);
  wvsprintf(buf, frmt, val);

  const COORD _80x50 = {80,50};
  static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}