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

#include "CamError.h"
#include "CamStudioCommandLineInfo.h"
#include "Profile.h"
#include "resource.h"       // main symbols

//#define WM_APP_REGIONUPDATE	0x00500	// TODO: Should this be a WM_USER/WM_APP message?
//#define WM_APP_REGIONUPDATE	WM_USER + 0x00100
#define WM_APP_REGIONUPDATE	(WM_APP + 100)

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

	LANGID LanguageID() const			{return m_wCurLangID;}
	LANGID LanguageID(LANGID wLangID)
	{
		m_wCurLangID = wLangID;
		VERIFY(m_cmSettings.Write(LANGUAGE, m_wCurLangID));
		return m_wCurLangID;
	}
	int VersionOp() const				{return m_iVersionOp;}
	CProfile& GetProfile()				{return m_cmSettings;}

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
	LANGID m_wCurLangID;
	int m_iVersionOp;
	CCamStudioCommandLineInfo m_cmdInfo;
	CProfile m_cmSettings;

	bool RegisterWindowClass();
	BOOL FirstInstance();
	bool LoadLanguage(LANGID LangID);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_)
