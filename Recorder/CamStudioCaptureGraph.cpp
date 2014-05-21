/**********************************************
 *  File: CamStudioCaptureGraph.cpp
 *  Desc: Represent the capture graph used by 
 *        Camstudio
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include "StdAfx.h"
#include "CamStudioCaptureGraph.h"
#include "Profile.h"
//#pragma comment(lib, "Quartz.lib")
//#pragma comment(lib, "Mpeg4Muxer.lib")
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if (x) { x->Release(); x = NULL; }
#endif

CCamStudioCaptureGraph::CCamStudioCaptureGraph(
		CScreenCaptureFilter *pVideoCap,
		CVideoCompressorFilter *pVidComp,
		CAudioCaptureFilter *pAudioCap,
		CAudioCompressorFilter *pAudioComp,
		const HWND hOwner
		) :
m_pVidCap(pVideoCap->GetFilter()),		// Video Capture filter
m_pAudCap(pAudioCap->GetFilter()),		// Audio Capture filter
m_pVidComp(pVidComp->GetFilter()),		// Video Compressor filter
m_pAudComp(pAudioComp->GetFilter()),	// Audio Compressor filter
m_hOwner(hOwner),
m_pGB(NULL),
m_pSink(NULL),
m_pConfigAviMux(NULL),
m_pMC(NULL),
m_pRender(NULL),
m_pBasicAudio(NULL),
m_pCapParam(NULL),
m_pReport(NULL),
m_lpwstrFilename(NULL),
m_lpwstrACD(pAudioCap->FilterNameW()),
m_lpwstrAC(pAudioComp->FilterNameW()),
m_lpwstrVCD(pVideoCap->FilterNameW()),
m_lpwstrVC(pVidComp->FilterNameW())
{
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, 
		IID_ICaptureGraphBuilder2, (void**)&m_pBuilder);
	ShowError(hr, "CoCreateInstance->CaptureGraphBuilder2");
	if(SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
			IID_IGraphBuilder, (LPVOID*)&m_pGB);
		ShowError(hr, "CoCreateInstance->FilterGraph");
	}
}
CCamStudioCaptureGraph::~CCamStudioCaptureGraph(void)
{
	FreeCaptureFilters();
	CoUninitialize();
}
void CCamStudioCaptureGraph::AutoPan() const
{
	m_pCapParam->EnableAutoPan(cProgramOpts.m_bAutoPan, cProgramOpts.m_iMaxPan);
}
CAPTUREREPORT& CCamStudioCaptureGraph::Report()
{
	if(!m_pReport)
		return m_Report;
	m_Report.fFPS = m_pReport->GetActualFPS();
	m_Report.ullCaptureFileSize = m_pReport->GetCurrentCaptureSize();
	m_Report.iFrameNumber = m_pReport->GetFramesWritten();
	unsigned long ulTimeElapsed = m_pReport->GetTimeElapsed();
	m_Report.timeElapsed.hours = ulTimeElapsed / 1000 / 60 / 60 % 60;
	m_Report.timeElapsed.minutes = ulTimeElapsed / 1000 / 60 % 60;
	m_Report.timeElapsed.seconds = ulTimeElapsed / 1000 % 60;
	m_Report.rc = m_pReport->GetCaptureRect();
	m_Report.pt = m_pReport->GetPoint();
	m_Report.nStatus = m_pReport->GetStatus();
	return m_Report;
}
HRESULT CCamStudioCaptureGraph::InitCaptureFilters(HWND hParent, HWND hWnd, HWND hFlashingWnd, const CRect rectFrame, const UINT nCaptureMode)
{
	HRESULT hr;
	// Video Capture filter
	if(m_pVidCap && CAMSTUDIO_SCREEN_CAPTURE.Compare(m_lpwstrVCD) == 0)
	{
		// ICaptureReport API
		hr = m_pVidCap->QueryInterface(IID_ICaptureReport, (void**)&m_pReport);
		if(FAILED(hr))
		{
			ShowError(hr, "ICaptureReport");
		}
		// ICaptureParam API
		hr = m_pVidCap->QueryInterface(IID_ICaptureParam, (void**)& m_pCapParam);
		if(FAILED(hr))
		{
			ShowError(hr, "ICaptureParam");
		}
		else
		{
			// Display flashing window?
			m_pCapParam->DisplayFlashingWindow(cProgramOpts.m_bFlashingRect);
			m_pCapParam->SetFlashingWindow(hFlashingWnd);
			// Auto pan ?
			m_pCapParam->EnableAutoPan(cProgramOpts.m_bAutoPan, cProgramOpts.m_iMaxPan);
			// Set FPS
			m_pCapParam->SetFPS(cVideoOpts.m_iFramesPerSecond);
			RECT rc;
			rc.left = rectFrame.left;
			rc.top = rectFrame.top;
			rc.right = rectFrame.right;
			rc.bottom = rectFrame.bottom;
			switch(nCaptureMode)
			{
				case CAPTURE_FIXED:

					m_pCapParam->CaptureFix(hWnd, rc);
					break;
				case CAPTURE_VARIABLE:
					m_pCapParam->CaptureVariable(hWnd, rc);
					break;
				case CAPTURE_FULLSCREEN:
					m_pCapParam->CaptureFullScreen(rc);
					break;
				case CAPTURE_WINDOW:
					m_pCapParam->CaptureWindow(hWnd, hFlashingWnd);
					break;
				case CAPTURE_ALLSCREENS:
					m_pCapParam->CaptureAllScreens(rc);
					break;
			}
		}
	}
	
	// Set graph
	hr = m_pBuilder->SetFiltergraph(m_pGB);

	if(FAILED(hr))
	{
		ShowError(hr, "SetFiltergraph");
		return hr;
	}
	// Add Video Capture filter to graph
	hr = m_pGB->AddFilter(m_pVidCap, m_lpwstrVCD);
	if(FAILED(hr))
	{
		ShowError(hr, "Video Capture Device");
		return E_FAIL;
	}
	// Add Video Compressor filter to graph
	hr = m_pGB->AddFilter(m_pVidComp, m_lpwstrVC);
	if(FAILED(hr))
	{
		ShowError(hr, "Video Codec");
		return E_FAIL;
	}
	// Add Audio Capture filter to graph
	hr = m_pGB->AddFilter(m_pAudCap, m_lpwstrACD);
	if(FAILED(hr))
	{
		ShowError(hr, "Audio Capture Device");
		return E_FAIL;
	}
	// Add Audio Compressor filter to graph
	hr = m_pGB->AddFilter(m_pAudComp, m_lpwstrAC);
	if(FAILED(hr))
	{
		ShowError(hr, "Audio Codec");
		return E_FAIL;
	}
	
	// we use this interface to set the captured wave format
	hr = m_pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, m_pAudCap,
		IID_IAMStreamConfig, (void **)&m_pASC);
	
	ShowError(hr, "InitCaptureFilters->IAMStreamConfig");
	
	return S_OK;
}

void CCamStudioCaptureGraph::FreeCaptureFilters()
{
	SAFE_RELEASE(m_pRender);
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pSink);
	SAFE_RELEASE(m_pConfigAviMux);
	SAFE_RELEASE(m_pMC);
}
void CCamStudioCaptureGraph::UnInitialize()
{
	// Make sure its our device being used
	if(m_pVidCap && CAMSTUDIO_SCREEN_CAPTURE.Compare(m_lpwstrVCD) == 0)
	{
	}
}
HRESULT CCamStudioCaptureGraph::BuildCaptureGraph(const CString sOutputFile, const BOOL bCaptureAudio, const eVideoFormat vf)
{
	CStringW sOutputFilenameW(sOutputFile);
	HRESULT hr;
	if(vf == ModeMP4)
	{
		hr = m_pBuilder->SetOutputFileName(&__uuidof(Mpeg4Mux), sOutputFilenameW,
										  &m_pRender, &m_pSink);
	}
	else
	{
		hr = m_pBuilder->SetOutputFileName(&MEDIASUBTYPE_Avi, sOutputFilenameW,
										  &m_pRender, &m_pSink);
	}
	if(hr != NOERROR)
	{
		ShowError(hr, "BuildCaptureGraph->SetOutputFileName");
		return hr;
	}

	// Now tell the AVIMUX to write out AVI files that old apps can read properly.
	// If we don't, most apps won't be able to tell where the keyframes are,
	// slowing down editing considerably
	// Doing this will cause one seek (over the area the index will go) when
	// you capture past 1 Gig, but that's no big deal.
	// NOTE: This is on by default, so it's not necessary to turn it on
	// Also, set the proper MASTER STREAM
	if(vf == ModeAVI)
	{
		hr = m_pRender->QueryInterface(IID_IConfigAviMux, (void **)&m_pConfigAviMux);
		if(hr == NOERROR && m_pConfigAviMux)
		{
			//m_pConfigAviMux->SetOutputCompatibilityIndex(TRUE);
			if(bCaptureAudio)
			{
				// Audio stream as master
				hr = m_pConfigAviMux->SetMasterStream(1);
				if(hr != NOERROR)
					ShowError(hr, "BuildCaptureGraph->AVI Mux->SetMasterStream(1)");
			}
			/*else
			{
				// Master stream none
				hr = m_pConfigAviMux->SetMasterStream(0);
				ShowError(hr, "BuildCaptureGraph->AVI Mux->SetMasterStream(0)");
			}*/
		}
	}
	
	// Stream 0 in Filter graph's view
	hr = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video,
		m_pVidCap, m_pVidComp, m_pRender);
	if(hr != NOERROR)
	{
		ShowError(hr, "RenderStream->Video");
		return hr;
	}

	IEnumPins *pEnum = NULL;
	IPin *pPin = NULL;
	// Find the pin that supports IAMVideoCompression (if any).
	m_pVidComp->EnumPins(&pEnum);
	while (S_OK == pEnum->Next(1, &pPin, NULL))
	{
		PIN_INFO pininfo;
		pPin->QueryPinInfo(&pininfo);
		if(pininfo.dir == PINDIR_OUTPUT)
		{
			hr = pPin->QueryInterface(IID_IAMVideoCompression, (void**)&m_pVC);
			pPin->Release();
			if (SUCCEEDED(hr)) // Found the interface
			{
				break;
			}
		}
	}
	if(SUCCEEDED(hr) && m_pVC)
	{
		long lCap;													// Capability flags
		long lKeyFrame = cVideoOpts.m_iKeyFramesEvery;				// Key frames
		double dQuality = (double)cVideoOpts.m_iCompQuality / 100;	// m_iCompQuality is expressed 1 to 100 (we need 0.0 to .10
		// Default values
		long lKeyFrameDef, lPFrameDef;
		double QualityDef;
		// Get default values and capabilities.
		hr = m_pVC->GetInfo(0, 0, 0, 0, &lKeyFrameDef, &lPFrameDef,
			&QualityDef, &lCap);
		if (SUCCEEDED(hr))
		{
			// Set values based on Video Options dialog values
			if (lCap & CompressionCaps_CanKeyFrame)
			{
				hr = m_pVC->put_KeyFrameRate((long)cVideoOpts.m_iKeyFramesEvery);
				ShowError(hr, "Set Compression Key Frame Rate");
			}
			if (lCap & CompressionCaps_CanQuality)
			{
				hr = m_pVC->put_Quality(dQuality);
				ShowError(hr, "Set Compression Quality");
			}
		}
	}

	// Render the audio capture pin?
	//

	if(bCaptureAudio)
	{
		// Stream 1 in Filter graph's view
		hr = m_pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio,
										 m_pAudCap, m_pAudComp, m_pRender);
		if(hr != NOERROR)
		{
			ShowError(hr, "RenderStream->Audio");
			return hr;
		}
		IEnumPins *pEnum = NULL;
		IPin *pPin = NULL;
		// Find the pin that supports IAMStreamConfig (if any).
		m_pAudComp->EnumPins(&pEnum);
		while (S_OK == pEnum->Next(1, &pPin, NULL))
		{
			PIN_INFO pininfo;
			pPin->QueryPinInfo(&pininfo);
			if(pininfo.dir == PINDIR_OUTPUT)
			{
				hr = pPin->QueryInterface(IID_IAMStreamConfig, (void**)&m_pASC);
				if (SUCCEEDED(hr)) // Found the interface
					break;
				pPin->Release();
			}
		}
		if(SUCCEEDED(hr) && m_pASC)
		{
			// Set values based on our config
			WAVEFORMATEX *pwfx = (WAVEFORMATEX*) CoTaskMemAlloc(sizeof(WAVEFORMATEX));
			if(pwfx)
			{
				pwfx->cbSize = cAudioFormat.m_iCbSize;
				pwfx->nAvgBytesPerSec = cAudioFormat.m_iAvgBytesPerSec;
				pwfx->nBlockAlign = cAudioFormat.m_iBlockAlign;
				pwfx->nChannels = cAudioFormat.m_iNumChannels;
				pwfx->nSamplesPerSec = cAudioFormat.m_iSamplesPerSeconds;
				pwfx->wBitsPerSample = cAudioFormat.m_iBitsPerSample;
				pwfx->wFormatTag = cAudioFormat.m_iFormatTag;

				AM_MEDIA_TYPE *pmt = (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
				if(pmt)
				{
					pmt->majortype = MEDIATYPE_Audio;
					if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) 
						pmt->subtype = ((PWAVEFORMATEXTENSIBLE)pwfx)->SubFormat;
					else 
						pmt->subtype = FOURCCMap(pwfx->wFormatTag);

					pmt->formattype = FORMAT_WaveFormatEx;
					pmt->bFixedSizeSamples = TRUE;
					pmt->bTemporalCompression = FALSE;
					pmt->lSampleSize = pwfx->nBlockAlign;
					pmt->pUnk = NULL;
					
					if (pwfx->wFormatTag == WAVE_FORMAT_PCM)
						pmt->cbFormat  = sizeof(WAVEFORMATEX);
					else
						pmt->cbFormat  = sizeof(WAVEFORMATEX) + pwfx->cbSize;
					pmt->pbFormat = (PBYTE)CoTaskMemAlloc(pmt->cbFormat);
					if (pmt->pbFormat != NULL)
					{
						if (pwfx->wFormatTag == WAVE_FORMAT_PCM) {
							CopyMemory(pmt->pbFormat, pwfx, sizeof(PCMWAVEFORMAT));
							((WAVEFORMATEX *)pmt->pbFormat)->cbSize = 0;
						} else {
							CopyMemory(pmt->pbFormat, pwfx, pmt->cbFormat);
						}
					}
					hr = m_pASC->SetFormat(pmt);
					ShowError(hr, "IAMStreamConfig->SetFormat");
					
					CoTaskMemFree((PVOID)pmt);
					CoTaskMemFree((PVOID)pwfx);
				}
			}
		}
	}
	return TRUE;
}

