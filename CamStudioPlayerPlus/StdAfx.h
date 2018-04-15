#pragma once

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include "targetver.h" // define WINVER

#include <afxwin.h>  // MFC core and standard components
#include <afxext.h>  // MFC extensions
#include <afxdisp.h> // MFC Automation classes
#include <afxcmn.h>  // MFC support for Windows Common Controls

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

// Enable Windows XP visual styles
#pragma comment(                                                                                                       \
    linker,                                                                                                            \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
