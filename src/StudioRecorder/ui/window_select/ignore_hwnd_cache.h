#pragma once

#include "window_select_common.h"

#include <vector>
#include <string>
#include <mutex>
#include <windows.h>

class HWNDDataCache
{
public:
    WindowAndProcPath get_window_and_path(HWND hwnd);
    HWND get_window(HWND hwnd);

private:
    // Return pointer to our internal cache - we cannot pass this to user
    // since next call to get_* might invalidate that pointer
    WindowAndProcPath *get_internal(HWND hwnd) noexcept;
    WindowAndProcPath *get_from_cache(HWND root, DWORD pid) noexcept;
    WindowAndProcPath *put_in_cache(HWND root, DWORD pid) noexcept;
    // Various validation routines
    bool is_invalid_hwnd(HWND hwnd) const noexcept;
    bool is_invalid_class(HWND hwnd) const noexcept;
    bool is_invalid_style(HWND hwnd) const noexcept;
    bool is_uwp_app(HWND hwnd) const noexcept;
    bool is_invalid_uwp_app(const std::wstring &binary_path) const noexcept;

    // List of HWNDs that are not interesting - like desktop, Cortana, etc
    std::vector<HWND> invalid_hwnds_ = {GetDesktopWindow(), GetShellWindow()};
    // List of invalid window basic styles
    std::vector<LONG> invalid_basic_styles_ = {WS_CHILD, WS_DISABLED};
    // List of invalid window extended styles
    std::vector<LONG> invalid_ext_styles_ = {WS_EX_TOOLWINDOW, WS_EX_NOACTIVATE};
    // List of invalid window classes - things like start menu, etc.
    std::vector<const char *> invalid_classes_ = {"SysListView32", "WorkerW", "Shell_TrayWnd", "Shell_SecondaryTrayWnd",
                                                 "Progman", "CamStudio"};
    // List of invalid persistent UWP app - like Cortana
    std::vector<std::wstring> invalid_uwp_apps_ = {L"SearchUI.exe"};

    // Cache for HWND/PID pair to process path. A collision here, where a new process
    // not in cache gets to reuse a cached PID and then reuses the same HWND handle
    // seems unlikely.
    std::mutex mutex_;
    // Handle timestamp wrap
    unsigned next_timestamp() noexcept;
    unsigned current_timestamp_ = 0;
    struct Entry
    {
        DWORD pid = 0;
        // access time - when retiring element from cache we pick
        // one with minimal atime value. We update this value
        // every time we query the cache
        unsigned atime = 0;
        WindowAndProcPath data;
    };
    std::vector<Entry> cache_{32};
};
