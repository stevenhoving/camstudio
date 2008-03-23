// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AB22A31C_DD6D_4321_9F3D_053A376C0BA8__INCLUDED_)
#define AFX_MAINFRM_H__AB22A31C_DD6D_4321_9F3D_053A376C0BA8__INCLUDED_

#include "SlideToolBar.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableSlideBarButton(BOOL setToOn, int nIndex);
	void AdjustTimeBar(CRect clientrect);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void ResizeToMovie(RECT movierect); 
	void OffsetRectZero(CRect& winrect); 

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	//CToolBar    m_wndToolBar;
	CSlideToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenu(CMenu* pMenu);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AB22A31C_DD6D_4321_9F3D_053A376C0BA8__INCLUDED_)
