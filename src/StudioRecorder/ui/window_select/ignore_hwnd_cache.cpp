#include "stdafx.h"
#include "ignore_hwnd_cache.h"
#include <array>

WindowAndProcPath HWNDDataCache::get_window_and_path(HWND hwnd)
{
    std::unique_lock lock(mutex_);
    auto ptr = get_internal(hwnd);
    return ptr ? *ptr : WindowAndProcPath{};
}

HWND HWNDDataCache::get_window(HWND hwnd)
{
    std::unique_lock lock(mutex_);
    auto ptr = get_internal(hwnd);
    return ptr ? ptr->hwnd : nullptr;
}

WindowAndProcPath *HWNDDataCache::get_internal(HWND hwnd) noexcept
{
    auto root = GetAncestor(hwnd, GA_ROOT);
    // Filter the fast and easy cases
    if (!IsWindowVisible(root) || is_invalid_hwnd(root) || is_invalid_class(root) || is_invalid_style(root))
    {
        return nullptr;
    }
    // Get the HWND process path from the cache
    DWORD pid = GetWindowThreadProcessId(root, nullptr);
    auto cache_ptr = get_from_cache(root, pid);
    if (cache_ptr == nullptr)
    {
        cache_ptr = put_in_cache(root, pid);
    }
    // If the app is a UWP app, check if it isn't banned
    if (is_uwp_app(root) && is_invalid_uwp_app(cache_ptr->process_path))
    {
        // cache the HWND of the invalid app so we wont search for it again
        invalid_hwnds_.push_back(root);
        return nullptr;
    }

    return cache_ptr;
}

WindowAndProcPath *HWNDDataCache::get_from_cache(HWND root, DWORD pid) noexcept
{
    auto next = next_timestamp();
    auto it = std::find_if(begin(cache_), end(cache_),
                           [&](const auto &entry) { return root == entry.data.hwnd && pid == entry.pid; });
    if (it == end(cache_))
        return nullptr;

    it->atime = next;
    return &(it->data);
}

WindowAndProcPath *HWNDDataCache::put_in_cache(HWND root, DWORD pid) noexcept
{
    auto next = next_timestamp();
    auto it = std::min_element(begin(cache_), end(cache_),
                               [](const auto &lhs, const auto &rhs) { return lhs.atime < rhs.atime; });
    it->atime = next;
    it->pid = pid;
    it->data.hwnd = root;
    it->data.process_path = get_process_path(root);
    return &(it->data);
}

bool HWNDDataCache::is_invalid_hwnd(HWND hwnd) const noexcept
{
    return std::find(begin(invalid_hwnds_), end(invalid_hwnds_), hwnd) != end(invalid_hwnds_);
}
bool HWNDDataCache::is_invalid_class(HWND hwnd) const noexcept
{
    std::array<char, 256> class_name;
    GetClassNameA(hwnd, std::data(class_name), static_cast<int>(std::size(class_name)));

    for (auto invalid : invalid_classes_)
    {
        if (strcmp(invalid, std::data(class_name)) == 0)
            return true;
    }
    return false;
}
bool HWNDDataCache::is_invalid_style(HWND hwnd) const noexcept
{
    auto style = GetWindowLong(hwnd, GWL_STYLE);
    for (const auto invalid : invalid_basic_styles_)
    {
        if ((invalid & style) != 0)
        {
            return true;
        }
    }
    style = GetWindowLong(hwnd, GWL_EXSTYLE);
    for (const auto invalid : invalid_ext_styles_)
    {
        if ((invalid & style) != 0)
        {
            return true;
        }
    }
    return false;
}
bool HWNDDataCache::is_uwp_app(HWND hwnd) const noexcept
{
    std::array<char, 256> class_name;
    GetClassNameA(hwnd, std::data(class_name), static_cast<int>(std::size(class_name)));
    return strcmp(std::data(class_name), "Windows.UI.Core.CoreWindow") == 0;
}
bool HWNDDataCache::is_invalid_uwp_app(const std::wstring &process_path) const noexcept
{
    for (const auto &invalid : invalid_uwp_apps_)
    {
        // check if process_path ends in "invalid"
        if (std::size(process_path) >= std::size(invalid) &&
            process_path.compare(std::size(process_path) - std::size(invalid), std::size(invalid), invalid) == 0)
        {
            return true;
        }
    }
    return false;
}

unsigned HWNDDataCache::next_timestamp() noexcept
{
    auto next = ++current_timestamp_;
    if (next == 0)
    {
        // Handle overflow by invalidating the cache
        for (auto &entry : cache_)
        {
            entry.data.hwnd = nullptr;
        }
    }
    return next;
}