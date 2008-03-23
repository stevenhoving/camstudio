#if !defined(AFX_TROUBLESHOOT_H__4E9048F9_4108_4B64_B5DF_04B0ECDAE07B__INCLUDED_)
#define AFX_TROUBLESHOOT_H__4E9048F9_4108_4B64_B5DF_04B0ECDAE07B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TroubleShoot.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TroubleShoot dialog

class TroubleShoot : public CDialog
{
// Construction
public:
	TroubleShoot(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TroubleShoot)
	enum { IDD = IDD_TROUBLESHOOT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TroubleShoot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TroubleShoot)
	afx_msg void OnCheck1();
	virtual void OnOK();
	afx_msg void OnCheck2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TROUBLESHOOT_H__4E9048F9_4108_4B64_B5DF_04B0ECDAE07B__INCLUDED_)
