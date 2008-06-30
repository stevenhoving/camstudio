#if !defined(AFX_SYNCDIALOG_H__C6FE6223_66FB_4857_A9BB_E1589B288B7F__INCLUDED_)
#define AFX_SYNCDIALOG_H__C6FE6223_66FB_4857_A9BB_E1589B288B7F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SyncDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSyncDialog dialog

class CSyncDialog : public CDialog
{
// Construction
public:
	CSyncDialog(CWnd* pParent = NULL);   // standard constructor
	void UpdateGui(); 

// Dialog Data
	//{{AFX_DATA(CSyncDialog)
	enum { IDD = IDD_SYNCHRONIZATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyncDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSyncDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	virtual void OnOK();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCDIALOG_H__C6FE6223_66FB_4857_A9BB_E1589B288B7F__INCLUDED_)
