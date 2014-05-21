// mp4mux.cpp : Defines the entry point for the DLL 
//
// Copyright (c) GDCL 2004-6. All Rights Reserved. 
// You are free to re-use this as the basis for your own filter development,
// provided you retain this copyright notice in the source.
// http://www.gdcl.co.uk

#include "stdafx.h"
#include "MuxFilter.h"

// --- registration tables ----------------

// filter registration -- these are the types that our
// pins accept and produce
const AMOVIESETUP_MEDIATYPE 
Mpeg4Mux::m_sudType[] = 
{
    {
        &MEDIATYPE_Video,
        &MEDIASUBTYPE_NULL      // wild card
    },
    {
        &MEDIATYPE_Audio,
        &MEDIASUBTYPE_NULL
    },
    {
        &MEDIATYPE_Stream,
        &MEDIASUBTYPE_NULL,
    },
};

// registration of our pins for auto connect and render operations
const AMOVIESETUP_PIN 
Mpeg4Mux::m_sudPin[] = 
{
    {
        L"Video",           // pin name
        FALSE,              // is rendered?    
        FALSE,              // is output?
        FALSE,              // zero instances allowed?
        FALSE,              // many instances allowed?
        &CLSID_NULL,        // connects to filter (for bridge pins)
        NULL,               // connects to pin (for bridge pins)
        1,                  // count of registered media types
        &m_sudType[0]       // list of registered media types    
    },
    {
        L"Audio",           // pin name
        FALSE,              // is rendered?    
        FALSE,              // is output?
        FALSE,              // zero instances allowed?
        FALSE,              // many instances allowed?
        &CLSID_NULL,        // connects to filter (for bridge pins)
        NULL,               // connects to pin (for bridge pins)
        1,                  // count of registered media types
        &m_sudType[1]       // list of registered media types    
    },
    {
        L"Output",          // pin name
        FALSE,              // is rendered?    
        TRUE,               // is output?
        FALSE,              // zero instances allowed?
        FALSE,              // many instances allowed?
        &CLSID_NULL,        // connects to filter (for bridge pins)
        NULL,               // connects to pin (for bridge pins)
        1,                  // count of registered media types
        &m_sudType[2]       // list of registered media types    
    },
};

// filter registration information. 
const AMOVIESETUP_FILTER 
Mpeg4Mux::m_sudFilter = 
{
    &__uuidof(Mpeg4Mux),    // filter clsid
    FILTER_NAME,            // filter name
    MERIT_DO_NOT_USE,       // ie explicit insertion only
    3,                      // count of registered pins
    m_sudPin                // list of pins to register
};
// --- COM factory table and registration code --------------

// DirectShow base class COM factory requires this table, 
// declaring all the COM objects in this DLL
CFactoryTemplate g_Templates[] = {
    // one entry for each CoCreate-able object
    {
        Mpeg4Mux::m_sudFilter.strName,
        Mpeg4Mux::m_sudFilter.clsID,
        Mpeg4Mux::CreateInstance,
        NULL,
        &Mpeg4Mux::m_sudFilter
    },
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// self-registration entrypoint
STDAPI DllRegisterServer()
{
    // base classes will handle registration using the factory template table
    HRESULT hr = AMovieDllRegisterServer2(true);
    return hr;
}

STDAPI DllUnregisterServer()
{
    // base classes will handle de-registration using the factory template table
    HRESULT hr = AMovieDllRegisterServer2(false);
    return hr;
}

// if we declare the correct C runtime entrypoint and then forward it to the DShow base
// classes we will be sure that both the C/C++ runtimes and the base classes are initialized
// correctly
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
    return DllEntryPoint(reinterpret_cast<HINSTANCE>(hDllHandle), dwReason, lpReserved);
}


