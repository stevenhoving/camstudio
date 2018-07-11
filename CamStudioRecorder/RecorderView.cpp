// RenderSoft CamStudio
//
// Copyright 2001 - 2003 RenderSoft Software & Web Publishing
//
//
// vscapView.cpp : implementation of the CRecorderView class

#include "stdafx.h"
#include "Recorder.h"
#include "RecorderView.h"
#include "RecorderDoc.h"
#include "MainFrm.h"

#include "AutopanSpeedDlg.h"
#include "AutoSearchDlg.h"
#include "FolderDlg.h"
#include "FixedRegionDlg.h"
#include "KeyshortcutsDlg.h"

#include <fmt/format.h>
#include <fmt/printf.h>

#include <CamAudio/sound_file.h>
#include <CamAudio/Buffer.h>
//#include <CamHook/CamHook.h> // for WM_USER_RECORDSTART_MSG
#include <CamLib/CStudioLib.h>
#include <CamLib/TrayIcon.h>
#include <CamEncoder/av_encoder.h>

#include <cam_hook/cam_hook.h>

#include <fmt/time.h>

#include "HotKey.h"
#include "MCI.h"

#include "addons/AnnotationEffectsOptionsDlg.h"
#include "addons/EffectsOptionsDlg.h"
#include "addons/EffectsOptions2Dlg.h"

// new stuff
#include "string_convert.h"
#include "video_settings_ui.h"
#include "cursor_settings_ui.h"
#include "application_settings_ui.h"

#include "settings_model.h"
#include "mouse_capture_ui.h"
#include "window_select_ui.h"


#include <memory>
#include <algorithm>
#include <filesystem>
#include <cassert>

#include <windowsx.h>
#include <fstream>
#include <iostream>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef CAPTUREBLT
#define CAPTUREBLT (DWORD)0x40000000
#endif

/////////////////////////////////////////////////////////////////////////////
// external functions
/////////////////////////////////////////////////////////////////////////////
extern void FreeWaveoutResouces();

extern BOOL useWavein(BOOL, int);
extern BOOL useWaveout(BOOL, int);
extern BOOL WaveoutUninitialize();

extern BOOL onLoadSettings(int iRecordAudio);

/////////////////////////////////////////////////////////////////////////////
// external variables
/////////////////////////////////////////////////////////////////////////////
extern int iRrefreshRate;

/////////////////////////////////////////////////////////////////////////////
// State variables
/////////////////////////////////////////////////////////////////////////////

// Vars used for selecting fixed /variable region
//CRect g_rc;         // Size:  0 .. MaxScreenSize-1
CRect g_rcUse;      // Size:  0 .. MaxScreenSize-1
//CRect g_rcClip;     // Size:  0 .. MaxScreenSize-1


// Autopan
CRect g_rectPanCurrent;
CRect g_rectPanDest;

HWND g_hFixedRegionWnd;

HBITMAP g_hLogoBM = nullptr;

// Misc Vars
bool g_bAlreadyMCIPause = false;
bool g_bRecordState = false;
bool g_bRecordPaused = false;
WPARAM g_interruptkey = 0;
DWORD g_dwInitialTime = 0;
bool g_bInitCapture = false;

unsigned long g_nTotalBytesWrittenSoFar = 0UL;

// Messaging
HWND g_hWndGlobal = nullptr;

// int iTempPathAccess = USE_WINDOWS_TEMP_DIR;
// CString specifieddir;

/////////////////////////////////////////////////////////////////////////////
// Variables/Options requiring interface
/////////////////////////////////////////////////////////////////////////////
int g_iBits = 24;
int g_iDefineMode = 0; // set only in FixedRegion.cpp

int g_selected_compressor = -1;

// Report variables
int g_nActualFrame = 0;
int g_nCurrFrame = 0;
float g_fRate = 0.0;
float g_fActualRate = 0.0;
float g_fTimeLength = 0.0;
int g_nColors = 24;
CString sTimeLength;

// Path to temporary video avi file
std::wstring strTempVideoFilePath;

// Path to temporary audio wav file
std::wstring strTempAudioWavFilePath;

// Ver 1.1
/////////////////////////////////////////////////////////////////////////////
// Audio Functions and Variables
/////////////////////////////////////////////////////////////////////////////
// The program records video and sound separately, into 2 files
// ~temp.avi and ~temp.wav, before merging these 2 file into a single avi file
// using the Merge_Video_And_Sound_File function
/////////////////////////////////////////////////////////////////////////////

HWAVEIN m_hWaveRecord;
DWORD m_ThreadID;
int m_QueuedBuffers = 0;
int iBufferSize = 1000; // number of samples
CSoundFile *g_pSoundFile = nullptr;

// Audio Options Dialog
// LPWAVEFORMATEX pwfx = nullptr;

/////////////////////////////////////////////////////////////////////////////
// ver 1.2
/////////////////////////////////////////////////////////////////////////////
// Key short-cuts variables
/////////////////////////////////////////////////////////////////////////////
// state vars
BOOL g_bAllowNewRecordStartKey = TRUE;

PSTR strFile;

int TroubleShootVal = 0;

int g_keySCOpened = 0;

int iAudioTimeInitiated = 0;
int sdwSamplesPerSec = 22050;
int sdwBytesPerSec = 44100;

int g_iCurrentLayout = 0;

//sProgramOpts cProgramOpts;
//sProducerOpts cProducerOpts;
//sAudioFormat cAudioFormat;
//sHotKeyOpts cHotKeyOpts;
//sRegionOpts cRegionOpts;
//CCamCursor CamCursor;
//sCaptionOpts cCaptionOpts;
//sTimestampOpts cTimestampOpts;
//sWatermarkOpts cWatermarkOpts;

// Files Directory
CString savedir("");

HBITMAP g_hSavedBitmap = nullptr;

/////////////////////////////////////////////////////////////////////////////
// ===========================================================================
// ver 1.2
// ===========================================================================
// Key short-cuts variables
// ===========================================================================

UINT keyRecordStart = VK_F8;
UINT keyRecordEnd = VK_F9;
UINT keyRecordCancel = VK_F10;
// ver 1.8 key shortcuts
UINT keyRecordStartCtrl = 0;
UINT keyRecordEndCtrl = 0;
UINT keyRecordCancelCtrl = 0;

UINT keyRecordStartAlt = 0;
UINT keyRecordEndAlt = 0;
UINT keyRecordCancelAlt = 0;

UINT keyRecordStartShift = 0;
UINT keyRecordEndShift = 0;
UINT keyRecordCancelShift = 0;

UINT keyNext = VK_F11;
UINT keyPrev = VK_F12;
UINT keyShowLayout = 100000; // none

UINT keyNextCtrl = 1;
UINT keyPrevCtrl = 1;
UINT keyShowLayoutCtrl = 0;

UINT keyNextAlt = 0;
UINT keyPrevAlt = 0;
UINT keyShowLayoutAlt = 0;

UINT keyNextShift = 0;
UINT keyPrevShift = 0;
UINT keyShowLayoutShift = 0;

int UnSetHotKeys();
int SetAdjustHotKeys();
int SetHotKeys(int succ[]);
// Region Display Functions
//void DrawSelect(HDC hdc, BOOL fDraw, LPRECT lprClip);

namespace
{ // anonymous

////////////////////////////////
// AUDIO_CODE
////////////////////////////////
void ClearAudioFile();
BOOL InitAudioRecording();
void GetTempAudioWavPath();
// BOOL StartAudioRecording(WAVEFORMATEX* format);
BOOL StartAudioRecording();
void StopAudioRecording();
void waveInErrorMsg(MMRESULT result, const char *);
int AddInputBufferToQueue();

////////////////////////////////
// HOTKEYS_CODE
////////////////////////////////
// bool UnSetHotKeys(HWND hWnd);
// int SetHotKeys(int succ[]);

} // namespace

