#pragma once

#include <fmt/printf.h>
#include <fmt/ostream.h>

template <typename... Args>
static void _log(fmt::string_view format_str, const Args &... args)
{
#ifdef _DEBUG
    fmt::vprint(format_str, fmt::make_format_args(args...));
#endif
}

static std::ostream &operator<<(std::ostream &os, const AVMediaType d)
{
    return os << av_get_media_type_string(d);
}

static std::ostream &operator<<(std::ostream &os, const AVCodecID d)
{
    return os << avcodec_get_name(d);
}

static std::ostream &operator<<(std::ostream &os, const AVRational d)
{
    return os << d.den << '/' << d.num;
}

static std::ostream &operator<<(std::ostream &os, const AVPixelFormat d)
{
    return os << av_get_pix_fmt_name(d);
}