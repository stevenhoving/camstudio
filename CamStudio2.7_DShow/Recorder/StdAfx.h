// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__EB5B2A10_2312_4984_8371_6DE5181F903F__INCLUDED_)
#define AFX_STDAFX_H__EB5B2A10_2312_4984_8371_6DE5181F903F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma message("Recorder: stdafx.h")
#ifdef UNICODE
#pragma message("UNICODE defined")
#else
#pragma message("UNICODE NOT defined")
#endif

//#define _CRT_SECURE_NO_WARNINGS //tell the compiler not to warn about using sprintf, fopen instead of *_s

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#define _AFX_SECURE_NO_WARNINGS
// replacing calls to deprecated functions with calls to the new secure versions of those functions.

//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES		1
//#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES		1

////////////////////////////
// Use MS Private Assemblies to prevent side by side configuration errors as reported by users.
// Based on info http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx
// #include "UseMSPrivateAssemblies.h"  

#include "targetver.h"		// define WINVER

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <vfw.h>

#pragma warning( disable : 4290)
#include "libconfig.h++"
#include <afxcontrolbars.h>

#include "Localization\Localization.h"
using namespace libconfig;

extern Config *cfg;
#pragma warning( default : 4290)

/*#include <streams.h>

#include <comdef.h>

#include <list>
#include <vector>
using namespace std;
#include "Mpeg4Muxer\smartptr.h"
*/
#include "Mpeg4Muxer\stdafx.h"
#include "Mpeg4Muxer\smartptr.h"
//#include "constants.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__EB5B2A10_2312_4984_8371_6DE5181F903F__INCLUDED_)

// Enable Windows XP visual styles
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