// Functions that select audio options based on settings read

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// ver 1.8
int UnSetHotKeys()
{
    UnregisterHotKey(g_hWndGlobal, HOTKEY_RECORD_START_OR_PAUSE);
    UnregisterHotKey(g_hWndGlobal, HOTKEY_RECORD_STOP);
    UnregisterHotKey(g_hWndGlobal, HOTKEY_RECORD_CANCELSTOP);
    //UnregisterHotKey(g_hWndGlobal, HOTKEY_LAYOUT_KEY_NEXT);
    //UnregisterHotKey(g_hWndGlobal, HOTKEY_LAYOUT_KEY_PREVIOUS);
    //UnregisterHotKey(g_hWndGlobal, HOTKEY_LAYOUT_SHOW_HIDE_KEY);
    UnregisterHotKey(g_hWndGlobal, HOTKEY_ZOOM);
    UnregisterHotKey(g_hWndGlobal, HOTKEY_AUTOPAN_SHOW_HIDE_KEY);

    return 0;
}

int SetAdjustHotKeys()
{
    int succ[8];
    int ret = SetHotKeys(succ);
    (void)ret; // \note todo: we are ignorning the sethotkeys result... why?

    return 7; // return the max value of #define for Hotkey in program???
}

int SetHotKeys(int succ[])
{
    UnSetHotKeys();

    for (int i = 0; i < 6; i++)
        succ[i] = 0;

    int tstatus = 0;

    BOOL ret;
    int nid = 0;
    if (cHotKeyOpts.m_RecordStart.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_RecordStart.m_fsMod, cHotKeyOpts.m_RecordStart.m_vKey);
        if (!ret)
            succ[0] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_RecordEnd.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_RecordEnd.m_fsMod, cHotKeyOpts.m_RecordEnd.m_vKey);
        if (!ret)
            succ[1] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_RecordCancel.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_RecordCancel.m_fsMod, cHotKeyOpts.m_RecordCancel.m_vKey);
        if (!ret)
            succ[2] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_Next.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_Next.m_fsMod, cHotKeyOpts.m_Next.m_vKey);
        if (!ret)
            succ[3] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_Prev.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_Prev.m_fsMod, cHotKeyOpts.m_Next.m_vKey);
        if (!ret)
            succ[4] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_ShowLayout.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_ShowLayout.m_fsMod, cHotKeyOpts.m_ShowLayout.m_vKey);
        if (!ret)
            succ[5] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_Zoom.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_Zoom.m_fsMod, cHotKeyOpts.m_Zoom.m_vKey);
        if (!ret)
            succ[6] = 1;
    }

    nid++;

    if (cHotKeyOpts.m_Autopan.m_vKey != VK_UNDEFINED)
    {
        ret = RegisterHotKey(g_hWndGlobal, nid, cHotKeyOpts.m_Autopan.m_fsMod, cHotKeyOpts.m_Autopan.m_vKey);
        if (!ret)
            succ[7] = 1;
    }
    return tstatus;
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView

UINT CRecorderView::WM_USER_RECORDINTERRUPTED = ::RegisterWindowMessage(WM_USER_RECORDINTERRUPTED_MSG);
UINT CRecorderView::WM_USER_RECORDPAUSED = ::RegisterWindowMessage(WM_USER_RECORDPAUSED_MSG);
UINT CRecorderView::WM_USER_SAVECURSOR = ::RegisterWindowMessage(WM_USER_SAVECURSOR_MSG);
UINT CRecorderView::WM_USER_GENERIC = ::RegisterWindowMessage(WM_USER_GENERIC_MSG);
UINT CRecorderView::WM_USER_RECORDSTART = ::RegisterWindowMessage(WM_USER_RECORDSTART_MSG);

IMPLEMENT_DYNCREATE(CRecorderView, CView)

BEGIN_MESSAGE_MAP(CRecorderView, CView)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SETFOCUS()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_RECORD, OnRecord)
    ON_UPDATE_COMMAND_UI(ID_RECORD, OnUpdateRecord)
    ON_COMMAND(ID_STOP, OnStop)
    ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
    ON_COMMAND(ID_REGION_RUBBER, OnRegionRubber)
    ON_UPDATE_COMMAND_UI(ID_REGION_RUBBER, OnUpdateRegionRubber)
    ON_COMMAND(ID_REGION_PANREGION, OnRegionPanregion)
    ON_UPDATE_COMMAND_UI(ID_REGION_PANREGION, OnUpdateRegionPanregion)
    ON_COMMAND(ID_OPTIONS_VIDEOOPTIONS, OnVideoSettings)
    ON_COMMAND(ID_OPTIONS_CURSOROPTIONS, OnOptionsCursoroptions)
    ON_COMMAND(ID_REGION_FULLSCREEN, OnRegionFullscreen)
    ON_UPDATE_COMMAND_UI(ID_REGION_FULLSCREEN, OnUpdateRegionFullscreen)
    ON_COMMAND(ID_SCREENS_SELECTSCREEN, OnRegionSelectScreen)
    ON_UPDATE_COMMAND_UI(ID_SCREENS_SELECTSCREEN, OnUpdateRegionSelectScreen)
    ON_COMMAND(ID_SCREENS_ALLSCREENS, OnRegionAllScreens)
    ON_UPDATE_COMMAND_UI(ID_SCREENS_ALLSCREENS, OnUpdateRegionAllScreens)
    ON_COMMAND(ID_PAUSE, OnPause)
    ON_UPDATE_COMMAND_UI(ID_PAUSE, OnUpdatePause)

    ON_COMMAND(ID_REGION_WINDOW, OnRegionWindow)
    ON_UPDATE_COMMAND_UI(ID_REGION_WINDOW, OnUpdateRegionWindow)
    ON_COMMAND(ID_OPTIONS_KEYBOARDSHORTCUTS, OnOptionsKeyboardshortcuts)
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
    ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDSTART, OnRecordStart)
    ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDINTERRUPTED, OnRecordInterrupted)
    ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_RECORDPAUSED, OnRecordPaused)
    ON_REGISTERED_MESSAGE(CRecorderView::WM_USER_GENERIC, OnUserGeneric)
    ON_MESSAGE(WM_HOTKEY, OnHotKey)
    ON_WM_CAPTURECHANGED()
    ON_COMMAND(ID_OPTIONS_PROGRAMSETTINGS, &CRecorderView::OnOptionsProgramsettings)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CRecorderView, CView)
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderView construction/destruction

CRecorderView::CRecorderView()
    : CView()
    , basic_msg_(nullptr)
    , zoom_(1)
    , zoom_direction_(-1) // zoomed out
    , zoom_when_(0)       // FIXME: I hope it is unlikely zoom start at 47 day boundary ever happen by accident
    , zoomed_at_(10, 10)
    , show_message_(true)
{
    mouse_hook_ = std::make_unique<mouse_hook>();

    video_settings_model_ = std::make_unique<video_settings_model>();
    video_settings_model_->load();

    settings_model_ = std::make_unique<settings_model>();
    settings_model_->load();

    // hackery to make sure things are okey
    //const auto capture_rect = settings_model_->get_capture_rect();
    //g_rc = {capture_rect.left(), capture_rect.top(), capture_rect.right(), capture_rect.bottom()};
    //g_rcClip = g_rc;
}

CRecorderView::~CRecorderView()
{
    if (basic_msg_ != nullptr)
    {
        basic_msg_->CloseWindow();
        delete basic_msg_;
        basic_msg_ = nullptr;
    }
}

