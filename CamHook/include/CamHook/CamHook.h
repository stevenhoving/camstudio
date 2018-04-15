#pragma once

#include "CamHookExport.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>

struct HotKey
{
    DWORD key;
    DWORD mod;

    HotKey(DWORD k, DWORD m)
        : key(k)
        , mod(m)
    {
    }

    HotKey(const HotKey &rhs)
    {
        key = rhs.key;
        mod = rhs.mod;
    }

    HotKey &operator=(const HotKey &rhs)
    {
        key = rhs.key;
        mod = rhs.mod;
        return *this;
    }

    bool operator<(const HotKey &rhs) const
    {
        if (key < rhs.key)
        {
            return true;
        }
        if (key == rhs.key && mod < rhs.mod)
        {
            return true;
        }
        return false;
    }
};

typedef std::map<HotKey, DWORD> HotKeyMap;      // key & mod => WM_HOTKEY code
typedef std::pair<HotKey, DWORD> HotKeyMapPair; // key & mod => WM_HOTKEY code


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    CAMHOOK_EXPORT void setHotKeyWindow(HWND hWnd);
    CAMHOOK_EXPORT HCURSOR getCursor();

    CAMHOOK_EXPORT BOOL InstallMouseHook(HWND hWnd, UINT msg);
    CAMHOOK_EXPORT BOOL UninstallMouseHook(HWND hWnd);

#ifdef __cplusplus
}
#endif // __cplusplus

#define WM_USER_RECORDINTERRUPTED_MSG _T("WM_USER_RECORDINTERRUPTED_MSG")
#define WM_USER_RECORDPAUSED_MSG _T("WM_USER_RECORDPAUSED_MSG")
#define WM_USER_SAVECURSOR_MSG _T("WM_USER_SAVECURSOR_MSG")
#define WM_USER_GENERIC_MSG _T("WM_USER_GENERIC_MSG")
#define WM_USER_RECORDSTART_MSG _T("WM_USER_RECORDSTART_MSG")
#define WM_USER_RECORDAUTO_MSG _T("WM_USER_RECORDAUTO_MSG")

// These declarations are because the latest SDK isn't installed...
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(p) ((int)(short)LOWORD(p))
#define GET_Y_LPARAM(p) ((int)(short)HIWORD(p))
#endif
