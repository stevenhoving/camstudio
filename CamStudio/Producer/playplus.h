// playplus.h : main header file for the PLAYPLUS application
//

#if !defined(AFX_PLAYPLUS_H__690F740A_CB50_4A20_8631_D538BD9BD425__INCLUDED_)
#define AFX_PLAYPLUS_H__690F740A_CB50_4A20_8631_D538BD9BD425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPlayplusApp:
// See playplus.cpp for the implementation of this class
//

class CPlayplusApp : public CWinApp
{
public:
	CPlayplusApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayplusApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CPlayplusApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYPLUS_H__690F740A_CB50_4A20_8631_D538BD9BD425__INCLUDED_)
