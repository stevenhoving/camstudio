#if !defined(AFX_AUTOSEARCHDIALOG_H__CAEFF236_0E36_49BC_8B1A_0F2D903EDE9D__INCLUDED_)
#define AFX_AUTOSEARCHDIALOG_H__CAEFF236_0E36_49BC_8B1A_0F2D903EDE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoSearchDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoSearchDialog dialog

class CAutoSearchDialog : public CDialog
{
// Construction
public:
	CAutoSearchDialog(CWnd* pParent = NULL);   // standard constructor
	void SetVarText(CString textstr);
	void SetVarTextLine2(CString textstr);
	void SetButtonEnable(BOOL enable);


// Dialog Data
	//{{AFX_DATA(CAutoSearchDialog)
	enum { IDD = IDD_AUTOSEARCH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoSearchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoSearchDialog)
	afx_msg void OnCloseDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOSEARCHDIALOG_H__CAEFF236_0E36_49BC_8B1A_0F2D903EDE9D__INCLUDED_)

