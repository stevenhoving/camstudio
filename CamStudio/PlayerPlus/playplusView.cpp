// playplusView.cpp : implementation of the CPlayplusView class
//

#include "stdafx.h"
#include "playplus.h"

#include "playplusDoc.h"
#include "playplusView.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <windowsx.h>
#include "muldiv32.h"
#include <vfw.h>

#include "fister/soundfile.h"
#include "AudioFormat.h"


// Program Mode -- to be set at compile time
#define PLAYER  0
#define DUBBER  1
int pmode = PLAYER;


//Ver 1.0
#define GlobalSizePtr(lp)   GlobalSize(GlobalPtrHandle(lp))
#define LPPAVIFILE PAVIFILE *
typedef BYTE * HPBYTE;
typedef UNALIGNED short * HPSHORT;


extern BOOL CALLBACK aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
extern void CALLBACK aviaudioMessage(HWND, UINT, WPARAM, LPARAM);
extern void CALLBACK aviaudioStop(void);
extern LONG CALLBACK aviaudioTime(void);

BOOL CALLBACK DlgProc(HWND    hwnd,UINT    msg, WPARAM  wParam, LPARAM  lParam);
static BOOL gfDefDlgEx = FALSE;         //the recursion flag for message crackers

#define BUFSIZE 260
static char gszBuffer[BUFSIZE];
static char gszExtBuffer[BUFSIZE];
static char gszFileName[BUFSIZE];
static char gszSaveFileName[BUFSIZE];
static char gszFileTitle[BUFSIZE];

#define AVI_EDIT_CLASS  "edit"
static LPAVISTREAMINFO glpavisi;
static int gnSel;
int     gSelectedStream = -1;   // Highlight this text area when painting
RECT    grcSelectedStream;      // where to highlight

char    gszFilter[512];     // for AVIBuildFilter - more than one string!
static  HINSTANCE   ghInstApp;
static  HACCEL      ghAccel;
static  WNDPROC     gOldEditProc;
static  HWND        ghwndEdit;

#define MAXTIMERANGE  30000
#define MAXNUMSTREAMS   25

int                 gcpavi;                     // # of streams
PAVISTREAM          gapavi[MAXNUMSTREAMS];      // the current streams
int                 gcpaviSel;                  // num of edit streams
PAVISTREAM          gapaviSel[MAXNUMSTREAMS];   // edit streams to put on clipbd
int                 gStreamTop[MAXNUMSTREAMS+1];// y position of each stream
AVICOMPRESSOPTIONS  gaAVIOptions[MAXNUMSTREAMS];// compression options
LPAVICOMPRESSOPTIONS  galpAVIOptions[MAXNUMSTREAMS];
PGETFRAME           gapgf[MAXNUMSTREAMS];       // data for decompressing video
HDRAWDIB            ghdd[MAXNUMSTREAMS];        // drawdib handles
LONG                galSelStart[MAXNUMSTREAMS];
LONG                galSelLen[MAXNUMSTREAMS];
int                 giFirstAudio = -1;          // 1st audio stream found
int                 giFirstVideo = -1;          // 1st video stream found

#define             gfVideoFound (giFirstVideo >= 0)
#define             gfAudioFound (giFirstAudio >= 0)

BOOL                gfPlaying = FALSE;          // are we currently playing?
LONG                glPlayStartTime;    // When did we start playing?
LONG                glPlayStartPos;     // Where were we on the scrollbar?
LONG                timeStart;          // cached start, end, length
LONG                timeEnd;
LONG                timeLength;
LONG                timehscroll;        // how much arrows scroll HORZ bar
int                 nVertSBLen;         // vertical scroll bar
int                 nHorzSBLen;         // horizontal scroll bar
int                 nVertHeight;
int                 nHorzWidth;
DWORD               gdwMicroSecPerPixel = 1000L;        // scale for video
WORD                gwZoom = 4;         // one-half zoom (divide by 4)
HWND                ghwndMCI;

// buffer for wave data
LPVOID lpAudio;

// constants for painting
#define VSPACE  8       // some vertical spacing
#define SELECTVSPACE 4 // height of selection line
#define HSPACE  0       // space between frames for video stream
#define TSPACE  10      // space for text area about each stream
#define AUDIOVSPACE  64 // height of an audio stream at X1 zoom
#define HIGHLIGHT       (GetSysColor(COLOR_HIGHLIGHT) ? GetSysColor(COLOR_HIGHLIGHT) : GetSysColor(COLOR_ACTIVECAPTION))

void             FrameVideo(HDC hdc, RECT *rcFrame, HBRUSH hbr);
int gfWait = 0;


//ver 1.1
long playtime=0;

long GetScrollTime();
void SetScrollTime(long time);



//ver 1.1
#define WM_USER_PLAY 0x00401
#define BUFFER_LENGTH  256

void GetImageDimension(RECT& rcFrame); 
char playfiledir[300];	
char seps[] = "*";
void OpenMovieFileInit(char *filename);
void ResizeToMovie(BOOL useDefault = FALSE);
void SetTitleBar(CString title);

BOOL playerAlive = TRUE;
UINT PlayAVIThread(LPVOID pParam);

#define MENU_OPEN           11
#define MENU_MERGE          17

int ErrMsg (LPSTR sz,...);
void FreeDrawStuff();
void FileStop(int mode);
void FixWindowTitle();
void FixScrollbars();
void InitStreams();
void InitAvi(LPSTR szFile, int nMenu);
void InitAvi(LPSTR szFile, int nMenu, long starttime, int keepcounter, int overwriteaudio, int resetslider);
void FreeAvi();
void PlayMovie(int mode);
void StopPlayingRecording();


int maxxScreen = 800;
int maxyScreen = 600;

int doneOnce=0;

HWND viewWnd = NULL;
HWND mainWnd = NULL;

HBITMAP hLogoBM = NULL;

int nColors=24;
int gfRecording = 0;
#define SOUND_MODE 0
#define SILENT_MODE 1

CSliderCtrl *sliderCtrlPtr;	
CStatusBar *statusbarCtrl;


void SetTimeIndicator(CString timestr);
int infoLineHeight = 20; 


/////////////////////////////////////////////
// Merging Module
/////////////////////////////////////////////

void NukeAVIStream(int i);
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor);	    
BOOL CALLBACK SaveCallback(int iProgress);
BOOL WinYield(void);
CString GetProgPath();

#define NUMSTREAMS   2

CString savedir("");
CString cursordir("");
CString videofilepath("");

//Path to temporary wav file
CString tempaudiopath;
CString tempsilencepath;
int recordaudio=1;

//Audio Recording Variables
UINT AudioDeviceID = WAVE_MAPPER;

HWAVEIN m_hRecord;
WAVEFORMATEX m_Format;
DWORD m_ThreadID;
int m_QueuedBuffers=0;
int	 m_BufferSize = 1000;	// number of samples

#define WM_USER_GENERIC  0x00401

//Audio Options Dialog
LPWAVEFORMATEX      pwfx = NULL;
DWORD               cbwfx;

//Audio Formats Dialog
DWORD waveinselected = WAVE_FORMAT_2S16;
int audio_bits_per_sample = 16;
int audio_num_channels = 2;
int audio_samples_per_seconds = 22050 ;
BOOL bAudioCompression = TRUE;


#define  MILLISECONDS 0
#define  FRAMES 1
BOOL interleaveFrames = TRUE;
int  interleaveFactor = 100;
int  interleaveUnit = MILLISECONDS;

/////////////////////////////////////////////
// Audio Recording Module
/////////////////////////////////////////////

#if !defined(WAVE_FORMAT_MPEGLAYER3)
#define  WAVE_FORMAT_MPEGLAYER3 0x0055  
#endif


void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio,int resetslider);
void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio);


CSoundFile *m_pFile = NULL;
CSoundFile *m_pSilenceFile = NULL;
BOOL silenceFileValid = FALSE;

BOOL StartAudioRecording(WAVEFORMATEX* format);
void waveInErrorMsg(MMRESULT result, const char *);
int AddInputBufferToQueue();
void SetBufferSize(int NumberOfSamples);
void CALLBACK OnMM_WIM_DATA(UINT parm1, LONG parm2);
void CALLBACK OnMM_WOM_DATA(UINT parm1, LONG parm2);
void DataFromSoundIn(CBuffer* buffer) ;
void StopAudioRecording();
BOOL InitAudioRecording();
void ClearAudioFile();
void GetTempWavePath();
CString GetTempPath();

#define BasicBufSize 32768
BOOL CreateSilenceFile();
void ClearAudioSilenceFile();
void GetSilenceWavePath();
void WriteSilenceFile(CBuffer* buffer); 
 
void RemoveExistingAudioTracks();
void AddAudioWaveFile(char  *FileName);

void BuildRecordingFormat();
void SuggestCompressFormat();
void SuggestRecordingFormat();
void AllocCompressFormat();

void TimeDependentInsert(CString filename, long shiftTime);

int recordstate=0;
int recordpaused=0;

int audioRecordable = 0;
int audioPlayable = 0;

BOOL endAudioRecording = FALSE;
BOOL initAudioRecording = FALSE;

long glRecordStartTimeValue = 0;

#define RESETCOUNTER 0
#define KEEPCOUNTER 1

#define NEW_AUDIO_TRACK 0
#define OVERWRITE_AUDIO 1

#define NO_RESET_SLIDER 0
#define RESET_TO_START 1
#define RESET_TO_CURRENT 1

int EditStreamReplace(PAVISTREAM pavi, LONG * plPos,  LONG * plLength, PAVISTREAM pstream, LONG lStart, LONG lLength);
int EditStreamPadSilence(PAVISTREAM pavi, LONG * plPos,  LONG * plLength);
long ExAVIStreamTimeToSample( PAVISTREAM pavi, LONG lTime);
int EditStreamSilenceShift(PAVISTREAM pavi, LONG * plPos, LONG * plLength);
long SafeStreamTimeToSample( PAVISTREAM pavi, LONG starttime );
void RecomputeStreamsTime(int resetslider,long timeCurrent);


void CloneAudioStream_ReplaceStreamPool(int i,PAVISTREAM pavi);
void ReInitAudioStream(int i);


void SetAdditionalCompressSettings(BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit);
LPWAVEFORMATEX  allocRetrieveAudioFormat(PAVISTREAM pavi);


void SetDurationLine();
void SetRPSLine();

int allowRecordExtension = 1;

int fileModified = 0;

void Msg(const char fmt[], ...);
void DumpFormat(WAVEFORMATEX*  pwfx, const char* str );


//ver 2.26
int autoplay = 0;
int autoexit = 0;;
int fullscreen = 0;

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView

IMPLEMENT_DYNCREATE(CPlayplusView, CView)

BEGIN_MESSAGE_MAP(CPlayplusView, CView)
	//{{AFX_MSG_MAP(CPlayplusView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_FILE_PLAY, OnFilePlay)
	ON_COMMAND(ID_FILE_REWIND, OnFileRewind)
	ON_COMMAND(ID_FILE_STOP, OnFileStop)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_LASTFRAME, OnFileLastframe)
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_ZOOM_RESIZETOMOVIESIZE, OnZoomResizetomoviesize)
	ON_COMMAND(ID_ZOOM_QUARTER, OnZoomQuarter)
	ON_COMMAND(ID_ZOOM_HALF, OnZoomHalf)
	ON_COMMAND(ID_ZOOM_1, OnZoom1)
	ON_COMMAND(ID_ZOOM_2, OnZoom2)
	ON_COMMAND(ID_ZOOM_4, OnZoom4)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_1, OnUpdateZoom1)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_2, OnUpdateZoom2)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_4, OnUpdateZoom4)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_HALF, OnUpdateZoomHalf)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_QUARTER, OnUpdateZoomQuarter)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_RESIZETOMOVIESIZE, OnUpdateZoomResizetomoviesize)
	ON_UPDATE_COMMAND_UI(ID_FILE_REWIND, OnUpdateFileRewind)
	ON_UPDATE_COMMAND_UI(ID_FILE_LASTFRAME, OnUpdateFileLastframe)
	ON_COMMAND(ID_BUTTON32785, OnButtonRecord)
	ON_UPDATE_COMMAND_UI(ID_BUTTON32785, OnUpdateRecord)
	ON_COMMAND(ID_AUDIO_REMOVEEXISTINGAUDIOTRACKS, OnAudioRemoveexistingaudiotracks)
	ON_COMMAND(ID_AUDIO_ADDAUDIOFROMWAVEFILE, OnAudioAddaudiofromwavefile)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_REMOVEEXISTINGAUDIOTRACKS, OnUpdateAudioRemoveexistingaudiotracks)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_ADDAUDIOFROMWAVEFILE, OnUpdateAudioAddaudiofromwavefile)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_AUDIO_AUDIOOPTIONS, OnAudioAudiooptions)
	ON_COMMAND(ID_ZOOM_TESTAUDIO, OnZoomTestaudio)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveas)
	ON_UPDATE_COMMAND_UI(ID_FILE_PLAY, OnUpdateFilePlay)
	ON_COMMAND(ID_AUDIO_EXTENSION, OnAudioExtension)
	ON_UPDATE_COMMAND_UI(ID_AUDIO_EXTENSION, OnUpdateAudioExtension)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_MESSAGE (WM_USER_GENERIC, OnUserGeneric)	
	ON_MESSAGE(MM_WIM_DATA, OnMM_WIM_DATA)
	ON_MESSAGE(MM_WOM_DONE, OnMM_WOM_DONE)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView construction/destruction

