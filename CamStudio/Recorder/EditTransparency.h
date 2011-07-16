// EditTransparency.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_EDITTRANSPARENCY_H__585926AC_E741_4A9F_8787_4D8651D48468__INCLUDED_)
#define AFX_EDITTRANSPARENCY_H__585926AC_E741_4A9F_8787_4D8651D48468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEditTransparencyDlg dialog

class CTransparentWnd;	// forward declaration

class CEditTransparencyDlg : public CDialog
{
// Construction
	CEditTransparencyDlg(CWnd* pParent = NULL);   // standard constructor; not used
public:
	CEditTransparencyDlg(bool& bEnable, int& iLevel, CTransparentWnd* pParent);

// Dialog Data
	//{{AFX_DATA(CEditTransparencyDlg)
	enum { IDD = IDD_TRANSPARENCY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditTransparencyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditTransparencyDlg)
	afx_msg void OnCheck1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

private:
	int m_iLevelOld;
	int& m_riLevel;
	bool& m_rbEnableTrans;
	bool m_bEnableTransOld;
	CTransparentWnd* m_pTransparentWnd;

	CButton m_bEnableTranparency;
	CSliderCtrl m_ctrlSliderTransparency;
	CStatic m_ctrlStaticTransparency;
protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTRANSPARENCY_H__585926AC_E741_4A9F_8787_4D8651D48468__INCLUDED_)

