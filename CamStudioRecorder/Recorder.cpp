// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// vscap.cpp : Defines the class behaviors for the application.
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"

#include "MainFrm.h"
#include "RecorderDoc.h"
#include "RecorderView.h"
#include "CamStudioCommandLineInfo.h"
#include "RecorderVersionReleaseInfo.h"
#include "CamCursor.h"
#include "HotKey.h"
#include "GdiPlusInitializer.h"
#include "string_convert.h"
#include <CamLib/CStudioLib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL bClassRegistered = FALSE;
// this global variable is mentioned in StdAfx.h
// thus everyone has an access
// not the best solution though
libconfig::Config *g_cfg;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum
    {
        IDD = IDD_ABOUTBOX
    };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    afx_msg void OnButtonlink();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    // afx_msg void OnBnClickedButtonlink();
    DECLARE_EVENTSINK_MAP()
    afx_msg void OnBnClickedButtonlink2();

public:
    virtual BOOL OnInitDialog();

private:
    CStatic m_ctrlStaticVersion;

public:
    afx_msg void OnStnClickedStaticVersion();
    afx_msg void OnBnClickedButtonlink3();
};

CAboutDlg::CAboutDlg()
    : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
    DDX_Control(pDX, IDC_STATIC_VERSION, m_ctrlStaticVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
ON_BN_CLICKED(IDC_BUTTONLINK, OnButtonlink)
ON_BN_CLICKED(IDC_BUTTONLINK2, OnBnClickedButtonlink2)
//}}AFX_MSG_MAP
ON_STN_CLICKED(IDC_STATIC_VERSION, &CAboutDlg::OnStnClickedStaticVersion)
ON_BN_CLICKED(IDC_BUTTONLINK3, &CAboutDlg::OnBnClickedButtonlink3)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAboutDlg, CDialog)
// ON_EVENT(CAboutDlg, IDC_BUTTONLINK2, DISPID_CLICK, OnBnClickedButtonlink2, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // only needs to be done once.
    CString strBuffer;
    CString strVersionInfo;
    strVersionInfo.Format(_T("v%s\n"), CURRENT_VERSION_NUMBER);
    CString strReleaseInfo;
    strReleaseInfo.Format(_T("Build on Mercurial release: r%s\n"), CURRENT_HG_RELEASE_NUMBER);

    m_ctrlStaticVersion.GetWindowText(strBuffer);
    strBuffer.Replace(_T("<VERSION>"), strVersionInfo);
    strBuffer.Replace(_T("<RELEASE>"), strReleaseInfo);
    strBuffer.Replace(_T("<BUGFIXES>"), _T("Bugfixes and additional features added by Nick the Geek and others.\n"));
    strBuffer.Replace(_T("<COPYRIGHT-1>"), _T("\nCopyright © 2001 RenderSoft Software and Web Publishing\n"));
    strBuffer.Replace(_T("<COPYRIGHT-2>"), _T("Copyright © 2008 CamStudio Group && Contributors\n"));

    m_ctrlStaticVersion.SetWindowText(strBuffer);

    return TRUE; // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnBnClickedButtonlink2()
{
    // \note i'm sorry, this is a personal port...
    // const auto *mode = _T("open");
    // ShellExecute(GetSafeHwnd(), mode, _T("http://www.camstudio.org/donate"), nullptr, nullptr, SW_SHOW);
}