void CCamStudioCaptureGraph::DestroyCaptureGraph() const
{
	if(!m_pGB)
		return;
	IEnumFilters *pEnum = NULL;
	HRESULT hr = m_pGB->EnumFilters(&pEnum);
	if (SUCCEEDED(hr))
	{
		IBaseFilter *pFilter = NULL;
		while (S_OK == pEnum->Next(1, &pFilter, NULL))
		{
			// Remove the filter.
			m_pGB->RemoveFilter(pFilter);
			// Reset the enumerator.
			pEnum->Reset();
			pFilter->Release();
		}
		pEnum->Release();
	}
}

BOOL CCamStudioCaptureGraph::StartCapture()
{
	HRESULT hr;
	BOOL bHasStreamControl;

	REFERENCE_TIME rtStart = MAXLONGLONG, rtStop = MAXLONGLONG;

	// don't capture quite yet...
	hr = m_pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL,
		NULL, &rtStart, NULL, 0, 0);

	// Do we have the ability to control capture and preview separately?
	bHasStreamControl = SUCCEEDED(hr);

	// prepare to run the graph
 
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if(FAILED(hr))
	{
		ShowError(hr, "StartCapture->IMediaControl");
		return FALSE;
	}

	if(bHasStreamControl)
		hr = m_pMC->Run();
	else
		hr = m_pMC->Pause();
	if(FAILED(hr))
	{
		ShowError(hr, "StartCapture->Run(1)");
		// stop parts that started
		m_pMC->Stop();
		return FALSE;
	}

	// Start capture NOW!
	if(bHasStreamControl)
	{
		// turn the capture pin on now!
		hr = m_pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, NULL,
			NULL, NULL, &rtStop, 0, 0);
		ShowError(hr, "StartCapture->ControlStream");
	}
	else
	{
		hr = m_pMC->Run();
		if(FAILED(hr))
		{
			ShowError(hr, "StartCapture->Run(2)");
			// stop parts that started
			m_pMC->Stop();
			return FALSE;
		}
	}
	return TRUE;
}
void CCamStudioCaptureGraph::StopCapture()
{
	//SAFE_RELEASE(m_pCapParam);
	//SAFE_RELEASE(m_pReport);
	if(!m_pMC)
		return;
	FILTER_STATE fs;
	HRESULT hr;
	ShowError(SUCCEEDED(m_pMC->GetState(OPERATION_TIMEOUT, (OAFilterState*)&fs)), "StopCapture->GetState");
	if(State_Running == fs || State_Paused == fs)
	if(!SUCCEEDED(hr = m_pMC->Stop()))
		ShowError(hr, "StopCapture");
	DestroyCaptureGraph();
}
BOOL CCamStudioCaptureGraph::ResumeCapture()
{
	if(!m_pMC)
		return FALSE;
	FILTER_STATE fs;
	HRESULT hr;
	ShowError(m_pMC->GetState(OPERATION_TIMEOUT, (OAFilterState*)&fs), "ResumeCapture->GetState");
	if(fs == State_Paused)
	{
		if(SUCCEEDED(hr = m_pMC->Run()))
			return TRUE;
		else
			ShowError(hr, "ResumeCapture");
	}
	return FALSE;
}
BOOL CCamStudioCaptureGraph::PauseCapture()
{
	HRESULT hr;
	if(!m_pMC)
		return FALSE;
	FILTER_STATE fs;
	ShowError(m_pMC->GetState(OPERATION_TIMEOUT, (OAFilterState*)&fs), "PauseCapture->GetState");
	if(fs == State_Running)
	{
		if(SUCCEEDED(hr = m_pMC->Pause()))
			return TRUE;
		else
			ShowError(hr, "PauseCapture");
	}
	return FALSE;
}

void CCamStudioCaptureGraph::ShowError(HRESULT hr, LPCTSTR lpCaption)
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
