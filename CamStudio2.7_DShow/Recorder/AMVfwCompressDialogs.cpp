/**********************************************
 *  File: AmVfwCompressDialgs.cpp
 *  Desc: Displays a dialog box provided by a Video for Windows (VFW) codec
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include "StdAfx.h"
#include "AMVfwCompressDialogs.h"


CAMVfwCompressDialogs::CAMVfwCompressDialogs(HWND parent) :
m_hWnd(parent),
m_pSysDevEnum(NULL),
m_pMoniker(NULL),
m_pEnum(NULL)
{
	CoInitialize(NULL);
}


CAMVfwCompressDialogs::~CAMVfwCompressDialogs(void)
{
	if(m_pSysDevEnum)
		m_pSysDevEnum->Release();
	if(m_pMoniker)
		m_pMoniker->Release();
	if(m_pEnum)
		m_pEnum->Release();
	CoUninitialize();
}
HRESULT CAMVfwCompressDialogs::CreateInstance(const IID clsid)
{
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&m_pSysDevEnum);
	if (FAILED(hr) || ! m_pSysDevEnum)
	{
		return E_FAIL;
	}
	hr = m_pSysDevEnum->CreateClassEnumerator(clsid, &m_pEnum, 0);
	if(FAILED(hr) || ! m_pEnum)
	{
			return E_FAIL;
	}
	return S_OK;
}

bool CAMVfwCompressDialogs::HasAboutDialog(const ACamstudioFilter *pFilter)
{
	return (GetPropertyWindow(VfwCompressDialog_QueryAbout, pFilter) == S_OK);
}
bool CAMVfwCompressDialogs::HasConfigDialog(const ACamstudioFilter *pFilter)
{
	return (GetPropertyWindow(VfwCompressDialog_QueryConfig, pFilter) == S_OK);
}
void CAMVfwCompressDialogs::DisplayAboutDialog(const ACamstudioFilter *pFilter)
{
	GetPropertyWindow(VfwCompressDialog_About, pFilter);
}
void CAMVfwCompressDialogs::DisplayConfigDialog(const ACamstudioFilter *pFilter)
{
	GetPropertyWindow(VfwCompressDialog_Config, pFilter);
}

HRESULT CAMVfwCompressDialogs::GetPropertyWindow(VfwCompressDialogs iDialog, const ACamstudioFilter *pFilter)
{
	if(!pFilter)
		return E_FAIL;
	if(!pFilter->GetFilter())
		return E_FAIL;
	
	IAMVfwCompressDialogs *pCompDialog = NULL;
	HRESULT hr = pFilter->GetFilter()->QueryInterface(IID_IAMVfwCompressDialogs, (void**)& pCompDialog);
	if(SUCCEEDED(hr))
	{
		hr = pCompDialog->ShowDialog(iDialog, m_hWnd);
		return hr;
	}
	if(iDialog == VfwCompressDialog_QueryAbout || iDialog == VfwCompressDialog_About)
	{
		return E_FAIL;
	}

	ISpecifyPropertyPages *pProp;
	hr = pFilter->GetFilter()->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) 
	{
		if(VfwCompressDialog_QueryConfig == iDialog || VfwCompressDialog_QueryAbout == iDialog)
		{
			return hr;
		}
		// Get the filter's name and IUnknown pointer.
		FILTER_INFO FilterInfo;
		hr = pFilter->GetFilter()->QueryFilterInfo(&FilterInfo);
		if(FAILED(hr))
			return E_FAIL;
		IUnknown *pFilterUnk = 0;

		if(SUCCEEDED(pFilter->GetFilter()->QueryInterface(IID_IUnknown, (void **)&pFilterUnk)))
		{
			// Show the page. 
			CAUUID caGUID;
			pProp->GetPages(&caGUID);
			pProp->Release();
			OleCreatePropertyFrame(
				m_hWnd,                 // Parent window
				0, 0,                   // Reserved
				FilterInfo.achName,     // Caption for the dialog box
				1,                      // Number of objects (just the filter)
				&pFilterUnk,            // Array of object pointers. 
				caGUID.cElems,          // Number of property pages
				caGUID.pElems,          // Array of property page CLSIDs
				0,                      // Locale identifier
				0, NULL                 // Reserved
				);

			// Clean up.
			if(pFilterUnk)
				pFilterUnk->Release();
			if(FilterInfo.pGraph)
				FilterInfo.pGraph->Release(); 
			if(caGUID.pElems)
				CoTaskMemFree(caGUID.pElems);
		}
	}
	return hr;
}
