// vscap.h : main header file for the VSCAP application
//

#if !defined(AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_)
#define AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVscapApp:
// See vscap.cpp for the implementation of this class
//

class CVscapApp : public CWinApp
{
public:
	CVscapApp();
	BOOL FirstInstance();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVscapApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVscapApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSCAP_H__CAB648E2_684F_4FF1_B574_9714ACAC6D57__INCLUDED_)
