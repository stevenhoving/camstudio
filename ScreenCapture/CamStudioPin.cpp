/**********************************************
 *  File: CamStudioPin.cpp
 *  Desc: CCamStudioPin Class. The Screen Capture filter class impl. Based on MS SDK sample PushSource filter and http://sourceforge.net/projects/screencapturer/
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/

#include <streams.h>
#include "CamStudioScreenCapture.h"
#include "Guids.h"
#include "CamStudioFilterHelper.h"
#include <wmsdkidl.h>


CCamStudioPin::CCamStudioPin(HRESULT *phr, CCamStudioFilter *pFilter)
	: CSourceStream(NAME("Camstudio Screen Capture Pin"), phr, pFilter, L"Capture"),
	m_iFrameNumber(0),
	m_pParent(pFilter),
	m_bFormatAlreadySet(false),
	m_hRawBitmap(NULL),
	m_previousFrameEndTime(0),
	m_hWndToTrack(NULL),
	m_iCaptureMode(CAPTURE_FULLSCREEN),
	m_ullFramesWritten(0),
	m_ullCaptureSize(0),
	m_ullTimeElapsed(0),
	m_nFPS(10),
	m_iZoom(1),
	m_dwZoomWhen(0),
	m_iZoomDirection(-1),
	m_clrBorderON(RGB(255, 255, 180)),
	m_clrBorderOFF(RGB(0, 255, 80)),
	m_hCursorMove(::LoadCursor(NULL, IDC_SIZEALL))
{
	m_ptZoomAt.x = 10;
	m_ptZoomAt.y = 10;
	
	m_hScrDc = GetDC(m_hWndToTrack);
	m_rScreen.left   = m_rScreen.top = 0;
	m_rScreen.right  = GetDeviceCaps(m_hScrDc, HORZRES);
	m_rScreen.bottom = GetDeviceCaps(m_hScrDc, VERTRES);

	m_nCaptureWidth = m_rScreen.right - m_rScreen.left;
	m_nCaptureHeight = m_rScreen.bottom - m_rScreen.top;
	SetCaptureScreen(NULL, m_rScreen, m_iCaptureMode);
	UpdateCaptureDimension();
	SetFPS(m_nFPS);
}

CCamStudioPin::~CCamStudioPin()
{
	::ReleaseDC(NULL, m_hScrDc);
	::DeleteDC(m_hScrDc);
	if (m_hRawBitmap)
		DeleteObject(m_hRawBitmap);
}

void CCamStudioPin::SetCaptureScreen(HWND hWnd, RECT rcSource, int iCaptureMode)
{
	m_iCaptureMode = iCaptureMode;
	// Get coordinates of the window on the screen
	//if(CAPTURE_WINDOW == m_iCaptureMode)
	{
		m_hWndToTrack = hWnd;
		::GetWindowRect(m_hWndToTrack, &m_rScreen);
		//m_hScrDc = GetDC(m_hWndToTrack);
	}
	m_rScreen = rcSource;
	m_rcPanCurrent = m_rScreen;
	m_rcPanDest = m_rScreen;
	UpdateCaptureDimension();
}

void CCamStudioPin::UpdateCaptureDimension()
{
	m_nCaptureWidth = m_rScreen.right - m_rScreen.left;
	m_nCaptureHeight = m_rScreen.bottom - m_rScreen.top;
}
void CCamStudioPin::AutoPan(bool bEnable, int iSpeed)
{
	m_bAutoPan = bEnable;
	m_iMaxPan = iSpeed;
}
HRESULT CCamStudioPin::Run(REFERENCE_TIME tStart)
{
	return CSourceStream::Run();
}
/*HRESULT CCamStudioPin::Pause()
{
	MessageBox(NULL, L"", L"", 0);
	return CSourceStream::Pause();
}
*/
DWORD CCamStudioPin::ThreadProc(void)
{
	HRESULT hr;  // the return code from calls
	Command com;
	//cWnd.SetCaptureScreen(m_hWndToTrack, m_rScreen, m_iCaptureMode);
	do 
	{
		com = GetRequest();
		if (com != CMD_INIT) 
		{
			DbgLog((LOG_ERROR, 1, TEXT("Thread expected init command")));
			Reply((DWORD) E_UNEXPECTED);
		}
	} while (com != CMD_INIT);

	DbgLog((LOG_TRACE, 1, TEXT("CSourceStream worker thread initializing")));

	hr = OnThreadCreate(); // perform set up tasks
	if (FAILED(hr)) 
	{
		DbgLog((LOG_ERROR, 1, TEXT("CSourceStream::OnThreadCreate failed. Aborting thread.")));
		OnThreadDestroy();
		Reply(hr);	// send failed return code from OnThreadCreate
		return 1;
	}

	// Initialisation suceeded
	Reply(NOERROR);
	if(m_bDisplayFlashingWindow)
	{
		SetUpRegion(m_rScreen, m_bAutoPan);
	}
	Command cmd;
	do 
	{
		cmd = GetRequest();
		switch (cmd) 
		{
		case CMD_EXIT:
			Reply(NOERROR);
			break;

		case CMD_RUN:
			Reply(NOERROR);
			DoBufferProcessingLoop();
			break;
		case CMD_PAUSE:
			Reply(NOERROR);
			break;

		case CMD_STOP:
			Reply(NOERROR);
			m_iFrameNumber = 0;
			break;

		default:
			Reply((DWORD) E_NOTIMPL);
			break;
		}
	} while (cmd != CMD_EXIT);
	hr = OnThreadDestroy();

	if (FAILED(hr)) 
	{
		return 1;
	}

	return 0;
}


