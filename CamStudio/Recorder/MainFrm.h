// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__DBFD9E21_C771_4321_8322_1F45ED8D4A66__INCLUDED_)
#define AFX_MAINFRM_H__DBFD9E21_C771_4321_8322_1F45ED8D4A66__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "TrayIcon.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CBitmap& Logo()	{return m_bmLogo;}

// Operations
public:
	void UpdateViewtype();
	CView *GetViewActive();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CBitmap		m_ToolbarBitmap256;
	CBitmap		m_ToolbarBitmapMask;
	CImageList  m_ilToolBar;
	CBitmap		m_bmLogo;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnViewCompactview();
	afx_msg void OnUpdateViewCompactview(CCmdUI* pCmdUI);
	afx_msg void OnViewButtonsview();
	afx_msg void OnUpdateViewButtonsview(CCmdUI* pCmdUI);
	afx_msg void OnViewNormalview();
	afx_msg void OnUpdateViewNormalview(CCmdUI* pCmdUI);
	afx_msg void OnViewtype();
	//}}AFX_MSG
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnXNote(UINT wParam, LONG lParam);
	afx_msg LRESULT OnMotionDetector(UINT wParam, LONG lParam);
	afx_msg LRESULT OnStopRecord(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
private:
	CTrayIcon m_TrayIcon;
	static const UINT WM_USER_XNOTE;
	static const UINT WM_USER_MOTIONDETECTOR;
public:
	static const UINT WM_USER_STOPRECORD;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

extern int maxxScreen;
extern int maxyScreen;
extern int minxScreen;
extern int minyScreen;

#endif // !defined(AFX_MAINFRM_H__DBFD9E21_C771_4321_8322_1F45ED8D4A66__INCLUDED_)
