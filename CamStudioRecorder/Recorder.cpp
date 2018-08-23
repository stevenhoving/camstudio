// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing

#include "stdafx.h"
#include "Recorder.h"

#include "MainFrm.h"
#include "RecorderDoc.h"
#include "RecorderView.h"
#include "AboutDlg.h"

#include "GdiPlusInitializer.h"
#include "utility/string_convert.h"
#include "utility/filesystem.h"
#include <CamLib/CStudioLib.h>

#include <filesystem>

static bool bClassRegistered = false;

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp
constexpr auto CAMSTUDIO_MUTEX = _T("VSCAP_CAB648E2_684F_4FF1_B574_9714ACAC6D57");
constexpr auto CAMSTUDIO_CLASS_NAME = L"CamStudio";

BEGIN_MESSAGE_MAP(CRecorderApp, CWinApp)
//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
ON_COMMAND(ID_APP_ABOUT, &CRecorderApp::OnAppAbout)
ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CRecorderApp theApp;

CRecorderApp::CRecorderApp()
    : CWinApp()
{
}

CRecorderApp::~CRecorderApp() = default;

BOOL CRecorderApp::InitInstance()
{
    CWinApp::InitInstance();
    AfxInitRichEdit2();

    // Initialize GDI+.
    m_gdi = std::make_unique<gdi_plus>();

    ::OnError(_T("CRecorderApp::InitInstance"));
    AfxEnableControlContainer();

    const auto app_data_path = utility::get_app_data_path();
    std::filesystem::create_directories(app_data_path);

    if (!FirstInstance())
        return FALSE;

    // new class and exit if it fails
    if (!RegisterWindowClass())
    {
        TRACE("Class Registration Failed\n");
        return FALSE;
    }

#if (WINVER < 0x600) // Windows Vista
//#ifdef _AFXDLL
//    Enable3dControls();            // Call this when using MFC in a shared DLL
//#else
//    Enable3dControlsStatic();    // Call this when linking to MFC statically
//#endif
#endif // Windows Vista

    // the application's document templates. Document templates
    // serve as the connection between documents, frame windows and views.
    AddDocTemplate(new CSingleDocTemplate(IDR_MAINFRAME,
        RUNTIME_CLASS(CRecorderDoc),
        RUNTIME_CLASS(CMainFrame), // main SDI frame window
        RUNTIME_CLASS(CRecorderView)));

    // dummy commandline parser.
    CCommandLineInfo cmd_info;
    ParseCommandLine(cmd_info);
    if (!ProcessShellCommand(cmd_info))
        return FALSE;

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}

int CRecorderApp::ExitInstance()
{
    if (bClassRegistered)
        ::UnregisterClass(CAMSTUDIO_CLASS_NAME, AfxGetInstanceHandle());

    m_gdi.reset();
    return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp commands

// App command to run the dialog
void CRecorderApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// Determine if another window with our class name exists...
// return TRUE if no previous instance running;    FALSE otherwise
BOOL CRecorderApp::FirstInstance()
{
    BOOL bPrevInstance = FALSE;
    m_hAppMutex = ::CreateMutex(0, TRUE, CAMSTUDIO_MUTEX);
    bPrevInstance = (0 != m_hAppMutex);
    if (!bPrevInstance)
    {
        ::OnError(_T("CRecorderApp::FirstInstance"));
        return bPrevInstance; // unable to check mutext, assume previous instance and quit.
    }
    // check last error to see if mutex existed
    bPrevInstance = (ERROR_ALREADY_EXISTS == ::GetLastError());
    if (bPrevInstance)
    {
        // previous instance exists
        ::OnError(_T("CRecorderApp::FirstInstance fails"));
        // todo: activate running instance
        return !bPrevInstance;
    }

    // check older version class name
    CWnd *pWndPrev = CWnd::FindWindow(CAMSTUDIO_CLASS_NAME, nullptr);
    bPrevInstance = (0 != pWndPrev);
    if (bPrevInstance)
    {
        // If iconic, restore the main window
        if (pWndPrev->IsIconic())
        {
            pWndPrev->ShowWindow(SW_RESTORE);
        }

        // Bring the main window or its popup to the foreground
        CWnd *pWndChild = pWndPrev->GetLastActivePopup();
        pWndChild->SetForegroundWindow(); // and we are done activating the previous one.
    }
    TRACE("FirstInstance: %s\n", bPrevInstance ? "false" : "true");
    return !bPrevInstance;
}

// unique application class name that we wish to use
bool CRecorderApp::RegisterWindowClass()
{
    // todo: add mutex code
    WNDCLASS wndcls;
    memset(&wndcls, 0, sizeof(WNDCLASS)); // start with nullptr defaults
    wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = AfxGetInstanceHandle();
    wndcls.hIcon = LoadIcon(IDR_MAINFRAME); // or load a different icon
    wndcls.hCursor = LoadCursor(IDC_ARROW);
    wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndcls.lpszMenuName = nullptr;

    // Specify our own class name for using FindWindow later
    wndcls.lpszClassName = CAMSTUDIO_CLASS_NAME;

    // new class and exit if it fails
    if (!AfxRegisterClass(&wndcls))
    {
        TRACE("Class Registration Failed\n");
        return false;
    }
    bClassRegistered = true;
    return true;
}