BOOL CRecorderView::PreCreateWindow(CREATESTRUCT &cs)
{
    // TODO: Modify the Window class or styles here by modifying
    // the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView drawing

void CRecorderView::OnDraw(CDC * /*pDC*/)
{
    //CRecorderDoc *pDoc = GetDocument();
    //ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView printing

BOOL CRecorderView::OnPreparePrinting(CPrintInfo *pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CRecorderView::OnBeginPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CRecorderView::OnEndPrinting(CDC * /*pDC*/, CPrintInfo * /*pInfo*/)
{
    // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderView diagnostics

#ifdef _DEBUG
void CRecorderView::AssertValid() const
{
    CView::AssertValid();
}

void CRecorderView::Dump(CDumpContext &dc) const
{
    CView::Dump(dc);
}

CRecorderDoc *CRecorderView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRecorderDoc)));
    return (CRecorderDoc *)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRecorderView message handlers

void CRecorderView::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // Draw Autopan Info
    // Draw Message msgRecMode
    if (!g_bRecordState)
    {
        DisplayRecordingMsg(dc);
        return;
    }
    // Display the record information when recording
    if (g_bRecordState)
    {
        CRect rectClient;
        GetClientRect(&rectClient);

        // OffScreen Buffer
        CDC dcBits;
        dcBits.CreateCompatibleDC(&dc);
        CBitmap bitmap;

        bitmap.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());
        CBitmap *pOldBitmap = dcBits.SelectObject(&bitmap);

        // Drawing to OffScreen Buffer
        // TRACE("\nRect coords: %d %d %d %d ", rectClient.left, rectClient.top, rectClient.right, rectClient.bottom);

        // CBrush brushBlack;
        // brushBlack.CreateStockObject(BLACK_BRUSH);
        // dcBits.FillRect(&rectClient, &brushBlack);

        DisplayBackground(dcBits);
        DisplayRecordingStatistics(dcBits);
        // OffScreen Buffer
        dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBits, 0, 0, SRCCOPY);
        dcBits.SelectObject(pOldBitmap);
    }

    // ver 2.26
    // Do not call CView::OnPaint() for painting messages
}

int CRecorderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    g_hWndGlobal = m_hWnd;
    //setHotKeyWindow(m_hWnd);

    LoadSettings();
    VERIFY(0 < SetAdjustHotKeys());

    mouse_capture_ui_ = std::make_unique<mouse_capture_ui>(AfxGetInstanceHandle(), GetSafeHwnd(),
        [this](const CRect &capture_rect)
        {
            settings_model_->set_capture_rect(from_crect(capture_rect));
            ::PostMessage(g_hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM)0);
        }
    );

    HDC hScreenDC = ::GetDC(nullptr);
    g_iBits = ::GetDeviceCaps(hScreenDC, BITSPIXEL);
    g_nColors = g_iBits;
    ::ReleaseDC(nullptr, hScreenDC);

    g_hLogoBM = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP3));

    // savedir default
    // We by default save to to user's configured my videos directory.
    // Using GetProgPath instead is not compatible with Vista/Win7 since we have no write permission.
    //savedir = GetMyVideoPath();
    // TRACE("## CRecorderView::OnCreate: initialSaveMMMode Default savedir=GetProgPath()=[%s]!\n", savedir);

    //if (!initialSaveMMMode())
    //{
    //    TRACE("CRecorderView::OnCreate: initialSaveMMMode FAILED!\n");
    //    // return -1;
    //}

    srand((unsigned)time(nullptr));

    return 0;
}

void CRecorderView::OnDestroy()
{
    settings_model_->save();

    // UnSetHotKeys(g_hWndGlobal);
    // getHotKeyMap().clear(); // who actually cares?

    //DestroyShiftWindow();

    if (g_hSavedBitmap)
    {
        DeleteObject(g_hSavedBitmap);
        g_hSavedBitmap = nullptr;
    }

    if (g_hLogoBM)
    {
        DeleteObject(g_hLogoBM);
        g_hLogoBM = nullptr;
    }

    CView::OnDestroy();
}

std::string CRecorderView::generate_temp_filename(video_container::type container)
{
    const auto output_directory = settings_model_->get_application_temp_directory();

    /* \todo fix this... */
    const auto temp_folder = get_temp_folder(dir_access::windows_temp_dir,
        //settings_model_->get_application_temp_directory_access(),
        std::wstring(output_directory.begin(), output_directory.end()));

    time_t osBinaryTime;
    time(&osBinaryTime);
    CTime ctime(osBinaryTime);

    int day = ctime.GetDay();
    int month = ctime.GetMonth();
    int year = ctime.GetYear();
    int hour = ctime.GetHour();
    int minutes = ctime.GetMinute();
    int second = ctime.GetSecond();

    // Create timestamp tag
    auto start_time = fmt::sprintf(_T("%04d%02d%02d_%02d%02d_%02d"), year, month, day, hour, minutes, second);

    // \todo this is wrong
    //cVideoOpts.m_cStartRecordingString = start_time;

    const auto file_extention = video_container::names().at(container);

    strTempVideoFilePath =
        fmt::format(_T("{}\\{}-{}.{}"), temp_folder, _T(TEMPFILETAGINDICATOR), start_time, file_extention);

    // TRACE("## CRecorderView::RecordAVIThread First  Temp.Avi file=[%s]\n", strTempVideoAviFilePath.GetString()  );

    srand(static_cast<unsigned int>(time(nullptr)));
    bool fileverified = false;
    while (!fileverified)
    {
        auto filepath = std::filesystem::path(strTempVideoFilePath);
        if (std::filesystem::exists(filepath))
        {
            fileverified = std::filesystem::remove(filepath);
            if (!fileverified)
            {
                strTempVideoFilePath = fmt::format(_T("{}\\{}-{}-{}.{}"), temp_folder,
                    _T(TEMPFILETAGINDICATOR), start_time, rand(), file_extention);
            }
        }
        else
        {
            fileverified = true;
        }
    }

    return wstring_to_utf8(strTempVideoFilePath);
}