HRESULT CCamStudioPin::DoBufferProcessingLoop() {

	Command com;
	OnThreadStartPlay();
	int ctr = 0;
	do 
	{
		while (!CheckRequest(&com))
		{
			IMediaSample *pSample;
			HRESULT hr;
			hr = GetDeliveryBuffer(&pSample,NULL,NULL,/*0*/AM_GBF_NOWAIT);
			if (FAILED(hr))
			{
				Sleep(1);
				continue;	// go round again. Perhaps the error will go away
				// or the allocator is decommited & we will be asked to
				// exit soon.
			}
			{
				hr = FillBuffer(pSample);
			}
			if (hr == S_OK)
			{
				{
					hr = Deliver(pSample);
				}
				pSample->Release();

				// downstream filter returns S_FALSE if it wants us to
				// stop or an error if it's reporting an error.
				if(hr != S_OK)
				{
					return S_OK;
				}

			} 
			else if (hr == S_FALSE) 
			{
				// derived class wants us to stop pushing data
				pSample->Release();
				DeliverEndOfStream();
				return S_OK;
			} 
			else
			{
				// derived class encountered an error
				pSample->Release();
				DeliverEndOfStream();
				m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
				return hr;
			}
			// all paths release the sample
		}

		// For all commands sent to us there must be a Reply call!
		if(com == CMD_PAUSE)
		{
			Reply(NOERROR);
			// Paused
			while(!CheckRequest(&com))
			{
				Sleep(1);
			}
		}
		else if (com == CMD_RUN)
		{
			Reply(NOERROR);
		} 
		else if (com != CMD_STOP) 
		{
			Reply((DWORD) E_UNEXPECTED);
		}
	} while (com != CMD_STOP);
	return S_FALSE;
}
void CCamStudioPin::StopStreamingThread()
{
}

HRESULT CCamStudioPin::FillBuffer(IMediaSample *pSample)
{
	BYTE *pData = NULL;
	CheckPointer(pSample, E_POINTER);
	
	// Access the sample's data buffer
	pSample->GetPointer(&pData);

	// Make sure that we're still using video format
	ASSERT(m_mt.formattype == FORMAT_VideoInfo);
	VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*) m_mt.pbFormat;
	CopyScreenImage(pData, (BITMAPINFO *) &(pVih->bmiHeader));
	//CopyScreenToDataBlock(m_hScrDc, pData, (BITMAPINFO *) &(pVih->bmiHeader), pSample);
	CRefTime now;
	CRefTime endFrame;
	CSourceStream::m_pFilter->StreamTime(now);

	// wait until we "should" send this frame out...
	if((now > 0) && (now < m_previousFrameEndTime) && m_iFrameNumber > 1 )//&& !m_bStopCapture) 
	{ 
		while(now < m_previousFrameEndTime)
		{ // guarantees monotonicity too
			Sleep(1);
			CSourceStream::m_pFilter->StreamTime(now);
		}
		// avoid a tidge of creep since we sleep until [typically] just past the previous end.
		endFrame = m_previousFrameEndTime + m_rtFrameLength;
		m_previousFrameEndTime = endFrame;
	}
	else 
	{
		// if there's no reference clock, it will "always" miss a frame
		// have to add a bit here, or it will always be "it missed some time" for the next round...forever!
		endFrame = now + m_rtFrameLength;
		// most of this stuff I just made up because it "sounded right"
		if(now > (m_previousFrameEndTime - (long long) m_rtFrameLength)) 
		{
			// let it pretend and try to catch up, it's not quite a frame behind
			m_previousFrameEndTime = m_previousFrameEndTime + m_rtFrameLength;
		}
		else 
		{
			endFrame = now + m_rtFrameLength/2;
		}
		m_previousFrameEndTime = endFrame;
	}
	m_previousFrameEndTime = max(0, m_previousFrameEndTime);

	pSample->SetTime((REFERENCE_TIME *) &now, (REFERENCE_TIME *) &endFrame);

	pSample->SetSyncPoint(TRUE);

	// only set discontinuous for the first...
	pSample->SetDiscontinuity(m_iFrameNumber <= 1);
	
	m_iFrameNumber++;
	m_ullTimeElapsed = now.Millisecs();
	return S_OK;
}
//void CCamStudioPin::CopyScreenToDataBlock(HDC hScrDC, BYTE *pData, BITMAPINFO *pHeader, IMediaSample *pSample)
//{
//	HDC         hMemDC;         // screen DC and memory DC
//	HBITMAP     hOldBitmap;    // handles to device-dependent bitmaps
//	
//	// create a DC for the screen and create
//	// a memory DC compatible to screen DC   
//	hMemDC = CreateCompatibleDC(hScrDC); //  0.02ms Anything else to reuse, this one's pretty fast...?
//	
//	if(CAPTURE_WINDOW == m_iCaptureMode)
//		::GetWindowRect(m_hWndToTrack, &m_rScreen);
//
//	// select new bitmap into memory DC
//	hOldBitmap = (HBITMAP) SelectObject(hMemDC, m_hRawBitmap);
//	// Bit block transfer from screen our compatible memory DC.	Apparently this is faster than stretching.
//	if(!BitBlt(hMemDC, 0, 0, m_nCaptureWidth, m_nCaptureHeight, hScrDC, m_rScreen.left, m_rScreen.top, SRCCOPY))
//		MessageBox(NULL, L"!", L"", 0);
//	AddMouse(hMemDC, &m_rScreen, hScrDC, m_hWndToTrack);
//
//	// select old bitmap back into memory DC and get handle to
//	// bitmap of the capture...whatever this even means...	
//	HBITMAP hRawBitmap2 = (HBITMAP) SelectObject(hMemDC, hOldBitmap);
//	GetDIBits(hScrDC, hRawBitmap2, 0, m_nCaptureHeight, pData, pHeader, DIB_RGB_COLORS); 
//	DeleteDC(hMemDC);
//}
float CCamStudioPin::GetFps() 
{
	return (float) (UNITS / m_rtFrameLength);
}

