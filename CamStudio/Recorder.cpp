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

#include <strsafe.h>		// for StringCchPrintf
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND hWndGlobal;
static BOOL bClassRegistered = FALSE;

/////////////////////////////////////////////////////////////////////////////
// OnError
// Windows Error handler
// Gets the last windows error and then resets the error; gets the string
// associated with the error and displays a messagebox of the error
/////////////////////////////////////////////////////////////////////////////
void OnError(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code
	DWORD dwError = ::GetLastError();
	if (ERROR_SUCCESS == dwError) {
		return;
	}
	TRACE("OnError: %s: %ud\n", lpszFunction, dwError);
	::SetLastError(ERROR_SUCCESS);	// reset the error

	LPVOID lpMsgBuf = 0;
	DWORD dwLen = ::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	if (0 == dwLen) {
		TRACE("OnError: FormatMessage error: %ud\n", ::GetLastError());
		::SetLastError(ERROR_SUCCESS);	// reset the error
		return;
	}
	// Display the error message and exit the process
	LPVOID lpDisplayBuf = (LPVOID)::LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	if (!lpDisplayBuf) {
		TRACE("OnError: LocalAlloc error: %ud\n", ::GetLastError());
		::SetLastError(ERROR_SUCCESS);	// reset the error
	}
	HRESULT hr = StringCchPrintf((LPTSTR)lpDisplayBuf, ::LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"), lpszFunction, dwError, lpMsgBuf);
	if (SUCCEEDED(hr)) {
		::MessageBox(0, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);
	} else {
		TRACE("OnError: StringCchPrintf error: %ud\n", ::GetLastError());
		::SetLastError(ERROR_SUCCESS);	// reset the error
	}

	::LocalFree(lpMsgBuf);
	::LocalFree(lpDisplayBuf);
}

void ErrMsg(char frmt[], ...)
{
	DWORD written;
	char buf[5000];
	va_list val;

	va_start(val, frmt);
	wvsprintf(buf, frmt, val);

	const COORD _80x50 = {80,50};
	static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}

int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus)
{
	CString tstr("");
	CString mstr("");
	tstr.LoadString(strTitle);
	mstr.LoadString(strMsg);

	return ::MessageBox(hWnd,mstr,tstr,mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val)
{
	CString tstr("");
	CString mstr("");
	CString fstr("");
	tstr.LoadString(strTitle);
	mstr.LoadString(strMsg);
	fstr.Format(mstr,val);

	return ::MessageBox(hWnd,fstr,tstr,mbstatus);
}

int MessageOut(HWND hWnd, long strMsg, long strTitle, UINT mbstatus, long val1, long val2)
{
	CString tstr("");
	CString mstr("");
	CString fstr("");
	tstr.LoadString(strTitle);
	mstr.LoadString(strMsg);
	fstr.Format(mstr,val1,val2);

	return ::MessageBox(hWnd,fstr,tstr,mbstatus);
}

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp
const char * CRecorderApp::CAMSTUDIO_MUTEX = _T("VSCAP_CAB648E2_684F_4FF1_B574_9714ACAC6D57");

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

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp initialization

BOOL CRecorderApp::InitInstance()
{
	OnError("CRecorderApp::InitInstance");
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.
	SetRegistryKey(_T("CamStudioOpenSource for Nick"));

	//First free the string allocated by MFC at CWinApp startup.
	//The string is allocated before InitInstance is called.
	free((void*)m_pszProfileName);
	// Change the name of the .INI file.
	// The CWinApp destructor will free the memory.
	m_pszProfileName = _tcsdup(_T(".\\CamStudio.ini"));

	CurLangID = static_cast<LANGID>(GetProfileInt(SEC_SETTINGS, ENT_LANGID, STANDARD_LANGID));
	if (!LoadLanguage(CurLangID)) {
		if (!LoadLanguage(::GetUserDefaultLangID())) {
			LoadLanguage(::GetSystemDefaultLangID());
		}
	}
	WriteProfileInt(SEC_SETTINGS, ENT_LANGID, CurLangID);

	if (!FirstInstance())
		return FALSE;

	// Register new class and exit if it fails
	if (!RegisterWindowClass()) {
		TRACE("Class Registration Failed\n");
		return FALSE;
	}

#if (WINVER < 0x600)	// Windows Vista
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif	// Windows Vista

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	// SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(); // Load standard INI file options (including MRU)

	// Register the application's document templates. Document templates
	// serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRecorderDoc),
		RUNTIME_CLASS(CMainFrame), // main SDI frame window
		RUNTIME_CLASS(CRecorderView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	//CCamStudioCommandLineInfo cmdInfo;
	ParseCommandLine(m_cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(m_cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnButtonlink();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedButtonlink();
	DECLARE_EVENTSINK_MAP()
	afx_msg void OnBnClickedButtonlink2();
private:
	CStatic m_ctrlStaticVersion;
	CString m_strVersion;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strVersion(_T("Version 2.6"))
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_STATIC_VERSION, m_ctrlStaticVersion);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_strVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BUTTONLINK, OnButtonlink)
	ON_BN_CLICKED(IDC_BUTTONLINK2, OnBnClickedButtonlink2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAboutDlg, CDialog)
	// ON_EVENT(CAboutDlg, IDC_BUTTONLINK2, DISPID_CLICK, OnBnClickedButtonlink2, VTS_NONE)
END_EVENTSINK_MAP()

void CAboutDlg::OnBnClickedButtonlink2()
{
	LPCTSTR mode = ("open");
	ShellExecute (GetSafeHwnd (), mode, "http://www.camstudio.org/donate", NULL, NULL, SW_SHOW);
}

void CAboutDlg::OnButtonlink()
{
	// TODO: Add your control notification handler code here
	::PostMessage(hWndGlobal,WM_COMMAND,ID_HELP_WEBSITE,0);
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
// return TRUE if no previous instance running;	FALSE otherwise
BOOL CRecorderApp::FirstInstance()
{
	BOOL bPrevInstance = FALSE;
	m_hAppMutex = ::CreateMutex(0, TRUE, CAMSTUDIO_MUTEX);
	bPrevInstance = (0 != m_hAppMutex);
	if (!bPrevInstance) {
		OnError(_T("CRecorderApp::FirstInstance"));
		return bPrevInstance;	// unable to check mutext, assume previous instance and quit.
	}
	// check last error to see if mutex existed
	bPrevInstance = (ERROR_ALREADY_EXISTS == ::GetLastError());
	if (bPrevInstance) {
		// previous instance exists
		OnError(_T("CRecorderApp::FirstInstance fails"));
		// todo: activate running instance
		return !bPrevInstance;
	}

	// check older version class name
	CWnd *pWndPrev = CWnd::FindWindow(_T("CamStudio"), NULL);
	bPrevInstance = (0 != pWndPrev);
	if (bPrevInstance) {
		// If iconic, restore the main window
		if (pWndPrev->IsIconic()) {
			pWndPrev->ShowWindow(SW_RESTORE);
		}

		// Bring the main window or its popup to the foreground
		CWnd *pWndChild = pWndPrev->GetLastActivePopup();
		pWndChild->SetForegroundWindow();	// and we are done activating the previous one.
	}
	TRACE("FirstInstance: %s\n", bPrevInstance ? "false" : "true");
	return !bPrevInstance;
}

int CRecorderApp::ExitInstance()
{
	//Multilanguage
	if (CurLangID != STANDARD_LANGID)
		FreeLibrary( AfxGetResourceHandle() );

	if (bClassRegistered)
		::UnregisterClass(_T("CamStudio"),AfxGetInstanceHandle());
	return CWinApp::ExitInstance();
}

bool CRecorderApp::LoadLanguage(LANGID LangID)
{
	// integrated language is the right one
	bool bResult = (LangID == STANDARD_LANGID);
	if (bResult) {
		ASSERT(CurLangID == LangID);
		TRACE("CurLangID: %d\n", CurLangID);
		return true;
	}

	CString strLangIDDLL;
	strLangIDDLL.Format(_T("RecorderLANG%.2x.dll"), LangID);
	HINSTANCE hInstance = ::LoadLibrary(strLangIDDLL);
	bResult = (0 != hInstance);
	if (bResult) {
		AfxSetResourceHandle(hInstance);
		CurLangID = LangID;
	}

	TRACE("CurLangID: %d %s\n", CurLangID, bResult ? "loaded" : "failed");
	return bResult;
}

// Register unique application class name that we wish to use
bool CRecorderApp::RegisterWindowClass()
{
	// todo: add mutex code
	WNDCLASS wndcls;
	memset(&wndcls, 0, sizeof(WNDCLASS)); // start with NULL defaults
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = LoadIcon(IDR_MAINFRAME); // or load a different icon
	wndcls.hCursor = LoadCursor( IDC_ARROW );
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;

	// Specify our own class name for using FindWindow later
	wndcls.lpszClassName = _T("CamStudio");

	// Register new class and exit if it fails
	if (!AfxRegisterClass(&wndcls)) {
		TRACE("Class Registration Failed\n");
		return false;
	}
	return true;
}
