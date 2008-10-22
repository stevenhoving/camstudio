// CamSys.h	- include file for CamStudio Library system api's
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMSYS_H
#define CAMSYS_H

#pragma once

int GetOperatingSystem();
BOOL WinYield();
LONG GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata);

#endif	// CAMSYS_H