CPlayplusView::CPlayplusView()
{
	// TODO: add construction code here

}

CPlayplusView::~CPlayplusView()
{
}

BOOL CPlayplusView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView drawing

void CPlayplusView::OnDraw(CDC* pDC)
{
	CPlayplusDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView printing

BOOL CPlayplusView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CPlayplusView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CPlayplusView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView diagnostics

#ifdef _DEBUG
void CPlayplusView::AssertValid() const
{
	CView::AssertValid();
}

void CPlayplusView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPlayplusDoc* CPlayplusView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlayplusDoc)));
	return (CPlayplusDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlayplusView message handlers

void CPlayplusView::OnFileOpen() 
{
	OPENFILENAME ofn;
	gszFileName[0] = 0;	    
	gszFileTitle[0] = 0;
    
	LoadString( ghInstApp, IDS_OPENTITLE, gszBuffer, BUFSIZE );	    
		    
	memset(&ofn, 0, sizeof(ofn));
	ofn.lpstrTitle = gszBuffer;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = "AVI Movie Files (*.avi)\0*.avi\0\0";
    ofn.lpstrFile = gszFileName;
	ofn.nMaxFile = sizeof(gszFileName);		
    ofn.lpstrFileTitle = gszFileTitle;
    ofn.nMaxFileTitle = sizeof(gszFileTitle);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		
	// If filename exists 
	if (GetOpenFileName(&ofn)) {

		OnFileStop(); 

		gwZoom = 4;
		
		InitAvi(gszFileName, MENU_OPEN);
		ResizeToMovie();							   

	}

	Invalidate();
}


void ResizeToMovie(BOOL useDefault)
{

	RECT rc;	
	if (useDefault) {

		rc.left = 100;	
		rc.top = 100;
		rc.right=rc.left + 308-1;   
		rc.bottom=rc.top + 222-1;    	
		

	}
	else
		GetImageDimension(rc) ;		
	

	CWnd* mainWindow = AfxGetMainWnd( );

	if (((rc.bottom-rc.top)<20) && ((rc.right-rc.left)<10)) return;


	((CMainFrame*) mainWindow)->ResizeToMovie(rc);


	

									   
}

void CPlayplusView::OnFileClose() 
{

	if ((fileModified == 1) && (pmode == DUBBER)) {

		int ret = MessageBox("Do you want to save changes ?","Note",MB_YESNOCANCEL | MB_ICONQUESTION);
		if (ret == IDYES) {
			SendMessage(WM_COMMAND,ID_FILE_SAVEAS,0);
			return;
		}
		else if (ret == IDCANCEL) 
			return;

	}

    FreeAvi();
	fileModified = 0;
    
	gszFileName[0] = '\0';
	gszFileTitle[0] = '\0';
    FixWindowTitle();

	ResizeToMovie(TRUE);

	//Invalidate();

}

void CPlayplusView::OnFilePlay() 
{
	 
	PlayMovie(SOUND_MODE);

}

void PlayMovie(int mode)
{
		glPlayStartTime = timeGetTime();
	    glPlayStartPos = GetScrollTime();


		//Recording sessions do not reset time
		if ((allowRecordExtension) && (pmode == DUBBER) && (mode == SILENT_MODE)) 
		{
			//do not reset time if this is a recording session (mode == silent) at the end of the longest stream and extension is allowed
		}
		else if ((!allowRecordExtension) && (pmode == DUBBER) && (mode == SILENT_MODE)) 
		{
			//do not reset time also if recording sesson even if  not allowRecordExtension
			
		}
		else if (glPlayStartPos>=timeEnd) {

			//resetting time for playing sessions

			SetScrollTime(timeStart);
			glPlayStartPos = GetScrollTime();
		
		}


		if (mode == SOUND_MODE) { 

			//should this be put here since there is adjustment to the scroll time 
			if (gfAudioFound) {			
				
					aviaudioPlay(viewWnd,
						 gapavi[giFirstAudio],
						 AVIStreamTimeToSample(gapavi[giFirstAudio], GetScrollTime()),
						 AVIStreamEnd(gapavi[giFirstAudio]),
						 FALSE);


			}

		}
		//else { //silent mode
		//}

		gfPlaying = TRUE;

}

void CPlayplusView::OnFileRewind() 
{
	// TODO: Add your command handler code here
	SetScrollTime( timeStart);
	::InvalidateRect(viewWnd, NULL, FALSE);
	::UpdateWindow(viewWnd);
	
}

void CPlayplusView::OnFileStop() 
{
	StopPlayingRecording();

	SetRPSLine();

	Sleep(50);
	if (autoexit)
		::PostMessage(AfxGetMainWnd()->m_hWnd,WM_COMMAND,ID_APP_EXIT,0);
	
}


void StopPlayingRecording()
{

	if (gfRecording) {		

		
		endAudioRecording  =TRUE;
		FileStop(SILENT_MODE);
		
		
	}
	else
		FileStop(SOUND_MODE);
	

}


void PaintVideo(HDC hdc, RECT rcFrame, int iStream, LPBITMAPINFOHEADER lpbi, LONG lCurSamp, LONG lPos)
{
    int         iLen;
    char        szText[BUFSIZE];



	//if lbpi is present, draw it
    if (lpbi)
    {



		//Added code for centering
		CRect rect;	
		::GetClientRect( viewWnd,&rect);

		int offsetx=0;
		int offsety=0;
		int bitmapwidth = rcFrame.right-rcFrame.left+1;
		int bitmapheight = rcFrame.bottom-rcFrame.top+1;
			
		if (rect.right> bitmapwidth)
				offsetx = (rect.right - bitmapwidth)/2;

		if (rect.bottom> bitmapheight)
				offsety = (rect.bottom - bitmapheight)/2;

	
		
		DrawDibDraw(ghdd[iStream], hdc,
		    rcFrame.left+offsetx, rcFrame.top+offsety,
		    rcFrame.right - rcFrame.left - 1,
		    rcFrame.bottom - rcFrame.top - 1,
		    lpbi, NULL,
		    0, 0, -1, -1,
		    (iStream == giFirstVideo) ? 0 :DDF_BACKGROUNDPAL);


	iLen = wsprintf(szText, "%ld %ld.%03lds",
			lCurSamp, lPos / 1000, lPos % 1000);
    }

    //
    // Before or after the movie (or read error) draw GRAY
    //
    else {

		//do nothing


    }

}




int PaintStuff(HDC hdc, HWND hwnd, BOOL fDrawEverything)
{
	int         xStreamLeft;
    int         yStreamTop;
    int         iFrameWidth;
    LONG        lSamp, lCurSamp;
    int         n;
    int         nFrames;
    LPBITMAPINFOHEADER lpbi = NULL;
    LONG        l;
    LONG        lTime;
    LONG        lSize = 0;
    RECT        rcFrame, rcC;
    int         i;
    

    GetClientRect(hwnd, &rcC);

    
    lTime = GetScrollTime();
    yStreamTop = -GetScrollPos(hwnd, SB_VERT);
	xStreamLeft = -GetScrollPos(hwnd, SB_HORZ);

    // for all streams
    for (i=0; i<gcpavi; i++) {
	
		AVISTREAMINFO   avis;		
		LONG             lEndTime;
		
		gStreamTop[i] = yStreamTop + GetScrollPos(hwnd, SB_VERT);	
		AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

	
		if (avis.fccType == streamtypeVIDEO) {
			if (gapgf[i] == NULL)
			continue;

			//
			// Which frame belongs at this time?
			//
			lEndTime = AVIStreamEndTime(gapavi[i]);
			if (lTime <= lEndTime)
			lSamp = AVIStreamTimeToSample(gapavi[i], lTime);
			else {      // we've scrolled past the end of this stream
				
				//lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i]));
				lSamp = AVIStreamTimeToSample(gapavi[i], AVIStreamStartTime(gapavi[i])+timeLength-1);
								 
			}
			
			//ver 1.1
			iFrameWidth = (avis.rcFrame.right - avis.rcFrame.left)  + HSPACE;
			nFrames = 0;
			
			
			for (n = -nFrames; n <= nFrames; n++) {

			
				if (i == giFirstVideo) {
				
					lCurSamp = lSamp + n;		
					l = AVIStreamSampleToTime(gapavi[i], lCurSamp);

				} else {    // NOT the first video stream
    
					l = lTime + MulDiv32(n * (iFrameWidth+HSPACE), gdwMicroSecPerPixel, 1000);		    		    
					lCurSamp = AVIStreamTimeToSample(gapavi[i], l);		    
					l = AVIStreamSampleToTime(gapavi[i], lCurSamp);
				}

			
				if (gapgf[i] && lCurSamp >= AVIStreamStart(gapavi[i])) 		    
					lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(gapgf[i], lCurSamp);
				else
					lpbi = NULL;

			
				// Figure out where to draw this frame		
				rcFrame.left    =xStreamLeft;
				rcFrame.top    = yStreamTop ;
				rcFrame.right  = rcFrame.left +
						 (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4;
				rcFrame.bottom = rcFrame.top +
						 (avis.rcFrame.bottom - avis.rcFrame.top) * gwZoom / 4;

				
				//Patch to prevent blank screen
				if (lpbi == NULL)
				{
					lpbi = (LPBITMAPINFOHEADER) AVIStreamGetFrame(gapgf[i], AVIStreamEnd(gapavi[i])-1);

				}

			
				PaintVideo(hdc, rcFrame, i, lpbi, lCurSamp, l);
			}

			
			if (lpbi)	AVIStreamSampleSize(gapavi[i], lSamp, &lSize);

			//ver 1.1
			yStreamTop +=   (rcFrame.bottom - rcFrame.top); 

		} //If Video Stream

		// Give up once we're painting below the bottom of the window
		if (!fDrawEverything && yStreamTop >= rcC.bottom)
			break;

	} //for all streams

    // The bottom of all the streams;
    gStreamTop[gcpavi] = yStreamTop + GetScrollPos(hwnd, SB_VERT);

    // Return number of lines drawn     
    return yStreamTop + GetScrollPos(hwnd, SB_VERT);

}



//Separation of init streams into 1)avioptions preparation 2)timestart, timeend, time length finding  3)reseting counter components
//Pre-condition : Init streams is called only in InserAVIfile..if it needed to be called in other places, make sure the relevant and only the revlant components are included
void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio,int resetslider)
{

	//Note: the keepcounter variable is not used	
	//It's function is taken by the resetslider

	//Currently the resetslider is used only if the mode is not OVERWRITEAUDIO
	//if it is FALSE, then we force the counter not to reset
	//by default, for most cases, the reset silder is set to TRUE, especially when insertAviFile  is used to insert new streams into the current stream pool

    int         i;
    PAVISTREAM  pavi;
	int needshift = 0;
	long timeCurrent = GetScrollTime();

    for (i = gcpavi; i <= MAXNUMSTREAMS; i++) {
		if (AVIFileGetStream(pfile, &pavi, 0L, i - gcpavi) != AVIERR_OK)
			break;
		
		if (i == MAXNUMSTREAMS)
		{
			AVIStreamRelease(pavi);
			LoadString( ghInstApp, IDS_MAXSTREAMS, gszBuffer, BUFSIZE );
			ErrMsg(gszBuffer);
			break;
		}
		if (CreateEditableStream(&gapavi[i], pavi) != AVIERR_OK) {
			AVIStreamRelease(pavi);
			break;
		}
		AVIStreamRelease(pavi);
		galSelStart[i] = galSelLen[i] = -1;
    }

    AVIFileRelease(pfile);

    if (gcpavi == i && i != MAXNUMSTREAMS)
    {

		LoadString( ghInstApp, IDS_NOOPEN, gszBuffer, BUFSIZE );

		ErrMsg(gszBuffer, lpszFile);
		return;
    }

    FreeDrawStuff();
    gcpavi = i;    
	InitStreams(); //This function inherent has a RecomputeStreamsTime component


	
	//This function is here mainly for the benefit of  needshift ==1 
	//however, for thecase where needshift == 0, the recomputation of Streams Time does not logically affect the original computation by InitStreams
	//in this case, the recomputStreamsTime is basically for 
	//(a) setting the slider value
	//or (b) for recomputing streamstime after a conversion

	RecomputeStreamsTime(resetslider,timeCurrent);
	FixScrollbars();	
	FixWindowTitle();

	


}

void InsertAVIFile(PAVIFILE pfile, LPSTR lpszFile,long starttime, int keepcounter, int overwriteaudio)
{

	//By default, if the resetslider is not specified, it is assumed to be TRUE
	//I.e, the slider will be reset after each InsertAVIFile
	InsertAVIFile(pfile, lpszFile, starttime, keepcounter, overwriteaudio, TRUE);

}


void FreeAvi()
{
    int i;

    FreeDrawStuff();

    AVISaveOptionsFree(gcpavi, galpAVIOptions);

    for (i = 0; i < gcpavi; i++) {
	AVIStreamRelease(gapavi[i]);
    }

    // Good a place as any to make sure audio data gets freed
    if (lpAudio)
	GlobalFreePtr(lpAudio);
    lpAudio = NULL;

    gcpavi = 0;
}


void InitAvi(LPSTR szFile, int nMenu)
{
	InitAvi(szFile, nMenu, -1, KEEPCOUNTER, NEW_AUDIO_TRACK, RESET_TO_START);
}


void InitAvi(LPSTR szFile, int nMenu, long starttime, int keepcounter, int overwriteaudio, int resetslider)
{
    HRESULT     hr;
    PAVIFILE    pfile;

    hr = AVIFileOpen(&pfile, szFile, 0, 0L);

    if (hr != 0)
    {
		//LoadString( ghInstApp, IDS_NOOPEN, gszBuffer, BUFSIZE );
		ErrMsg("Unable to open file %s", szFile);
		return;
    }

    //
    // If we're opening something new, close other open files, otherwise
    // just close the draw stuff so we'll merge streams with the new file
    //
    if (nMenu == MENU_OPEN) {
		FreeAvi();	
		fileModified = 0;
	}

	//ErrMsg("overwite in initavi %d",overwriteaudio);
    InsertAVIFile(pfile, szFile, starttime,  keepcounter,  overwriteaudio,resetslider);

}


UINT PlayAVIThread(LPVOID pParam) {

	while (playerAlive)
    {	

		if (!gfRecording && initAudioRecording) {

			initAudioRecording = FALSE;			
			gfRecording = TRUE;

			InitAudioRecording();
			StartAudioRecording(&m_Format);				

		}


		if (gfRecording && endAudioRecording) 
		{

			endAudioRecording = FALSE;
			gfRecording = FALSE;
			//gfPlaying = FALSE;

			StopAudioRecording();
			ClearAudioFile();		

			CreateSilenceFile();
			
			::PostMessage(viewWnd,WM_USER_GENERIC,0,0);
			//OnUserGeneric(0,0);
		

		}

			
		if (gfPlaying) {			
			

			
			LONG    l;			
			l = aviaudioTime();         // returns -1 if no audio playing

			
			//
			// If we can't use the audio clock to tell us how long we've been
			// playing, calculate it ourself
			//
			if (l == -1)
				l = timeGetTime() - glPlayStartTime + glPlayStartPos;
				


			if (l != (LONG)GetScrollTime()) {

				if (l < timeStart)      // make sure number isn't out of bounds
					l = timeStart;
				if (l > timeEnd)       { // looks like we're all done!
					
					if (pmode == PLAYER)
						PostMessage(viewWnd,WM_COMMAND,ID_FILE_STOP,0);
					else if (!allowRecordExtension) {
						//StopPlayingRecording();
						PostMessage(viewWnd,WM_COMMAND,ID_FILE_STOP,0);
					}
					else {

						//Allowing Recording Extension even if even if end of current file is reached
						if (gfRecording) {
							//Stop only the playing but not the recording
							gfPlaying = FALSE;
							//l = timeEnd;

						}
						else {

							//playing mode...stop the playing
							PostMessage(viewWnd,WM_COMMAND,ID_FILE_STOP,0);

						}

					}

					

						
				}

				

				SetScrollTime(l);			 

													
				InvalidateRect(viewWnd, NULL, FALSE);
				 UpdateWindow(viewWnd);

				continue;

			}

						
		}

		::Sleep(50); 

				
	}



	return 0;
}

int ErrMsg (LPSTR sz,...)
{
    static char szOutput[4*BUFSIZE];

    va_list va;

    va_start(va, sz);
    wvsprintf (szOutput,sz,va);      /* Format the string */
    va_end(va);
    MessageBox(NULL,szOutput,NULL, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
    return FALSE;
}


void FreeDrawStuff()
{
    int i;

    // Make sure we're not playing!
    aviaudioStop();

    for (i = 0; i < gcpavi; i++) {
	if (gapgf[i]) {
	    AVIStreamGetFrameClose(gapgf[i]);
	    gapgf[i] = NULL;
	}
	if (ghdd[i]) {
	    DrawDibClose(ghdd[i]);
	    ghdd[i] = 0;
	}
    }

    ::SetScrollRange(viewWnd, SB_HORZ, 0, 0, TRUE);
    giFirstVideo = giFirstAudio = -1;
}

void FileStop(int mode)
{	
	if (mode == SOUND_MODE)
	{
		if	(gfAudioFound)
				aviaudioStop();

	}
	//else {//silent mode
	//}

	gfPlaying = FALSE;
	
}



void RecomputeStreamsTime(int resetslider,long timeCurrent)
{
    
    timeStart = 0x7FFFFFFF;
    timeEnd   = 0;

	
    // Walk through and init all streams loaded    
    for (int i = 0; i < gcpavi; i++) {
		timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
		timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));
	}

	timeLength = timeEnd - timeStart;

    if (timeLength == 0) timeLength = 1;

    // Make sure HSCROLL scrolls enough to be noticeable.
    timehscroll = max(timehscroll, timeLength / MAXTIMERANGE + 2);
	

	//Set the slider positiom after the timeLenght has been adjusted
	if (resetslider == NO_RESET_SLIDER) {
		//Do nothing
	}
	else if (resetslider == RESET_TO_START) 
		SetScrollTime( timeStart);
	else if (resetslider == RESET_TO_CURRENT) //This setting has not been tested, it is set to the position of the scrolltime where no stream functions has been previously altered
		SetScrollTime( timeCurrent);

	return;


}


