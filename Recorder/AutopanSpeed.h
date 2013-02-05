// AutopanSpeed.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUTOPANSPEED_H__49D4B2FA_02DF_4EAC_BA42_F2EEDEA2FE58__INCLUDED_)
#define AFX_AUTOPANSPEED_H__49D4B2FA_02DF_4EAC_BA42_F2EEDEA2FE58__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAutopanSpeedDlg dialog

class CAutopanSpeedDlg : public CDialog
{
// Construction
public:
	CAutopanSpeedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutopanSpeedDlg)
	enum { IDD = IDD_AUTOPANSPEED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutopanSpeedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutopanSpeedDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl m_ctrlSliderPanSpeed;
	CStatic m_ctrlStaticMaxSpeed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOPANSPEED_H__49D4B2FA_02DF_4EAC_BA42_F2EEDEA2FE58__INCLUDED_)