void CAboutDlg::OnButtonlink()
{
    // TODO: Add your control notification handler code here
    //::PostMessage(g_hWndGlobal, WM_COMMAND, ID_CAMSTUDIO4XNOTE_WEBSITE, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp
const TCHAR *CRecorderApp::CAMSTUDIO_MUTEX = _T("VSCAP_CAB648E2_684F_4FF1_B574_9714ACAC6D57");

BEGIN_MESSAGE_MAP(CRecorderApp, CWinApp)
//{{AFX_MSG_MAP(CRecorderApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// NOTE - the ClassWizard will add and remove mapping macros here.
// DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp construction

CRecorderApp::CRecorderApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRecorderApp object

CRecorderApp theApp;

std::string get_config_path()
{
    std::string profile_path;
    profile_path += wstring_to_utf8(GetAppDataPath().GetString());
    profile_path += "\\CamStudio\\CamStudio.cfg";
    return profile_path;
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp initialization

BOOL CRecorderApp::InitInstance()
{
    // Initialize GDI+.
    m_gdi = std::make_unique<gdi>();

    ::OnError(_T("CRecorderApp::InitInstance"));
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need.
    SetRegistryKey(_T("CamStudio"));

    // First free the string allocated by MFC at CWinApp startup.
    // The string is allocated before InitInstance is called.
    free((void *)m_pszProfileName);

    // Change the name of the .INI file.
    // The CWinApp destructor will free the memory.
    //CString strProfile;

    // Only reading, if the user has no file yet, see if a starter file was provided:
    std::string profile_path = get_config_path();

    // \todo fix this..
    m_pszProfileName = (LPCTSTR)_strdup(profile_path.c_str());

    // TODO: re-enable when class complete
    // Read the file. If there is an error, report it and exit.
    g_cfg = new libconfig::Config();
    try
    {
        g_cfg->readFile(profile_path.c_str());
    }
    catch (const libconfig::FileIOException)
    { // TODO: move me to resource
      // MessageBox(nullptr, "CamStudio.cfg Config file was not found. Using defaults.", "Error", MB_OK);
      //        return(EXIT_FAILURE);
    }
    catch (const libconfig::ParseException &pex)
    {
        TCHAR buf[1024];

        // \todo fix
        //_snprintf_s(buf, 1024, _TRUNCATE, "Config file parse error at %s:%d - %s", pex.getFile(), pex.getLine(), pex.getError());
        MessageBox(nullptr, buf, _T("Error"), MB_OK);
    }

    //    m_cmSettings.Read();
    // initialize global values
    //    VERIFY(m_cmSettings.Read(LANGUAGE, m_wCurLangID));

    //    m_wCurLangID = static_cast<LANGID>(GetProfileInt(SEC_SETTINGS, ENT_LANGID, STANDARD_LANGID));
    if (!LoadLanguage(m_wCurLangID))
    {
        if (!LoadLanguage(::GetUserDefaultLangID()))
        {
            LoadLanguage(::GetSystemDefaultLangID());
        }
    }

    // WriteProfileInt(SEC_SETTINGS, ENT_LANGID, m_wCurLangID);
    //    VERIFY(m_cmSettings.Write(LANGUAGE, m_wCurLangID));

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

    // Change the registry key under which our settings are stored.
    // You should modify this string to be something appropriate
    // such as the name of your company or organization.
    // SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    LoadStdProfileSettings(); // Load standard INI file options (including MRU)

    m_iVersionOp = GetOperatingSystem();

    if (g_cfg->exists("Audio"))
        cAudioFormat.Read(g_cfg->lookup("Audio"));
    if (g_cfg->exists("Video"))
        cVideoOpts.Read(g_cfg->lookup("Video"));
    if (g_cfg->exists("Cursor"))
        CamCursor.Read(g_cfg->lookup("Cursor"));
    if (g_cfg->exists("Program"))
        cProgramOpts.Read(g_cfg->lookup("Program"));
    if (g_cfg->exists("HotKeys"))
        cHotKeyOpts.Read(g_cfg->lookup("HotKeys"));
    if (g_cfg->exists("Region"))
        cRegionOpts.Read(g_cfg->lookup("Region"));
    if (g_cfg->exists("Caption"))
        cCaptionOpts.Read(g_cfg->lookup("Caption"));
    if (g_cfg->exists("TimeStamp"))
        cTimestampOpts.Read(g_cfg->lookup("TimeStamp"));
    if (g_cfg->exists("Watermark"))
        cWatermarkOpts.Read(g_cfg->lookup("Watermark"));
    if (g_cfg->exists("Producer"))
        cProducerOpts.Read(g_cfg->lookup("Producer"));

    // the application's document templates. Document templates
    // serve as the connection between documents, frame windows and views.

    CSingleDocTemplate *pDocTemplate;
    // TODO, Possible memory leak, where is the delete operation of the new below done?
    pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME, RUNTIME_CLASS(CRecorderDoc),
                                          RUNTIME_CLASS(CMainFrame), // main SDI frame window
                                          RUNTIME_CLASS(CRecorderView));
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    // CCamStudioCommandLineInfo cmdInfo;
    ParseCommandLine(m_cmdInfo);

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(m_cmdInfo))
        return FALSE;

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    if (cProgramOpts.m_iViewType != VIEW_NORMAL)
    {
        CMainFrame *cwind = (CMainFrame *)AfxGetMainWnd();
        cwind->UpdateViewtype();
    }
    return TRUE;
}

