/**********************************************
 *  File: CamStudioScreenCapture.h
 *  Desc: CamStudioScreenCapture.h Class. The Screen Capture filter class main header file. Based on MS SDK sample PushSource filter and http://sourceforge.net/projects/screencapturer/
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/


#include <strsafe.h>
#include "ICaptureParam.h"
#include "ICaptureReport.h"

// screen capture types
enum
{
	CAPTURE_FIXED = 0,  // fix rectangle
	CAPTURE_VARIABLE,   // rectangle drawn using mouse
	CAPTURE_FULLSCREEN, // entire screen
	CAPTURE_WINDOW,		// any open window..can be moved by dragging
	CAPTURE_ALLSCREENS  // in case of dual monitor... not fully supported
};

#define FILTER_NAME    L"CamStudio Screen Capture"
#define WINDOW_CLASS   L"FlashingWindow"

class CCamStudioPin;

//******************************************************
//* CFlashingWnd
//******************************************************
class CFlashingWnd
{
public:
	/*
	 * Name : CFlashingWnd
	 * Desc : Creates instance of CFlashingWnd
	 * In   : None
	 * Out  : None
	 */
	CFlashingWnd();
	/*
	 * Name : ~CFlashingWnd
	 * Desc : Free resources used by CFlashingWnd
	 * In   : None
	 * Out  : None
	 */
	virtual ~CFlashingWnd();
	/*
	 * Name : CreateFlashing
	 * Desc : Creates the window where flashing rect is drawn
	 * In   : NOne
	 * Out  : Returns TRUE is window is created, otherwise FALSE
	 */
	BOOL CreateFlashing();
	/*
	 * Name : DestroyFlashing
	 * Desc : Destroys window using its handle
	 * In   : None
	 * Out  : None
	 */
	void DestroyFlashing();
	/*
	 * Name : GetWindowHandle
	 * Desc : Returns the underlying window handle of CFlashingWnd class
	 * In   : None
	 * Out  : HWND - Handle to this window
	 */
	//HWND GetWindowHandle() const
	//{
		//return m_hFlashingWnd;
	//}
	/*
	 * Name : SetUpRegion
	 * Desc : Draw the visible part of the window by combining HRGN objects
	 * In   : cRect - The rectangle where regions are drawn
	 *		  bAutoPan - Determines whether autopan is enabled/disabled
	 * Out  : None
	 */
	//void SetUpRegion(const RECT& cRect, BOOL bAutoPan);
	/*
	 * Name : SetFlashingWindow
	 * Desc : Receives the window where flashing rect is drawn from the Recorder app
	 * In   : hwnd - Handle of the window
	 * Out  : None
	 */
	//void SetFlashingWindow(const HWND hwnd)
	//{
		//m_hFlashingWnd = hwnd;
	//}

private:
	/*
	 * Name : PaintBorder
	 * Desc : Paints the flashing rect of the captured part of the screen unless full-screen
	 * In   : bInvert - Determines how the border is painted
	 *		  bDraw   - Determines whether to paint the border or not
	 * Out  : None
	 */
	//void PaintBorder(bool bInvert, bool bDraw = true);

