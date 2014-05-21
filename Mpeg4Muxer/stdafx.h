// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// truncation of name in debug info (long template names)
#pragma warning(disable:4786)

// ignore deprecated warnings in standard headers
#pragma warning(push)
#pragma warning(disable: 4995)

#include <streams.h>

#include <comdef.h>
_COM_SMARTPTR_TYPEDEF(IMediaSample, IID_IMediaSample);
_COM_SMARTPTR_TYPEDEF(IMediaSeeking, IID_IMediaSeeking);
_COM_SMARTPTR_TYPEDEF(IMemAllocator, IID_IMemAllocator);

#include <list>
#include <vector>
using namespace std;

#pragma warning(pop)

#include "smartptr.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

