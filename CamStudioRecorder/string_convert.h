#pragma once

#include <codecvt>
#include <string>

//https://stackoverflow.com/questions/4358870/convert-wstring-to-string-encoded-in-utf-8
// convert UTF-8 string to wstring
static
std::wstring utf8_to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}

// convert wstring to UTF-8 string
static
std::string wstring_to_utf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(str);
}