void InitStreams()
{
    AVISTREAMINFO     avis;
    LONG        lTemp;
    int         i;
	

    //
    // Start with bogus times
    //
    timeStart = 0x7FFFFFFF;
    timeEnd   = 0;

    //
    // Walk through and init all streams loaded
    //
    for (i = 0; i < gcpavi; i++) {

	AVIStreamInfo(gapavi[i], &avis, sizeof(avis));

	//
	// Save and SaveOptions code takes a pointer to our compression opts
	//
	galpAVIOptions[i] = &gaAVIOptions[i];

	//
	// clear options structure to zeroes
	//
	_fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));

	//
	// Initialize the compression options to some default stuff
	// !!! Pick something better
	//
	galpAVIOptions[i]->fccType = avis.fccType;

	switch(avis.fccType) {

	    case streamtypeVIDEO:
		galpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID |
				AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
		galpAVIOptions[i]->fccHandler = 0;
		galpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
		galpAVIOptions[i]->dwKeyFrameEvery = (DWORD)-1; // Default
		galpAVIOptions[i]->dwBytesPerSecond = 0;
		galpAVIOptions[i]->dwInterleaveEvery = 1;
		break;

	    case streamtypeAUDIO:
		galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
		galpAVIOptions[i]->dwInterleaveEvery = 1;
		AVIStreamReadFormat(gapavi[i],
				    AVIStreamStart(gapavi[i]),
				    NULL,
				    &lTemp);
		galpAVIOptions[i]->cbFormat = lTemp;
		if (lTemp)
		    galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
		// Use current format as default format
		if (galpAVIOptions[i]->lpFormat)
		    AVIStreamReadFormat(gapavi[i],
					AVIStreamStart(gapavi[i]),
					galpAVIOptions[i]->lpFormat,
					&lTemp);
		break;

	    default:
		break;
	}

	//
	// We're finding the earliest and latest start and end points for
	// our scrollbar.
	//
	timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
	timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));

	//
	// Initialize video streams for getting decompressed frames to display
	//
	if (avis.fccType == streamtypeVIDEO) {

	    gapgf[i] = AVIStreamGetFrameOpen(gapavi[i], NULL);

	    if (gapgf[i] == NULL)
		continue;

	    ghdd[i] = DrawDibOpen();
	    // !!! DrawDibBegin?

	    if (!gfVideoFound) {
	    DWORD        dw;

		//
		// Remember the first video stream --- treat it specially
		//
		giFirstVideo = i;

		//
		// Set the horizontal scrollbar scale to show every frame
		// of the first video stream exactly once
		//
		dw = (avis.rcFrame.right - avis.rcFrame.left) * gwZoom / 4 + HSPACE;

		gdwMicroSecPerPixel = muldiv32(1000000,
					       avis.dwScale,
					       dw * avis.dwRate);
		// Move one frame on the top video screen for each HSCROLL
		timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
	    }

	} else if (avis.fccType == streamtypeAUDIO) {

	    // These aren't used and better be NULL!
		ghdd[i] =   NULL;
		gapgf[i] = NULL;
		

	    //
	    // If there are no video streams, we base everything on this
	    // audio stream.
	    //
	    if (!gfAudioFound && !gfVideoFound) {

		// Show one sample per pixel
		gdwMicroSecPerPixel = muldiv32(1000000,
					       avis.dwScale,
					       avis.dwRate);
		// Move one sample per HSCROLL
		// Move at least enough to show movement
		timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
	    }

	    //
	    // Remember the first audio stream --- treat it specially
	    //
		if (!gfAudioFound) {
			//ErrMsg("reset");
			giFirstAudio = i;
			
		}

	}

    }

    timeLength = timeEnd - timeStart;

    if (timeLength == 0)
	timeLength = 1;

    // Make sure HSCROLL scrolls enough to be noticeable.
    timehscroll = max(timehscroll, timeLength / MAXTIMERANGE + 2);

	
}





void FixWindowTitle()
{

	char szTitle[512];    
	
	if (pmode == PLAYER) {

		if (gszFileTitle[0])     
			//wsprintf(szTitle, "Player - %s", (LPSTR)gszFileTitle);
			wsprintf(szTitle, "Player :- %s", (LPSTR)gszFileTitle);
		else
			wsprintf(szTitle, "Player");	

	}
	else {

		if (gszFileTitle[0])     
			wsprintf(szTitle, "Dubber :- %s", (LPSTR)gszFileTitle);
		else
			wsprintf(szTitle, "Dubber");

	}
	

	SetTitleBar(szTitle);
    
	
}

void SetTitleBar(CString title) {
	
	CWinApp* app = NULL;
	app	= AfxGetApp();
	if (app) {

		HWND mainwnd = NULL;
		mainwnd= app->m_pMainWnd->m_hWnd;
		if (mainwnd)
			::SetWindowText(mainwnd, LPCTSTR(title));

	}
}



void FixScrollbars()
{
	
    int                 nHeight = 0;
	int                 nWidth = 0;
    RECT                rc;
    HDC                 hdc;
	HWND				hwnd;


	hwnd = viewWnd;

	
    //
    // Determine how tall our window needs to be to display everything.
    //
    hdc = GetDC(NULL);
    ExcludeClipRect(hdc, 0, 0, 32767, 32767);   // don't actually draw
    nHeight = PaintStuff(hdc, hwnd, TRUE);
    ReleaseDC(NULL, hdc);

    //
    // Set vertical scrollbar for scrolling the visible area
    //
    GetClientRect(hwnd, &rc);
    nVertHeight = nHeight;      // total height in pixels of entire display
	
	
	RECT rcImage;
	GetImageDimension(rcImage);	
	
	if (gwZoom>=4)
		nHorzWidth = ((rcImage.right-rcImage.left+1)*gwZoom)/4;
	else
		nHorzWidth = rcImage.right-rcImage.left+1;

    //
    // We won't fit in the window... need scrollbars
    //    
	if (nHeight > rc.bottom)
    {
	nVertSBLen = nHeight - rc.bottom;
	SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);
	SetScrollPos(hwnd, SB_VERT, 0, TRUE);

	//
	// We will fit in the window!  No scrollbars necessary
	//
    }
    else
    {
	nVertSBLen = 0;
	SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
    }



	//
    // Horzizontal Scrollbar
    //    
	if (nWidth > rc.right)
    {
		nHorzSBLen = nWidth - rc.right;
		SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);
		SetScrollPos(hwnd, SB_HORZ, 0, TRUE);

	//
	// We will fit in the window!  No scrollbars necessary
	//
    }
    else
    {
		nHorzSBLen = 0;
		SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
    }
    return;
	
}