private:
	
	// Instance variables
	COLORREF	m_clrBorderON;		// border ON color
	COLORREF	m_clrBorderOFF;		// border OFF color
	RECT		m_cRect;			// border region rectangle
	HCURSOR		m_hCursorMove;
	POINT		m_ptStart;
	HWND		m_hFlashingWnd;
	HRGN		m_rgnWindow;
	
	// log4cplus
	//Logger m_flashingWindowLogger;

};
//*********************************************
//* CCaptureWindow
//*********************************************
class CCaptureWindow
{
public:
	/*
	 * Name : CCaptureWindow
	 * Desc : Initializes CCaptureWindow
	 * In   : None
	 * Out  : None
	 */
	CCaptureWindow();
	/*
	 * Name : ~CCaptureWindow
	 * Desc : Frees resources used by CCaptureWindow
	 * In   : None
	 * Out  : None
	 */
	virtual ~CCaptureWindow();
	/*
	 * Name : CopyScreenImage
	 * Desc : Copies the captured bitmap to BYTE pointer and fill out BITMAPINFO pointer
	 * In   : pData   - Pointer buffer where to copy bitmap
	 *		  pHeader - Pointer to BITMAPINFO to fill out w/ info such as size of image ...etc
	 * Out  :
	 */
	//void CopyScreenImage(BYTE *pData, BITMAPINFO *pHeader, int& status);
	/*
	 * Name : SetCaptureScreen
	 * Desc : Sets the size, coordinates of the part of the screen to be captured
	 * In   : hWnd     - Handle of the invisible window to be captured or a window or NULL for full screen
	 *		  rcSource - The rectangle to be captured; screen coordinates height and width
	 * Out  : None
	 */
	//void SetCaptureScreen(HWND hWnd, RECT rcSource, int iCaptureMode);
	/*
	 * Name : DisplayFlashingWindow
	 * Desc : Use to hide/display window where flashing rect is drawn
	 * In   : bDisplay - Value of true displays the window, false hide the window
	 * Out  : None
	 */
	//void DisplayFlashingWindow(bool bDisplay);
	/*
	 * Name : AutoPan
	 * Desc : Enable/disable autopan
	 * In   : bEnable - true enables autopan other wise disabled
	 *		  iSpeed  - Autopan speed (if autopan is enbled)
	 * Out  : None
	 */
	//void AutoPan(bool bEnable, int iSpeed);
	/*
	 * Name : CreateSample
	 * Desc : Creates the sample bitmap after agreement bitween filter pins
	 * In   : None
	 * Out  : None
	 */
	//void CreateSample();
	/*
	 * Name : SetCaptureRect
	 * Desc : Sets the location of the captured part of the screen (most useful when autopan is enabled)
	 * In   : rectFrame - New location of the rectangle to be captured
	 * Out  : None
	 */
	//void SetCaptureRect(RECT& rectFrame);
	/*
	 * Name : SetFlashingWindow
	 * Desc : Sets our instance of CFlashingWnd object
	 * In   : flashingWnd - Address of the CFlashingWnd
	 * Out  : None
	 */
	//void SetFlashingWindow(const CFlashingWnd& flashingWnd)
	//{
		//m_FlashingWnd = flashingWnd;
	//}
	/*
	 * Name : Width
	 * Desc : Returns the width of the rectangle to be captured, a window or width of the screen on full screen capture
	 * In   : None
	 * Out  : UINT - width
	 */
	//UINT Width()const;
	/*
	 * Name : Height
	 * Desc : Returns the height of the rectangle to be captured, a window or width of the screen on full screen capture
	 * In   : None
	 * Out  : UINT - height
	 */
	//UINT Height()const;
	/*
	 * Name : CaptureRect
	 * Desc : Returns the current rectangle captured with top left right and buttom coordinates of the screen
	 * In   : None
	 * Out  : RECT - rectangle
	 */
	RECT CaptureRect();
	/*
	 * Name : GetPoint
	 * Desc : Returns the screen coordinates of the mouse pointer location
	 * In   : None
	 * Out  : POINT - x,y coordinates
	 */
	POINT GetPoint();
	/*
	 * Name : AutoPan
	 * Desc : Returns true if autopan is enabled otherwise false
	 * In   : None
	 * Out  : bool - true/false
	 */
	bool AutoPan()const
	{
		return m_bAutoPan;
	}
private:
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	RECT& GetWindowRect();
	/*
	 * Name : UpdateCaptureDimension
	 * Desc : Updates the dimension of the part of the screen to be captured passed by the Recorder app
	 * In   : None
	 * Out  : None
	 */
	void UpdateCaptureDimension();
	/*
	 * Name : CaptureScreenFrame
	 * Desc : Captures the new rectangle to be recorded (most useful when autopan is enabled)
	 * In   : 
	 * Out  :
	 */
	void CaptureScreenFrame(RECT &rc);
private:

	// Instance variables

	HDC m_hScrDc;
	HBITMAP m_hRawBitmap;
	RECT m_rcScreen;
	// Autopan
	RECT m_rcPanCurrent;
	RECT m_rcPanDest;
	POINT m_xPoint;

