// playplusView.h : interface of the CPlayplusView class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PLAYPLUSVIEW_H__8B1773D6_C15C_4371_BEB4_1943076AD478__INCLUDED_)
#define AFX_PLAYPLUSVIEW_H__8B1773D6_C15C_4371_BEB4_1943076AD478__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <assert.h>

#include "FlashConversionDlg.h"		// property page
#include "FlashInterface.h"			// property page
#include "AdvProperty.h"			// property page

//Multilanguage
#define ENT_LANGID _T("LanguageID")
#define ENT_LANGINI _T("LangINI")
#define SEC_SETTINGS _T("Language")
#define STANDARD_LANGID 0x09	// English
//#define STANDARD_LANGID 0x07	// German

class CPlayplusView : public CView
{
protected: // create from serialization only
	CPlayplusView();
	DECLARE_DYNCREATE(CPlayplusView)

	// Attributes
public:
	CPlayplusDoc* GetDocument();
	BOOL Openlink (CString);
	BOOL OpenUsingShellExecute (CString);
	LONG GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata);
	BOOL OpenUsingRegisteredClass (CString);

	bool PerformFlash(int &ww, int &hh, LONG& currenttime);

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
	afx_msg void OnFileConverttoswf();
	afx_msg void OnUpdateFileConverttoswf(CCmdUI* pCmdUI);
	afx_msg void OnConvert();
	afx_msg void OnHelpHelp();
	afx_msg void OnHelpSwfproducerfaq();
	//}}AFX_MSG
	afx_msg LRESULT OnUserGeneric	(UINT wParam, LONG lParam);
	afx_msg LRESULT OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2);
	afx_msg LRESULT OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2);
	DECLARE_MESSAGE_MAP()

private:
	LANGID CurLangID;
	BOOL LoadLangIDDLL(LANGID LangID);

	CPropertySheet		flashProp;
	FlashConversionDlg  page1;
	CFlashInterface		page2;
	CAdvProperty		page3;
	void CreatePropertySheet();
};

#ifndef _DEBUG  // debug version in playplusView.cpp
inline CPlayplusDoc* CPlayplusView::GetDocument()
{ return (CPlayplusDoc*)m_pDocument; }
#endif

class SWITCH_RESOURCE_HANDLE
{
public:
	SWITCH_RESOURCE_HANDLE(HMODULE new_resource_handle):
	  OldResourceHandle_(AfxGetResourceHandle())
	  {
		  ASSERT(OldResourceHandle_);
		  ASSERT(new_resource_handle);
		  AfxSetResourceHandle(new_resource_handle);
	  }

	  ~SWITCH_RESOURCE_HANDLE()
	  {
		  AfxSetResourceHandle(OldResourceHandle_);
	  }

private:
	HMODULE OldResourceHandle_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYPLUSVIEW_H__8B1773D6_C15C_4371_BEB4_1943076AD478__INCLUDED_)
