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
#include "CStudioLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL bClassRegistered = FALSE;
// this global variable is mentioned in StdAfx.h
// thus everyone has an access
// not the best solution though
Config *cfg;

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
public:
	virtual BOOL OnInitDialog();
private:
	CStatic m_ctrlStaticVersion;
public:
	afx_msg void OnStnClickedStaticVersion();
	afx_msg void OnBnClickedButtonlink3();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
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
	strVersionInfo.Format("v%s\n", CURRENT_VERSION_NUMBER );
	CString strReleaseInfo;
#if _DEBUG 
	strReleaseInfo.Format("Build on Mercurial release: r%s%s\n", CURRENT_HG_RELEASE_NUMBER, "+ (For testers only..!)" );
#else
	strReleaseInfo.Format("Build on Mercurial release: r%s%s\n", CURRENT_HG_RELEASE_NUMBER, "" );
#endif


    // CamStudio Recorder <VERSION><RELEASE><XNOTESTOPWATCHSUPPORT><BUGFIXES><COPYRIGHT-1><COPYRIGHT-2>
	m_ctrlStaticVersion.GetWindowText(strBuffer);
	strBuffer.Replace("<VERSION>", _T(strVersionInfo) );
	strBuffer.Replace("<RELEASE>", _T(strReleaseInfo) );
	strBuffer.Replace("<XNOTESTOPWATCHSUPPORT>", "\nXNoteStopwatch support coded by JanHgm (www.jahoma.nl/timereg)\n" );
	strBuffer.Replace("<BUGFIXES>", "Bugfixes and additional features added by Nick the Geek and others.\n" );
	strBuffer.Replace("<COPYRIGHT-1>", "\nCopyright © 2001 RenderSoft Software and Web Publishing\n" );
	strBuffer.Replace("<COPYRIGHT-2>", "Copyright © 2008 CamStudio Group && Contributors\n" );

	m_ctrlStaticVersion.SetWindowText(strBuffer);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnBnClickedButtonlink2()
{
	LPCTSTR mode = ("open");
	ShellExecute (GetSafeHwnd (), mode, "http://www.camstudio.org/donate", NULL, NULL, SW_SHOW);
}

