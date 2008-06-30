#if !defined(AFX_FIXEDREGION_H__FA179C45_83DC_4DE0_BF6B_0C6E841C7BD2__INCLUDED_)
#define AFX_FIXEDREGION_H__FA179C45_83DC_4DE0_BF6B_0C6E841C7BD2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FixedRegion.h : header file
//

#define WM_USER_REGIONUPDATE 0x00500

/////////////////////////////////////////////////////////////////////////////
// CFixedRegion dialog

class CFixedRegion : public CDialog
{
// Construction
public:
	CFixedRegion(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFixedRegion)
	enum { IDD = IDD_FIXEDREGION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedRegion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFixedRegion)
	virtual void OnOK();
	afx_msg void OnSelect();
	virtual BOOL OnInitDialog();
	afx_msg void OnFixedtopleft();
	//}}AFX_MSG
	afx_msg LRESULT OnRegionUpdate (WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXEDREGION_H__FA179C45_83DC_4DE0_BF6B_0C6E841C7BD2__INCLUDED_)
