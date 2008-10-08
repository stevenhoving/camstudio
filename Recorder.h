// Recorder.h : main header file for the VSCAP application
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_)
#define AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "CamStudioCommandLineInfo.h"

//Multilanguage
#define ENT_LANGID _T("LanguageID")
#define ENT_LANGINI _T("LangINI")
#define SEC_SETTINGS _T("Language")
#define STANDARD_LANGID 0x09	// English

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp:
// See vscap.cpp for the implementation of this class
//

class CRecorderApp : public CWinApp
{
public:
	CRecorderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecorderApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRecorderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	static const char * CAMSTUDIO_MUTEX;
	HANDLE m_hAppMutex;
	LANGID CurLangID;
	CCamStudioCommandLineInfo m_cmdInfo;

	bool RegisterWindowClass();
	BOOL FirstInstance();
	bool LoadLanguage(LANGID LangID);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// general error handler
void OnError(LPTSTR lpszFunction);
extern void ErrMsg(char format[], ...);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1,long val2);

#endif // !defined(AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_)