void CAboutDlg::OnButtonlink()
{
	// TODO: Add your control notification handler code here
	::PostMessage(hWndGlobal,WM_COMMAND,ID_CAMSTUDIO4XNOTE_WEBSITE,0);
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
	// Parse command line for standard shell commands, DDE, file open
 	//CCamStudioCommandLineInfo cmdInfo;
 	ParseCommandLine(m_cmdInfo);
 
 	if (m_cmdInfo.hasLoadCodec()) {
 		// auto install codec here
 	}
 
 	if (m_cmdInfo.isStart()) {
 		m_iExitValue = 1; 
 	} else if(m_cmdInfo.isCodecs()) {
 		CString codecs=VideoCodecsInfo();
 		if(m_cmdInfo.hasOutFile()) {
 			CFile codecsFile(m_cmdInfo.OutFile(), CFile::modeCreate|CFile::modeWrite);
 			codecsFile.Write(codecs, codecs.GetLength());
 			codecsFile.Close();
 		} else {
 			AfxMessageBox(codecs);
 		}
 		return FALSE;
 	}
	
	// Initialize GDI+.
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	::OnError("CRecorderApp::InitInstance");
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
	CString strProfile;
	strProfile.Format("%s\\CamStudio.cfg", (LPCSTR)(GetAppDataPath()));
	if (!DoesFileExist(strProfile)) {
       //Only reading, if the user has no file yet, see if a starter file was provided:
	   strProfile.Format("%s\\CamStudio.cfg", (LPCSTR)(GetProgPath()));
	}
	m_pszProfileName = _tcsdup(strProfile);

	// TODO: re-enable when class complete
	// Read the file. If there is an error, report it and exit.
	cfg = new Config();
	try
	{
		cfg->readFile(strProfile);
	}
	catch(const FileIOException)
	{// TODO: move me to resource
//		return(EXIT_FAILURE);
	}
	catch(const ParseException &pex)
	{
		char buf[1024];
		_snprintf_s(buf, 1024, _TRUNCATE, "Config file parse error at %s:%d - %s", pex.getFile(), pex.getLine(), pex.getError());
//		return(EXIT_FAILURE);
	}

//	m_cmSettings.Read();
	// initialize global values
//	VERIFY(m_cmSettings.Read(LANGUAGE, m_wCurLangID));

//	m_wCurLangID = static_cast<LANGID>(GetProfileInt(SEC_SETTINGS, ENT_LANGID, STANDARD_LANGID));
	if (!LoadLanguage(m_wCurLangID)) {
		if (!LoadLanguage(::GetUserDefaultLangID())) {
			LoadLanguage(::GetSystemDefaultLangID());
		}
	}
	//WriteProfileInt(SEC_SETTINGS, ENT_LANGID, m_wCurLangID);
//	VERIFY(m_cmSettings.Write(LANGUAGE, m_wCurLangID));

	// If stop command has been requested, try to exit the previous instance
 	if (m_cmdInfo.isStop()) {
 		CWnd *pWnd=PreviousInstance();
 		if(pWnd) {
 			((CMainFrame*)pWnd)->PostMessage(CMainFrame::WM_USER_STOPRECORD,0,0);
 			return TRUE;
 		} else {
 			MessageBox(NULL,"Previous instance not found","From Exe",MB_OK);
 			return FALSE;
 		}
 	}

	if (!FirstInstance())
		return FALSE;

	// Register new class and exit if it fails
	if (!RegisterWindowClass()) {
		TRACE("Class Registration Failed\n");
		return FALSE;
	}

#if (WINVER < 0x600)	// Windows Vista
//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
#endif	// Windows Vista

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	// SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(); // Load standard INI file options (including MRU)

	m_iVersionOp = GetOperatingSystem();

	if(cfg->exists("Audio"))
		cAudioFormat.Read(cfg->lookup("Audio"));
	if(cfg->exists("Video"))
		cVideoOpts.Read(cfg->lookup("Video"));
	if(cfg->exists("Cursor"))
		CamCursor.Read(cfg->lookup("Cursor"));
	if(cfg->exists("Program"))
		cProgramOpts.Read(cfg->lookup("Program"));
	if(cfg->exists("HotKeys"))
		cHotKeyOpts.Read(cfg->lookup("HotKeys"));
	if(cfg->exists("Region"))
		cRegionOpts.Read(cfg->lookup("Region"));
	if(cfg->exists("Caption"))
		cCaptionOpts.Read(cfg->lookup("Caption"));
	if(cfg->exists("TimeStamp"))
		cTimestampOpts.Read(cfg->lookup("TimeStamp"));
	if(cfg->exists("XNote"))
		cXNoteOpts.Read(cfg->lookup("XNote"));
	if(cfg->exists("Watermark"))
		cWatermarkOpts.Read(cfg->lookup("Watermark"));
	if(cfg->exists("Producer"))
		cProducerOpts.Read(cfg->lookup("Producer"));

	// Register the application's document templates. Document templates
	// serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	// TODO, Possible memory leak, where is the delete operation of the new below done?
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRecorderDoc),
		RUNTIME_CLASS(CMainFrame), // main SDI frame window
		RUNTIME_CLASS(CRecorderView));
	AddDocTemplate(pDocTemplate);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(m_cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	if (cProgramOpts.m_iViewType != VIEW_NORMAL) {
      CMainFrame* cwind = (CMainFrame*)AfxGetMainWnd();
      cwind->UpdateViewtype();
	}

	// Set property to find previous instance if needed
 	if (m_cmdInfo.isStart()) {
		::SetProp(m_pMainWnd->GetSafeHwnd(),CAMSTUDIO_MUTEX,(HANDLE)1);
 		cRegionOpts.m_iCaptureMode=CAPTURE_FIXED;
 		if(m_cmdInfo.X()>=0)
 			cRegionOpts.m_iLeft=m_cmdInfo.X();
 		if(m_cmdInfo.Y()>=0)
 			cRegionOpts.m_iTop=m_cmdInfo.Y();
 		if(m_cmdInfo.Width()>=0)
 			cRegionOpts.m_iWidth=m_cmdInfo.Width();
 		if(m_cmdInfo.Height()>=0)
 			cRegionOpts.m_iHeight=m_cmdInfo.Height();
 		((CMainFrame*)m_pMainWnd)->GetViewActive()->PostMessage(CRecorderView::WM_USER_RECORDAUTO,0,0);
 	}

	return TRUE;
}

