#if !defined(AFX_TRANSRATEDIALOG_H__DC88D68B_2E44_4167_B066_11937CA0009B__INCLUDED_)
#define AFX_TRANSRATEDIALOG_H__DC88D68B_2E44_4167_B066_11937CA0009B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VideoWnd.h"

// TransRateDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransRateDlg dialog

class CTransRateDlg : public CDialog
{
// Construction
public:
	CTransRateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransRateDlg)
	enum { IDD = IDD_TRANSRATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransRateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransRateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CVideoWnd* m_myparent;
	CSliderCtrl m_ctrlSliderTransRate;
	CStatic m_ctrlStaticFrameRate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSRATEDIALOG_H__DC88D68B_2E44_4167_B066_11937CA0009B__INCLUDED_)
