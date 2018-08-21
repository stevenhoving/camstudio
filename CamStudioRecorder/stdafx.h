#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

// \todo enable this
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h> // MFC core and standard components
#include <afxext.h> // MFC extensions

#include <afxdisp.h> // MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#if 0

//#include "resource.h"

#include <afxwin.h>  // MFC core and standard components
#include <afxext.h>  // MFC extensions
#include <afxdisp.h> // MFC OLE automation classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>
#endif

// just use camcapture gdiplus helper file for now
#include <CamCapture/cam_gdiplus.h>

#include <afxcontrolbars.h> // MFC support for ribbons and control bars

#include "resource.h"
//#include <localization/localization.h>
#include <memory>

