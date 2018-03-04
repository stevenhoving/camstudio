#pragma once

int GetOperatingSystem();
BOOL WinYield();
LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
