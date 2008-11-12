// ResizeDialog.h : header file
//
#if !defined(AFX_RESIZEDIALOG_H__C4C59E6A_3C80_47C5_AB9C_A71C5A0BEE6D__INCLUDED_)
#define AFX_RESIZEDIALOG_H__C4C59E6A_3C80_47C5_AB9C_A71C5A0BEE6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransparentWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CResizeDlg dialog

class CResizeDlg : public CDialog
{
// Construction
public:
	CResizeDlg(CWnd* pParent = NULL);   // standard constructor
	CTransparentWnd * m_transWnd;
	void PreModal(CTransparentWnd *transWnd);

// Dialog Data
	//{{AFX_DATA(CResizeDlg)
	enum { IDD = IDD_RESIZE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResizeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnReset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl m_ctrlSliderWidth;
	CSliderCtrl m_ctrlSliderHeight;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZEDIALOG_H__C4C59E6A_3C80_47C5_AB9C_A71C5A0BEE6D__INCLUDED_)