	HWND m_hWndToTrack;
	POINT m_ptZoomAt;
	int m_iCaptureMode;
	int m_iZoom;
	int m_iZoomDirection;
	int m_iMaxPan;
	DWORD m_dwZoomWhen;
	UINT m_nCaptureWidth;
	UINT m_nCaptureHeight;
	bool m_bAutoPan;
	bool m_bDisplay;
	CFlashingWnd m_FlashingWnd;

};
//*********************************************
//* CCamStudioFilter
//*********************************************
class CCamStudioFilter : public CSource // public IAMFilterMiscFlags // CSource is CBaseFilter is IBaseFilter is IMediaFilter is IPersist which is IUnknown
{

private:
	/*
	 * Name : CCamStudioFilter
	 * Desc : Initializes CamStudio Screen Capture filter
	 * In   : pUnk - Pointer to IUnknown
	 *		  phr - Pointer to HRESULT
	 * Out  : None
	 */
	CCamStudioFilter(IUnknown *pUnk, HRESULT *phr);
	/*
	 * Name : CCamStudioFilter
	 * Desc : Frees resources used by CamStudio Screen Capture filter
	 * In   : None
	 * Out  : None
	 */
	~CCamStudioFilter();

	CCamStudioPin *m_pPin;


public:
	//////////////////////////////////////////////////////////////////////////
	//  IUnknown
	//////////////////////////////////////////////////////////////////////////
	//DECLARE_IUNKNOWN;
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);
	//STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);

	// ?? compiler error that these be required here? huh?
	ULONG STDMETHODCALLTYPE AddRef() { return CBaseFilter::AddRef(); };
	ULONG STDMETHODCALLTYPE Release() { return CBaseFilter::Release(); };
	/*
	 * Name : GetGraph
	 * Desc : Returns the pointer of the IFilterGraph where CamStudio Screen Capture belongs
	 * In   : None
	 * Out  : IFilterGraph - Capture graph pointer
	 */
	IFilterGraph *GetGraph() {return m_pGraph;}

	/*
	 * Name : GetState
	 * Desc : Retrieves the states of the filter (stopped, paused or running)
	 * In   : dwMilliSecsTimeout - Time to wait for the operation to finish
	 *		  State - FILTER_STATE pointer to copy the filter state
	 * Out  : HRESULT - Status of the operation
	 */
	//STDMETHODIMP GetState(DWORD dwMilliSecsTimeout, FILTER_STATE *State);
	STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	//STDMETHODIMP Run(REFERENCE_TIME tStart);

	HRESULT GetFilterState()const;
};
class CCamStudioPin : public CSourceStream, /*public CMediaControl,*/ public IAMStreamConfig, public IKsPropertySet, /*public IMediaControl,*/ ICaptureParam, ICaptureReport //CSourceStream is ... CBasePin
{

protected:

	REFERENCE_TIME m_rtFrameLength; // also used to get the fps
	REFERENCE_TIME m_previousFrameEndTime;

	RECT m_rScreen; // Rect containing screen coordinates we are currently "capturing"
	
	CCamStudioFilter* m_pParent;
	HDC m_hScrDc;
	HBITMAP m_hRawBitmap;
	//CCritSec m_cSharedState;
	
	bool m_bFormatAlreadySet;
	float GetFps();

	int m_millisToSleepBeforePollForChanges;
	HWND m_hWndToTrack;

	int m_iCaptureMode;
public:

	//////////////////////////////////////////////////////////////////////////
	//  IUnknown
	//////////////////////////////////////////////////////////////////////////
	//DECLARE_IUNKNOWN;
	//STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv); 
	STDMETHODIMP_(ULONG) AddRef() { return GetOwner()->AddRef(); } 
	STDMETHODIMP_(ULONG) Release() { return GetOwner()->Release(); }

