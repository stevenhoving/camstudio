#pragma once

//#pragma message("Recorder: stdafx.h")
//#ifdef UNICODE
//#pragma message("UNICODE defined")
//#else
//#pragma message("UNICODE NOT defined")
//#endif

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

////////////////////////////
// Use MS Private Assemblies to prevent side by side configuration errors as reported by users.
// Based on info http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx
// #include "UseMSPrivateAssemblies.h"

#include "targetver.h" // define WINVER

#include <afxwin.h>  // MFC core and standard components
#include <afxext.h>  // MFC extensions
#include <afxdisp.h> // MFC OLE automation classes

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

#include <vfw.h>

#include <libconfig.hh>

#include "Localization\Localization.h"

extern libconfig::Config *g_cfg;