//Using this function assume that gcpavi, the number of streams
//and gapavi[], the stream array is properly initialized
//Return the Dimension of the first video stream
void GetImageDimension(RECT& rcFrame) 
{
	AVISTREAMINFO   avis;	

	avis.rcFrame.left=0;
	avis.rcFrame.right=0;
	avis.rcFrame.top=0;
	avis.rcFrame.bottom=0;


	int j;
	for (j=0; j<gcpavi; j++) {			
	
		AVIStreamInfo(gapavi[j], &avis, sizeof(avis));

		if (avis.fccType == streamtypeVIDEO) 
		{
			if (gapgf[j] != NULL)			
				break; //break from for loop	

		}		

	}

	rcFrame=avis.rcFrame;

}

int CPlayplusView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
    WORD wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a)
    {		
		::MessageBox(NULL, "This program requires Video For Windows 1.1", "Note", MB_OK|MB_ICONSTOP);
		return -1;
    }

	AVIFileInit();

	
	HDC hScreenDC = ::GetDC(NULL);	
	nColors = ::GetDeviceCaps(hScreenDC, BITSPIXEL );		 	
	maxxScreen = GetDeviceCaps(hScreenDC,HORZRES);
	maxyScreen = GetDeviceCaps(hScreenDC,VERTRES);		
	::ReleaseDC(NULL,hScreenDC);		

	
	if (pmode == PLAYER)
		hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO_PLAYER)); 
	else if (pmode == DUBBER)
		hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO_DUBBER)); 
	else
		hLogoBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP2)); 
	

	int tdata=0;
	CWinThread * pThread = AfxBeginThread(PlayAVIThread, &tdata);	

	audioPlayable = waveOutGetNumDevs ();
	
	audioRecordable = waveInGetNumDevs();
	if (audioRecordable) {
		
		SuggestRecordingFormat();
		SuggestCompressFormat();

	}

	if (pmode == PLAYER)
		allowRecordExtension = 0;
	else if (pmode == DUBBER)
		allowRecordExtension = 1;


	return 0;
}

void CPlayplusView::OnDestroy() 
{
		
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	AVIClearClipboard();
    FreeAvi();      // close all open streams
    AVIFileExit();      // shuts down the AVIFile system

	if (hLogoBM) {
		DeleteObject(hLogoBM);
		hLogoBM = NULL;
	}


	if (pwfx) {
		
		GlobalFreePtr(pwfx);
		pwfx = NULL;

	}


	
}

void CPlayplusView::OnPaint() 
{


	
	if (!doneOnce) {
	
		if (strlen(playfiledir)!=0) {	
	
			
			OpenMovieFileInit(playfiledir);

		}
		doneOnce=1;

	}
	


	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (gcpavi<=0) {

		

		if (nColors >= 8) {

			HDC hdcBits = CreateCompatibleDC(dc.m_hDC);
			
			CRect rect;	
			GetClientRect( &rect);


			int offsetx=0;
			int offsety=0;
			int bitmapwidth = 300;
			int bitmapheight = 220;
			
			if (rect.right> bitmapwidth)
				offsetx = (rect.right - bitmapwidth)/2;

			if (rect.bottom> bitmapheight)
				offsety = (rect.bottom - bitmapheight)/2;

	
			
			HBITMAP old_bitmap =  (HBITMAP) ::SelectObject(hdcBits,hLogoBM);
		
			//OffScreen Buffer
			::BitBlt(dc.m_hDC, offsetx, offsety, rect.right-rect.left+1, rect.bottom-rect.top+1, hdcBits, 0, 0, SRCCOPY);              
		
			::SelectObject(hdcBits, old_bitmap);        	

			DeleteDC(hdcBits);


		}



	}
	else
		PaintStuff(dc.m_hDC, viewWnd, FALSE);






	SetDurationLine();
	SetRPSLine();

	
	// Do not call CView::OnPaint() for painting messages
}

void SetDurationLine()
{

	CString durationStr(" 0.00 / 0.00 sec");
	if (timeLength > 0) {	
		
		long timenow = GetScrollTime();
		float durationPlayed = (float) ((timenow - timeStart)/1000.0);		
		float durationLength = (float) ((timeEnd - timeStart)/1000.0);
		durationStr.Format(" %6.1f /%6.1f sec",durationPlayed,durationLength);		
		
	}

	if (statusbarCtrl)
			statusbarCtrl->SetPaneText(2,durationStr);

}


void SetRPSLine()
{

	CString istr;
	if (gfRecording)
		istr = "Recording";
	else if (gfPlaying)
		istr = "Playing";
	else
		istr = "Stopped";


	if (statusbarCtrl)
		statusbarCtrl->SetPaneText(1,istr);

}

BOOL CPlayplusView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{

	BOOL retval = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	viewWnd = m_hWnd;

								
	return retval;
}

void CPlayplusView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	HWND hwnd;
    RECT rc;
	RECT rcWnd;

	hwnd = viewWnd;

	if (gcpavi<=0) {

		if (hwnd) {
			::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
			::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);		
		}
		
		return;
	}

	//AdjustToolBar (hwnd);

    ::GetClientRect(hwnd, &rc);

	::GetWindowRect(hwnd, &rcWnd);
    //
    // There is not enough vertical room to show all streams. Scrollbars
    // are required.
    //	


	//Added ver 1.0
	int ScrollVertWidth = GetSystemMetrics(SM_CXVSCROLL); 
	int ScrollHorzHeight = GetSystemMetrics(SM_CYHSCROLL ); 

	int MinPos; 
	int MaxPos; 	
	int IncYVal = 0;

	int origbottom=0;
	int origright=0;
	origbottom=rc.bottom;
	origright=rc.right;
	
	//Obtaining true client area including the scroll bars
	
	MaxPos = 0;
	::GetScrollRange( hwnd, SB_HORZ, &MinPos , &MaxPos);	
	if (MaxPos>0)
		rc.bottom += ScrollHorzHeight;


	
	MaxPos = 0;
	::GetScrollRange( hwnd, SB_VERT, &MinPos , &MaxPos);
	if (MaxPos>0) {
	
		rc.right += ScrollVertWidth;

		//This is a hack, as the above code GetScrollRange may sometimes detect the presence of a scroll bar even if it is absent
		if (rc.right>rcWnd.right-rcWnd.left)
			rc.right -= ScrollVertWidth;  //deduct back 

	}
	
	//rc.right and rc.bottom now refers to the true client area
	if ((nVertHeight > rc.bottom  ) && (nHorzWidth > rc.right)) {
	
		
		//Both Scroll Bars
	
		//Adjust the client client to reflect both scroll bars
		rc.bottom -= ScrollHorzHeight;
		rc.right -= ScrollVertWidth;

		nVertSBLen = nVertHeight - rc.bottom;
		::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

		nHorzSBLen = nHorzWidth - rc.right;
		::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);		

	}
	else if ((nVertHeight <= rc.bottom  ) && (nHorzWidth <= rc.right))
	{
		
		//No Scroll Bars
		nVertSBLen = 0;
		::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
		nHorzSBLen = 0;
		::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);


	}		
	else  if ((nVertHeight >rc.bottom  ) && (nHorzWidth <= rc.right))
	{

		if (nHorzWidth <= rc.right - ScrollVertWidth) { //if after the addition of vertical SCROLLBAR, the horizontal width is still larger than image

			//Vert Scroll Bar Only
			nHorzSBLen = 0;
			::SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);

			nVertSBLen = nVertHeight - rc.bottom;
			::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);

		}
		else  {			
			
			//Activate Both ScrollBars

			//Adjust the client client to reflect both scroll bars
			rc.bottom -= ScrollHorzHeight;
			rc.right -= ScrollVertWidth;

			nVertSBLen = nVertHeight - rc.bottom;
			::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);
	
			nHorzSBLen = nHorzWidth - rc.right;
			::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);				

		}



	}
	else  if ((nVertHeight <= rc.bottom  ) && (nHorzWidth > rc.right))
	{


		if (nVertHeight <= rc.bottom - ScrollHorzHeight) { //if after the addition of horiz SCROLLBAR, the vertical height is still larger than image

			//Horiz Scroll Bar Only
			nVertSBLen = 0;
			::SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);

			nHorzSBLen = nHorzWidth - rc.right;
			::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);		

		}
		else  {			
			
			//Activate Both ScrollBars

			//Adjust the client client to reflect both scroll bars
			rc.bottom -= ScrollHorzHeight;
			rc.right -= ScrollVertWidth;

			nVertSBLen = nVertHeight - rc.bottom;
			::SetScrollRange(hwnd, SB_VERT, 0, nVertSBLen, TRUE);
	
			nHorzSBLen = nHorzWidth - rc.right;
			::SetScrollRange(hwnd, SB_HORZ, 0, nHorzSBLen, TRUE);				

		}

	}

	
}

void CPlayplusView::OnFileLastframe() 
{

	
	SetScrollTime(timeStart + timeLength - 1);
	::InvalidateRect(viewWnd, NULL, FALSE);
	::UpdateWindow(viewWnd);

	
}


BOOL CPlayplusView::OnEraseBkgnd(CDC* pDC) 
{
	
      // Set brush to desired background color
      CBrush backBrush(RGB(0, 0, 0));

      // Save old brush
      CBrush* pOldBrush = pDC->SelectObject(&backBrush);

      CRect rect;
      pDC->GetClipBox(&rect);     // Erase the area needed

      pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(),
          PATCOPY);
      pDC->SelectObject(pOldBrush);
      return TRUE;
    
}


void CPlayplusView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	HWND hwnd;
    int nScrollPos;
    RECT rc;

	hwnd = viewWnd;

    nScrollPos = ::GetScrollPos(hwnd, SB_HORZ);

    ::GetClientRect(hwnd, &rc);

    switch (nSBCode)
    {
	case SB_LINEDOWN:
	    nScrollPos += 10;
	    break;
	case SB_LINEUP:
	    nScrollPos -= 10;
	    break;
	case SB_PAGEDOWN:
	    nScrollPos += rc.right;
	    break;
	case SB_PAGEUP:
	    nScrollPos -= rc.right;
	    break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
	    nScrollPos = nPos;
	    break;
    }

    if (nScrollPos < 0)
	nScrollPos = 0;

    if (nScrollPos > nHorzSBLen)
	nScrollPos = nHorzSBLen;

    if (nScrollPos == ::GetScrollPos(hwnd, SB_HORZ))
	return;

    ::SetScrollPos(hwnd, SB_HORZ, nScrollPos, TRUE);
	::InvalidateRect(hwnd, NULL, FALSE);
    ::UpdateWindow(hwnd);

	
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlayplusView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	
	HWND hwnd;

	int nScrollPos;
    RECT rc;

	hwnd = viewWnd;

    nScrollPos = ::GetScrollPos(hwnd, SB_VERT);

    ::GetClientRect(hwnd, &rc);

    switch (nSBCode)
    {
	case SB_LINEDOWN:
	    nScrollPos += 10;
	    break;
	case SB_LINEUP:
	    nScrollPos -= 10;
	    break;
	case SB_PAGEDOWN:
	    nScrollPos += rc.bottom;
	    break;
	case SB_PAGEUP:
	    nScrollPos -= rc.bottom;
	    break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
	    nScrollPos = nPos;
	    break;
    }

    if (nScrollPos < 0)
	nScrollPos = 0;

    if (nScrollPos > nVertSBLen)
	nScrollPos = nVertSBLen;

    if (nScrollPos == ::GetScrollPos(hwnd, SB_VERT))
	return;

    ::SetScrollPos(hwnd, SB_VERT, nScrollPos, TRUE);
	//InvalidateRect(hwnd, NULL, TRUE);
    ::InvalidateRect(hwnd, NULL, FALSE);
    ::UpdateWindow(hwnd);
	
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}


void OpenMovieFileInit(char *gzFileName)
{
	
	
	InitAvi(gzFileName, MENU_OPEN);

	ResizeToMovie();
		

	if (autoplay)
		PlayMovie(SOUND_MODE);


}

void CPlayplusView::OnZoomResizetomoviesize() 
{

	ResizeToMovie();
	
}

void CPlayplusView::OnZoomQuarter() 
{

	gwZoom = 1;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);

	
}

void CPlayplusView::OnZoomHalf() 
{

	gwZoom = 2;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);

	
}

void CPlayplusView::OnZoom1() 
{

    gwZoom = 4;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);

	
}

void CPlayplusView::OnZoom2() 
{

	gwZoom = 8;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);

	
}

void CPlayplusView::OnZoom4() 
{

	gwZoom = 16;
    FixScrollbars();
    ::InvalidateRect(viewWnd, NULL, TRUE);


	
}

void CPlayplusView::OnUpdateZoom1(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==4);
	
}

void CPlayplusView::OnUpdateZoom2(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==8);
	
}

void CPlayplusView::OnUpdateZoom4(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==16);
	
}

