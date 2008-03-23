#if !defined(AFX_CURSOROPTIONSDLG_H__53E9FA38_56DF_4034_A58F_A0434F2A8EAA__INCLUDED_)
#define AFX_CURSOROPTIONSDLG_H__53E9FA38_56DF_4034_A58F_A0434F2A8EAA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CursorOptionsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCursorOptionsDlg dialog

class CCursorOptionsDlg : public CDialog
{
// Construction
public:
	CCursorOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	void RefreshPreviewCursor();
	void RefreshHighlight();

// Dialog Data
	//{{AFX_DATA(CCursorOptionsDlg)
	enum { IDD = IDD_CURSOROPTIONS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCursorOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCursorOptionsDlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnCursor1();
	afx_msg void OnCursor2();
	afx_msg void OnCursor3();
	virtual BOOL OnInitDialog();
	afx_msg void OnFilecursor();
	afx_msg void OnSelchangeCustomcursor();
	virtual void OnOK();
	afx_msg void OnSelchangeHighlightshape();
	afx_msg void OnHighlightcolor();
	afx_msg void OnHighlightcursor();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURSOROPTIONSDLG_H__53E9FA38_56DF_4034_A58F_A0434F2A8EAA__INCLUDED_)
