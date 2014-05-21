/**********************************************
 *  File: CamStudioCaptureGraph.h
 *  Desc: Represent the capture graph used by Camstudio
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/
#include <ks.h>
#include "dshow.h"
#include <fourcc.h>
#include "ScreenCaptureFilter.h"
#include "VideoCompressorFilter.h"
#include "AudioCaptureFilter.h"
#include "AudioCompressorFilter.h"
#include "Recorder\Profile.h"
//#include "Filter\ICaptureParam.h"
//#include "Filter\ICaptureReport.h"

#include "ScreenCapture\ICaptureParam.h"
#include "ScreenCapture\ICaptureReport.h"

#include "Mpeg4Muxer\MuxFilter.h"
#pragma once

#define OPERATION_TIMEOUT 5000
const CStringW CAMSTUDIO_SCREEN_CAPTURE = L"CamStudio Screen Capture";

typedef struct TimeElapsed
{
	int hours;
	int minutes;
	int seconds;
	TimeElapsed()
	{
		hours = 0;
		minutes = 0;
		seconds = 0;
	}
} TIMEELAPSED;

typedef struct ReportInfo
{
	unsigned long long ullCaptureFileSize;
	float fFPS;
	int iFrameNumber;
	TIMEELAPSED timeElapsed;
	RECT rc;
	POINT pt;
	int nStatus;
	ReportInfo()
	{
		ullCaptureFileSize = 0;
		fFPS = 0;
		iFrameNumber = 0;
	}
} CAPTUREREPORT;
class CCamStudioCaptureGraph
{
public:
	/*
	 * Name : CCamStudioCaptureGraph
	 * Desc : Initializes CCamStudioCaptureGraph (DS capture graph)
	 * In   : pVideoCap  - Pointer to instance of video capture device
	 *		  pVidComp   - Pointer to instance of video codec
	 *		  pAudioCap  - Pointer to instance of audio capture device
	 *		  pAudioComp - Pointer to instance of audio codec
	 *		  hOwner	 - Window handle of the main app (Recorder app)
	 * Out  : 
	 */
	CCamStudioCaptureGraph(
		CScreenCaptureFilter *pVideoCap,
		CVideoCompressorFilter *pVidComp,
		CAudioCaptureFilter *pAudioCap,
		CAudioCompressorFilter *pAudioComp,
		const HWND hOwner
		);
	/*
	 * Name : ~CCamStudioCaptureGraph
	 * Desc : Frees resources used by CCamStudioCaptureGraph
	 * In   : None
	 * Out  : None
	 */
	~CCamStudioCaptureGraph(void);
	/*
	 * Name : InitCaptureFilters
	 * Desc : Initilizes all filter and add them to capture graph
	 * In   : hWnd		   - Handle of the window to be captured
	 *		  hFlashingWnd - Window handle of the window where flashing rect is drawn
	 *		  rectFrame	   - Rectangle to be captured or recorded
	 *		  nCaptureMode - Capture type (fixed, region, window or full screen)
	 * Out  : HRESULT	   - Result of operation
	 */
	HRESULT InitCaptureFilters(HWND hParent, HWND hWnd, HWND hFlashingWnd, const CRect rectFrame, const UINT nCaptureMode);
	/*
	 * Name : BuildCaptureGraph
	 * Desc : Builds the downstream video and or audio capture graph (includes setting avi file)
	 * In   : sOutFile		- Filename of avi file
	 *		  bCaptureAudio - Determine if audio should be captured or not
	 * Out  : HRESULT - Status of the operation
	 */
	HRESULT BuildCaptureGraph(const CString sOutputFile, const BOOL bCaptureAudio, const eVideoFormat vf);
	/*
	 * Name : 
	 * Desc : 
	 * In   : 
	 * Out  :
	 */
	void UnInitialize();

	// Graph control

	/*
	 * Name : StartCapture
	 * Desc : Start recording (plays capture graph)
	 * In   : None
	 * Out  : True/false - Status of play operation
	 */
	BOOL StartCapture();
	/*
	 * Name : StopCapture
	 * Desc : Stops recording (stops capture graph)
	 * In   : None
	 * Out  : None
	 */
	void StopCapture();
	/*
	 * Name : PauseCapture
	 * Desc : Pause recording (pauses capture graph)
	 * In   : None
	 * Out  : True/false - Status of pause operation
	 */
	BOOL PauseCapture();
	/*
	 * Name : ResumeCapture
	 * Desc : Resumes recording (plays capture graph)
	 * In   : None
	 * Out  : True/false - Resume action is sucessful or failed
	 */
	BOOL ResumeCapture();
	// Misc
	/*
	 * Name : Report
	 * Desc : Returns recording statistics report to Recorder UI
	 * In   : None
	 * Out  : CAPTUREREPORT - Report data structure
	 */
	CAPTUREREPORT& Report();
	/*
	 * Name : AutoPan
	 * Desc : Enable/disable autopan
	 * In   : None
	 * Out  : None
	 */
	void AutoPan()const;