void CPlayplusView::OnUpdateZoomHalf(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==2);
	
}

void CPlayplusView::OnUpdateZoomQuarter(CCmdUI* pCmdUI) 
{

	pCmdUI->SetCheck(gwZoom==1);
	
}

void CPlayplusView::OnUpdateZoomResizetomoviesize(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable(gwZoom==4);
	
}



long GetScrollTime() {

	return playtime;

}


void SetScrollTime(long time)
{
		
	playtime = time;

	
	if (timeLength > 0) {
	
		float durationPlayed = (float) ((time - timeStart)/1000.0);
		CString durationStr;
		durationStr.Format("%8.1f sec",durationPlayed);
		SetTimeIndicator(durationStr);	

	}
	

	int x=0;
	if (timeLength>0)
		x =  ((time - timeStart) * MAXTIMERANGE) / timeLength;
	else
		x = 0;
		

	if (x<0)
		x = 0;

	if (x>MAXTIMERANGE)
		x = MAXTIMERANGE;

	if (sliderCtrlPtr) 
		sliderCtrlPtr->SetPos(x);
	
	
}


void CPlayplusView::OnUpdateFileRewind(CCmdUI* pCmdUI) 
{
	
	BOOL enablebutton = (!gfPlaying) && (!gfRecording);
	pCmdUI->Enable(enablebutton);
	
}


void CPlayplusView::OnUpdateFileLastframe(CCmdUI* pCmdUI) 
{
	
	BOOL enablebutton = (!gfPlaying) && (!gfRecording);
	pCmdUI->Enable(enablebutton);
		
}

void CPlayplusView::OnButtonRecord() 
{
	
	if (!gfRecording) {				
		
		initAudioRecording  =TRUE;
		
	    glRecordStartTimeValue = GetScrollTime();

		PlayMovie(SILENT_MODE);

		fileModified = 1;

	}
	
	
}

void CPlayplusView::OnUpdateRecord(CCmdUI* pCmdUI) 
{
	
	//BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstAudio<0) && (giFirstVideo>=0) && (audioRecordable);
	BOOL enablebutton = (!gfPlaying) && (!gfRecording) && (giFirstVideo>=0) && (audioRecordable);
	pCmdUI->Enable(enablebutton);
	
}



/////////////////////////////////////////////
// Code from Recorder
/////////////////////////////////////////////

/////////////////////////////////////////////
// Merging Module
/////////////////////////////////////////////


/////////////////////////////////////////////
// Merge_Video_And_Sound_File
/////////////////////////////////////////////
int Merge_Video_And_Sound_File(CString input_video_path, CString input_audio_path, CString output_avi_path, BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor) {	    

	PAVISTREAM            AviStream[NUMSTREAMS];      // the editable streams
	AVICOMPRESSOPTIONS    gaAVIOptions[NUMSTREAMS];   // compression options
	LPAVICOMPRESSOPTIONS  galpAVIOptions[NUMSTREAMS];	
   
	PAVIFILE pfileVideo = NULL;
	
	
	AVIFileInit();	    
	
	//Open Video and Audio Files	
	HRESULT hr = AVIFileOpen(&pfileVideo, LPCTSTR(input_video_path), OF_READ | OF_SHARE_DENY_NONE, 0L);
    if (hr != 0)
    {		
		MessageBox(NULL,"Unable to open video file.","Note",MB_OK | MB_ICONEXCLAMATION);
		return 1;
    }
    
        
    
    //Get Video Stream from Video File and Audio Stream from Audio File        
	
	// ==========================================================
	// Important Assumption
    // Assume stream 0 is the correct stream in the files
    // ==========================================================
    if (pfileVideo) {

		PAVISTREAM  pavi;		
	   	if (AVIFileGetStream(pfileVideo, &pavi, streamtypeVIDEO , 0) != AVIERR_OK)
	   	{
			AVIFileRelease(pfileVideo);	   	
	 	  	MessageBox(NULL,"Unable to open video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 1;

    	}
		
		//Set editable stream number as 0
		if (CreateEditableStream(&AviStream[0], pavi) != AVIERR_OK) {
	
			AVIStreamRelease(pavi);
			AVIFileRelease(pfileVideo);
		    MessageBox(NULL,"Unable to create editable video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 1;

		}
		
		AVIStreamRelease(pavi);

		AVIFileRelease(pfileVideo);
		pfileVideo = NULL;	
	
	}

	

	// =============================
	// Getting Audio Stream
	// =============================
	{
		
		PAVISTREAM  pavi;		
		if (AVIStreamOpenFromFile(&pavi,input_audio_path,streamtypeAUDIO,0,OF_READ | OF_SHARE_DENY_NONE,NULL)!=AVIERR_OK)
		{
	   		AVIStreamRelease(AviStream[0]);
 	  		MessageBox(NULL,"Unable to open audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 2;
   		}
			
		//Set editable stream number as 1
		if (CreateEditableStream(&AviStream[1], pavi) != AVIERR_OK) {
		
			AVIStreamRelease(pavi);
			AVIStreamRelease(AviStream[0]);
			MessageBox(NULL,"Unable to create editable audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
			return 2;
		}		
		AVIStreamRelease(pavi);

	}	
	
	

    //Verifying streams are of the correct type
    AVISTREAMINFO     avis[NUMSTREAMS];
       
	AVIStreamInfo(AviStream[0], &avis[0], sizeof(avis[0]));
	AVIStreamInfo(AviStream[1], &avis[1], sizeof(avis[1]));
	
	//Assert that the streams we are going to work with are correct in our assumption
	//such that stream 0 is video and stream 1 is audio
	
	if (avis[0].fccType != streamtypeVIDEO) {
	
		MessageBox(NULL,"Unable to verify video stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 3;	
	
	}
		
	
	if (avis[1].fccType != streamtypeAUDIO) {
	
		MessageBox(NULL,"Unable to verify audio stream.","Note",MB_OK | MB_ICONEXCLAMATION);			
		AVIStreamRelease(AviStream[0]);
		AVIStreamRelease(AviStream[1]);
		return 4;	
	
	}
		
	  
	//
	// AVISaveV code takes a pointer to compression opts
	//
	galpAVIOptions[0] = &gaAVIOptions[0];
	galpAVIOptions[1] = &gaAVIOptions[1];

	//
	// clear options structure to zeroes
	//
	_fmemset(galpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
	_fmemset(galpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

	//=========================================
	//Set Video Stream Compress Options
	//=========================================
	//No Recompression
	galpAVIOptions[0]->fccType = streamtypeVIDEO;
	galpAVIOptions[0]->fccHandler = 0;
	galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
	if (bInterleave) 
		galpAVIOptions[0]->dwFlags = galpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
	galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
	galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
	galpAVIOptions[0]->dwBytesPerSecond = 0;
	galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
	//galpAVIOptions[0]->cbParms = 0;
	//galpAVIOptions[0]->cbFormat = 0;
	
	
	//=========================================
	//Set Audio Stream Compress Options
	//=========================================
	//Recompression may be applied
	//
	//Audio Compress Options seems to be specified by the audio format in avicompressoptions
	galpAVIOptions[1]->fccType = streamtypeAUDIO;
	galpAVIOptions[1]->fccHandler = 0;
	galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
	if (bInterleave) 
		galpAVIOptions[1]->dwFlags = galpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
	galpAVIOptions[1]->dwKeyFrameEvery = 0;
	galpAVIOptions[1]->dwQuality = 0;
	galpAVIOptions[1]->dwBytesPerSecond = 0;
	galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;
	//galpAVIOptions[1]->cbParms = 0;

	if (recompress_audio) {

		galpAVIOptions[1]->cbFormat = audio_format_size;
		galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, audio_format_size); 	
		memcpy( (void *) galpAVIOptions[1]->lpFormat,  (void *) audio_recompress_format, audio_format_size );
		
	}
	else {

	
		LONG lTemp;
		AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
		galpAVIOptions[1]->cbFormat = lTemp;
		
		if (lTemp)  galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);		
		// Use existing format as compress format
		if (galpAVIOptions[1]->lpFormat)     
			AVIStreamReadFormat(AviStream[1],	AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
	

	}

	// ============================
	// Do the work! Merging
	// ============================
			
	//Save 	fccHandlers 
	DWORD fccHandler[NUMSTREAMS]; 
	fccHandler[0] = galpAVIOptions[0]->fccHandler;
	fccHandler[1] = galpAVIOptions[1]->fccHandler;
	
	hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) SaveCallback, NUMSTREAMS, AviStream, galpAVIOptions);	
	//hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);
	if (hr != AVIERR_OK) {

		//Error merging with audio compress options, retry merging with default audio options (no recompression)
		if (recompress_audio) {

			
			AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);
			
			galpAVIOptions[0] = &gaAVIOptions[0];
			galpAVIOptions[1] = &gaAVIOptions[1];
			
			//Resetting Compress Options
			_fmemset(galpAVIOptions[0], 0, sizeof(AVICOMPRESSOPTIONS));
			_fmemset(galpAVIOptions[1], 0, sizeof(AVICOMPRESSOPTIONS));

			galpAVIOptions[0]->fccType = streamtypeVIDEO;
			galpAVIOptions[0]->fccHandler = 0;
			galpAVIOptions[0]->dwFlags = AVICOMPRESSF_VALID | AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
			if (bInterleave) 
				galpAVIOptions[0]->dwFlags = galpAVIOptions[0]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
			galpAVIOptions[0]->dwKeyFrameEvery = (DWORD) -1;
			galpAVIOptions[0]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
			galpAVIOptions[0]->dwBytesPerSecond = 0;
			galpAVIOptions[0]->dwInterleaveEvery = interleave_factor;
			
			galpAVIOptions[1]->fccType = streamtypeAUDIO;
			galpAVIOptions[1]->fccHandler = 0;
			galpAVIOptions[1]->dwFlags = AVICOMPRESSF_VALID;
			if (bInterleave) 
				galpAVIOptions[1]->dwFlags = galpAVIOptions[1]->dwFlags | AVICOMPRESSF_INTERLEAVE; 
			galpAVIOptions[1]->dwKeyFrameEvery = 0;
			galpAVIOptions[1]->dwQuality = 0;
			galpAVIOptions[1]->dwBytesPerSecond = 0;
			galpAVIOptions[1]->dwInterleaveEvery = interleave_factor;	

			//Use default audio format
			LONG lTemp;
			AVIStreamReadFormat(AviStream[1], AVIStreamStart(AviStream[1]), NULL, &lTemp);
			galpAVIOptions[1]->cbFormat = lTemp;			
			if (lTemp)  galpAVIOptions[1]->lpFormat = GlobalAllocPtr(GHND, lTemp);		
			// Use existing format as compress format
			if (galpAVIOptions[1]->lpFormat)     
					AVIStreamReadFormat(AviStream[1],	AVIStreamStart(AviStream[1]),galpAVIOptions[1]->lpFormat, &lTemp);
			
			//Do the Work .... Merging
			hr = AVISaveV(LPCTSTR(output_avi_path),  NULL, (AVISAVECALLBACK) NULL, NUMSTREAMS, AviStream, galpAVIOptions);

			if (hr != AVIERR_OK) {		
			
				AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);						
				AVIStreamRelease(AviStream[0]);
				AVIStreamRelease(AviStream[1]);	    		
				MessageBox(NULL,"Unable to merge audio and video streams (1).","Note",MB_OK | MB_ICONEXCLAMATION);	
				return 5;

			}

			//Succesful Merging, but with no audio recompression
			MessageBox(NULL,"Unable to apply audio compression with the selected options. Your movie is saved without audio compression.","Note",MB_OK | MB_ICONEXCLAMATION);		


		} // if recompress audio retry
		else {
			
			AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);						
			AVIStreamRelease(AviStream[0]);
			AVIStreamRelease(AviStream[1]);	    		
			MessageBox(NULL,"Unable to audio and video merge streams (2).","Note",MB_OK | MB_ICONEXCLAMATION);	
			return 5;

		}
	    	 
	}

		    	    
	// Restore fccHandlers
	galpAVIOptions[0]->fccHandler = fccHandler[0];
	galpAVIOptions[1]->fccHandler = fccHandler[1];
	
	//Set Title Bar
	HWND mainwnd = NULL;
	mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd)
		::SetWindowText(mainwnd,"CamStudio");
    
    
    AVISaveOptionsFree(NUMSTREAMS,galpAVIOptions);		
	
	// Free Editable Avi Streams
	for (int i=0;i<	NUMSTREAMS;i++) {
	
		if (AviStream[i]) {
			AVIStreamRelease(AviStream[i]);
			AviStream[i]=NULL;		
		}
		
	}	
	
	AVIFileExit();
	
	return 0;

}


