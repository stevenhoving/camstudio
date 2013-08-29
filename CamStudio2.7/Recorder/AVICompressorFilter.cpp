#include "StdAfx.h"
#include "AVICompressorFilter.h"

#pragma comment(lib, "strmiids")

AVICompressorFilter::AVICompressorFilter(void) :
m_pSysDevEnum(NULL),
m_pMoniker(NULL),
m_pEnumCat(NULL),
m_Hr(S_OK)
{
	CoInitialize(NULL);
	CreateInstance();
}


AVICompressorFilter::~AVICompressorFilter(void)
{
	if(m_pEnumCat)
		m_pEnumCat->Release();
	if(m_pSysDevEnum)
		m_pSysDevEnum->Release();
	CoUninitialize();
}
void AVICompressorFilter::CreateInstance()
{

	m_Hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&m_pSysDevEnum);
	if (FAILED(m_Hr))
	{
		return;
	}
}

void AVICompressorFilter::EnumerateCompressors(std::list<CString>& compressors)
{
	if(NULL == m_pSysDevEnum)
		return;

	m_Hr = m_pSysDevEnum->CreateClassEnumerator(CLSID_VideoCompressorCategory, &m_pEnumCat, 0);
	if(NULL == m_pEnumCat)
		return;
	if(S_OK == m_Hr)
	{
		ULONG cFetched;
		while(m_pEnumCat->Next(1, &m_pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			m_Hr = m_pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
				(void **)&pPropBag);
			if (SUCCEEDED(m_Hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				m_Hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(m_Hr))
				{
					char szName[128];
					wcstombs(szName, varName.bstrVal, sizeof(szName)); 
					compressors.push_back(szName);
				}
				VariantClear(&varName);
				pPropBag->Release();
			}
			m_pMoniker->Release();
		}
	}
}