private:
	// Clean up
	/*
	 * Name : FreeCaptureFilters
	 * Desc : Call Release on each filter
	 * In   : None
	 * Out  : None
	 */
	void FreeCaptureFilters();
	/*
	 * Name : DestroyCaptureGraph
	 * Desc : Frees all filter resources
	 * In   : None
	 * Out  : None
	 */
	void DestroyCaptureGraph()const;

	// Diagnostic

	/*
	 * Name : ShowError
	 * Desc : Shows a dialog box w/ the error message text
	 * In   : hr		- Error code
	 *		  lpCaption - Caption text of the dialog box
	 * Out  : None
	 */
	void ShowError(HRESULT hr, LPCTSTR lpCaption);
private:

	// Instance variables

	HWND m_hOwner;
	LPCOLESTR m_lpwstrFilename;
	ICaptureGraphBuilder2 *m_pBuilder;
	IAMDroppedFrames *m_pDF;
	IAMVideoCompression *m_pVC;
	IAMVfwCaptureDialogs *m_pDlg;
	
	IAMStreamConfig *m_pASC;	// for audio cap
	IAMStreamConfig *m_pVSC;	// for video cap
	
	IBaseFilter *m_pRender;		// Mux filter
	IBaseFilter *m_pVidCap;		// Video Capture filter
	IBaseFilter *m_pAudCap;		// Audio Capture filter
	IBaseFilter *m_pVidComp;	// Video Compressor filter
	IBaseFilter *m_pAudComp;	// Audio Compressor filter
	IBasicAudio *m_pBasicAudio;
	ICaptureReport *m_pReport;
	ICaptureParam *m_pCapParam;
	IGraphBuilder *m_pGB;
	IFileSinkFilter *m_pSink;
	IMediaControl *m_pMC;
	IConfigAviMux *m_pConfigAviMux;
	BOOL m_bCaptureGraphBuilt;
	BOOL m_bCapAudio;
	IMoniker *m_pmVideo;
	IMoniker *m_pmAudio;
	IMoniker *m_pmVidComp;
	IMoniker *m_pmAudComp;
	
	double m_FrameRate;
	long m_lCapStartTime;
	long m_lCapStopTime;
	int  m_iMasterStream;
	BOOL m_bUseTimeLimit;
	BOOL m_bUseFrameRate;
	DWORD m_dwTimeLimit;

	LPCOLESTR m_lpwstrVCD; // video capture device
	LPCOLESTR m_lpwstrACD; // audio capture device
	LPCOLESTR m_lpwstrAC;  // audio compressor
	LPCOLESTR m_lpwstrVC;  // video compressor
	
	BOOL m_bCaptureAudio;
	UINT m_nCaptureMode;
	DWORD m_dwGraphRegister;
	CRect m_CaptureFrame;
	HRESULT m_hResult;
	CAPTUREREPORT m_Report;
};