BOOL CALLBACK SaveCallback(int iProgress)
{
    //Set Progress in Title Bar
	
	char    szText[300];

    wsprintf(szText, "Compressing Audio %d%%", iProgress);

	HWND mainwnd = NULL;
	mainwnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if (mainwnd)
		::SetWindowText(mainwnd, szText);
    
    return WinYield();
	//return FALSE;
}



BOOL WinYield(void)
{

	//Process 3 messages, then return false
	MSG msg;
	for (int i=0;i<3; i++) {

		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}

	return FALSE;	

}



//////////////////////////////////////////////////////////////////
// Output File Module 
// This function is called when the avi saving is completed
//////////////////////////////////////////////////////////////////
LRESULT CPlayplusView::OnUserGeneric	(UINT wParam, LONG lParam) 
{
	
	TimeDependentInsert(tempaudiopath,glRecordStartTimeValue);

	SetRPSLine();


	return 0;

}


void TimeDependentInsert(CString filename,long shiftTime)
{
	int overwriteaudio = NEW_AUDIO_TRACK;
	if (giFirstAudio>=0)
		overwriteaudio = OVERWRITE_AUDIO;
	else
		overwriteaudio = NEW_AUDIO_TRACK;

	//Only the NEW_AUDIO_TRACK selection will cause the resetslider var (FALSE) to have any effect
	if (shiftTime > timeStart)	
		InitAvi((char *) LPCTSTR(filename), MENU_MERGE, shiftTime, KEEPCOUNTER, overwriteaudio,RESET_TO_CURRENT);
	else
		InitAvi((char *) LPCTSTR(filename), MENU_MERGE, -1, KEEPCOUNTER, overwriteaudio,RESET_TO_START);

}


CString GetProgPath()
{
	// locals
	TCHAR    szTemp[300];
	CFile converter;
	CString result;
  
	// get root
	GetModuleFileName( NULL, szTemp, 300 );

	CString path=(CString)szTemp;
	path=path.Left(path.ReverseFind('\\'));
	return path;	

}


CString GetTempPath()
{
	
	char dirx[300];
	GetWindowsDirectory(dirx,300);
	CString tempdir(dirx);
	tempdir = tempdir + "\\temp";			


	//Verify the chosen temp path is valid
	
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof (wfd));	
	HANDLE hdir = FindFirstFile(LPCTSTR(tempdir), &wfd);
	if (!hdir) {
		return GetProgPath();
	}	
	FindClose(hdir);	
	
	//If valid directory, return Windows\temp as temp directory
	if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		return tempdir;
	
	//else return program path as temp directory	
	return GetProgPath();	

}


//===============================================
// AUDIO CODE  
//===============================================
// Ver 1.1
//===============================================

void waveInErrorMsg(MMRESULT result, const char * addstr)
{
	// say error message
	char errorbuffer[300];
	waveInGetErrorText(result, errorbuffer,300);
	//ErrorMsg("WAVEIN:%x:%s %s", result, errorbuffer, addstr);
	CString msgstr;
	msgstr.Format("%s %s",errorbuffer, addstr);
	MessageBox(NULL,msgstr,"Wave in Error",MB_OK | MB_ICONEXCLAMATION);
}

BOOL InitAudioRecording()
{
	m_ThreadID = ::GetCurrentThreadId();
	m_QueuedBuffers = 0;
	m_hRecord = NULL;

	m_BufferSize		= 1000;  // samples per callback

	
	BuildRecordingFormat();

	ClearAudioFile();			
	
	//Create temporary wav file for audio recording
	GetTempWavePath();	
	
	//look up here
	m_pFile = new CSoundFile(tempaudiopath, &m_Format);	
	
	//even if we recording with firstaudio's format, we still get the unable to insert problem!
	//m_pFile = new CSoundFile(tempaudiopath, (LPWAVEFORMATEX) galpAVIOptions[giFirstAudio]->lpFormat);	
		
	
	if  (!(m_pFile && m_pFile->IsOK()))
		MessageBox(NULL,"Error Creating Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
			
	return TRUE;
}


//Initialize the tempaudiopath variable with a valid temporary path
void GetTempWavePath() {

	CString fileName("\\~twav001.wav");
	tempaudiopath = GetTempPath () + fileName;

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified) 
	{
		OFSTRUCT ofstruct;	
		HFILE fhandle = OpenFile( tempaudiopath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );  
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle );
			DeleteFile(tempaudiopath);
		}	 
		else {
			srand( (unsigned)time( NULL ) );
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			CString fxstr("\\~temp");
			CString exstr(".wav");
			tempaudiopath = GetTempPath () + fxstr + cnumstr + exstr;


		}
 
	} 	
	
}


//Using this function affects the silenceFileValid variable
//which may be set in a variety of places

//The silenceFileValid, when TRUE, imply the tempsilencepath is pointing to a compatible silence sound file
BOOL CreateSilenceFile()
{
	silenceFileValid = FALSE;

	BuildRecordingFormat();

	ClearAudioSilenceFile();			
	
	//Create temporary wav file for audio recording
	GetSilenceWavePath();	
	m_pSilenceFile = new CSoundFile(tempsilencepath, &m_Format);	
	
	
	if  (!(m_pSilenceFile && m_pSilenceFile->IsOK())) {
		MessageBox(NULL,"Error Creating Silence Sound File","Note",MB_OK | MB_ICONEXCLAMATION);		
		silenceFileValid = FALSE;
		return silenceFileValid;

	}
	
	CBuffer buf(BasicBufSize, TRUE);
	buf.Erase();
	WriteSilenceFile(&buf) ;

	ClearAudioSilenceFile();

	silenceFileValid = TRUE;
	return silenceFileValid;
}



void ClearAudioSilenceFile()	
{
	if(m_pSilenceFile) {
	
		//will close output file
		delete m_pSilenceFile;
		m_pSilenceFile = NULL;
	}

}


void GetSilenceWavePath() {

	CString fileName("\\~tsil001.wav");
	tempsilencepath = GetTempPath () + fileName;

	//Test the validity of writing to the file
	int fileverified = 0;
	while (!fileverified) 
	{
		OFSTRUCT ofstruct;	
		HFILE fhandle = OpenFile( tempsilencepath, &ofstruct, OF_SHARE_EXCLUSIVE | OF_WRITE  | OF_CREATE );  
		if (fhandle != HFILE_ERROR) {
			fileverified = 1;
			CloseHandle( (HANDLE) fhandle );
			DeleteFile(tempsilencepath);
		}	 
		else {
			
			srand( (unsigned)time( NULL ) );
			int randnum = rand();
			char numstr[50];
			sprintf(numstr,"%d",randnum);

			CString cnumstr(numstr);
			CString fxstr("\\~tsil");
			CString exstr(".wav");
			tempsilencepath = GetTempPath () + fxstr + cnumstr + exstr;
			
			
		}
 
	} 	
	
}




//Delete the m_pFile variable and close existing audio file
void ClearAudioFile()	
{
	if(m_pFile) {
	
		//will close output file
		delete m_pFile;
		m_pFile = NULL;
	}

}

void SetBufferSize(int NumberOfSamples)
{
	m_BufferSize = NumberOfSamples;
}

BOOL StartAudioRecording(WAVEFORMATEX* format)
{
		MMRESULT mmReturn = 0;	
	
		if (format != NULL)
			m_Format = *format;

		// open wavein device 
		mmReturn = ::waveInOpen( &m_hRecord, AudioDeviceID, &m_Format,(DWORD) viewWnd, NULL, CALLBACK_WINDOW  ); //use on message to map.....
						
		
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
			return FALSE;
		}
		else
		{
			// make several input buffers and add them to the input queue
			for(int i=0; i<3; i++)
			{
				AddInputBufferToQueue();
			}
			
			// start recording
			mmReturn = ::waveInStart(m_hRecord);
			if(mmReturn )
			{
				waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
				return FALSE;
			}
			
		}
	
		return TRUE;
}


int AddInputBufferToQueue()
{
	
	MMRESULT mmReturn = 0;
	
	// create the header
	LPWAVEHDR pHdr = new WAVEHDR;
	if(pHdr == NULL) return NULL;
	ZeroMemory(pHdr, sizeof(WAVEHDR));

	// new a buffer
	CBuffer buf(m_Format.nBlockAlign*m_BufferSize, false);
	pHdr->lpData = buf.ptr.c;
	pHdr->dwBufferLength = buf.ByteLen;
	
	// prepare it
	mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
		return m_QueuedBuffers;
	}

	// add the input buffer to the queue
	mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "Error in AddInputBufferToQueue()");
		return m_QueuedBuffers;
	}

	// no error
	// increment the number of waiting buffers
	return m_QueuedBuffers++;
	
	
}



void StopAudioRecording()
{

	MMRESULT mmReturn = MMSYSERR_NOERROR;
	
	mmReturn = ::waveInReset(m_hRecord);
	if(mmReturn)
	{		
		waveInErrorMsg(mmReturn, "in Stop()");
		return;
	}	
	else
	{				


		mmReturn = ::waveInStop(m_hRecord);
		if(mmReturn) waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");
	
		mmReturn = ::waveInClose(m_hRecord);
		if(mmReturn) waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinClose)");
						

	}	
	
	if(m_QueuedBuffers != 0) MessageBox(NULL,"Audio buffers still in queue!","note", MB_OK);
	
	
}


LRESULT CPlayplusView::OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2)
{
		
	MMRESULT mmReturn = 0;
	
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;

	

	mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in OnWIM_DATA() Unprepare Header");
		return 0;
	}

	TRACE("WIM_DATA %4d\n", pHdr->dwBytesRecorded);
	
	if (gfRecording) 	
	{	
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
	
		if (!recordpaused) { //write only if not paused
		
			//Write Data to file
			DataFromSoundIn(&buf);

		}

		// reuse the buffer:
		// prepare it again
		mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "in OnWIM_DATA() Prepare Header");
		}
		else // no error
		{
			// add the input buffer to the queue again
			mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
			if(mmReturn) waveInErrorMsg(mmReturn, "in OnWIM_DATA() Add Buffer");
			else return 0;  // no error
		}
	
	}	

	// we are closing the waveIn handle, 
	// all data must be deleted
	// this buffer was allocated in Start()
	delete pHdr->lpData;
	delete pHdr;
	m_QueuedBuffers--;

	return 0;
	
}


void WriteSilenceFile(CBuffer* buffer) 
{		

	
	if(m_pSilenceFile)
	{

		if(!m_pSilenceFile->Write(buffer))
		{
			
			ClearAudioSilenceFile();
			MessageBox(NULL,"Error Writing Silence Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
			silenceFileValid = FALSE;
		}
		
			
	}
	 

}


void DataFromSoundIn(CBuffer* buffer) 
{		

	
	if(m_pFile)
	{

		if(!m_pFile->Write(buffer))
		{
			//m_SoundIn.Stop();
			StopAudioRecording();
			ClearAudioFile();
			MessageBox(NULL,"Error Writing Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
		}
		
			
	}
	 

}



void BuildRecordingFormat() {

	m_Format.wFormatTag	= WAVE_FORMAT_PCM;		
	m_Format.wBitsPerSample = audio_bits_per_sample;
	m_Format.nSamplesPerSec = audio_samples_per_seconds;
	m_Format.nChannels = audio_num_channels;
	m_Format.nBlockAlign = m_Format.nChannels * (m_Format.wBitsPerSample/8);
	m_Format.nAvgBytesPerSec = m_Format.nSamplesPerSec * m_Format.nBlockAlign;
	m_Format.cbSize = 0;

}


//Suggest Save/Compress Format to pwfx
void SuggestRecordingFormat() {

		
	WAVEINCAPS pwic;
	MMRESULT mmr = waveInGetDevCaps( AudioDeviceID ,  &pwic, sizeof(pwic) ); 

	//Ordered in preference of choice
	if ((pwic.dwFormats) & WAVE_FORMAT_2S16) { 

		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 22050;				
		waveinselected = WAVE_FORMAT_2S16;
		

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_2M08) { 

		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;			
		waveinselected = WAVE_FORMAT_2M08;
	

	}	
	else if ((pwic.dwFormats) & WAVE_FORMAT_2S08)  {

		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 22050;					
		waveinselected = WAVE_FORMAT_2S08;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_2M16) { 

		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;						
		waveinselected = WAVE_FORMAT_2M16;
	
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1M08) {

		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 11025;		
		waveinselected = WAVE_FORMAT_1M08;
		
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1M16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 11025;	
		waveinselected = WAVE_FORMAT_1M16;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1S08) {

		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 11025;				
		waveinselected = WAVE_FORMAT_1S08;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_1S16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 11025;			
		waveinselected = WAVE_FORMAT_1S16;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4M08) { 		

		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 44100;						
		waveinselected = WAVE_FORMAT_4M08;

	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4M16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 1;
		audio_samples_per_seconds = 44100;				
		waveinselected = WAVE_FORMAT_4M16;
		
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4S08) { 

		audio_bits_per_sample = 8;
		audio_num_channels = 2;
		audio_samples_per_seconds = 44100;		
		waveinselected = WAVE_FORMAT_4S08;
		
	}
	else if ((pwic.dwFormats) & WAVE_FORMAT_4S16) {

		audio_bits_per_sample = 16;
		audio_num_channels = 2;
		audio_samples_per_seconds = 44100;			
		waveinselected = WAVE_FORMAT_4S16;
	}
	else {

		//Arbitrarily choose one 
		audio_bits_per_sample = 8;
		audio_num_channels = 1;
		audio_samples_per_seconds = 22050;			
		waveinselected = WAVE_FORMAT_2M08;

	}

	BuildRecordingFormat();
	
}


void SuggestCompressFormat() {

	bAudioCompression = TRUE;

	AllocCompressFormat();		
	MMRESULT mmr;

	  
	//1st try MPEGLAYER3	
	BuildRecordingFormat();		
	if ((m_Format.nSamplesPerSec == 22050) && (m_Format.nChannels==2) && (m_Format.wBitsPerSample <= 16)) {

		pwfx->wFormatTag = WAVE_FORMAT_MPEGLAYER3;
		mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);	

	}	
	
	
	if (mmr!=0) {
	

			//Use PCM in order to handle most cases
			BuildRecordingFormat();
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			MMRESULT mmr = acmFormatSuggest(NULL, &m_Format,  pwfx, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG);


			if (mmr!=0) {

				bAudioCompression = FALSE;

			}


	}

}


void AllocCompressFormat() {

	int initial_audiosetup=1;

	if (pwfx) {

		initial_audiosetup=0;
		//Do nothing....already allocated

	}
	else {

		MMRESULT mmresult = acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cbwfx);
		if (MMSYSERR_NOERROR != mmresult)
		{

			CString msgstr;
			msgstr.Format("Metrics failed mmresult=%u!", mmresult);
            ::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);			
			return ;
		}


		pwfx = (LPWAVEFORMATEX)GlobalAllocPtr(GHND, cbwfx);
		if (NULL == pwfx)
		{

			CString msgstr;
			msgstr.Format("GlobalAllocPtr(%lu) failed!", cbwfx);
            ::MessageBox(NULL,msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);			
			return ;
		}

		initial_audiosetup=1;

	}

}



