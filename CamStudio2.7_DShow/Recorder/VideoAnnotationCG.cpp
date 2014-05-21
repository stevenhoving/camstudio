#include "StdAfx.h"
#include "VideoAnnotationCG.h"


CVideoAnnotationCG::CVideoAnnotationCG(
	CVideoCaptureDevice *pVideoCap,
	const HWND hOwner) :
m_pVCD(pVideoCap->GetFilter()),
	m_hOwner(hOwner)
{
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, 
		IID_ICaptureGraphBuilder2, (void**)&m_pBuilder);
	if(SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IGraphBuilder, (LPVOID*)&m_pGB);
	}
}


CVideoAnnotationCG::~CVideoAnnotationCG(void)
{
	CoUninitialize();
}

HRESULT CVideoAnnotationCG::BuildCaptureGraph()
{
	// Set graph
	HRESULT hr = m_pBuilder->SetFiltergraph(m_pGB);
	ShowError(hr, "SetFiltergraph");
	hr = m_pGB->AddFilter(m_pVCD, L"");
	if(FAILED(hr))
	{
		ShowError(hr, "AddFilter");
		return hr;
	}
	hr = m_pBuilder->RenderStream(
		&PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Video,
		m_pVCD,
		NULL,
		NULL);
	ShowError(hr, "RenderStream");
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (void **)&m_pVW);
	if(hr != NOERROR)
	{
		ShowError(hr, "QueryInterface");
		return hr;
	}
	m_pVW->put_Owner((OAHWND)m_hOwner);
	m_pVW->put_WindowStyle(WS_CLIPCHILDREN);
	CRect rc;
	GetClientRect(m_hOwner, &rc);
	m_pVW->SetWindowPosition(rc.left,rc.top,rc.Width(), rc.Height());
	m_pVW->put_Visible(OATRUE);
}

HRESULT CVideoAnnotationCG::InitFilters()
{
	return 1;
}
void CVideoAnnotationCG::StartPreview()
{

	IMediaControl *pMC = NULL;
	HRESULT hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Run();
		if(FAILED(hr))
		{
			// stop parts that ran
			pMC->Stop();
		}
		pMC->Release();
	}
	if(FAILED(hr))
	{
		ShowError(hr, "StartPreview");
	}
}
void CVideoAnnotationCG::StopPreview()
{
	// stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if(SUCCEEDED(hr))
	{
		hr = pMC->Stop();
		pMC->Release();
	}
	if(FAILED(hr))
	{
		ShowError(hr, "StopPreview");
	}
}
void CVideoAnnotationCG::ShowError(HRESULT hr, LPCTSTR lpCaption)
{
	if (FAILED(hr))
	{
		TCHAR szErr[MAX_ERROR_TEXT_LEN];
		DWORD res = AMGetErrorText(hr, szErr, MAX_ERROR_TEXT_LEN);
		if (res == 0)
		{
			StringCchPrintf(szErr, MAX_ERROR_TEXT_LEN, "Unknown Error: 0x%2x", hr);
		}
		MessageBox(m_hOwner, szErr, lpCaption, MB_OK | MB_ICONERROR);
	}
}