	//////////////////////////////////////////////////////////////////////////
	//  IAMStreamConfig
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE SetFormat(AM_MEDIA_TYPE *pmt);
	HRESULT STDMETHODCALLTYPE GetFormat(AM_MEDIA_TYPE **ppmt);
	HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(int *piCount, int *piSize);
	HRESULT STDMETHODCALLTYPE GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pmt, BYTE *pSCC);

	HRESULT Run(REFERENCE_TIME tStart);
	//HRESULT Pause();
	CCamStudioPin(HRESULT *phr, CCamStudioFilter *pFilter);
	~CCamStudioPin();

	// Override the version that offers exactly one media type
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
	HRESULT FillBuffer(IMediaSample *pSample);

	void StopStreamingThread();
	//HRESULT OnThreadCreate(void);
	//HRESULT OnThreadDestroy(void);
	// Set the agreed media type and set up the necessary parameters
	HRESULT SetMediaType(const CMediaType *pMediaType);

	// Support multiple display formats (CBasePin)
	HRESULT CheckMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	// IQualityControl
	// Not implemented because we aren't going in real time.
	// If the file-writing filter slows the graph down, we just do nothing, which means
	// wait until we're unblocked. No frames are ever dropped.
	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
	{
		return E_FAIL;
	}
////////////////////////////////////////////////////////////////////
// ICaptureParam - interface to Recorder app
////////////////////////////////////////////////////////////////////
	/*
	 * Name : CaptureFix
	 * Desc : Sets capture mode to CAPTURE_FIXED
	 * In   : hWnd - Handle of the window to capture
	 *		  rc - The rectangle to capture
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP CaptureFix(HWND hWnd, RECT rc);
	/*
	 * Name : CaptureVariable
	 * Desc : Sets capture mode to CAPTURE_VARIABLE
	 * In   : hWnd - Handle of the window to capture
	 *		  rc - The rectangle to capture
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP CaptureVariable(HWND hWnd, RECT rc);
	/*
	 * Name : CaptureAllScreens
	 * Desc : Sets capture mode to CAPTURE_ALLSCREENS
	 * In   : rc - The rectangle to capture (rc is disgarded or maybe NULL since capture is all screens)
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP CaptureAllScreens(RECT rc);
	/*
	 * Name : CaptureWindow
	 * Desc : Sets capture mode to CAPTURE_WINDOW
	 * In   : hWnd - Handle of the window to be captured
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP CaptureWindow(HWND hWnd, HWND hOwner);
	/*
	 * Name : CaptureFullScreen
	 * Desc : Sets capture mode to CAPTURE_FULLSCREEN
	 * In   : None
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP CaptureFullScreen(RECT rc);
	/*
	 * Name : SetFPS
	 * Desc : Sets desired FPS (frames per second)
	 * In   : nFPS - FPS set by Recorder app
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP SetFPS(int nFPS);
	/*
	 * Name : EnableAutoPan
	 * Desc : Enable/disable autopan
	 * In   : bEnable - true/false value set by Recorder app
	 *		  iSpeed  - autopan speed value set by Recorder app
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP EnableAutoPan(bool bEnable, int iSpeed);
	/*
	 * Name : DisplayFlashingWindow
	 * Desc : true/false value set by Recorder app
	 * In   : bDisplay - true causes the flashing rect to be displayed otherwise flashing rect will be not be shown
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP DisplayFlashingWindow(bool bDisplay);
	/*
	 * Name : SetFlashingWindow
	 * Desc : Set the handle of the window created by Recorder app
	 * In   : hwnd - Handle of the window created by Recorder app
	 * Out  : HRESULT - S_OK else error code
	 */
	virtual STDMETHODIMP SetFlashingWindow(HWND hwnd);

	/*
	 * Name : StopCapture
	 * Desc : Signal stop capturing on capture pin
	 * In   : None
	 * Out  : None
	 */
	//virtual STDMETHODIMP StopCapture();
	
	/*
	 * Name : SetParentWindow
	 * Desc : Set the parent window handle
	 * In   : hWnd - Window handle of Recorder app
	 * Out  : None
	 */
	//virtual STDMETHODIMP SetParentWindow(HWND hWnd);

