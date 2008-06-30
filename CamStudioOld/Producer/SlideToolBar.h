#if !defined(AFX_SLIDETOOLBAR)
#define AFX_SLIDETOOLBAR

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SlideToolBar.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSlideToolBar window

class CSlideToolBar : public CToolBar
{
// Construction
public:
	CSlideToolBar();

// Attributes
public:
	
	CSliderCtrl m_wndSliderCtrl;	
	int	m_slidevalue;

		   

// Operations
public:

	void SetPositions();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlideToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	void AdjustTimeBar(CRect clientrect);
	void UpdateValues();
	void EnableButton(BOOL setToOn, int nIndex);
	virtual ~CSlideToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSlideToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLIDETOOLBAR_H__992196A1_7C20_11D4_BEC3_B9FC12288B45__INCLUDED_)
