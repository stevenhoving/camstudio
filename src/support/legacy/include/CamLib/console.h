#pragma once

#include <windows.h>
#include <cstdio>

namespace console
{
void create()
{
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CONOUT$", "w", stdout);

    // \todo add buffer changes so we can control the console's history and window size.
}
} // namespace console