///////////////////////////////////////////////////////////////////////////
// ICaptureReport - interface to Recorder app
///////////////////////////////////////////////////////////////////////////

	/*
	 * Name : GetCurrentCaptureSize
	 * Desc : Get the current size of the file where captured frames are dumped
	 * In   : None
	 * Out  : Size of the file in bytes
	 */
	virtual STDMETHODIMP_(unsigned long) GetCurrentCaptureSize(void)const;
	/*
	 * Name : GetFramesWritten
	 * Desc : Returns the number of frames written while recording is going on
	 * In   : None
	 * Out  : Number of frames
	 */
	virtual STDMETHODIMP_(int) GetFramesWritten(void)const;
	/*
	 * Name : GetActualFPS
	 * Desc : Returns the actual FPS while recording is going on
	 * In   : None
	 * Out  : FPS
	 */
	virtual STDMETHODIMP_(float) GetActualFPS(void);
	/*
	 * Name : GetTimeElapsed
	 * Desc : Returns time elapsed since recording started
	 * In   : None
	 * Out  : Time (to be translated to x hours x mins x secs
	 */
	virtual STDMETHODIMP_(unsigned long) GetTimeElapsed(void)const;
	/*
	 * Name : GetCaptureRect
	 * Desc : None used
	 * In   : 
	 * Out  :
	 */
	virtual STDMETHODIMP_(RECT) GetCaptureRect();
	/*
	 * Name : GetPoint
	 * Desc : Not used
	 * In   : 
	 * Out  :
	 */
	virtual STDMETHODIMP_(POINT) GetPoint();
	virtual STDMETHODIMP_(int) GetStatus();
	//////////////////////////////////////////////////////////////////////////
	//  IKsPropertySet
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData);
	HRESULT STDMETHODCALLTYPE Get(REFGUID guidPropSet, DWORD dwPropID, void *pInstanceData,DWORD cbInstanceData, void *pPropData, DWORD cbPropData, DWORD *pcbReturned);
	HRESULT STDMETHODCALLTYPE QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport);

	int m_iFrameNumber;
	HWND m_hParent;
protected:
	/*
	 * Name : ThreadProc
	 * Desc : Camstudio Screen Capture pin thread
	 * In   :  None
	 * Out  : DWORD - Returns thread operation status
	 */
	DWORD ThreadProc(void);
	/*
	 * Name : DoBufferProcessingLoop
	 * Desc : Processes captured frames and send it downstream
	 * In   : None
	 * Out  : HRESULT - Returns status of operation upon exit
	 */
	HRESULT DoBufferProcessingLoop(void);

