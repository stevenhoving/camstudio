#include "stdafx.h"
#include "playplus.h"

#include "MainFrm.h"
#include "playplusDoc.h"
#include "playplusView.h"

#include <filesystem>
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern std::wstring playfiledir;
extern void OpenMovieFileInit(const std::wstring &filename);

#define PLAYER 0
#define DUBBER 1
extern int pmode;

HBITMAP hAboutBM = nullptr;

extern int autoplay;
extern int autoexit;

BEGIN_MESSAGE_MAP(CPlayplusApp, CWinApp)
//{{AFX_MSG_MAP(CPlayplusApp)
ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
// Standard file based document commands
ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
// Standard print setup command
ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


CPlayplusApp::CPlayplusApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

CPlayplusApp theApp;


template<typename T>
T replace_all(T str, const T& from, const T& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.size(), to);
        start_pos += to.size();
    }
    return str;
}

BOOL CPlayplusApp::InitInstance()
{
    // Multilang
    CurLangID = STANDARD_LANGID;
    HKEY hKey;
    LANGID language = 7;
    LONG returnStatus;
    DWORD Type = REG_DWORD;
    DWORD Size = sizeof(language);
    returnStatus = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\CamStudio\\vscap\\Language"), 0L, KEY_ALL_ACCESS, &hKey);

    // create default LanguageID no exists
    if (returnStatus != ERROR_SUCCESS)
    {
        returnStatus = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\CamStudio"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, 0);
        returnStatus = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\CamStudio\\vscap"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, 0);
        returnStatus = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\CamStudio\\vscap\\Language"), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, 0);
        RegSetValueEx(hKey, _T("LanguageID"), 0, REG_DWORD, (BYTE *)&language, sizeof(language));
        returnStatus = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\CamStudio\\vscap\\Language"), 0L, KEY_ALL_ACCESS, &hKey);
    }

    // read LanguageID
    if (returnStatus == ERROR_SUCCESS)
    {
        returnStatus = RegQueryValueEx(hKey, _T("LanguageID"), nullptr, &Type, (LPBYTE)&language, &Size);

        if (returnStatus == ERROR_SUCCESS)
        {
            // load ResDLL
            if (!LoadLangIDDLL(language))
            {
                if (!LoadLangIDDLL(GetUserDefaultLangID()))
                {
                    LoadLangIDDLL(GetSystemDefaultLangID());
                }
            }
        }
    }

    RegCloseKey(hKey);

    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

    /*
    #ifdef _AFXDLL
        Enable3dControls();            // Call this when using MFC in a shared DLL
    #else
        Enable3dControlsStatic();    // Call this when linking to MFC statically
    #endif
    */

    // Change the registry key under which our settings are stored.
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization.
    SetRegistryKey(_T("CamStudioOpenSource for Nick"));

    LoadStdProfileSettings(); // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    CSingleDocTemplate *pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(IDR_MAINFRAME, RUNTIME_CLASS(CPlayplusDoc),
                                          RUNTIME_CLASS(CMainFrame), // main SDI frame window
                                          RUNTIME_CLASS(CPlayplusView));
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    playfiledir[0] = 0;
    if (_tcslen(m_lpCmdLine) != 0)
    {
        if ((m_lpCmdLine[0] == '-') && ((m_lpCmdLine[1] == 'a') || (m_lpCmdLine[1] == 'x')))
        {
            autoplay = 1;
            if (m_lpCmdLine[1] == 'x')
                autoexit = 1;

            int i;
            int lenx = _tcslen(m_lpCmdLine);
            for (i = 2; i < lenx; i++)
            {
                if ((m_lpCmdLine[i] != ' ') && (m_lpCmdLine[i] != '\t'))
                    break;
            }

            if (lenx > 4)
            {
                playfiledir = &m_lpCmdLine[i];
            }
        }
        else
        {
            playfiledir = m_lpCmdLine;
        }

        // Fix to open long filename or filename with quotes on launch
        auto strCleanCmdLineFileName = playfiledir;
        playfiledir = replace_all(strCleanCmdLineFileName, std::wstring(_T("\"")), std::wstring());
        cmdInfo.m_strFileName = playfiledir.c_str();
    }
    /*
    if (strlen(m_lpCmdLine)!=0) {

        strcpy(playfiledir,m_lpCmdLine);

        //Fix to open long filename on launch
        cmdInfo.m_strFileName = playfiledir;
    }
    */

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
    {
        return false;
    }

    // The one and only window has been initialized, so show and update it.
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    return TRUE;
}

BOOL CPlayplusApp::LoadLangIDDLL(LANGID LangID)
{
    HINSTANCE hInstance;
    CString strLangIDDLL;

    // if ( LangID == STANDARD_LANGID )    // integrated language is the right one
    //    return true;

    strLangIDDLL.Format(_T("PlayPlusLANG%.2x.dll"), LangID);

    hInstance = LoadLibrary(strLangIDDLL);
    if (hInstance)
    {
        AfxSetResourceHandle(hInstance);
        CurLangID = LangID;
        return true;
    }
    return false;
}

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
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonlink1();
};

CAboutDlg::CAboutDlg()
    : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT

    if (pmode == PLAYER)
        hAboutBM = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_ABOUT_PLAYER));
    else
        hAboutBM = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_ABOUT_DUBBER));
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTONLINK1, OnBnClickedButtonlink1)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CPlayplusApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPlayplusApp message handlers

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    if (pmode == PLAYER)
        ((CStatic *)(GetDlgItem(IDC_TITLE)))->SetWindowText(_T("RenderSoft CamStudio Player 2.1"));
    else if (pmode == DUBBER)
        ((CStatic *)(GetDlgItem(IDC_TITLE)))->SetWindowText(_T("RenderSoft CamStudio Dubber 1.0"));

    if ((pmode == PLAYER) || (pmode == DUBBER))
    {
        if (hAboutBM)
            ((CStatic *)(GetDlgItem(IDC_IMAGE)))->SetBitmap(hAboutBM);
    }

    return TRUE; // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDestroy()
{
    CDialog::OnDestroy();

    //::MessageBox(nullptr,"Note Destory","nn",MB_OK);

    // TODO: Add your message handler code here
    if (hAboutBM)
    {
        DeleteObject(hAboutBM);
        hAboutBM = nullptr;
    }
}

int CPlayplusApp::ExitInstance()
{
    // TODO: Add your specialized code here and/or call the base class
    // Multilanguage
    if (CurLangID != STANDARD_LANGID)
        FreeLibrary(AfxGetResourceHandle());

    return CWinApp::ExitInstance();
}

void CAboutDlg::OnBnClickedButtonlink1()
{
    LPCTSTR mode;
    mode = _T("open");

    ShellExecute(GetSafeHwnd(), mode, _T("http://www.camstudio.org"), nullptr, nullptr, SW_SHOW);
}