//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated (this is negotiatebuffersize). So we have a look at m_mt to see what size image we agreed.
// Then we can ask for buffers of the correct size to contain them.
//
HRESULT CCamStudioPin::DecideBufferSize(IMemAllocator *pAlloc,
	ALLOCATOR_PROPERTIES *pProperties)
{
	CheckPointer(pAlloc,E_POINTER);
	CheckPointer(pProperties,E_POINTER);

	HRESULT hr = NOERROR;

	VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
	BITMAPINFOHEADER header = pvi->bmiHeader;
	ASSERT(header.biPlanes == 1);

	pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;
	pProperties->cBuffers = 1; 
	// Ask the allocator to reserve us some sample memory. NOTE: the function
	// can succeed (return NOERROR) but still not have allocated the
	// memory that we requested, so we must check we got whatever we wanted.
	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pProperties,&Actual);
	if(FAILED(hr))
	{
		return hr;
	}

	// Is this allocator unsuitable?
	if(Actual.cbBuffer < pProperties->cbBuffer)
	{
		return E_FAIL;
	}
	// create a bitmap compatible with the screen DC
	//if(m_hRawBitmap)
		//DeleteObject (m_hRawBitmap);
	//m_hRawBitmap = CreateCompatibleBitmap(m_hScrDc, m_nCaptureWidth, m_nCaptureHeight);
	CreateSample();
	m_previousFrameEndTime = 0;
	m_iFrameNumber = 0;
	return NOERROR;
}


//
// SetMediaType
//
// Called when a media type is agreed between filters (i.e. they call GetMediaType+GetStreamCaps/ienumtypes I guess till they find one they like, then they call SetMediaType).
// all this after calling Set Format, if they even do, I guess...
// pMediaType is assumed to have passed CheckMediaType "already" and be good to go...
// except WFMLE sends us a junk type, so we check it anyway LODO do we? Or is it the other method Set Format that they call in vain? Or it first?
HRESULT CCamStudioPin::SetMediaType(const CMediaType *pMediaType)
{
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	// Pass the call up to my base class
	HRESULT hr = CSourceStream::SetMediaType(pMediaType); // assigns our local m_mt via m_mt.Set(*pmt) ... 

	if(SUCCEEDED(hr))
	{
		VIDEOINFO *pvi = (VIDEOINFO *) m_mt.Format();
		if (pvi == NULL)
			return E_UNEXPECTED;

		switch(pvi->bmiHeader.biBitCount)
		{
		case 12:     // i420
			hr = S_OK;
			break;
		case 8:     // 8-bit palettized
		case 16:    // RGB565, RGB555
		case 24:    // RGB24
		case 32:    // RGB32
			// Save the current media type and bit depth
			//m_MediaType = *pMediaType; // use SetMediaType above instead
			hr = S_OK;
			break;

		default:
			// We should never agree any other media types
			ASSERT(FALSE);
			hr = E_INVALIDARG;
			break;
		}
		// The frame rate at which your filter should produce data is determined by the AvgTimePerFrame field of VIDEOINFOHEADER
		if(pvi->AvgTimePerFrame) // or should Set Format accept this? hmm...
			m_rtFrameLength = pvi->AvgTimePerFrame; // allow them to set whatever fps they request, i.e. if it's less than the max default
		m_rScreen.right = m_rScreen.left + pvi->bmiHeader.biWidth;
		m_rScreen.bottom = m_rScreen.top + pvi->bmiHeader.biHeight;
	}
	return hr;
} // SetMediaType

/*HRESULT CCamStudioPin::Run( void)
{
	MessageBox(NULL, L"run", L"", 0);
	return S_OK;
}
HRESULT CCamStudioPin::Pause( void)
{
	return S_OK;
}

HRESULT CCamStudioPin::Stop( void)
{
	return S_OK;
}
*/
//HRESULT CCamStudioPin::GetState( 
//	/* [in] */ LONG msTimeout,
//	/* [out] */ __RPC__out OAFilterState *pfs)
//{
//	return S_OK;
//}

//HRESULT CCamStudioPin::RenderFile( 
//		/* [in] */ __RPC__in BSTR strFilename)
//{
//	return E_NOTIMPL;
//}

//HRESULT CCamStudioPin::AddSourceFilter( 
//	/* [in] */ __RPC__in BSTR strFilename,
//	/* [out] */ __RPC__deref_out_opt IDispatch **ppUnk)
//{
//	return E_NOTIMPL;
//}

//HRESULT CCamStudioPin::get_FilterCollection( 
//		/* [retval][out] */ __RPC__deref_out_opt IDispatch **ppUnk)
//{
//	return E_NOTIMPL;
//}

