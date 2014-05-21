#include "StdAfx.h"
#include "SystemDeviceEnumerator.h"
#include "Dmo.h"
#pragma comment(lib, "strmiids")
#pragma comment(lib, "Msdmo")
#pragma comment(lib, "Dmoguids")
CSystemDeviceEnumerator::CSystemDeviceEnumerator(void) :
m_pSysDevEnum(NULL),
m_pMoniker(NULL),
m_pEnumCat(NULL),
m_Hr(S_OK)
{
	CoInitialize(NULL);
	CreateInstance();
}


CSystemDeviceEnumerator::~CSystemDeviceEnumerator(void)
{
	m_listDMOs.empty();
	if(m_pEnumCat)
		m_pEnumCat->Release();
	if(m_pSysDevEnum)
		m_pSysDevEnum->Release();
	CoUninitialize();
}
void CSystemDeviceEnumerator::CreateInstance()
{
	m_Hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&m_pSysDevEnum);
	if (FAILED(m_Hr))
	{
		return;
	}
}
void CSystemDeviceEnumerator::EnumerateDMOs(const IID clsidDMOCategory)
{
	IEnumDMO* pEnum = NULL; 
	HRESULT hr = DMOEnum(
		clsidDMOCategory,			// Category
		DMO_ENUMF_INCLUDE_KEYED,	// Included keyed DMOs
		0, NULL,					// Input types (don't care)
		0, NULL,					// Output types (don't care)
		&pEnum);

	if (SUCCEEDED(hr)) 
	{
		CLSID clsidDMO;
		WCHAR* wszName;
		do
		{
			hr = pEnum->Next(1, &clsidDMO, &wszName, NULL);
			if (hr == S_OK) 
			{  
				// Now wszName holds the friendly name of the DMO, 
				// and clsidDMO holds the CLSID. 
				CString sName(wszName);
				m_listDMOs.push_back(sName);
				CoTaskMemFree(wszName);
			}
		} while (hr == S_OK);
		pEnum->Release();
	}
}
HRESULT CSystemDeviceEnumerator::EnumerateVideoCompressors(std::vector<CString>& vectorVidComp)
{
	EnumerateDMOs(DMOCATEGORY_VIDEO_ENCODER);
	return Enumerate(vectorVidComp, CLSID_VideoCompressorCategory);
}
HRESULT CSystemDeviceEnumerator::EnumerateAudioDevices(std::vector<CString>& vectorAudDev)
{
	return Enumerate(vectorAudDev, CLSID_AudioInputDeviceCategory);
}
HRESULT CSystemDeviceEnumerator::EnumerateAudioCompressors(std::vector<CString>& vectorAudComp)
{
	EnumerateDMOs(DMOCATEGORY_AUDIO_ENCODER);
	return Enumerate(vectorAudComp, CLSID_AudioCompressorCategory);
}
HRESULT CSystemDeviceEnumerator::EnumerateVideoCaptureDevices(std::vector<CString>& vectorVidCap)
{
	return Enumerate(vectorVidCap, CLSID_VideoInputDeviceCategory);
}

HRESULT CSystemDeviceEnumerator::Enumerate(std::vector<CString>& vectorDevices, const IID clsidDeviceClass)
{
	if(NULL == m_pSysDevEnum)
		return E_POINTER;

	m_Hr = m_pSysDevEnum->CreateClassEnumerator(clsidDeviceClass, &m_pEnumCat, 0);
	if(NULL == m_pEnumCat)
		return E_POINTER;
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
				// To retrieve the filter's friendly name, do the following
				VARIANT varName;
				VariantInit(&varName);
				m_Hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(m_Hr))
				{
					char szName[128];
					wcstombs(szName, varName.bstrVal, sizeof(szName));
					if(std::find(vectorDevices.begin(), vectorDevices.end(), szName) == vectorDevices.end())
					{
						//if(std::find(m_listDMOs.begin(), m_listDMOs.end(), szName) == m_listDMOs.end())
						{
							vectorDevices.push_back(szName);
						}
					}
				}
				VariantClear(&varName);
				pPropBag->Release();
			}
			m_pMoniker->Release();
		}
	}
	return S_OK;
}


