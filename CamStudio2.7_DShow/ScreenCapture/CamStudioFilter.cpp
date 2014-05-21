/**********************************************
 *  File: CamStudioFilter.cpp
 *  Desc: CCamStudioFilter Class impl. The Screen Capture filter. Based on MS SDK sample PushSource filter and http://sourceforge.net/projects/screencapturer/
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include <streams.h>
#include "CamStudioScreenCapture.h"
#include "Guids.h"
#include "CamStudioFilterHelper.h"
#include "ICaptureParam.h"

CCamStudioFilter::CCamStudioFilter(IUnknown *pUnk, HRESULT *phr)
	: CSource(NAME("Camstudio Screen Capture"), pUnk, CLSID_PushSourceDesktop)
	, m_pPin(NULL)
{
	// The pin magically adds itself to our pin array.
	// except its not an array since we just have one [?]
	m_pPin = new CCamStudioPin(phr, this);
	if (phr)
	{
		if (m_pPin == NULL)
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}
}


CCamStudioFilter::~CCamStudioFilter() // parent destructor
{
	if(m_pPin)
		delete m_pPin;
}


CUnknown * WINAPI CCamStudioFilter::CreateInstance(IUnknown *pUnk, HRESULT *phr)
{
	// the first entry point
	CCamStudioFilter *pNewFilter = new CCamStudioFilter(pUnk, phr);

	if (phr)
	{
		if (pNewFilter == NULL) 
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}
	return pNewFilter;
}

STDMETHODIMP CCamStudioFilter::QueryInterface(REFIID riid, void **ppv)
{
	if(riid == _uuidof(IAMStreamConfig))
		return m_pPin->QueryInterface(riid, ppv);
	else if(riid == _uuidof(IKsPropertySet))
		return m_pPin->QueryInterface(riid, ppv);
	else if(riid == IID_ICaptureParam)
		return m_pPin->QueryInterface(riid, ppv);
	else if(riid == IID_ICaptureReport)
		return m_pPin->QueryInterface(riid, ppv);
	else
		return CSource::QueryInterface(riid, ppv);
}

HRESULT CCamStudioFilter::GetFilterState() const
{
	if (m_State == State_Paused)
		return VFW_S_CANT_CUE;
	else
		return S_OK;
}

STDMETHODIMP CCamStudioFilter::Stop() {

	CAutoLock filterLock(m_pLock);
	//Default implementation
	HRESULT hr = CSource::Stop();
	//Reset pin resources
	m_pPin->m_iFrameNumber = 0;

	return hr;
}

STDMETHODIMP CCamStudioFilter::Pause() {

	CAutoLock filterLock(m_pLock);
	if(m_State == State_Running)
		m_pPin->Pause();
	//Default implementation
	HRESULT hr = CSource::Pause();
	return hr;
}