//HRESULT CCamStudioPin::get_RegFilterCollection( 
//		/* [retval][out] */ __RPC__deref_out_opt IDispatch **ppUnk)
//{
//	return E_NOTIMPL;
//}

//HRESULT CCamStudioPin::StopWhenReady( void)
//{
	//return E_NOTIMPL;
//}
HRESULT CCamStudioPin::QueryInterface(REFIID riid, void **ppv)
{
	// Standard OLE stuff, needed for capture source
	if(riid == _uuidof(IAMStreamConfig))
		return GetInterface((IAMStreamConfig*) this, ppv);
	else if(riid == _uuidof(IKsPropertySet))
		return GetInterface((IKsPropertySet*) this, ppv);
	else if(riid == IID_ICaptureParam)
		return GetInterface((ICaptureParam*) this, ppv);
	else if(riid == IID_ICaptureReport)
		return GetInterface((ICaptureReport*)this, ppv);
	else
		return CSourceStream::QueryInterface(riid, ppv);

	AddRef(); // avoid interlocked decrement error... // I think
	return S_OK;
}

//
// CheckMediaType
// We will accept 8, 16, 24 or 32 bit video formats, in any
// image size that gives room to bounce.
// Returns E_INVALIDARG if the mediatype is not acceptable
//
HRESULT CCamStudioPin::CheckMediaType(const CMediaType *pMediaType)
{
	CAutoLock cAutoLock(m_pFilter->pStateLock());

	CheckPointer(pMediaType,E_POINTER);

	const GUID Type = *(pMediaType->Type());
	if(Type != GUID_NULL && (Type != MEDIATYPE_Video) ||	// we only output video, GUID_NULL means any
		!(pMediaType->IsFixedSize()))						// in fixed size samples
	{
		return E_INVALIDARG;
	}
	// Check for the subtypes we support
	if (pMediaType->Subtype() == NULL)
		return E_INVALIDARG;

	const GUID SubType2 = *pMediaType->Subtype();
	
	// Get the format area of the media type
	VIDEOINFO *pvi = (VIDEOINFO *) pMediaType->Format();
	if(pvi == NULL)
		return E_INVALIDARG; // usually never this...

	if(	   (SubType2 != MEDIASUBTYPE_RGB8) // these are all the same value? But maybe the pointers are different. Hmm.
		&& (SubType2 != MEDIASUBTYPE_RGB565)
		&& (SubType2 != MEDIASUBTYPE_RGB555)
		&& (SubType2 != MEDIASUBTYPE_RGB24)
		&& (SubType2 != MEDIASUBTYPE_RGB32)
		&& (SubType2 != GUID_NULL) // means "anything", I guess...
		)
	{
		if(SubType2 == WMMEDIASUBTYPE_I420) 
		{ // 30323449-0000-0010-8000-00AA00389B71 MEDIASUBTYPE_I420 == WMMEDIASUBTYPE_I420
			if(pvi->bmiHeader.biBitCount == 12)
			{ // biCompression 808596553 == 0x30323449
				// 12 is correct for i420 -- WFMLE uses this, VLC *can* also use it, too
			}
			else 
			{
			  return E_INVALIDARG;
			}
		} 
		else 
		{
			return E_INVALIDARG;
		}
	}

	if(m_bFormatAlreadySet) 
	{
		// then it must be the same as our current...see SetFormat msdn
		if(m_mt == *pMediaType) 
		{
			return S_OK;
		} 
		else 
		{
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
	}


	// Don't accept formats with negative height, which would cause the desktop
	// image to be displayed upside down.
	// also reject 0's, that would be weird.
	if (pvi->bmiHeader.biHeight <= 0)
		return E_INVALIDARG;

	if (pvi->bmiHeader.biWidth <= 0)
		return E_INVALIDARG;

	return S_OK;  // This format is acceptable.

} // CheckMediaType

//**************************************************************
//* ICaptureParam implementation
//**************************************************************
STDMETHODIMP CCamStudioPin::CaptureFix(HWND hWnd, RECT rc)
{
	//m_CaptureWindow.
	SetCaptureScreen(hWnd, rc, CAPTURE_FIXED);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::CaptureVariable(HWND hWnd, RECT rc)
{
	//m_CaptureWindow.
	SetCaptureScreen(hWnd, rc, CAPTURE_VARIABLE);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::CaptureAllScreens(RECT rc)
{
	//m_CaptureWindow.
	SetCaptureScreen(NULL, rc, CAPTURE_ALLSCREENS);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::CaptureWindow(HWND hWnd, HWND hOwner)
{
	RECT rc;
	GetWindowRect(hWnd, &rc); 
	//m_CaptureWindow.
	SetCaptureScreen(hWnd, rc, CAPTURE_WINDOW);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::CaptureFullScreen(RECT rc)
{
	//m_CaptureWindow.
	SetCaptureScreen(NULL, rc, CAPTURE_FULLSCREEN);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::SetFPS(int nFPS)
{
	m_nFPS = nFPS;
	m_rtFrameLength = UNITS / m_nFPS;
	return S_OK;
}
 STDMETHODIMP CCamStudioPin::EnableAutoPan(bool bEnable, int iSpeed)
{
	//m_CaptureWindow.
	AutoPan(bEnable, iSpeed);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::DisplayFlashingWindow(bool bDisplay)
{
	m_bDisplayFlashingWindow = bDisplay;
	//m_CaptureWindow.DisplayFlashingWindow(bDisplay);
	return S_OK;
}
STDMETHODIMP CCamStudioPin::SetFlashingWindow(HWND hwnd)
{
	//m_FlashingWnd.SetFlashingWindow(hwnd);
	m_hFlashingWnd = hwnd;
	return S_OK;
}
/*STDMETHODIMP CCamStudioPin::StopCapture()
{
	return S_OK;
}*/
/*STDMETHODIMP CCamStudioPin::SetParentWindow(HWND hWnd)
{
	m_hParent = hWnd;
	return S_OK;
}*/
//**************************************************************
//* ICaptureParam implementation ends here
//**************************************************************
//void CCamStudioPin::SetupFlashingWindow()
//{
	//m_FlashingWnd.SetUpRegion(m_CaptureWindow.CaptureRect(), m_CaptureWindow.AutoPan() ? 1 : 0);
	//m_CaptureWindow.SetFlashingWindow(m_FlashingWnd);
//}

unsigned long CCamStudioPin::GetCurrentCaptureSize()const
{
	return m_ullCaptureSize;
}
int CCamStudioPin::GetFramesWritten()const
{
	return m_iFrameNumber;
}

float CCamStudioPin::GetActualFPS()
{
	if(!m_iFrameNumber || !m_ullTimeElapsed || (m_ullTimeElapsed /1000 == 0))
		return 0;
	float dActualFPS = float(m_iFrameNumber / float(m_ullTimeElapsed /1000));
	return dActualFPS;
}
unsigned long CCamStudioPin::GetTimeElapsed() const
{
	return m_ullTimeElapsed;
}
RECT CCamStudioPin::GetCaptureRect()
{
	RECT rc;
	return rc;
	//return m_CaptureWindow.CaptureRect();
}
POINT CCamStudioPin::GetPoint()
{
	POINT pt;
	return pt;
	//return m_CaptureWindow.GetPoint();
}
int CCamStudioPin::GetStatus()
{
	return 0;
	//return m_nRecordingStatus;
}
//********************************************************************************
//* CFlashingWnd class
//********************************************************************************

namespace {	// annonymous

const int SMALLTHICKNESS	= 4;
const int THICKNESS			= 8;

const int DOUBLESMALLTHICKNESS	= 8;
const int DOUBLETHICKNESS	= 16;

const int SIDELEN			= 12;
const int SIDELEN2			= 24;

}	// namespace annonymous
LRESULT CALLBACK MainWndProc(
	HWND hwnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam)    // second message parameter
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam); 
} 
//********************************************************************************
//* Constructor
//********************************************************************************

CFlashingWnd::CFlashingWnd()
: m_clrBorderON(RGB(255, 255, 180))
, m_clrBorderOFF(RGB(0, 255, 80))
, m_hCursorMove(::LoadCursor(NULL, IDC_SIZEALL))
{
}

//********************************************************************************
//* Destructor
//********************************************************************************

CFlashingWnd::~CFlashingWnd()
{
	if (m_hCursorMove) {
		DeleteObject(m_hCursorMove);
	}
	if(m_rgnWindow)
		DeleteObject(m_rgnWindow);
}

//********************************************************************************
//* CreateFlashing()
//*
//* Creates the main application window Flashing
//********************************************************************************

BOOL CFlashingWnd::CreateFlashing(/*LPCTSTR pTitle, RECT &rect*/)
{
	HICON hcur = LoadCursor(0, IDC_ARROW);
	WNDCLASS wndclass;
	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)MainWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = WINDOW_CLASS;
	wndclass.hCursor = hcur;
	wndclass.hbrBackground = NULL;

	if (!RegisterClass(&wndclass))
	{
		return false;
	}
	m_hFlashingWnd = ::CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT, WINDOW_CLASS, L"", WS_POPUP,
			::GetSystemMetrics(SM_XVIRTUALSCREEN), ::GetSystemMetrics(SM_YVIRTUALSCREEN), 
			::GetSystemMetrics(SM_CXVIRTUALSCREEN), ::GetSystemMetrics(SM_CYVIRTUALSCREEN), 
			NULL, NULL, NULL, NULL);
	return IsWindow(m_hFlashingWnd);
}
//********************************************************************************
//* Destroy flashing window
//********************************************************************************
void CFlashingWnd::DestroyFlashing()
{
}

//********************************************************************************
//* SetupRegion()
//*
//* Set the Window Region for transparancy outside the mask region
//*
//* cRect : The area are that should be recorded
//*
//********************************************************************************

void CCamStudioPin::SetUpRegion(const RECT& cRect, BOOL bAutoPan)
{
	m_cRect = cRect;
	bool bNoError = false;
	m_rgnWindow = NULL;
	HRGN rgnOuter = NULL;
	int nWidth = m_cRect.right - m_cRect.left;
	int nHeight = m_cRect.bottom - m_cRect.top;
	if (!bAutoPan)
	{
		HRGN rgnHorizontal = NULL;
		HRGN rgnVertical = NULL;
		m_rgnWindow = CreateRectRgn(0, 0, nWidth + DOUBLETHICKNESS, nHeight + DOUBLETHICKNESS);
		rgnOuter = CreateRectRgn(THICKNESS, THICKNESS, nWidth + THICKNESS, nHeight + THICKNESS);
		rgnHorizontal = CreateRectRgn(0, SIDELEN2, nWidth + DOUBLETHICKNESS, nHeight - SIDELEN);
		rgnVertical = CreateRectRgn(SIDELEN2, 0, nWidth - SIDELEN, nHeight + DOUBLETHICKNESS);
		if(m_rgnWindow && rgnOuter && rgnHorizontal && rgnVertical)
		{
			if(ERROR != CombineRgn(m_rgnWindow, m_rgnWindow, rgnOuter, RGN_DIFF))
				if(ERROR != CombineRgn(m_rgnWindow, m_rgnWindow, rgnHorizontal, RGN_DIFF))
					if(ERROR != CombineRgn(m_rgnWindow, m_rgnWindow, rgnVertical, RGN_DIFF))
						if(ERROR != OffsetRgn(m_rgnWindow, cRect.left - THICKNESS,  cRect.top - THICKNESS))
							bNoError = TRUE;
		}
		if(rgnHorizontal)
			DeleteObject(rgnHorizontal);
		if(rgnVertical)
			DeleteObject(rgnVertical);
	}
	else
	{
		
		m_rgnWindow = CreateRectRgn(0, 0, nWidth + DOUBLESMALLTHICKNESS, nHeight + DOUBLESMALLTHICKNESS);
		rgnOuter = CreateRectRgn(SMALLTHICKNESS, SMALLTHICKNESS, nWidth + SMALLTHICKNESS, nHeight + SMALLTHICKNESS);
		if(m_rgnWindow && rgnOuter)
		{
			if(ERROR != CombineRgn(m_rgnWindow, m_rgnWindow, rgnOuter, RGN_DIFF))
			{
				if(ERROR != OffsetRgn(m_rgnWindow, m_cRect.left - SMALLTHICKNESS, m_cRect.top - SMALLTHICKNESS))
					bNoError = true;
			}
		}
	}

	if(m_hFlashingWnd && m_rgnWindow)
	{
		SetWindowRgn(m_hFlashingWnd, m_rgnWindow, TRUE);
		PaintBorder(bAutoPan ? true : false);
		PaintBorder(bAutoPan ? false : true);
	}
	// Clean up
	if(rgnOuter)
		DeleteObject(rgnOuter);
	if(m_rgnWindow)
		DeleteObject(m_rgnWindow);
}

//********************************************************************************
//* CFlashingWnd message handlers
//********************************************************************************

void CCamStudioPin::PaintBorder(bool bInvert, bool bDraw)
{

	if ((m_cRect.right <= m_cRect.left) || (m_cRect.bottom <= m_cRect.top)) {
		return;
	}
	COLORREF clrColor = bInvert
		? m_clrBorderON
		: m_clrBorderOFF;
	HDC hWndDC = GetDC(m_hFlashingWnd);
	::SetROP2(hWndDC, R2_XORPEN);
	HBRUSH newbrush = CreateSolidBrush(clrColor);
	HPEN newpen = CreatePen(PS_SOLID, 1, clrColor);
	HBRUSH oldbrush = (HBRUSH) SelectObject(hWndDC, newbrush);
	HPEN oldpen = (HPEN)SelectObject(hWndDC, newpen);
	if (bInvert) 
	{
		PatBlt(hWndDC, 0, 0, ::GetSystemMetrics(SM_CXVIRTUALSCREEN), ::GetSystemMetrics(SM_CYVIRTUALSCREEN), PATINVERT);
	} 
	else 
	{
		RECT rectNew = m_cRect;
		InflateRect(&rectNew, THICKNESS, THICKNESS);
		Rectangle(hWndDC, rectNew.left, rectNew.top, rectNew.right, rectNew.bottom);
	}
	SelectObject(hWndDC, oldpen);
	SelectObject(hWndDC, oldbrush);
	// Clean up
	if(newbrush)
		DeleteObject(newbrush);
	if(newpen)
		DeleteObject(newpen);
	ReleaseDC(m_hFlashingWnd, hWndDC);
}

//*********************************************
//* CCaptureWindow implementation
//*********************************************
#include <math.h>
CCaptureWindow::CCaptureWindow() : 
m_hWndToTrack(NULL),
m_hRawBitmap(NULL),
m_iCaptureMode(CAPTURE_FULLSCREEN),
m_bAutoPan(false),
m_bDisplay(false),
m_iZoom(1),
m_dwZoomWhen(0),
m_iZoomDirection(-1) // Zoom out
{
	m_hScrDc = GetDC(m_hWndToTrack);
	// Get the dimensions of the main desktop window as the default
	m_rcScreen.left   = m_rcScreen.top = 0;
	m_rcScreen.right  = GetDeviceCaps(m_hScrDc, HORZRES);
	m_rcScreen.bottom = GetDeviceCaps(m_hScrDc, VERTRES);
	//SetCaptureScreen(NULL, m_rcScreen, m_iCaptureMode);
	UpdateCaptureDimension();
	m_ptZoomAt.x = 10;
	m_ptZoomAt.y = 10;
}
CCaptureWindow::~CCaptureWindow()
{
	::ReleaseDC(NULL, m_hScrDc);
	::DeleteDC(m_hScrDc);
	if (m_hRawBitmap)
		DeleteObject(m_hRawBitmap);
}
UINT CCamStudioPin::Width() const
{
	return m_nCaptureWidth;
}
UINT CCamStudioPin::Height() const
{
	return m_nCaptureHeight;
}
/*void CCamStudioPin::AutoPan(bool bEnable, int iSpeed)
{
	m_bAutoPan = bEnable;
	m_iMaxPan = iSpeed;
}*/
/*void CCamStudioPin::DisplayFlashingWindow(bool bDisplay)
{
	m_bDisplay = bDisplay;
}
*/

/*void CCamStudioPin::SetCaptureScreen(HWND hWnd, RECT rcSource, int iCaptureMode)
{
	m_iCaptureMode = iCaptureMode;
	// Get coordinates of the window on the screen
	//if(CAPTURE_WINDOW == m_iCaptureMode)
	//{
		m_hWndToTrack = hWnd;
		::GetWindowRect(m_hWndToTrack, &m_rScreen);
	//}
	//m_hScrDc = GetDC(m_hWndToTrack);
	m_rcScreen = rcSource;
	m_rcPanCurrent = m_rcScreen;
	m_rcPanDest = m_rcScreen;
	UpdateCaptureDimension();
}
*/
void CCaptureWindow::UpdateCaptureDimension()
{
	m_nCaptureWidth = m_rcScreen.right - m_rcScreen.left;
	m_nCaptureHeight = m_rcScreen.bottom - m_rcScreen.top;
}

RECT& CCaptureWindow::GetWindowRect()
{
	::GetWindowRect(m_hWndToTrack, &m_rcScreen);
	return m_rcScreen;
}
void CCamStudioPin::CreateSample()
{
	if(m_hRawBitmap)
		DeleteObject (m_hRawBitmap);
	m_hRawBitmap = CreateCompatibleBitmap(m_hScrDc, m_nCaptureWidth, m_nCaptureHeight);
}
RECT CCaptureWindow::CaptureRect()
{
	return m_rcScreen;
}
POINT CCaptureWindow::GetPoint()
{
	return m_xPoint;
}
void CCamStudioPin::CopyScreenImage(BYTE *pData, BITMAPINFO *pHeader)
{
	HDC hMemDC;         // screen DC and memory DC
	HBITMAP hOldBitmap; // handles to device-dependent bitmaps
	SetCaptureRect(m_rScreen);
	// create a DC for the screen and create
	// a memory DC compatible to screen DC
	hMemDC = CreateCompatibleDC(m_hScrDc);
	if(CAPTURE_WINDOW == m_iCaptureMode)
		::GetWindowRect(m_hWndToTrack, &m_rScreen);
	// select new bitmap into memory DC
	hOldBitmap = (HBITMAP) SelectObject(hMemDC, m_hRawBitmap);
	// Bit block transfer from screen our compatible memory DC.	Apparently this is faster than stretching.
	if(hMemDC && m_hScrDc)
		BitBlt(hMemDC, 0, 0, m_nCaptureWidth, m_nCaptureHeight, m_hScrDc, m_rScreen.left, m_rScreen.top, SRCCOPY);
	AddMouse(hMemDC, &m_rScreen, m_hScrDc, m_hWndToTrack);
	// select old bitmap back into memory DC and get handle to
	// bitmap of the capture...whatever this even means...	
	HBITMAP hRawBitmap2 = (HBITMAP) SelectObject(hMemDC, hOldBitmap);
	GetDIBits(m_hScrDc, hRawBitmap2, 0, m_nCaptureHeight, pData, pHeader, DIB_RGB_COLORS);
	DeleteDC(hMemDC);
}
void CCamStudioPin::SetCaptureRect(RECT& rectFrame)
{
	// Can't pan nor display flashing rect!
	if(m_iCaptureMode == CAPTURE_FULLSCREEN)
		return;
	if(m_iCaptureMode == CAPTURE_WINDOW)
	{
		if(!m_bAutoPan)
			CaptureScreenFrame(rectFrame);
		return;
	}
		// Autopan
		int iRectFrameWidth = rectFrame.right - rectFrame.left;
		int iRectFrameHeight = rectFrame.bottom - rectFrame.top;
		if(m_bAutoPan && (iRectFrameWidth < ::GetSystemMetrics(SM_CXVIRTUALSCREEN)) && (iRectFrameHeight < ::GetSystemMetrics(SM_CYVIRTUALSCREEN)))
		{
			::GetCursorPos(&m_xPoint);
			POINT xPoint = m_xPoint;
			int iWidth = m_rcPanCurrent.right - m_rcPanCurrent.left;
			int iHeight = m_rcPanCurrent.bottom - m_rcPanCurrent.top;
			int extleft = (iWidth *1)/4 + m_rcPanCurrent.left;
			int extright = (iWidth *3)/4 + m_rcPanCurrent.left;
			int exttop = (iHeight *1)/4 + m_rcPanCurrent.top;
			int extbottom = (iHeight *3)/4 + m_rcPanCurrent.top;

			if (xPoint.x < extleft ) 
			{
				//need to pan left
				m_rcPanDest.left = xPoint.x - iWidth/2;
				m_rcPanDest.right = m_rcPanDest.left + iRectFrameWidth - 1;
				if (m_rcPanDest.left < 0) 
				{
					m_rcPanDest.left = 0;
					m_rcPanDest.right = m_rcPanDest.left + iRectFrameWidth - 1;
				}
			} 
			else if (extright < xPoint.x) 
			{
				//need to pan right
				m_rcPanDest.left = xPoint.x - iRectFrameWidth/2;
				m_rcPanDest.right = m_rcPanDest.left +  iRectFrameWidth - 1;
				if (::GetSystemMetrics(SM_CXVIRTUALSCREEN) <= m_rcPanDest.right) 
				{
					m_rcPanDest.right = ::GetSystemMetrics(SM_CXVIRTUALSCREEN) - 1;
					m_rcPanDest.left  = m_rcPanDest.right - iRectFrameWidth + 1;
				}
			} 
			else 
			{
				m_rcPanDest.right = m_rcPanCurrent.right;
				m_rcPanDest.left  = m_rcPanCurrent.left;
			}

			if (xPoint.y < exttop) 
			{ //need to pan up
				m_rcPanDest.top = xPoint.y - iRectFrameHeight/2;
				m_rcPanDest.bottom = m_rcPanDest.top + iRectFrameHeight - 1;
				if (m_rcPanDest.top < 0)
				{
					m_rcPanDest.top = 0;
					m_rcPanDest.bottom = m_rcPanDest.top + iRectFrameHeight - 1;
				}
			}
			else if (extbottom < xPoint.y) 
			{ //need to pan down
				m_rcPanDest.top = xPoint.y - iRectFrameHeight/2;
				m_rcPanDest.bottom = m_rcPanDest.top + iRectFrameHeight - 1;
				if (::GetSystemMetrics(SM_CYVIRTUALSCREEN) <= m_rcPanDest.bottom) 
				{
					m_rcPanDest.bottom = ::GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;
					m_rcPanDest.top  = m_rcPanDest.bottom - iRectFrameHeight + 1;
				}
			}
			else 
			{
				m_rcPanDest.top = m_rcPanCurrent.top;
				m_rcPanDest.bottom  = m_rcPanCurrent.bottom;
			}

			// Determine Pan Values
			int xdiff = m_rcPanDest.left - m_rcPanCurrent.left;
			int ydiff = m_rcPanDest.top - m_rcPanCurrent.top;
			if (abs(xdiff) < m_iMaxPan) 
			{
				m_rcPanCurrent.left += xdiff;
			} else if (xdiff < 0) 
			{
				m_rcPanCurrent.left -= m_iMaxPan;
			} 
			else 
			{
				m_rcPanCurrent.left += m_iMaxPan;
			}

			if (abs(ydiff) < m_iMaxPan) 
			{
				m_rcPanCurrent.top += ydiff;
			} else  if (ydiff<0) 
			{
				m_rcPanCurrent.top -= m_iMaxPan;
			} else 
			{
				m_rcPanCurrent.top += m_iMaxPan;
			}

			m_rcPanCurrent.right = m_rcPanCurrent.left + iRectFrameWidth - 1;
			m_rcPanCurrent.bottom =  m_rcPanCurrent.top + iRectFrameHeight - 1;

			RECT rectPanFrame(m_rcPanCurrent);
			rectPanFrame.right++;
			rectPanFrame.bottom++;
			CaptureScreenFrame(rectPanFrame);
			m_rScreen = rectPanFrame;
		} 
		else 
		{
			// Just capture since window is not moving
			CaptureScreenFrame(rectFrame);
		}
}
void CCamStudioPin::CaptureScreenFrame(RECT &rectView)
{
	int iWidth = (int)m_nCaptureWidth/m_iZoom; // width of being captured screen stuff
	int iHeight = (int)m_nCaptureWidth/m_iZoom; // width of being captured screen stuff
	RECT rc;
	if(m_iZoom == 1.)
	{
		rc = rectView;
	}
	else
	{
		POINT pt;
		SIZE sz;
		pt.x = min(max(rectView.left, m_ptZoomAt.x - iWidth/2), rectView.right - iWidth);
		pt.y = min(max(rectView.top, m_ptZoomAt.y - iHeight/2), rectView.bottom - iHeight);
		sz.cx = iWidth;
		sz.cy = iHeight;
		rc.left = pt.x;
		rc.top = pt.y;
		rc.right = sz.cx;
		rc.bottom = sz.cy;
	}
	
	if (m_dwZoomWhen) 
	{
		DWORD threshold = 1000; // 1 sec
		DWORD now = ::GetTickCount();
		DWORD ago = now - m_dwZoomWhen;
		if (ago>threshold)
		{
			m_dwZoomWhen = 0;
		} else
		{	// FIXME: change zoom from current state in case use changed mind zooming
			m_iZoom = 1.5 - cos(ago*3.141592/threshold)/1.9* m_iZoomDirection;
			if (m_iZoom>2.) m_iZoom = 2.;
			if (m_iZoom<1.) m_iZoom = 1.;
		}
	}
	POINT pt;
	::GetCursorPos(&pt);
	double dist = sqrt((double)(pt.x-m_ptZoomAt.x)*(pt.x-m_ptZoomAt.x) + (pt.y-m_ptZoomAt.y)*(pt.y-m_ptZoomAt.y));

	if (m_bAutoPan)
	{	// always cursor centered
		::GetCursorPos(&m_ptZoomAt);
	} 
	else 
	{
		if (abs(pt.x-m_ptZoomAt.x) > .4*iWidth)
			m_ptZoomAt.x += (pt.x - m_ptZoomAt.x)* m_iMaxPan/iWidth;
		if (abs(pt.y-m_ptZoomAt.y) > .4*iHeight)
			m_ptZoomAt.y += (pt.y - m_ptZoomAt.y)* m_iMaxPan/iHeight;
	}

	// Get most recent window position
	// Window may have moved lately
	if(m_iCaptureMode == CAPTURE_WINDOW)
		::GetWindowRect(m_hWndToTrack, &rc);
	if(m_bDisplayFlashingWindow)
	{
		SetUpRegion(rc, m_bAutoPan);
	}
}
