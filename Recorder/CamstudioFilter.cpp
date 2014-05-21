/**********************************************
 *  File: CamstudioFilter.cpp
 *  Desc: Base class for all directshow filters loaded by Camstudio
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include "StdAfx.h"
#include "CamstudioFilter.h"


ACamstudioFilter::ACamstudioFilter(void) :
m_pFilter(NULL)
{
	CoInitialize(NULL);
}


ACamstudioFilter::~ACamstudioFilter(void)
{
	if(m_pFilter)
		m_pFilter->Release();
	CoUninitialize();
}
IBaseFilter *ACamstudioFilter::GetFilter() const
{
	return m_pFilter;
}
const CString& ACamstudioFilter::FilterName()const
{
	return m_sName;
}
const CStringW& ACamstudioFilter::FilterNameW()const
{
	return m_sNameW;
}
HRESULT ACamstudioFilter::LoadFilter(const GUID clsid, CString strName)
{
	m_sName = strName;
	CStringW strNameW(strName);
	m_sNameW = strNameW;
	ICreateDevEnum *pCreateDevEnum = 0;
	IMoniker *pMoniker = 0;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if(hr != NOERROR)
	{
		return hr;
	}

	IEnumMoniker *pEm=0;
	hr = pCreateDevEnum->CreateClassEnumerator(clsid, &pEm, 0);
	if(hr != NOERROR)
	{
		return hr;
	}

	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM = 0;

	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		IPropertyBag *pBag=0;

		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if(hr == NOERROR)
			{
				if(wcscmp((WCHAR*)var.pbstrVal, strNameW.GetBuffer()) == 0)
				{
					pM->AddRef();
					pMoniker = pM;
					break;
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
	}
	pEm->Release();

	pCreateDevEnum->Release();
	if(pMoniker)
	{
		if(!SUCCEEDED(pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&m_pFilter)))
			m_pFilter = NULL;
		pMoniker->Release();
	}
	return hr;

}

