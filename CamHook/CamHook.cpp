/////////////////////////////////////////////////////////////////////////////
// hook.cpp
// Mouse button/move event hook DLL.
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CamHook/CamHook.h"
#include "CamHook/ClickQueue.h"

#include <mmsystem.h>

// actual cursor stuff http://msdn.microsoft.com/en-us/magazine/cc301524.aspx

HHOOK g_mouseHookLL = nullptr;
HHOOK g_keyHook = nullptr;
HWND g_hotkeyWnd = nullptr;
HINSTANCE g_hInst = nullptr;

// Keyboard stuff
// HotKeyMap hkm; // write only during key's update, problem is unlikely
// typedef std::map<DWORD,DWORD> ModMap; // write at the beginning only, thread safe
// ModMap modMap; // VK -> Modifier for WM_HOTKEY
// DWORD mod; // current state
// bool PassThrough = false; // pass hotkey to other application

//__declspec(dllexport) HotKeyMap& getHotKeyMap() { return hkm; }
void setHotKeyWindow(HWND hWnd)
{
    g_hotkeyWnd = hWnd;
}
//__declspec(dllexport) void setPassThrough(bool pass) { PassThrough = pass; }

// all that stuff is only here in mouse handler since it brakes double clicks when in the main app
// I guess it is because of stealing thread
HCURSOR m_hSavedCursor = nullptr;
HCURSOR getCursor()
{
    return m_hSavedCursor;
}

// DONE: few thing to consider:
// This way we get an attack by window messages which loads the system. We can
// 1) simply use some function to pull data from the dll / I don't like to export data
// 2) rely on Hit Test procedure (See mouse (not _ll) hook) and hope that in most cases cursor doesn't change by itself.
// This is 99% true for office applications.

// ok.. initiative is punishable...
// cursor update in CamCursor & m_cCamera via CRecorderView::OnSaveCursor is now broken!

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        if (wParam != WM_LBUTTONDOWN && wParam != WM_LBUTTONUP)
        {
            POINT pt;
            if (GetCursorPos(&pt))
            {
                // may need to profile this. dunno how long does it take
                HWND h = WindowFromPoint(pt);
                DWORD id = GetWindowThreadProcessId(h, nullptr);
                DWORD cid = GetCurrentThreadId();
                if (id != cid)
                {
                    if (AttachThreadInput(cid, id, TRUE))
                    {
                        m_hSavedCursor = GetCursor();
                        AttachThreadInput(cid, id, FALSE);
                    }
                }
                else
                {
                    m_hSavedCursor = GetCursor();
                }
            }
        }
        MSLLHOOKSTRUCT mhs = *(LPMSLLHOOKSTRUCT)lParam;
        mhs.flags = wParam;
        if (WM_LBUTTONDOWN <= wParam && wParam <= WM_MOUSEHWHEEL)
        {
            ClickQueue::getInstance().Enqueue(&mhs); // message, pt, time
        }
    }
    return ::CallNextHookEx(g_mouseHookLL, nCode, wParam, lParam);
}
// 2.7
// LRESULT CALLBACK KeyboardProc(
//  __in  int code,
//  __in  WPARAM wParam,
//  __in  LPARAM lParam
//)
//{
//    if (code >= 0) {
//        if (hotkeyWnd) {
//            LPKBDLLHOOKSTRUCT khs = (LPKBDLLHOOKSTRUCT)lParam;
//            DWORD down = (wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN);
//            ModMap::iterator iter = modMap.find(khs->vkCode);
//            if (iter != modMap.end()) {
//                if (down)
//                    mod |= iter->second; // can't use xor :(
//                else
//                    mod &= ~ iter->second;
//            }
//            if (down) { // some key is pressed ... no "else" to make like 'ctrl' itself as a hotkey
//                HotKeyMap::iterator it = hkm.find(HotKey(khs->vkCode, mod));
//                if(it != hkm.end()) {
//                    DWORD id = it->second;
//                    ::SendNotifyMessage(hotkeyWnd, WM_HOTKEY, id, mod);
//                    if (!PassThrough)
//                        return 1;
//                }
//            }
//        }
//    }
//    return ::CallNextHookEx(keyHook, code, wParam, lParam);
//}

//}    // namespace anonymous

/////////////////////////////////////////////////////////////////////////////
// DllMain
// Entry point for hook.dll.
/////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD Reason, LPVOID /*Reserved*/)
{
    switch (Reason)
    {
        case DLL_PROCESS_ATTACH:
            g_hInst = hInstance;
            // 2.7
            // modMap[VK_SHIFT] = MOD_SHIFT;
            // modMap[VK_LSHIFT] = MOD_SHIFT;
            // modMap[VK_RSHIFT] = MOD_SHIFT;
            // modMap[VK_MENU] = MOD_ALT;
            // modMap[VK_LMENU] = MOD_ALT;
            // modMap[VK_RMENU] = MOD_ALT;
            // modMap[VK_CONTROL] = MOD_CONTROL;
            // modMap[VK_LCONTROL] = MOD_CONTROL; // shall we distinguish left & right?
            // modMap[VK_RCONTROL] = MOD_CONTROL;
            //// We are always interested in keyboard shortcuts
            // keyHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, g_hInst, 0);
            break;
        case DLL_PROCESS_DETACH:
            UnhookWindowsHookEx(g_keyHook);
            UninstallMouseHook(nullptr);
            break;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// InstallMouseHook
// Installs hookproc into the hook chain for all desktop threads, sets
// the notification window to hWnd using message_to_call.
/////////////////////////////////////////////////////////////////////////////
BOOL InstallMouseHook(HWND /*hWnd*/, UINT /*message_to_call*/)
{
    if (g_mouseHookLL == nullptr)
    {
        SetWindowsHookEx(WH_MOUSE_LL, static_cast<HOOKPROC>(LowLevelMouseProc), g_hInst, 0);
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// UninstallMouseHook
// Removes hookproc fromn hook chain for window hWnd
/////////////////////////////////////////////////////////////////////////////
BOOL UninstallMouseHook(HWND /*hWnd*/)
{
    if (g_mouseHookLL && UnhookWindowsHookEx(g_mouseHookLL))
    {
        g_mouseHookLL = nullptr;
    }
    return TRUE;
}
