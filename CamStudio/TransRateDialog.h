#if !defined(AFX_TRANSRATEDIALOG_H__DC88D68B_2E44_4167_B066_11937CA0009B__INCLUDED_)
#define AFX_TRANSRATEDIALOG_H__DC88D68B_2E44_4167_B066_11937CA0009B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransRateDialog.h : header file
//

#include "VideoWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CTransRateDialog dialog

class CTransRateDialog : public CDialog
{
// Construction
public:
	CTransRateDialog(CWnd* pParent = NULL);   // standard constructor
	CVideoWnd* m_myparent;
	void PreModal( CVideoWnd * parent); 

// Dialog Data
	//{{AFX_DATA(CTransRateDialog)
	enum { IDD = IDD_TRANSRATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransRateDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransRateDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSRATEDIALOG_H__DC88D68B_2E44_4167_B066_11937CA0009B__INCLUDED_)
