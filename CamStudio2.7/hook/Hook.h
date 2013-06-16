#ifndef _DEFINED_44E531B1_14D3_11d5_A025_006067718D04
#define _DEFINED_44E531B1_14D3_11d5_A025_006067718D04
#if _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#include <map>

   struct HotKey {
	   DWORD key;
	   DWORD mod;
	   HotKey(const HotKey& rhs) {
		   key = rhs.key;
		   mod = rhs.mod;
	   }
	   HotKey(DWORD k, DWORD m)
		   :key(k), mod(m)
	   {}
	   bool operator < (const HotKey& rhs) const {
		   if (key < rhs.key)
			   return true;
		   if (key == rhs.key && mod < rhs.mod)
			   return true;
		   return false;
	   }
   };

   typedef std::map<HotKey, DWORD> HotKeyMap; // key & mod => WM_HOTKEY code
   typedef std::pair<HotKey, DWORD> HotKeyMapPair; // key & mod => WM_HOTKEY code
__declspec(dllexport) HotKeyMap& getHotKeyMap();
__declspec(dllexport) void setHotKeyWindow(HWND hWnd);
__declspec(dllexport) void setPassThrough(bool pass);
__declspec(dllexport) HCURSOR getCursor();

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#ifdef _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif // _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
   LIBSPEC BOOL InstallMyHook(HWND hWnd, UINT msg);
   LIBSPEC BOOL UninstallMyHook(HWND hWnd);
#undef LIBSPEC
#ifdef __cplusplus
}

#endif // __cplusplus

#define WM_USER_RECORDINTERRUPTED_MSG	_T("WM_USER_RECORDINTERRUPTED_MSG")
#define WM_USER_RECORDPAUSED_MSG		_T("WM_USER_RECORDPAUSED_MSG")
#define WM_USER_SAVECURSOR_MSG			_T("WM_USER_SAVECURSOR_MSG")
#define WM_USER_GENERIC_MSG				_T("WM_USER_GENERIC_MSG")
#define WM_USER_RECORDSTART_MSG			_T("WM_USER_RECORDSTART_MSG")
#define WM_USER_RECORDAUTO_MSG			_T("WM_USER_RECORDAUTO_MSG")
// These declarations are because the latest SDK isn't installed...
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(p) ((int)(short)LOWORD(p))
#define GET_Y_LPARAM(p) ((int)(short)HIWORD(p))
#endif

#endif // _DEFINED_44E531B1_14D3_11d5_A025_006067718D04