void SetTimeIndicator(CString timestr)
{
	
	//restore
	//if (statusbarCtrl)
	//	statusbarCtrl->SetPaneText(1,timestr);
		
	//CWnd* mainWindow = AfxGetMainWnd( );
	//((CMainFrame*) mainWindow)->SetTimeIndicator(timestr);

}

void NukeAVIStream(int i)
{
    int j;

    //
    // Make sure it's a real stream number
    //
    if (i < 0 || i >=gcpavi)
	return;

    //
    // Free all the resources associated with this stream
    //
    AVIStreamRelease(gapavi[i]);
    if (galpAVIOptions[i]->lpFormat) {
	GlobalFreePtr(galpAVIOptions[i]->lpFormat);
    }
    if (gapgf[i]) {
	AVIStreamGetFrameClose(gapgf[i]);
	gapgf[i] = NULL;
    }
    if (ghdd[i]) {
	DrawDibClose(ghdd[i]);
	ghdd[i] = 0;
    }

    //
    // Compact the arrays of junk
    //
    for (j = i; j < gcpavi - 1; j++) {
	gapavi[j] = gapavi[j+1];
	galpAVIOptions[j] = galpAVIOptions[j+1];
	gapgf[j] = gapgf[j+1];
	ghdd[j] = ghdd[j+1];
    }

    gcpavi--;
}


LRESULT CPlayplusView::OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2)
{

	//::MessageBox(NULL,"data","wmdata",MB_OK);		
	aviaudioMessage(viewWnd, MM_WOM_DONE, 0, 0);

	return 0; 
}

void CPlayplusView::OnAudioRemoveexistingaudiotracks() 
{
	RemoveExistingAudioTracks();
	fileModified = 1;

}

void CPlayplusView::OnAudioAddaudiofromwavefile() 
{
	// TODO: Add your command handler code here

	char Buffer[BUFSIZE];
	char FileName[BUFSIZE];
	char FileTitle[BUFSIZE];


	OPENFILENAME ofn;
	FileName[0] = 0;	    
	FileTitle[0] = 0;
    
	LoadString( ghInstApp, IDS_OPENWAVE, Buffer, BUFSIZE );	    
		    
	memset(&ofn, 0, sizeof(ofn));
	ofn.lpstrTitle = Buffer;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter = "Audio Wave Files\0*.wav\0\0";
    ofn.lpstrFile = FileName;
	ofn.nMaxFile = sizeof(FileName);		
    ofn.lpstrFileTitle = FileTitle;
    ofn.nMaxFileTitle = sizeof(FileTitle);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		
	// If filename exists 
	if (GetOpenFileName(&ofn)) {
		
		TimeDependentInsert(FileName,-1);
		fileModified = 1;
		
	}
	
}


void RemoveExistingAudioTracks() {

	AVISTREAMINFO     avis;
	 int i;
	for (i = gcpavi - 1; i >= 0; i--) {
		
		AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
		if (avis.fccType==streamtypeAUDIO) {
			
		    NukeAVIStream(i);
		}
	}	
	giFirstAudio = -1;

	RecomputeStreamsTime(RESET_TO_START,0);

}


void AddAudioWaveFile(char  *FileName)
{
	//if merging (ie audio exists).. then we do not use initavi..but overwrite existing...
	InitAvi(FileName, MENU_MERGE);

}

void CPlayplusView::OnUpdateAudioRemoveexistingaudiotracks(CCmdUI* pCmdUI) 
{
	
	BOOL enablebutton = (!gfPlaying) && (giFirstAudio>=0) && (giFirstVideo>=0) && (!gfRecording);
	pCmdUI->Enable(enablebutton);
	
}

void CPlayplusView::OnUpdateAudioAddaudiofromwavefile(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL enablebutton;

	if  (!audioRecordable)
		//allows only the adding (no editing/recording) of audio track from wave file 
		enablebutton = (!gfPlaying) && (giFirstVideo>=0) && (giFirstAudio<0) && (!gfRecording);
	else
		enablebutton = (!gfPlaying) && (giFirstVideo>=0) && (!gfRecording);

	pCmdUI->Enable(enablebutton);
	
}

void CPlayplusView::OnFileSaveas() 
{
	
	OPENFILENAME openfilename;
	gszSaveFileName[0] = 0;


	LoadString( ghInstApp, IDS_SAVETITLE, gszBuffer, BUFSIZE );	
	openfilename.lStructSize = sizeof(OPENFILENAME);	
	openfilename.lpstrTitle = gszBuffer;
	openfilename.hwndOwner = viewWnd;
	openfilename.hInstance = NULL;

		
	//AVIBuildFilter(gszFilter, sizeof(gszFilter), TRUE);
	//openfilename.lpstrFilter = gszFilter;
	openfilename.lpstrFilter = "AVI Movie Files (*.avi)\0*.avi\0\0";	
	openfilename.lpstrCustomFilter = NULL;
	openfilename.nMaxCustFilter = 0;
	openfilename.nFilterIndex = 0;
	openfilename.lpstrFile = gszSaveFileName;
	openfilename.nMaxFile = sizeof(gszSaveFileName);
	openfilename.lpstrFileTitle = NULL;
	openfilename.nMaxFileTitle = 0;
	openfilename.lpstrInitialDir = NULL;		
	openfilename.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
		OFN_OVERWRITEPROMPT ;
	openfilename.nFileOffset = 0;
	openfilename.nFileExtension = 0;
	
	LoadString( ghInstApp, IDS_DEFEXT, gszExtBuffer, BUFSIZE );	
	openfilename.lpstrDefExt = gszExtBuffer;
	openfilename.lCustData = 0;
	openfilename.lpfnHook = NULL;
	openfilename.lpTemplateName = NULL;


	// If we get a filename, save it
	if (GetSaveFileName(&openfilename))
	{
		DWORD       fccHandler[MAXNUMSTREAMS];
		int         i;
		HRESULT     hr;
	
		//StartWait();
	
		for (i = 0; i < gcpavi; i++)
		fccHandler[i] = galpAVIOptions[i]->fccHandler;

		SetAdditionalCompressSettings(bAudioCompression, pwfx, cbwfx,interleaveFrames,interleaveFactor, interleaveUnit);
	
		hr = AVISaveV(gszSaveFileName,
			  NULL,
			  (AVISAVECALLBACK) SaveCallback,
			  gcpavi,
			  gapavi,
			  galpAVIOptions);


		if (hr != AVIERR_OK) {
			switch (hr) {
			case AVIERR_FILEOPEN:
				LoadString( ghInstApp, IDS_ERROVERWRITE, gszBuffer, BUFSIZE );
				ErrMsg(gszBuffer);
			break;
				default:
				LoadString( ghInstApp, IDS_SAVEERROR, gszBuffer, BUFSIZE );
				ErrMsg(gszBuffer);
			}
		}
		else
			fileModified = 0;

		
		// Now put the video compressors back that we stole
		for (i = 0; i < gcpavi; i++)
		galpAVIOptions[i]->fccHandler = fccHandler[i];
	
		//EndWait();
		FixWindowTitle();

		
	}

	
}

void CPlayplusView::OnAudioAudiooptions() 
{
	
	AudioFormat aod;
	aod.DoModal();

	
}



void CPlayplusView::OnZoomTestaudio() 
{

	
}


void Msg(const char fmt[], ...) {
  

  DWORD written;
  char buf[2000];
  va_list val;
  
  va_start(val, fmt);
  wvsprintf(buf, fmt, val);

  const COORD _80x50 = {80,50};
  static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
  WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}

void DumpFormat(WAVEFORMATEX*  pwfx,const char* str)
{

	Msg("\n String %s: wFormatTag %d channels %d nSamplesPerSec %d abytesPerSec %d blockalign %d wBitsPerSample %d cbSize  %d",str,pwfx->wFormatTag,pwfx->nChannels,pwfx->nSamplesPerSec,pwfx->nAvgBytesPerSec,pwfx->nBlockAlign,pwfx->wBitsPerSample,pwfx->cbSize );
	

		

}
	
	
//1)function fails (in streamcut) if stream length is smaller then stream start
//2)editpaste : if pasting at point much further than the end of the stream, the pasting point is set to the end instead rathr than the specified point that is beyond
	//this may cause the cutting to fails...

//The plLength returns the number of bytes *removed* from the original streams
//This value may be *different* from the number of bytes inserted into the stream

//This function allows the lStart to be larger than the existing stream length
//In this case, the stream will be padded with silence
int EditStreamReplace(PAVISTREAM pavi, LONG * plPos,  LONG * plLength, PAVISTREAM pstream, LONG lStart, LONG lLength)
{

	// dumping of format indicates they are the same format
	long cbFormat;
	WAVEFORMATEX *lpFormat, *orgFormat;    
	
	AVIStreamFormatSize(pstream, 0, &cbFormat);
    lpFormat = (WAVEFORMATEX *)GlobalAllocPtr(GHND, cbFormat);
    AVIStreamReadFormat(pstream, 0, lpFormat, &cbFormat);	

	AVIStreamFormatSize(pavi, 0, &cbFormat);
    orgFormat = (WAVEFORMATEX *)GlobalAllocPtr(GHND, cbFormat);
    AVIStreamReadFormat(pavi, 0, orgFormat, &cbFormat);
	
	DumpFormat((WAVEFORMATEX *) galpAVIOptions[giFirstAudio]->lpFormat,"FirstAudio (Existing)");
	DumpFormat((WAVEFORMATEX *) &m_Format,"Working");
	DumpFormat((WAVEFORMATEX *) lpFormat,"To replace");
	DumpFormat((WAVEFORMATEX *) orgFormat,"pavi (Original, supposed to be same as firstaudio");
	
    
	int value = 0;
	
	
	//Add Silence if start point is larger than existing stream length
	long startPos = *plPos;
	long silenceLengthPasted = *plLength;
	if (value = EditStreamPadSilence( pavi,  &startPos,  &silenceLengthPasted))
		return value;
	
	
	if (value = EditStreamPaste(pavi, plPos, plLength, pstream, lStart,lLength) !=  AVIERR_OK)
	{
			ErrMsg("Unable to add audio at insertion point (Stream Replace)");
	}

	
	PAVISTREAM tempStream;
	long cutStartPoint;
	cutStartPoint = *plPos + *plLength;

	
	long lx = AVIStreamLength(pavi);
	if (lx >= (cutStartPoint + *plLength)) {

		//if pavi is long enough to be cut
		//do nothing...

	}
	else {
	
		//if not, try to cut from startpoint to 
		 *plLength  = lx - cutStartPoint - 1;

	}

	
	if  (*plLength>0) {

		if (value = EditStreamCut(pavi, &cutStartPoint, plLength,  &tempStream ) !=  AVIERR_OK)
		{
					ErrMsg("Unable to remove audio at replace point (Stream Replace)");
		}
		AVIStreamRelease(tempStream);



	}
	
	

	return value;

}