LRESULT CRecorderView::OnRecordStart(WPARAM /*wParam*/, LPARAM lParam)
{
    TRACE("CRecorderView::OnRecordStart\n");
    CStatusBar *pStatus = (CStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    pStatus->SetPaneText(0, _T("Press the Stop Button to stop recording"));

    if (settings_model_->get_application_minimize_on_capture_start())
        ::PostMessage(AfxGetMainWnd()->m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);

    // Check validity of g_rc and fix it
    // FixRectSizePos(&g_rc, g_maxx_screen, g_maxy_screen, g_minx_screen, g_miny_screen);

    // TODO: mouse events handler should be installed only if we are interested in highlighting
    // Shall we empty the click queue? if we stop & start again we have a chance to see previous stuff
    // may be no relevant since will likely expire in normal circumstances.
    // Though if we dump events in file, it is better to clean up
    // FIXME: second parameter is never used
    // We shall wrap all that stuff in class so it is created in constructor and guaranteed to be destroyed in destructor
    //InstallMyHook(g_hWndGlobal, WM_USER_SAVECURSOR);

    g_bRecordState = true;
    g_interruptkey = 0;

    mouse_hook_->attach();

    capture_settings settings;
    settings.capture_hwnd_ = reinterpret_cast<HWND>(lParam);
    settings.capture_rect_ = settings_model_->get_capture_rect();
    settings.video_settings = *video_settings_model_;
    settings.settings = *settings_model_;

    const auto video_container_type = static_cast<video_container::type>(video_settings_model_->video_container_.get_index());
    settings.filename = generate_temp_filename(video_container_type);

    capture_thread_ = std::make_unique<capture_thread>();
    capture_thread_->start(settings);

    // Ver 1.3
    //if (pThread)
    //{
    //    pThread->SetThreadPriority(cProgramOpts.m_iThreadPriority);
    //}

    // Ver 1.2
    g_bAllowNewRecordStartKey = TRUE; // allow this only after g_bRecordState is set to 1
    return 0;
}

LRESULT CRecorderView::OnRecordPaused(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    // TRACE("## CRecorderView::OnRecordPaused\n");
    if (g_bRecordPaused)
    {
        return 0;
    }
    // TRACE("## CRecorderView::OnRecordPaused Tick:[%lu] Call OnPause() now\n", GetTickCount() );
    OnPause();

    return 0;
}

LRESULT CRecorderView::OnRecordInterrupted(WPARAM wParam, LPARAM /*lParam*/)
{
    //UninstallMouseHook(g_hWndGlobal);

    // Ver 1.1
    if (g_bRecordPaused)
    {
        CMainFrame *pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
        pFrame->SetTitle(_T("CamStudio"));
    }

    g_bRecordState = false;
    capture_thread_->stop();
    capture_thread_.reset();

    mouse_hook_->detach();

    // Store the interrupt key in case this function is triggered by a keypress
    g_interruptkey = wParam;

    CStatusBar *pStatus = (CStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    pStatus->SetPaneText(0, _T("Press the Record Button to start recording"));

    Invalidate();

    // ver 1.2
    ::SetForegroundWindow(AfxGetMainWnd()->m_hWnd);
    AfxGetMainWnd()->ShowWindow(SW_RESTORE);

    return 0;
}

void CRecorderView::OnRegionRubber()
{
    settings_model_->set_capture_mode(capture_type::variable);
}

void CRecorderView::OnUpdateRegionRubber(CCmdUI *pCmdUI)
{
    bool is_rubber = settings_model_->get_capture_mode() == capture_type::variable;
    pCmdUI->SetCheck(is_rubber);
}

void CRecorderView::OnRegionPanregion()
{
    CFixedRegionDlg cfrdlg(this, *settings_model_.get());
    if (cfrdlg.DoModal() == IDOK)
    {
        settings_model_->set_capture_mode(capture_type::fixed);
    }
}

void CRecorderView::OnUpdateRegionPanregion(CCmdUI *pCmdUI)
{
    const auto capture_mode = settings_model_->get_capture_mode();
    pCmdUI->SetCheck(capture_mode == capture_type::fixed);
}

void CRecorderView::OnRegionSelectScreen()
{
    settings_model_->set_capture_mode(capture_type::fullscreen);
}

void CRecorderView::OnUpdateRegionSelectScreen(CCmdUI *pCmdUI)
{
    const auto capture_mode = settings_model_->get_capture_mode();
    pCmdUI->SetCheck(capture_mode == capture_type::fullscreen);
}

void CRecorderView::OnRegionFullscreen()
{
    settings_model_->set_capture_mode(capture_type::allscreens);
}

void CRecorderView::OnUpdateRegionFullscreen(CCmdUI *pCmdUI)
{
    if (::GetSystemMetrics(SM_CMONITORS) == 1)
    {
        const auto capture_mode = settings_model_->get_capture_mode();

        // Capture all screens has the same effect as full screen for
        // a system with only one monitor.
        pCmdUI->SetCheck(capture_mode == capture_type::allscreens);
    }
    else
    {
        pCmdUI->m_pMenu->RemoveMenu(ID_REGION_FULLSCREEN, MF_BYCOMMAND);
    }
}

void CRecorderView::OnRegionAllScreens()
{
    settings_model_->set_capture_mode(capture_type::allscreens);
}

void CRecorderView::OnUpdateRegionAllScreens(CCmdUI *pCmdUI)
{
    if (::GetSystemMetrics(SM_CMONITORS) == 1)
    {
        const auto capture_mode = settings_model_->get_capture_mode();
        pCmdUI->SetCheck(capture_mode == capture_type::allscreens);
        pCmdUI->m_pMenu->RemoveMenu(ID_SCREENS_SELECTSCREEN, MF_BYCOMMAND);
        pCmdUI->m_pMenu->RemoveMenu(ID_SCREENS_ALLSCREENS, MF_BYCOMMAND);
    }
    else
    {
        settings_model_->set_capture_mode(capture_type::allscreens);
        //pCmdUI->SetCheck(cRegionOpts.isCaptureMode(CAPTURE_ALLSCREENS));
    }
}

void CRecorderView::restore_window()
{
    ::SetForegroundWindow(AfxGetMainWnd()->m_hWnd);
    AfxGetMainWnd()->ShowWindow(SW_RESTORE);
}

// This function is called when the avi saving is completed
LRESULT CRecorderView::OnUserGeneric(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    restore_window();

    if (g_interruptkey == cHotKeyOpts.m_RecordCancel.m_vKey)
    {
        // recording was canceled, so remove the temp file.
        std::filesystem::remove(strTempVideoFilePath);
        return 0;
    }

    std::filesystem::path target_directory;

    if (std::filesystem::exists(settings_model_->get_application_output_directory()))
    {
        target_directory = settings_model_->get_application_output_directory();
    }
    else
    {
        // \note this is wrong!!
        //strTargetDir = get_temp_folder(settings_model_->get_application_temp_directory_access(),
            //settings_model_->get_application_temp_directory(), true);
        assert(false);
    }

    const auto file_extention = video_settings_model_->get_video_container_file_extention();
    const auto filter = fmt::format(L"Video files (*.{0}|*/{0})", file_extention);
    const auto title = L"Save video file";
    const auto extended_filter = fmt::format(L"*.{}", file_extention);

    CFileDialog file_dialog(FALSE, extended_filter.c_str(), extended_filter.c_str(),
        OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING, filter.c_str(), this);
    file_dialog.m_ofn.lpstrTitle = title;

    // Savedir is a global var and therefor mostly set before.
    if (savedir == "")
    {
        savedir = GetMyVideoPath();
    }

    const auto initial_directory = target_directory.generic_wstring();
    file_dialog.m_ofn.lpstrInitialDir = initial_directory.c_str();

    std::filesystem::path target_filepath;
    if (settings_model_->get_application_auto_filename())
    {
        const auto t = std::time(nullptr);
        const auto tm = fmt::localtime(t);

        // auto filename name format should be configurable.
        target_filepath = std::filesystem::path(settings_model_->get_application_output_directory()) /
            fmt::format("T{:%Y%m%d-%H%M-%s}", tm);
    }
    else if (file_dialog.DoModal() == IDOK)
    {
        const auto file_dialog_filepath = file_dialog.GetPathName();
        target_filepath = std::filesystem::path(file_dialog_filepath.GetString());
    }
    else
    {
        return 0;
    }

    if (!target_filepath.has_extension())
        target_filepath.replace_extension(video_settings_model_->get_video_container_file_extention());

    std::error_code ec;
    std::filesystem::rename(strTempVideoFilePath, target_filepath, ec);
    if (ec)
    {
        // Unable to rename/copy file.
        // In case of an move problem we do nothing. Source has an unique name and not to delete the source file
        // don't cause problems any longer The file may be opened by another application. Please use another
        // filename."
        MessageOut(m_hWnd, IDS_STRING_MOVEFILEFAILURE, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        // Repeat this function until success
        ::PostMessage(g_hWndGlobal, WM_USER_GENERIC, 0, 0);
        return 0;
    }

    return 0;
}

void CRecorderView::OnRecord()
{
    CStatusBar *pStatus = (CStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    pStatus->SetPaneText(0, _T("Press the Stop Button to stop recording"));

    // Version 1.1
    if (g_bRecordPaused)
    {
        g_bRecordPaused = false;
        // ver 1.8
        // if (iRecordAudio==2)
        // mciRecordResume(strTempAudioWavFilePath);

        // Set Title Bar
        CMainFrame *pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
        pFrame->SetTitle(_T("CamStudio"));

        return;
    }
    g_bRecordPaused = false;

    g_nActualFrame = 0;
    g_nCurrFrame = 0;
    g_fRate = 0.0;
    g_fActualRate = 0.0;
    g_fTimeLength = 0.0;

    // r272: How we define rect checked and changed because some pixel got dropped:  100x100 => 99x99
    // g_rcUse is the rect that will be used in the end to define the size of the recording.
    // BTW. g_rcUse is also used to store previous values.
    // A Full screen area is 0, 0 to MaxX-1, MaxY-1

    // TRACE( _T("## CRecorderView::OnRecord /CAPTURE_FIXED/ before / cRegionOpts / T=%d, L=%d, B=.. , R=.. , W=%d, H=%d
    // \n"), cRegionOpts.m_iTop, cRegionOpts.m_iCaptureLeft, cRegionOpts.m_iWidth,
    // cRegionOpts.m_iHeight );

    switch (settings_model_->get_capture_mode())
    {
    case capture_type::fixed:
        // \todo auto panning fixed rect is not supported for now
        ::PostMessage(g_hWndGlobal, WM_USER_RECORDSTART, 0, 0);
    break;
    case capture_type::variable:
    {
        const auto capture_rect = rect<int>();
        mouse_capture_ui_->set_modify_mode(modify_mode::select);
        mouse_capture_ui_->show(capture_rect, capture_type::variable);
    } break;
    case capture_type::allscreens:
    {
        // Applicable when Option region is set as 'Full Screen'
        rect<int> capture_rect(
            g_minx_screen,
            g_miny_screen,
            g_minx_screen + g_maxx_screen,
            g_miny_screen + g_maxy_screen);

        // \todo fix this hack
        settings_model_->set_capture_rect(capture_rect);
        ::PostMessage(g_hWndGlobal, WM_USER_RECORDSTART, 0, 0);
    } break;

    case capture_type::window:
    {
        window_select_ui window_select(this,
            [](const HWND selected_window)
            {
                const unsigned long style = ::GetWindowLong(selected_window, GWL_STYLE);
                if ((style & WS_MINIMIZE) == WS_MINIMIZE)
                    ::ShowWindow(selected_window, SW_SHOWNORMAL);
                ::BringWindowToTop(selected_window);

                ::PostMessage(g_hWndGlobal, WM_USER_RECORDSTART, 0,
                    reinterpret_cast<LPARAM>(selected_window));
            });

        window_select.DoModal();

    } break;
    case capture_type::fullscreen:
        /* \todo rewrite this the full screen function */
        if (basic_msg_)
        {
            delete basic_msg_;
            basic_msg_ = nullptr;
        }

        basic_msg_ = new CBasicMessageDlg();
        basic_msg_->Create(CBasicMessageDlg::IDD);
        basic_msg_->SetText(_T("Click on screen to be captured."));
        basic_msg_->ShowWindow(SW_SHOW);
        SetCapture(); // what is this for?

        break;
    }
}

void CRecorderView::OnStop()
{
    // Version 1.1
    if (!g_bRecordState)
    {
        return;
    }

    if (g_bRecordPaused)
    {
        g_bRecordPaused = false;

        // Set Title Bar
        CMainFrame *pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
        pFrame->SetTitle(_T("CamStudio"));
    }

    OnRecordInterrupted(0, 0);
}

void CRecorderView::OnVideoSettings()
{
    video_settings_ui settings_dialog(this, *video_settings_model_);
    settings_dialog.DoModal();
}

void CRecorderView::OnOptionsCursoroptions()
{
    cursor_settings_ui cursor_settings(this, settings_model_.get());
    cursor_settings.DoModal();
    // just always save the settings model after possible modification.
    settings_model_->save();
}


//void CRecorderView::OnUpdateOptionsAutopan(CCmdUI *pCmdUI)
//{
//    pCmdUI->SetCheck(cProgramOpts.m_bAutoPan);
//}
//
//void CRecorderView::OnOptionsMinimizeonstart()
//{
//    cProgramOpts.m_bMinimizeOnStart = !cProgramOpts.m_bMinimizeOnStart;
//}
//
//void CRecorderView::OnUpdateOptionsMinimizeonstart(CCmdUI *pCmdUI)
//{
//    pCmdUI->SetCheck(cProgramOpts.m_bMinimizeOnStart);
//}
//
//void CRecorderView::OnOptionsHideflashing()
//{
//    cProgramOpts.m_bFlashingRect = !cProgramOpts.m_bFlashingRect;
//}
//
//void CRecorderView::OnUpdateOptionsHideflashing(CCmdUI *pCmdUI)
//{
//    pCmdUI->SetCheck(!cProgramOpts.m_bFlashingRect);
//}
//
//void CRecorderView::OnOptionsProgramoptionsPlayavi()
//{
//    cProgramOpts.m_iLaunchPlayer = (cProgramOpts.m_iLaunchPlayer) ? NO_PLAYER : CAM1_PLAYER;
//}
//
//void CRecorderView::OnUpdateOptionsProgramoptionsPlayavi(CCmdUI *pCmdUI)
//{
//    pCmdUI->SetCheck(cProgramOpts.m_iLaunchPlayer);
//}

void CRecorderView::OnHelpWebsite()
{
    // Openlink("http://www.atomixbuttons.com/vsc");
    // Openlink("http://www.rendersoftware.com");
    // Openlink("http://www.camstudio.org");
}

void CRecorderView::OnHelpHelp()
{
    CString progdir = GetProgPath();
    CString helppath = progdir + "\\help.htm";

    //Openlink(helppath);

    // HtmlHelp( g_hWndGlobal, progdir + "\\help.chm", HH_DISPLAY_INDEX, (DWORD)"CamStudio");
}

void CRecorderView::OnPause()
{
    // TRACE ("## CRecorderView::OnPause BEGIN RecordState:[%d] RecordPaused:[%d]\n", g_bRecordState, g_bRecordPaused);

    // return if not current recording or already in paused state
    if (!g_bRecordState || g_bRecordPaused)
        return;

    g_bRecordPaused = true;
    // TRACE ("## CRecorderView::OnPause  STATE SWITCHED RecordPaused:[%d]\n", g_bRecordPaused);

    // TODO: Set flag that will switch from recording to pause to off if nothing is happening for al very long time.

    // ver 1.8
    // if (iRecordAudio==2)
    // mciRecordPause(strTempAudioWavFilePath);

    CStatusBar *pStatus = (CStatusBar *)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    pStatus->SetPaneText(0, _T("Recording Paused"));

    // Set Title Bar
    CMainFrame *pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
    pFrame->SetTitle(_T("Paused"));
}

void CRecorderView::OnUpdatePause(CCmdUI *pCmdUI)
{
    // Version 1.1
    // pCmdUI->Enable(g_bRecordState && (!g_bRecordPaused));
    pCmdUI->Enable(!g_bRecordPaused);
}

void CRecorderView::OnUpdateStop(CCmdUI * /*pCmdUI*/)
{
    // Version 1.1
    // pCmdUI->Enable(g_bRecordState);
}

void CRecorderView::OnUpdateRecord(CCmdUI *pCmdUI)
{
    // Version 1.1
    pCmdUI->Enable(!g_bRecordState || g_bRecordPaused);
}

void CRecorderView::OnHelpFaq()
{
    // Openlink("http://www.atomixbuttons.com/vsc/page5.html");
    //Openlink("http://www.camstudio.org/faq.htm");
}

#if 0
void CRecorderView::OnOptionsRecordaudio()
{
    if (waveInGetNumDevs() == 0)
    {
        // CString msgstr;
        // msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
        // MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);
        MessageOut(m_hWnd, IDS_STRING_NOINPUT3, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    cAudioFormat.m_iRecordAudio = (!cAudioFormat.isInput(NONE)) ? NONE : MICROPHONE;
}

void CRecorderView::OnUpdateOptionsRecordaudio(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(!cAudioFormat.isInput(NONE));
}

void CRecorderView::OnOptionsAudioformat()
{
    if (waveInGetNumDevs() == 0)
    {
        // CString msgstr;
        // msgstr.Format("Unable to detect audio input device. You need a sound card with microphone input.");
        // MessageBox(msgstr,"Note", MB_OK | MB_ICONEXCLAMATION);

        MessageOut(m_hWnd, IDS_STRING_NOINPUT3, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    CAudioFormatDlg aod(cAudioFormat, this);
    if (IDOK == aod.DoModal())
    {
        // update settings
        cAudioFormat = aod.Format();
    }

    // if (iInterleaveUnit == MILLISECONDS) {
    //    double interfloat = (((double) iInterleaveFactor) * ((double) iFramesPerSecond))/1000.0;
    //    int interint = (int) interfloat;
    //    if (interint<=0)
    //        interint = 1;

    //    CString bstr;
    //    bstr.Format("interleave Unit = %d",interint);
    //    //MessageBox(bstr,"Note",MB_OK);
    //}
}

void CRecorderView::OnOptionsAudiospeakers()
{
    if (waveOutGetNumDevs() == 0)
    {
        MessageOut(m_hWnd, IDS_STRING_NOAUDIOOUTPUT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
    }

    CAudioSpeakersDlg aos;
    aos.DoModal();
}
#endif

void CRecorderView::OnOptionsKeyboardshortcuts()
{
    if (!g_keySCOpened)
    {
        g_keySCOpened = 1;
        CKeyshortcutsDlg kscDlg(this);
        kscDlg.DoModal();
        g_keySCOpened = 0;

        SetAdjustHotKeys();
    }
}

bool CRecorderView::SaveAppSettings()
{
    return false;
}

void CRecorderView::SaveSettings()
{
}


void CRecorderView::LoadSettings()
{
}

void CRecorderView::OnSetFocus(CWnd *pOldWnd)
{
    CView::OnSetFocus(pOldWnd);
}

/////////////////////////////////////////////////////////////////////////////
// OnHotKey WM_HOTKEY message handler
// The WM_HOTKEY message is posted when the user presses a hot key registered
// by the RegisterHotKey function.
//
// wParam - Specifies the identifier of the hot key that generated the message.
// lParam - The low-order word specifies the keys that were to be pressed in
// combination with the key specified by the high-order word to generate the
// WM_HOTKEY message. This word can be one or more of the following values.
//    MOD_ALT - Either ALT key was held down.
//    MOD_CONTROL - Either CTRL key was held down.
//    MOD_SHIFT - Either SHIFT key was held down.
//    MOD_WIN - Either WINDOWS key was held down.
// The high-order word specifies the virtual key code of the hot key.
//
// HOTKEY_RECORD_START_OR_PAUSE        0
// HOTKEY_RECORD_STOP                1
// HOTKEY_RECORD_CANCELSTOP            2
// HOTKEY_LAYOUT_KEY_NEXT            3
// HOTKEY_LAYOUT_KEY_PREVIOUS        4
// HOTKEY_LAYOUT_SHOW_HIDE_KEY        5
// HOTKEY_ZOOM 6
//
/////////////////////////////////////////////////////////////////////////////
LRESULT CRecorderView::OnHotKey(WPARAM wParam, LPARAM /*lParam*/)
{
    switch (wParam)
    {
        case HOTKEY_RECORD_START_OR_PAUSE: // 0 = start recording
            if (g_bRecordState)
            {
                // pause if currently recording
                if (!g_bRecordPaused)
                {
                    OnPause();
                }
                else
                {
                    OnRecord();
                }
            }
            else
            {
                if (g_bAllowNewRecordStartKey)
                {
                    // prevent the case which CamStudio presents more than one region
                    // for the user to select
                    g_bAllowNewRecordStartKey = FALSE;
                    OnRecord();
                }
            }
            break;
        case HOTKEY_RECORD_STOP: // 1
            if (g_bRecordState)
            {
                if (cHotKeyOpts.m_RecordEnd.m_vKey != cHotKeyOpts.m_RecordCancel.m_vKey)
                {
                    OnRecordInterrupted(cHotKeyOpts.m_RecordEnd.m_vKey, 0);
                }
                else
                { // FIXME: something is not quite right here
                    OnRecordInterrupted(cHotKeyOpts.m_RecordCancel.m_vKey + 1, 0);
                }
            }
            break;
        case HOTKEY_RECORD_CANCELSTOP: // 2:
            if (g_bRecordState)
            {
                OnRecordInterrupted(cHotKeyOpts.m_RecordCancel.m_vKey, 0);
            }
            break;
        case HOTKEY_ZOOM: // FIXME: make yet another constant
            if (zoom_when_ == 0)
            {
                if (zoom_ <= 1.0)
                    VERIFY(::GetCursorPos(&zoomed_at_));
                zoom_when_ = ::GetTickCount();
            }
            zoom_direction_ *= -1;
            break;
        case HOTKEY_AUTOPAN_SHOW_HIDE_KEY:
            //OnOptionsAutopan();
            show_message_ = true;
            break;
    }

    return 1;
}

BOOL CRecorderView::OnEraseBkgnd(CDC *pDC)
{
    CMainFrame *pFrame = dynamic_cast<CMainFrame *>(AfxGetMainWnd());
    if (!pFrame)
    {
        return CView::OnEraseBkgnd(pDC);
    }
    CDC dcBits;
    VERIFY(dcBits.CreateCompatibleDC(pDC));
    CBitmap &bitmapLogo = pFrame->Logo();
    BITMAP bitmap;
    bitmapLogo.GetBitmap(&bitmap);

    CRect rectClient;
    GetClientRect(&rectClient);
    CBitmap *pOldBitmap = dcBits.SelectObject(&bitmapLogo);
    // pDC->BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcBits, 0, 0, SRCCOPY);
    pDC->StretchBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBits, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
                    SRCCOPY);
    dcBits.SelectObject(pOldBitmap);

    return TRUE;
}

void CRecorderView::OnRegionWindow()
{
    settings_model_->set_capture_mode(capture_type::window);
}

void CRecorderView::OnUpdateRegionWindow(CCmdUI *pCmdUI)
{
    const auto capture_mode = settings_model_->get_capture_mode();
    pCmdUI->SetCheck(capture_mode == capture_type::window);
}

void CRecorderView::OnCaptureChanged(CWnd *pWnd)
{
    CPoint ptMouse;
    VERIFY(GetCursorPos(&ptMouse));

    const auto capture_mode = settings_model_->get_capture_mode();
    if (capture_mode == capture_type::window)//.isCaptureMode(CAPTURE_WINDOW))
    {
        CWnd *pWndPoint = WindowFromPoint(ptMouse);
        if (pWndPoint)
        {
            // tell windows we don't need capture change events anymore
            ReleaseCapture();

            // store the windows rect into the rect used for capturing
            ::GetWindowRect(pWndPoint->m_hWnd, &g_rcUse);

            // close the window to show user selection was successful
            // post message to start recording
            if (pWndPoint->m_hWnd != basic_msg_->m_hWnd)
                ::PostMessage(g_hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM)0);
        }
    }
    //else if (cRegionOpts.isCaptureMode(CAPTURE_FULLSCREEN))
    else if (capture_mode == capture_type::fullscreen)
    {
        HMONITOR hMonitor = nullptr;
        MONITORINFO mi;

        // get the nearest monitor to the mouse point
        hMonitor = MonitorFromPoint(ptMouse, MONITOR_DEFAULTTONEAREST);
        mi.cbSize = sizeof(mi);
        GetMonitorInfo(hMonitor, &mi);

        // set the rectangle used for recording to the monitor's
        CopyRect(g_rcUse, &mi.rcMonitor);

        // tell windows we don't need capture change events anymore
        ReleaseCapture();

        // post message to start recording
        if (pWnd->m_hWnd != basic_msg_->m_hWnd)
            ::PostMessage(g_hWndGlobal, WM_USER_RECORDSTART, 0, (LPARAM)0);
    }
    CView::OnCaptureChanged(pWnd);
}

void CRecorderView::DisplayRecordingStatistics(CDC &srcDC)
{
    // TRACE("CRecorderView::DisplayRecordingStatistics\n");

    CFont fontANSI;
    fontANSI.CreateStockObject(ANSI_VAR_FONT);
    CFont *pOldFont = srcDC.SelectObject(&fontANSI);

    COLORREF rectcolor = (g_nColors <= 8) ? RGB(255, 255, 255) : RGB(225, 225, 225);
    COLORREF textcolor = (g_nColors <= 8) ? RGB(0, 0, 128) : RGB(0, 0, 100);
    textcolor = RGB(0, 144, 0);

    CBrush brushSolid;
    brushSolid.CreateSolidBrush(rectcolor);
    CBrush *pOldBrush = srcDC.SelectObject(&brushSolid);

    COLORREF oldTextColor = srcDC.SetTextColor(textcolor);
    COLORREF oldBkColor = srcDC.SetBkColor(rectcolor);
    int iOldBkMode = srcDC.SetBkMode(TRANSPARENT);

    CRect rectText;
    GetClientRect(&rectText);
    CSize sizeExtent = (CSize)0;

    CString csMsg;
    int xoffset = 16;
    int yoffset = 10;
    int iLines = 9;       // Number of lines of information to display
    int iLineSpacing = 6; // Distance between two lines
    int iStartPosY = rectText.bottom;

    //////////////////////////////////
    // Prepare information lines
    //////////////////////////////////

    // First line: Start recording
    //csMsg.Format(_T("Start recording : %s"), cVideoOpts.m_cStartRecordingString.c_str());
    sizeExtent = srcDC.GetTextExtent(csMsg);
    iStartPosY -= (iLines * (sizeExtent.cy + iLineSpacing));
    yoffset = iStartPosY;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    //    srcDC.Rectangle(&rectText);                        // Do we want to draw a fancy border around text?
    srcDC.TextOut(xoffset, yoffset, csMsg);

    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line: Current frame
    csMsg.Format(_T("Current Frame : %d"), g_nCurrFrame);
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line: Current file sizing
    const unsigned long MEGABYTE = (1024UL * 1024UL);
    double dMegaBtyes = g_nTotalBytesWrittenSoFar;
    dMegaBtyes /= MEGABYTE;

    csMsg.Format(_T("Current File Size : %.2f Mb"), dMegaBtyes);
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line : Input rate
    csMsg.Format(_T("Actual Input Rate : %.2f fps"), g_fActualRate);
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line : Elapsed time
    // csMsg.Format("Time Elapsed : %.2f sec",  g_fTimeLength);
    csMsg = "Time Elapsed : " + sTimeLength;
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line : Colors info
    csMsg.Format(_T("Number of Colors : %d g_iBits"), g_nColors);
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line : Codex
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    // Line 9 : Dimension,sizing
    csMsg.Format(_T("Dimension : %u X %d"), GetDocument()->FrameWidth(), GetDocument()->FrameHeight());
    sizeExtent = srcDC.GetTextExtent(csMsg);
    yoffset += sizeExtent.cy + iLineSpacing;
    rectText.top = yoffset - 2;
    rectText.bottom = yoffset + sizeExtent.cy + 4;
    srcDC.TextOut(xoffset, yoffset, csMsg);

    //////////////////////////////////
    // Print information lines
    //////////////////////////////////
    srcDC.SelectObject(pOldFont);
    srcDC.SelectObject(pOldBrush);
    srcDC.SetTextColor(oldTextColor);
    srcDC.SetBkColor(oldBkColor);
    srcDC.SetBkMode(iOldBkMode);
}

void CRecorderView::DisplayBackground(CDC &srcDC)
{
    // TRACE("CRecorderView::DisplayBackground\n");
    // Ver 1.1
    if (8 <= g_nColors)
    {
        CRect rectClient;
        GetClientRect(&rectClient);
        CDC dcBits;
        dcBits.CreateCompatibleDC(&srcDC);
        CBitmap bitmapLogo;
        bitmapLogo.Attach(g_hLogoBM);

        BITMAP bitmap;
        bitmapLogo.GetBitmap(&bitmap);
        CBitmap *pOldBitmap = dcBits.SelectObject(&bitmapLogo);

        srcDC.StretchBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcBits, 0, 0, bitmap.bmWidth, bitmap.bmHeight,
                         SRCCOPY);
        dcBits.SelectObject(pOldBitmap);

        bitmapLogo.Detach();
    }
}

void CRecorderView::DisplayRecordingMsg(CDC &srcDC)
{
    // TRACE("CRecorderView::DisplayRecordingMsg\n");

    CPen penSolid;
    penSolid.CreatePen(PS_SOLID, 1, RGB(225, 225, 225));
    CPen *pOldPen = srcDC.SelectObject(&penSolid);

    CBrush brushSolid;
    brushSolid.CreateSolidBrush(RGB(0, 0, 0));
    CBrush *pOldBrush = srcDC.SelectObject(&brushSolid);

    CFont fontANSI;
    fontANSI.CreateStockObject(ANSI_VAR_FONT);
    CFont *pOldFont = srcDC.SelectObject(&fontANSI);

    COLORREF oldTextColor = srcDC.SetTextColor(RGB(225, 225, 225));
    COLORREF oldBkColor = srcDC.SetBkColor(RGB(0, 0, 0));

    CString msgRecMode;
    msgRecMode.LoadString(IDS_RECAVI);

    // msgRecMode.LoadString((cProgramOpts.m_iRecordingMode == ModeAVI) ? IDS_RECAVI : IDS_RECSWF);
    CSize sizeExtent = srcDC.GetTextExtent(msgRecMode);

    CRect rectClient;
    GetClientRect(&rectClient);
    int xoffset = 12;
    int yoffset = 6;
    int xmove = rectClient.Width() - sizeExtent.cx - xoffset;
    int ymove = yoffset;

    CRect rectMode(xmove, ymove, xmove + sizeExtent.cx, ymove + sizeExtent.cy);
    srcDC.Rectangle(&rectMode);
    srcDC.Rectangle(rectMode.left - 3, rectMode.top - 3, rectMode.right + 3, rectMode.bottom + 3);
    srcDC.TextOut(rectMode.left, rectMode.top, msgRecMode);

    srcDC.SelectObject(pOldPen);
    srcDC.SelectObject(pOldBrush);
    srcDC.SelectObject(pOldFont);
    srcDC.SetTextColor(oldTextColor);
    srcDC.SetBkColor(oldBkColor);
}

av_video_meta create_video_config(const int width, const int height, const int fps)
{
    av_video_meta meta;
    meta.quality = 26; // a q factor of 26 should be enough for nice quality
    meta.bpp = 24;
    meta.width = width;
    meta.height = height;
    meta.fps = {fps, 1};
    meta.gop = fps * 10;
    meta.preset = video::preset::ultrafast;
    meta.profile = video::profile::baseline;
    meta.tune = video::tune::zerolatency;
    return meta;
}

std::unique_ptr<av_video> create_video_codec(const av_video_meta &meta)
{
    av_video_codec video_codec_config;

    // The video input pixel format should be automatically detected.
    video_codec_config.pixel_format = AV_PIX_FMT_RGB24;

    return std::make_unique<av_video>(video_codec_config, meta);
}

std::string create_launch_path(const std::string &application, const std::string &arguments)
{
    std::string launch_path = wstring_to_utf8(get_prog_path());
    launch_path += "\\";
    launch_path += application;
    launch_path += " ";
    launch_path += arguments;
    return launch_path;
}

bool CRecorderView::RunViewer(const CString &strNewFile)
{
    // Launch the player

    // open default application for video files.
    //if (Openlink(strNewFile))
    {
    }
    //else
    {
        //MessageBox(L"Error launching avi player!","Note",MB_OK | MB_ICONEXCLAMATION);
        //MessageOut(m_hWnd, IDS_STRING_ERRDEFAULTPLAYER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
    }

    return true;
}

bool CRecorderView::GetRecordState()
{
    return g_bRecordState;
}

bool CRecorderView::GetPausedState()
{
    return g_bRecordPaused;
}

void CRecorderView::DisplayAutopanInfo(CRect rc)
{
#if 0
    if (g_bRecordState)
    {
        CRect rectDraw(rc);
        HDC hdc = ::GetDC(g_hFixedRegionWnd);
        HBRUSH newbrush = (HBRUSH)CreateHatchBrush(HS_CROSS, RGB(0, 0, 0));
        HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, newbrush);
        HFONT newfont;
        newfont = CreateFont(16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, 0, VARIABLE_PITCH, nullptr);
        HFONT oldfont = (HFONT)SelectObject(hdc, newfont);
        CString strmessage;
        //strmessage.LoadString((cProgramOpts.m_bAutoPan == true) ? IDS_STRING_AUTOPAN_ENABLED
                                                                //: IDS_STRING_AUTOPAN_DISABLED);
        // m_Loc.LoadString((cProgramOpts.m_bAutoPan == true ) ? IDS_STRING_AUTOPAN_ENABLED :
        // IDS_STRING_AUTOPAN_DISABLED ); strmessage.Format("%d - %d -%d - %d", rectDraw.left, rectDraw.top,
        // rectDraw.right, rectDraw.bottom);
        SIZE sExtent;
        DWORD dw = GetTextExtentPoint(hdc, (LPCTSTR)strmessage, strmessage.GetLength(), &sExtent);
        VERIFY(0 != dw);
        COLORREF oldtextcolor = SetTextColor(hdc, RGB(255, 255, 255));
        COLORREF oldbkcolor = SetBkColor(hdc, RGB(0, 0, 0));
        int dx = sExtent.cx;
        int dy = sExtent.cy;
        int x = rectDraw.right - dx;
        int y = rectDraw.bottom - dy;
        Rectangle(hdc, x - 3, y - 3, x + dx + 2, y + dy + 1);
        ExtTextOut(hdc, x, y, 0, nullptr, (LPCTSTR)strmessage, strmessage.GetLength(), nullptr);
        SetBkColor(hdc, oldbkcolor);
        SetTextColor(hdc, oldtextcolor);
        SetBkMode(hdc, OPAQUE);
        ::ReleaseDC(g_hFixedRegionWnd, hdc);
        Sleep(500);
    }
#endif
}

namespace
{

#if 0

void DataFromSoundIn(CBuffer *buffer)
{
    if (g_pSoundFile)
    {
        if (!g_pSoundFile->Write(buffer))
        {
            // m_SoundIn.Stop();
            StopAudioRecording();
            ClearAudioFile();

            // MessageBox(nullptr,"Error Writing Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
            MessageOut(nullptr, IDS_STRING_ERRSOUND2, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        }
    }
}

int AddInputBufferToQueue()
{
    // create the header
    // TODO, Possible memory leak, where is the delete operation of the new below done?
    LPWAVEHDR pHdr = new WAVEHDR;

    ZeroMemory(pHdr, sizeof(WAVEHDR));

    // new a buffer
    CBuffer buf(cAudioFormat.AudioFormat().nBlockAlign * iBufferSize, false);
    pHdr->lpData = buf.ptr.c;
    pHdr->dwBufferLength = buf.ByteLen;

    // prepare it
    MMRESULT mmReturn = ::waveInPrepareHeader(m_hWaveRecord, pHdr, sizeof(WAVEHDR));
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
        // todo: leak? did pHdr get deleted?
        return m_QueuedBuffers;
    }

    // add the input buffer to the queue
    mmReturn = ::waveInAddBuffer(m_hWaveRecord, pHdr, sizeof(WAVEHDR));
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in AddInputBufferToQueue()");
        // todo: leak? did pHdr get deleted?
        return m_QueuedBuffers;
    }

    // no error
    // increment the number of waiting buffers
    return m_QueuedBuffers++;
}



//===============================================
// AUDIO CODE
//===============================================

void waveInErrorMsg(MMRESULT result, const char *addstr)
{
    // say error message
    char errorbuffer[500];
    waveInGetErrorTextA(result, errorbuffer, 500);
    CString msgstr;
    msgstr.Format(_T("%s %s"), errorbuffer, addstr);

    CString tstr;
    tstr.LoadString(IDS_STRING_WAVEINERR);
    MessageBox(nullptr, msgstr, tstr, MB_OK | MB_ICONEXCLAMATION);
}

// Delete the g_pSoundFile variable and close existing audio file
void ClearAudioFile()
{
    if (g_pSoundFile)
    {
        delete g_pSoundFile; // will close output file
        g_pSoundFile = nullptr;
    }
}

BOOL InitAudioRecording()
{
    m_ThreadID = ::GetCurrentThreadId();
    m_QueuedBuffers = 0;
    m_hWaveRecord = nullptr;

    iBufferSize = 1000; // samples per callback

    cAudioFormat.BuildRecordingFormat();

    ClearAudioFile();

    // Create temporary wav file for audio recording
    GetTempAudioWavPath();
    // TODO, Possible memory leak, where is the delete operation of the new below done?
    g_pSoundFile = new CSoundFile(wstring_to_utf8(strTempAudioWavFilePath), &cAudioFormat.AudioFormat());

    if (!(g_pSoundFile && g_pSoundFile->IsOK()))
        // MessageBox(nullptr,"Error Creating Sound File","Note",MB_OK | MB_ICONEXCLAMATION);
        MessageOut(nullptr, IDS_STRING_ERRSOUND, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);

    return TRUE;
}


// Initialize the strTempAudioWavFilePath variable with a valid temporary path
void GetTempAudioWavPath()
{
    const auto csTempFolder = get_temp_folder(cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir);

    // CString fileName;
    // fileName.Format("\\%s001.wav", TEMPFILETAGINDICATOR );
    // strTempAudioWavFilePath = GetTempFolder (cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir) +
    // fileName;

    strTempAudioWavFilePath = csTempFolder;
    strTempAudioWavFilePath += _T("\\");
    strTempAudioWavFilePath += _T(TEMPFILETAGINDICATOR);
    strTempAudioWavFilePath += _T("-");
    strTempAudioWavFilePath += cVideoOpts.m_cStartRecordingString;
    strTempAudioWavFilePath += _T(".wav");

    //.Format(_T("%s\\%s-%s.%s"), csTempFolder.c_str(), TEMPFILETAGINDICATOR,
    //                               cVideoOpts.m_cStartRecordingString.c_str(), "wav");

    // Test the validity of writing to the file
    bool fileverified = false;
    while (!fileverified)
    {
        if (std::filesystem::exists(strTempAudioWavFilePath))
        {
            fileverified = std::filesystem::remove(strTempAudioWavFilePath);
            if (!fileverified)
            {
                srand((unsigned)time(nullptr));
                int randnum = rand();

                // CString fxstr;
                // fxstr.Format("\\%s", TEMPFILETAGINDICATOR );
                // CString exstr(".wav");
                // strTempAudioWavFilePath = GetTempFolder (cProgramOpts.m_iTempPathAccess, cProgramOpts.m_strSpecifiedDir)
                // + fxstr + cnumstr + exstr;

                std::wstring filename;
                filename += _T(TEMPFILETAGINDICATOR);
                filename += '-';
                filename += cVideoOpts.m_cStartRecordingString;
                filename += '-';
                filename += std::to_wstring(randnum);
                filename += _T(".wav");

                std::filesystem::path path(csTempFolder);
                path /=filename;

                strTempAudioWavFilePath = path.wstring();

                //.Format(_T("%s\\%s-%s-%s.%s"), csTempFolder.GetString(), TEMPFILETAGINDICATOR,
                  //  cVideoOpts.m_cStartRecordingString.GetString(), numstr.c_str(), _T("wav"));
                assert(false); // \todo fix this

                // MessageBox(nullptr,strTempAudioWavFilePath,"Uses Temp File",MB_OK);
                // fileverified = 1;
                // Try choosing another temporary filename
                //fileverified = true;
            }
        }
        else
        {
            fileverified = true;
        }
    }
}

BOOL StartAudioRecording()
{
    TRACE(_T("StartAudioRecording\n"));

    // open wavein device
    // use on message to map.....
    MMRESULT mmReturn = ::waveInOpen(&m_hWaveRecord, cAudioFormat.m_uDeviceID, &(cAudioFormat.AudioFormat()),
                                     (DWORD_PTR)g_hWndGlobal, 0, CALLBACK_WINDOW);
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
        return FALSE;
    }

    // make several input buffers and add them to the input queue
    for (int i = 0; i < 3; i++)
    {
        AddInputBufferToQueue();
    }

    // start recording
    mmReturn = ::waveInStart(m_hWaveRecord);
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in StartAudioRecording()");
        return FALSE;
    }

    iAudioTimeInitiated = 1;
    sdwSamplesPerSec = cAudioFormat.AudioFormat().nSamplesPerSec;
    sdwBytesPerSec = cAudioFormat.AudioFormat().nAvgBytesPerSec;

    return TRUE;
}

void StopAudioRecording()
{
    MMRESULT mmReturn = ::waveInReset(m_hWaveRecord);
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "in Stop()");
        return;
    }
    Sleep(500);

    mmReturn = ::waveInStop(m_hWaveRecord);
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinStop)");
    }

    mmReturn = ::waveInClose(m_hWaveRecord);
    if (mmReturn)
    {
        waveInErrorMsg(mmReturn, "Error in StopAudioRecording() (WaveinClose)");
    }

    // if (m_QueuedBuffers != 0) ErrorMsg("Still %d buffers in waveIn queue!", m_QueuedBuffers);
    if (m_QueuedBuffers != 0)
    {
        // MessageBox(nullptr,"Audio buffers still in queue!","note", MB_OK);
        MessageOut(nullptr, IDS_STRING_AUDIOBUF, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
    }

    iAudioTimeInitiated = 0;
}
#endif
}

void CRecorderView::OnOptionsProgramsettings()
{
    application_settings_ui settings(this, *settings_model_);
    settings.DoModal();

    // just always force a safe, ignore the model result for now
    settings_model_->save();
}
