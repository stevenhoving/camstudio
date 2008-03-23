// playplus.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "playplus.h"

#include "MainFrm.h"
#include "playplusDoc.h"
#include "playplusView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char playfiledir[800];	
extern void OpenMovieFileInit(char *filename);

#define PLAYER  0
#define DUBBER  1
extern int pmode;

HBITMAP hAboutBM = NULL;
extern void MsgC(const char fmt[], ...);


extern int runmode;

/////////////////////////////////////////////////////////////////////////////
// CPlayplusApp

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

/////////////////////////////////////////////////////////////////////////////
// CPlayplusApp construction

CPlayplusApp::CPlayplusApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlayplusApp object

CPlayplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPlayplusApp initialization

BOOL CPlayplusApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPlayplusDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPlayplusView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	playfiledir[0]=0;	
	if (strlen(m_lpCmdLine)!=0) {				
		
		if ((m_lpCmdLine[0]=='-') && ((m_lpCmdLine[1]=='b') || (m_lpCmdLine[1]=='x')))
		{
			if (m_lpCmdLine[1]=='x')
				runmode = 1;	//Used internally by CamStudio recorder
			else
				runmode = 2;	//no html launched on finish, no prompt asked
			
			int i;
			int lenx = strlen(m_lpCmdLine);
			for (i=2;i<lenx;i++)
			{
				if ((m_lpCmdLine[i]!=' ') && (m_lpCmdLine[i]!='\t'))
					break;

			}

			if (lenx>4)
			{
				strcpy(playfiledir,&m_lpCmdLine[i]);
				//::MessageBox(NULL,playfiledir,"Note",MB_OK);
			}			


			cmdInfo.m_strFileName = playfiledir;
			
			//strcpy(playfiledir,m_lpCmdLine);
			
			//MsgC("length %d",strlen(m_lpCmdLine));
			//strcpy(playfiledir,m_lpCmdLine);
			//playfiledir[0]= ' ';
			//playfiledir[1]= ' ';
			
			//::MessageBox(NULL,&m_lpCmdLine[3],"Note",MB_OK);

		}
		else
		{
			strcpy(playfiledir,m_lpCmdLine);

			//Fix to open long filename on launch
			cmdInfo.m_strFileName = playfiledir;

		}
		

	}
	
	
	
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT

	if (pmode == PLAYER)
		hAboutBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_ABOUT_PLAYER)); 
	else
		hAboutBM = LoadBitmap( AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_ABOUT_DUBBER)); 
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_DESTROY()
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
	/*
	if (pmode == PLAYER)
		((CStatic *) (GetDlgItem(IDC_TITLE)))->SetWindowText("RenderSoft CamStudio Player 2.0");		
	else if (pmode == DUBBER)
		((CStatic *) (GetDlgItem(IDC_TITLE)))->SetWindowText("RenderSoft CamStudio Producer 1.0");		
		*/


	if ((pmode == PLAYER) || (pmode == DUBBER))  {
		if (hAboutBM)
			((CStatic *) (GetDlgItem(IDC_IMAGE)))->SetBitmap( hAboutBM );		

	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	//::MessageBox(NULL,"Note Destory","nn",MB_OK);
	
	// TODO: Add your message handler code here
	if (hAboutBM) {
		DeleteObject(hAboutBM);
		hAboutBM = NULL;
	}
	
}