int CRecorderApp::ExitInstance()
{
    try
    {
        libconfig::Setting *s;
        if (!g_cfg->exists("Audio"))
            s = &g_cfg->getRoot().add("Audio", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Audio");
        cAudioFormat.Write(*s);

        if (!g_cfg->exists("Video"))
            s = &g_cfg->getRoot().add("Video", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Video");
        cVideoOpts.Write(*s);

        if (!g_cfg->exists("Cursor"))
            s = &g_cfg->getRoot().add("Cursor", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Cursor");
        CamCursor.Write(*s);

        if (!g_cfg->exists("Program"))
            s = &g_cfg->getRoot().add("Program", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Program");
        cProgramOpts.Write(*s);

        if (!g_cfg->exists("HotKeys"))
            s = &g_cfg->getRoot().add("HotKeys", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("HotKeys");
        cHotKeyOpts.Write(*s);

        if (!g_cfg->exists("Region"))
            s = &g_cfg->getRoot().add("Region", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Region");
        cRegionOpts.Write(*s);

        if (!g_cfg->exists("Caption"))
            s = &g_cfg->getRoot().add("Caption", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Caption");
        cCaptionOpts.Write(*s);

        if (!g_cfg->exists("TimeStamp"))
            s = &g_cfg->getRoot().add("TimeStamp", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("TimeStamp");
        cTimestampOpts.Write(*s);

        if (!g_cfg->exists("Watermark"))
            s = &g_cfg->getRoot().add("Watermark", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Watermark");
        cWatermarkOpts.Write(*s);

        if (!g_cfg->exists("Producer"))
            s = &g_cfg->getRoot().add("Producer", libconfig::Setting::TypeGroup);
        else
            s = &g_cfg->lookup("Producer");
        cProducerOpts.Write(*s);
    }
    catch (libconfig::SettingTypeException &e)
    {
        const auto path = utf8_to_wstring(e.getPath());
        const auto what = utf8_to_wstring(e.what());
        MessageBox(nullptr, path.c_str(), what.c_str(), MB_OK);
    }

    // Save the configuration file out to the user appdata directory.
    std::string profile_path = get_config_path();
    g_cfg->writeFile(profile_path.c_str());
    delete g_cfg;

    // Multilanguage
    if (m_wCurLangID != STANDARD_LANGID)
        ::FreeLibrary(AfxGetResourceHandle());

    if (bClassRegistered)
        ::UnregisterClass(_T("CamStudio"), AfxGetInstanceHandle());

    // Gdiplus::GdiplusShutdown(gdiplusToken);
    // gdi_shutdown(gdiplusToken);
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
    CWnd *pWndPrev = CWnd::FindWindow(_T("CamStudio"), nullptr);
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

bool CRecorderApp::LoadLanguage(LANGID LangID)
{
    // integrated language is the right one
    bool bResult = (LangID == STANDARD_LANGID);
    if (bResult)
    {
        ASSERT(m_wCurLangID == LangID);
        TRACE("m_wCurLangID: %d\n", m_wCurLangID);
        return true;
    }

    CString strLangIDDLL;
    strLangIDDLL.Format(_T("RecorderLANG%.2x.dll"), LangID);
    HINSTANCE hInstance = ::LoadLibrary(strLangIDDLL);
    bResult = (0 != hInstance);
    if (bResult)
    {
        AfxSetResourceHandle(hInstance);
        m_wCurLangID = LangID;
    }

    TRACE("m_wCurLangID: %d %s\n", m_wCurLangID, bResult ? "loaded" : "failed");
    return bResult;
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
    wndcls.lpszClassName = _T("CamStudio");

    // new class and exit if it fails
    if (!AfxRegisterClass(&wndcls))
    {
        TRACE("Class Registration Failed\n");
        return false;
    }
    return true;
}

void CAboutDlg::OnStnClickedStaticVersion()
{
    // TODO: Add your control notification handler code here
}

void CAboutDlg::OnBnClickedButtonlink3()
{
    // TODO: Add your control notification handler code here
    ::PostMessage(g_hWndGlobal, WM_COMMAND, ID_HELP_WEBSITE, 0);
}