private:

	//void AutoPan(bool bEnable, int iSpeed);

	//void SetupFlashingWindow();
	
	//void CopyScreenToDataBlock(HDC hScrDc, BYTE *pData, BITMAPINFO *pHeader, IMediaSample *pSample);

	//void SetCaptureScreen(HWND hWnd, RECT rcSource, int iCaptureMode);

	void UpdateCaptureDimension();
	/*
	 * Name : CopyScreenImage
	 * Desc : Copies the captured bitmap to BYTE pointer and fill out BITMAPINFO pointer
	 * In   : pData   - Pointer buffer where to copy bitmap
	 *		  pHeader - Pointer to BITMAPINFO to fill out w/ info such as size of image ...etc
	 * Out  :
	 */
	void CopyScreenImage(BYTE *pData, BITMAPINFO *pHeader);
	/*
	 * Name : SetCaptureScreen
	 * Desc : Sets the size, coordinates of the part of the screen to be captured
	 * In   : hWnd     - Handle of the invisible window to be captured or a window or NULL for full screen
	 *		  rcSource - The rectangle to be captured; screen coordinates height and width
	 * Out  : None
	 */
	void SetCaptureScreen(HWND hWnd, RECT rcSource, int iCaptureMode);
	/*
	 * Name : DisplayFlashingWindow
	 * Desc : Use to hide/display window where flashing rect is drawn
	 * In   : bDisplay - Value of true displays the window, false hide the window
	 * Out  : None
	 */
	//void DisplayFlashingWindow(bool bDisplay);
	/*
	 * Name : AutoPan
	 * Desc : Enable/disable autopan
	 * In   : bEnable - true enables autopan other wise disabled
	 *		  iSpeed  - Autopan speed (if autopan is enbled)
	 * Out  : None
	 */
	void AutoPan(bool bEnable, int iSpeed);
	/*
	 * Name : CreateSample
	 * Desc : Creates the sample bitmap after agreement bitween filter pins
	 * In   : None
	 * Out  : None
	 */
	void CreateSample();
	/*
	 * Name : SetCaptureRect
	 * Desc : Sets the location of the captured part of the screen (most useful when autopan is enabled)
	 * In   : rectFrame - New location of the rectangle to be captured
	 * Out  : None
	 */
	void SetCaptureRect(RECT& rectFrame);
	/*
	 * Name : SetFlashingWindow
	 * Desc : Sets our instance of CFlashingWnd object
	 * In   : flashingWnd - Address of the CFlashingWnd
	 * Out  : None
	 */
	void SetFlashingWindow(const CFlashingWnd& flashingWnd)
	{
		//m_FlashingWnd = flashingWnd;
	}
	/*
	 * Name : CaptureScreenFrame
	 * Desc : Captures the new rectangle to be recorded (most useful when autopan is enabled)
	 * In   : 
	 * Out  :
	 */
	void CaptureScreenFrame(RECT &rc);
	/*
	 * Name : Width
	 * Desc : Returns the width of the rectangle to be captured, a window or width of the screen on full screen capture
	 * In   : None
	 * Out  : UINT - width
	 */
	UINT Width()const;
	/*
	 * Name : Height
	 * Desc : Returns the height of the rectangle to be captured, a window or width of the screen on full screen capture
	 * In   : None
	 * Out  : UINT - height
	 */
	UINT Height()const;


		/*
	 * Name : GetWindowHandle
	 * Desc : Returns the underlying window handle of CFlashingWnd class
	 * In   : None
	 * Out  : HWND - Handle to this window
	 */
	HWND GetWindowHandle() const
	{
		return m_hFlashingWnd;
	}
	/*
	 * Name : SetUpRegion
	 * Desc : Draw the visible part of the window by combining HRGN objects
	 * In   : cRect - The rectangle where regions are drawn
	 *		  bAutoPan - Determines whether autopan is enabled/disabled
	 * Out  : None
	 */
	void SetUpRegion(const RECT& cRect, BOOL bAutoPan);
	/*
	 * Name : PaintBorder
	 * Desc : Paints the flashing rect of the captured part of the screen unless full-screen
	 * In   : bInvert - Determines how the border is painted
	 *		  bDraw   - Determines whether to paint the border or not
	 * Out  : None
	 */
	void PaintBorder(bool bInvert, bool bDraw = true);


	BOOL CreateFlashing();

	// Instance variables

	//CFlashingWnd m_FlashingWnd;
	//CCaptureWindow m_CaptureWindow;
	CRefTime m_rtTimeElapsed;
	int m_nFPS;
	//int m_iFrameNumber;
	bool m_bPaused;
	bool m_bAutoPanEnabled;
	bool m_bDisplayFlashingWindow;
	unsigned long m_ullTimeElapsed;
	unsigned long m_ullCaptureSize;
	unsigned long long m_ullFramesWritten;

	bool m_bAutoPan;
	int m_iMaxPan;
	UINT m_nCaptureWidth;
	UINT m_nCaptureHeight;
	POINT m_ptZoomAt;
	//int m_iCaptureMode;
	int m_iZoom;
	int m_iZoomDirection;
	//int m_iMaxPan;
	DWORD m_dwZoomWhen;

	// Autopan
	RECT m_rcPanCurrent;
	RECT m_rcPanDest;
	POINT m_xPoint;

	COLORREF	m_clrBorderON;		// border ON color
	COLORREF	m_clrBorderOFF;		// border OFF color
	RECT		m_cRect;			// border region rectangle
	HCURSOR		m_hCursorMove;
	POINT		m_ptStart;
	HWND		m_hFlashingWnd;
	HRGN		m_rgnWindow;
};