//Will pad silence from end of stream to plPos if plPos>length(pavi)
//  before pad> |---org stream-----|                         plPos
//  after  pad> |---org stream-----|_____silence_padded_____|plPos
int EditStreamPadSilence(PAVISTREAM pavi, LONG * plPos,  LONG * plLength)
{

	HRESULT   hr;

	long lz = AVIStreamLength(pavi);
	long startPasteLocation = *plPos;

	
	if (startPasteLocation>=lz) {

		if  (silenceFileValid) {

			long SilencePasteLength = startPasteLocation - (lz-1);

			PAVISTREAM paviSilence;
			PAVIFILE    pfileSilence;
			
			hr = AVIFileOpen(&pfileSilence, tempsilencepath, 0, 0L);

			if (hr != 0)
			{
				ErrMsg("Unable to open silence file");
				return hr;
			}

			if (hr = AVIFileGetStream(pfileSilence, &paviSilence, streamtypeAUDIO , 0) != AVIERR_OK) 
			{
					ErrMsg("Unable to load silence stream");
					return hr; 
			}

			long SilenceStreamLength = AVIStreamLength(paviSilence);
			//long initialStreamEnd = AVIStreamEnd(pavi)+1;
			long initialStreamEnd = AVIStreamEnd(pavi);
			long totalPastedLength = 0;
			while (SilencePasteLength>0) {

				LONG PastedLength;
				LONG lengthToPaste;
				
				if (SilencePasteLength>=SilenceStreamLength)
					lengthToPaste=SilenceStreamLength;   //restricting to ceiling of SilenceStreamLength
				else
					lengthToPaste=SilencePasteLength;

				//CString msgstr;
				//msgstr.Format("initialStreamEnd %ld lengthToPaste %ld SilenceStreamLength %ld totalPastedLength %ld,SilencePasteLength %ld",initialStreamEnd, lengthToPaste, SilenceStreamLength,totalPastedLength,SilencePasteLength);
				//MessageBox(NULL,msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

				if (hr = EditStreamPaste(pavi, &initialStreamEnd, &PastedLength, paviSilence, 0,lengthToPaste) !=  AVIERR_OK)
				{
					ErrMsg("Unable to pad silence to existing stream at position %ld (Stream Replace)",initialStreamEnd);
					return hr;
				}

				totalPastedLength += PastedLength;
				lz = AVIStreamLength(pavi);
				SilencePasteLength = startPasteLocation - (lz-1);

			}


			AVIStreamRelease(paviSilence);
			AVIFileRelease(pfileSilence);		

			*plLength = totalPastedLength;
			*plPos = initialStreamEnd;

		}
		else {
			
			//no silence added
			ErrMsg("Invalid Silence File! No audio [silence] added");
			
			//return -1;

		}

	}

	return 0;

}


//Will insert silence at begining of stream to shift the original stream to start sounding at a later time
int EditStreamSilenceShift(PAVISTREAM pavi, LONG * plPos, LONG * plLength)
{

	HRESULT   hr;

	long lz = AVIStreamLength(pavi);
	long ShiftToLocation = *plPos;
	long ShiftFromLocation = AVIStreamStart(pavi);
		
	if (ShiftToLocation>ShiftFromLocation) {

		if  (silenceFileValid) {

			long SilencePasteLength = ShiftToLocation - ShiftFromLocation;

			PAVISTREAM paviSilence;
			PAVIFILE    pfileSilence;
			
			hr = AVIFileOpen(&pfileSilence, tempsilencepath, 0, 0L);

			if (hr != 0)
			{
				ErrMsg("Unable to open silence file (2) ");
				return hr;
			}

			if (hr = AVIFileGetStream(pfileSilence, &paviSilence, streamtypeAUDIO , 0) != AVIERR_OK) 
			{
					ErrMsg("Unable to load silence stream (2)");
					return hr; 
			}

			long SilenceStreamLength = AVIStreamLength(paviSilence);
			long StreamPastePoint = ShiftFromLocation;
			long totalPastedLength = 0;
			while (SilencePasteLength>0) {

				LONG PastedLength;
				LONG lengthToPaste;
				
				if (SilencePasteLength>=SilenceStreamLength)
					lengthToPaste=SilenceStreamLength;   //restricting to ceiling of SilenceStreamLength
				else
					lengthToPaste=SilencePasteLength;

				//CString msgstr;
				//msgstr.Format("initialStreamEnd %ld lengthToPaste %ld SilenceStreamLength %ld totalPastedLength %ld,SilencePasteLength %ld",initialStreamEnd, lengthToPaste, SilenceStreamLength,totalPastedLength,SilencePasteLength);
				//MessageBox(NULL,msgstr,"Note",MB_OK | MB_ICONEXCLAMATION);

				PastedLength = 0;
				if (hr = EditStreamPaste(pavi, &StreamPastePoint, &PastedLength, paviSilence, 0,lengthToPaste) !=  AVIERR_OK)
				{
					ErrMsg("Unable to pad silence to existing stream at position %ld (2)",StreamPastePoint);
					return hr;
				}

				if (PastedLength<=0) {
					ErrMsg("Unable to pad silence ! Pad Length <= 0 ! (2)");
					return -1;
				}

				totalPastedLength += PastedLength;				
				SilencePasteLength = SilencePasteLength - PastedLength;

			}


			AVIStreamRelease(paviSilence);
			AVIFileRelease(pfileSilence);		

			*plLength = totalPastedLength;
			*plPos = StreamPastePoint;

		}
		else {
			
			//no silence added
			ErrMsg("Invalid Silence File! No audio [silence] added (2)");
			
			//return -1;

		}

	}

	return 0;

}

//Returns a sample that correspond to the lTime of the stream
//The lTime can be *larger* than  the length of the original pavi
long ExAVIStreamTimeToSample( PAVISTREAM pavi, LONG lTime  )
{

	AVISTREAMINFO avis;
	AVIStreamInfo(pavi, &avis, sizeof(avis));

	long sampleStreamStart;
	long sampleStreamEnd;
	long timeStreamStart;
	long timeStreamEnd;
	
	sampleStreamStart = AVIStreamStart( pavi );
	sampleStreamEnd = AVIStreamEnd( pavi );
	timeStreamStart  = AVIStreamStartTime( pavi );
	timeStreamEnd = AVIStreamEndTime( pavi );

	
	long sampleByAVIS = (long) (((double) (avis.dwRate/avis.dwScale)) * ((double) (lTime-timeStreamStart)/1000.0)) + sampleStreamStart;
	return sampleByAVIS;	

}


long SafeStreamTimeToSample( PAVISTREAM pavi, LONG starttime  )
{

	long startsample = 0;
	if (starttime > AVIStreamLengthTime(pavi))
		startsample = ExAVIStreamTimeToSample( pavi,  starttime);
	else
		startsample = AVIStreamTimeToSample( pavi,  starttime);
	

	
	if (startsample <= -1) 
		startsample = 0;

	return startsample;
			
}

// The interleave factor is set * correctly * (i.e adjusted with respect to the interleaveUnit of MILLISECONDS or FRAMES for saving 
// on other occasations, it is assumed to be 1 (and is not used?)
void SetAdditionalCompressSettings(BOOL recompress_audio, LPWAVEFORMATEX audio_recompress_format, DWORD  audio_format_size, BOOL bInterleave, int interleave_factor, int interleave_unit)
{ 	   
	int i=0;
	int frames_per_second = -1;

	for (i = 0; i < gcpavi; i++) {

		//use the firstvideo to calculate the frames per seconds for use in interleave
		if (bInterleave) {

			if (i==giFirstVideo) {

				AVISTREAMINFO avis;
				AVIStreamInfo(gapavi[giFirstVideo], &avis, sizeof(avis));		

				if (avis.dwScale>0) 
					frames_per_second = avis.dwRate/avis.dwScale; 
		
			}
			else {
				//Do nothing	
			}
		}

	}


	
	for (i = 0; i < gcpavi; i++) {

		if (bInterleave) {
			galpAVIOptions[i]->dwFlags = galpAVIOptions[i]->dwFlags | AVICOMPRESSF_INTERLEAVE; 

			if (interleave_unit==FRAMES)
				galpAVIOptions[i]->dwInterleaveEvery = interleave_factor;
			else {


				//Interleave by milliseconds
				if (frames_per_second>0) {

					double interfloat = (((double) interleaveFactor) * ((double) frames_per_second))/1000.0;
					int interint = (int) interfloat;
					if (interint<=0)
						interint = 1;
					galpAVIOptions[i]->dwInterleaveEvery = interint;

				}
				else
					galpAVIOptions[i]->dwInterleaveEvery = interleave_factor;

			}

			
		}
		else {
			

			galpAVIOptions[i]->dwFlags = galpAVIOptions[i]->dwFlags & ~AVICOMPRESSF_INTERLEAVE; 
			galpAVIOptions[i]->dwInterleaveEvery = 1;
		}




		if (recompress_audio) {

			if (i==giFirstAudio) {
			
					if (galpAVIOptions[i]->lpFormat) {
						GlobalFreePtr(galpAVIOptions[i]->lpFormat);
    				}
					galpAVIOptions[i]->cbFormat = audio_format_size;
					galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, audio_format_size); 	
					memcpy( (void *) galpAVIOptions[i]->lpFormat,  (void *) audio_recompress_format, audio_format_size );

				}			
			}
			else {
				//Do nothing	
			}

	}



}




void CPlayplusView::OnUpdateFileSaveas(CCmdUI* pCmdUI) 
{
	
 	pCmdUI->Enable(giFirstVideo>=0);
	
}



//Note : Using test convert will affect the behavior of player
//use an option that can turn test convert off


//Creates an Editable copy of pavi and use it to replace the orignal stream i
void CloneAudioStream_ReplaceStreamPool(int i,PAVISTREAM pavi)
{
    
    if (i < 0 || i >=gcpavi)
	return;	 
   
    //Creates an editable clone of pavi and set it to 
    PAVISTREAM paviConverted;
   	if (CreateEditableStream(&paviConverted, pavi) != AVIERR_OK) {
   	
   		//error
   		ErrMsg("Unable to Create Editable Stream for Converted Audio");
   		
   		//will not affect original stream
   		
   		return;
	
	}
	else {	
				
	    // Free all the resources associated with old stream
	    AVIStreamRelease(gapavi[i]);
	    if (galpAVIOptions[i]->lpFormat) {
			GlobalFreePtr(galpAVIOptions[i]->lpFormat);
	    }
	    if (gapgf[i]) {
			AVIStreamGetFrameClose(gapgf[i]);
			gapgf[i] = NULL;
	    }
	    if (ghdd[i]) {
			DrawDibClose(ghdd[i]);
			ghdd[i] = 0;
	    }
	    
	    
	    gapavi[i]=paviConverted;		
	    galSelStart[i] = galSelLen[i] = -1;
		
	
		ReInitAudioStream(i); 	
	}

    
}


//This function should be called inside initstreams before the line  
//timeLength = timeEnd - timeStart;
void ReInitAudioStream(int i)
{
	
	//if (giFirstAudio>=0)
	//	i=giFirstAudio;

	AVISTREAMINFO avis;
	AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
	galpAVIOptions[i] = &gaAVIOptions[i];
	_fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));	
	galpAVIOptions[i]->fccType = avis.fccType;

    //case streamtypeAUDIO:
	LONG        lTemp;
	galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
	galpAVIOptions[i]->dwInterleaveEvery = 1;
	AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), NULL, &lTemp);
	galpAVIOptions[i]->cbFormat = lTemp;
	if (lTemp)  galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
	
	// Use current format as default format
	if (galpAVIOptions[i]->lpFormat)
	    AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]),galpAVIOptions[i]->lpFormat,&lTemp);


	// We're finding the earliest and latest start and end points for
	// our scrollbar.
	//timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
	//timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));
	
	ghdd[i] =   NULL;
	gapgf[i] = NULL;

	
	//RecomputeStreamsTime(RESET_TO_START,0);


}

void CPlayplusView::OnUpdateFilePlay(CCmdUI* pCmdUI) 
{

	BOOL ev = ((!gfPlaying) && (!gfRecording));
	pCmdUI->Enable(ev);
}

void CPlayplusView::OnAudioExtension() 
{

	if (allowRecordExtension)
		allowRecordExtension = 0;
	else
		allowRecordExtension = 1; 
	
}

void CPlayplusView::OnUpdateAudioExtension(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(!allowRecordExtension);
	
}




