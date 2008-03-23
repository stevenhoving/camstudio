#if !defined(AFX_EDITTRANSPARENCY_H__585926AC_E741_4A9F_8787_4D8651D48468__INCLUDED_)
#define AFX_EDITTRANSPARENCY_H__585926AC_E741_4A9F_8787_4D8651D48468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditTransparency.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditTransparency dialog

class CEditTransparency : public CDialog
{
// Construction
public:
	CEditTransparency(CWnd* pParent = NULL);   // standard constructor
	void PreModal(int *enableTrans, int *valTrans, CWnd *myParent); 
	int *m_enableTrans;
	int *m_valTrans;

	int m_backup_enableTrans;
	int m_backup_valTrans;
	CWnd * m_myparent;

// Dialog Data
	//{{AFX_DATA(CEditTransparency)
	enum { IDD = IDD_TRANSPARENCY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditTransparency)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditTransparency)
	afx_msg void OnCheck1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTRANSPARENCY_H__585926AC_E741_4A9F_8787_4D8651D48468__INCLUDED_)
