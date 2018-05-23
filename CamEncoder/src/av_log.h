#pragma once

#include <fmt/printf.h>

template <typename... Args>
static void _log(fmt::string_view format_str, const Args &... args)
{
#ifdef _DEBUG
    fmt::vprint(format_str, fmt::make_format_args(args...));
#endif
}