int CRecorderApp::ExitInstance()
{
	if(m_cmdInfo.isCodecs()) {
 		return 0;
 	}

	Setting* s;
	try {
		if (!cfg->exists("Audio"))
			s=&cfg->getRoot().add("Audio", Setting::TypeGroup);
		else
			s=&cfg->lookup("Audio");
		cAudioFormat.Write(*s);

		if (!cfg->exists("Video"))
			s=&cfg->getRoot().add("Video", Setting::TypeGroup);
		else
			s=&cfg->lookup("Video");
		cVideoOpts.Write(*s);

		if (!cfg->exists("Cursor"))
			s=&cfg->getRoot().add("Cursor", Setting::TypeGroup);
		else
			s=&cfg->lookup("Cursor");
		CamCursor.Write(*s);

		if (!cfg->exists("Program"))
			s=&cfg->getRoot().add("Program", Setting::TypeGroup);
		else
			s=&cfg->lookup("Program");
		cProgramOpts.Write(*s);

		if (!cfg->exists("HotKeys"))
			s=&cfg->getRoot().add("HotKeys", Setting::TypeGroup);
		else
			s=&cfg->lookup("HotKeys");
		cHotKeyOpts.Write(*s);

		if (!cfg->exists("Region"))
			s=&cfg->getRoot().add("Region", Setting::TypeGroup);
		else
			s=&cfg->lookup("Region");
		cRegionOpts.Write(*s);

		if (!cfg->exists("Caption"))
			s=&cfg->getRoot().add("Caption", Setting::TypeGroup);
		else
			s=&cfg->lookup("Caption");
		cCaptionOpts.Write(*s);

		if (!cfg->exists("TimeStamp"))
			s=&cfg->getRoot().add("TimeStamp", Setting::TypeGroup);
		else
			s=&cfg->lookup("TimeStamp");
		cTimestampOpts.Write(*s);

		if (!cfg->exists("XNote"))
			s=&cfg->getRoot().add("XNote", Setting::TypeGroup);
		else
			s=&cfg->lookup("XNote");
		cXNoteOpts.Write(*s);

		if (!cfg->exists("Watermark"))
			s=&cfg->getRoot().add("Watermark", Setting::TypeGroup);
		else
			s=&cfg->lookup("Watermark");
		cWatermarkOpts.Write(*s);

		if (!cfg->exists("Producer"))
			s=&cfg->getRoot().add("Producer", Setting::TypeGroup);
		else
			s=&cfg->lookup("Producer");
		cProducerOpts.Write(*s);
	}
	catch(SettingTypeException& e) {
		MessageBox(NULL, e.getPath(), e.what(), MB_OK);
	}

   //Save the configuration file out to the user appdata directory.
	CString strProfile;
	if(m_cmdInfo.hasCfgFile()) {
 		strProfile.Format("%s", m_cmdInfo.CfgFile());
 	} else {
 		strProfile.Format("%s\\CamStudio.cfg", (LPCSTR)(GetAppDataPath()));
 	}
	cfg->writeFile(strProfile);
	delete cfg;

	//Multilanguage
	if (m_wCurLangID != STANDARD_LANGID)
		::FreeLibrary(AfxGetResourceHandle());

	if (bClassRegistered)
		::UnregisterClass(_T("CamStudio"), AfxGetInstanceHandle());

	Gdiplus::GdiplusShutdown(gdiplusToken);

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

CString CRecorderApp::VideoCodecsInfo()
{
 	CString str="";
 	ICINFO icinfo;
 	for (int i=0; ICInfo(0x63646976, i, &icinfo); i++) {
 		HIC hic = ICOpen(icinfo.fccType, icinfo.fccHandler, ICMODE_QUERY);
 		if (hic) {
 			ICGetInfo(hic, &icinfo, sizeof(ICINFO));
 			CString icstr;
 			icstr.Format("%d^%s^%s^%s\n",
 				icinfo.fccHandler,
 				(LPCTSTR)CString(icinfo.szName),
 				(LPCTSTR)CString(icinfo.szDescription),
 				(LPCTSTR)CString(icinfo.szDriver));
 			str += icstr;
 			ICClose(hic);
 		}
 	}
 	return str;
}

// Get the handle of a previous application instance
CWnd *CRecorderApp::PreviousInstance()
{
 	HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS,FALSE, CAMSTUDIO_MUTEX);
 	if(hMutex == NULL) { // no such mutex there must be no existing instance
 		MessageBox(NULL,"Cannot open mutex","From Exe",MB_OK);
 		return NULL;
 	}
 	CWnd *pPrevWnd = CWnd::GetDesktopWindow()->GetWindow(GW_CHILD);
 	while (pPrevWnd) {
 		HWND hWnd = pPrevWnd->GetSafeHwnd();
 		if (::GetProp(hWnd,CAMSTUDIO_MUTEX)) {
 			return pPrevWnd;
 		}
 		pPrevWnd = pPrevWnd->GetWindow(GW_HWNDNEXT);
 	}
 	MessageBox(NULL,"No instance found","From Exe",MB_OK);
 	return NULL;
}
 

