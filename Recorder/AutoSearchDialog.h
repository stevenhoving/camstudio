// AutoSearchDialog.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUTOSEARCHDIALOG_H__CAEFF236_0E36_49BC_8B1A_0F2D903EDE9D__INCLUDED_)
#define AFX_AUTOSEARCHDIALOG_H__CAEFF236_0E36_49BC_8B1A_0F2D903EDE9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAutoSearchDlg dialog

class CAutoSearchDlg : public CDialog
{
// Construction
public:
	CAutoSearchDlg(CWnd* pParent = NULL);   // standard constructor
	void SetVarText(CString textstr);
	void SetVarTextLine2(CString textstr);
	void SetButtonEnable(BOOL enable);

// Dialog Data
	//{{AFX_DATA(CAutoSearchDlg)
	enum { IDD = IDD_AUTOSEARCH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoSearchDlg)
	afx_msg void OnCloseDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CStatic m_ctrlStaticText1;
	CStatic m_ctrlStaticText2;
	CButton m_ctrlButtonClose;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOSEARCHDIALOG_H__CAEFF236_0E36_49BC_8B1A_0F2D903EDE9D__INCLUDED_)

