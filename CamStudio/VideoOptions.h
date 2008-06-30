#if !defined(AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_)
#define AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VideoOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoOptions dialog

class CVideoOptions : public CDialog
{
// Construction
public:
	CVideoOptions(CWnd* pParent = NULL);   // standard constructor
	void RefreshCompressorButtons(); 
	void RefreshAutoOptions();
	void UpdateAdjustSliderVal();

// Dialog Data
	//{{AFX_DATA(CVideoOptions)
	enum { IDD = IDD_VIDEOOPTIONS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideoOptions)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbout();
	afx_msg void OnSelchangeCompressors();
	afx_msg void OnConfigure();
	virtual void OnCancel();
	afx_msg void OnAuto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_)