// Determine if another window with our class name exists...
// return TRUE if no previous instance running;	FALSE otherwise
bool CRecorderApp::FirstInstance()
{
	bool bPrevInstance = FALSE;
	m_hAppMutex = ::CreateMutex(0, TRUE, CAMSTUDIO_MUTEX);
	bPrevInstance = (0 != m_hAppMutex);
	if (!bPrevInstance) {
		::OnError(_T("CRecorderApp::FirstInstance"));
		return bPrevInstance;	// unable to check mutext, assume previous instance and quit.
	}
	// check last error to see if mutex existed
	bPrevInstance = (ERROR_ALREADY_EXISTS == ::GetLastError());
	if (bPrevInstance) {
		// previous instance exists
		::OnError(_T("CRecorderApp::FirstInstance fails"));
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

bool CRecorderApp::LoadLanguage(LANGID wLangID)
{
	// integrated language is the right one
	bool bResult = (wLangID == STANDARD_LANGID);
	if (bResult) {
		ASSERT(m_wCurLangID == wLangID);
		TRACE("m_wCurLangID: %d\n", m_wCurLangID);
		return true;
	}

	CString strLangIDDLL;
	strLangIDDLL.Format(_T("RecorderLANG%.2x.dll"), wLangID);
	HINSTANCE hInstance = ::LoadLibrary(strLangIDDLL);
	bResult = (0 != hInstance);
	if (bResult) {
		AfxSetResourceHandle(hInstance);
		m_wCurLangID = wLangID;
	}

	TRACE("m_wCurLangID: %d %s\n", m_wCurLangID, bResult ? "loaded" : "failed");
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


void CAboutDlg::OnStnClickedStaticVersion()
{
	// TODO: Add your control notification handler code here
}

void CAboutDlg::OnBnClickedButtonlink3()
{
	// TODO: Add your control notification handler code here
	::PostMessage(hWndGlobal,WM_COMMAND,ID_HELP_WEBSITE,0);
}
