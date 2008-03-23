// playplusView.h : interface of the CPlayplusView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYPLUSVIEW_H__8B1773D6_C15C_4371_BEB4_1943076AD478__INCLUDED_)
#define AFX_PLAYPLUSVIEW_H__8B1773D6_C15C_4371_BEB4_1943076AD478__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlayplusView : public CView
{
protected: // create from serialization only
	CPlayplusView();
	DECLARE_DYNCREATE(CPlayplusView)

// Attributes
public:
	CPlayplusDoc* GetDocument();



// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlayplusView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPlayplusView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPlayplusView)
	afx_msg void OnFileOpen();
	afx_msg void OnFileClose();
	afx_msg void OnFilePlay();
	afx_msg void OnFileRewind();
	afx_msg void OnFileStop();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLastframe();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomResizetomoviesize();
	afx_msg void OnZoomQuarter();
	afx_msg void OnZoomHalf();
	afx_msg void OnZoom1();
	afx_msg void OnZoom2();
	afx_msg void OnZoom4();
	afx_msg void OnUpdateZoom1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoom2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoom4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomHalf(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomQuarter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomResizetomoviesize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileRewind(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileLastframe(CCmdUI* pCmdUI);
	afx_msg void OnButtonRecord();
	afx_msg void OnUpdateRecord(CCmdUI* pCmdUI);
	afx_msg void OnAudioRemoveexistingaudiotracks();
	afx_msg void OnAudioAddaudiofromwavefile();
	afx_msg void OnUpdateAudioRemoveexistingaudiotracks(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAudioAddaudiofromwavefile(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveas();
	afx_msg void OnAudioAudiooptions();
	afx_msg void OnZoomTestaudio();
	afx_msg void OnUpdateFileSaveas(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePlay(CCmdUI* pCmdUI);
	afx_msg void OnAudioExtension();
	afx_msg void OnUpdateAudioExtension(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnUserGeneric	(UINT wParam, LONG lParam);	
	//afx_msg void OnMM_WIM_DATA(UINT parm1, LONG parm2);
	afx_msg LRESULT OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2);
	afx_msg LRESULT OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in playplusView.cpp
inline CPlayplusDoc* CPlayplusView::GetDocument()
   { return (CPlayplusDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYPLUSVIEW_H__8B1773D6_C15C_4371_BEB4_1943076AD478__INCLUDED_)
