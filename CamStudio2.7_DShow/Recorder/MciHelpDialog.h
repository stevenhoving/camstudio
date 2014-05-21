#if !defined(AFX_MCIHELPDIALOG_H__A16677B4_8855_4D99_8ECA_C074067101A3__INCLUDED_)
#define AFX_MCIHELPDIALOG_H__A16677B4_8855_4D99_8ECA_C074067101A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MciHelpDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMciHelpDialog dialog

class CMciHelpDialog : public CDialog
{
// Construction
public:
	CMciHelpDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMciHelpDialog)
	enum { IDD = IDD_HELPMCI };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMciHelpDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMciHelpDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MCIHELPDIALOG_H__A16677B4_8855_4D99_8ECA_C074067101A3__INCLUDED_)
