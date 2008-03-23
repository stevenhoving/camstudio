#if !defined(AFX_AUTOPANSPEED_H__49D4B2FA_02DF_4EAC_BA42_F2EEDEA2FE58__INCLUDED_)
#define AFX_AUTOPANSPEED_H__49D4B2FA_02DF_4EAC_BA42_F2EEDEA2FE58__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AutopanSpeed.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutopanSpeed dialog

class CAutopanSpeed : public CDialog
{
// Construction
public:
	CAutopanSpeed(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutopanSpeed)
	enum { IDD = IDD_AUTOPANSPEED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutopanSpeed)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutopanSpeed)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPANSPEED_H__49D4B2FA_02DF_4EAC_BA42_F2EEDEA2FE58__INCLUDED_)
