// vscapView.h : interface of the CVscapView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSCAPVIEW_H__DCC4865E_3B37_402E_AC1B_C8ABF4519F51__INCLUDED_)
#define AFX_VSCAPVIEW_H__DCC4865E_3B37_402E_AC1B_C8ABF4519F51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CVscapView : public CView 
{
protected: // create from serialization only
	CVscapView();
	DECLARE_DYNCREATE(CVscapView)

// Attributes
public:
	CVscapDoc* GetDocument();
	void SaveSettings(); 
	void LoadSettings(); 
	void DecideSaveSettings();

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVscapView)
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
	virtual ~CVscapView();
	BOOL Openlink (CString);
	BOOL OpenUsingShellExecute (CString);
	LONG GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata);
	BOOL OpenUsingRegisteredClass (CString);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVscapView)
	afx_msg void OnRegionRubber();
	afx_msg void OnRegionPanregion();
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRecord();
	afx_msg void OnStop();
	afx_msg void OnUpdateRegionPanregion(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRegionRubber(CCmdUI* pCmdUI);
	afx_msg void OnFileVideooptions();
	afx_msg void OnOptionsCursoroptions();
	afx_msg void OnOptionsAutopan();
	afx_msg void OnOptionsAtuopanspeed();
	afx_msg void OnUpdateOptionsAutopan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecord(CCmdUI* pCmdUI);
	afx_msg void OnRegionFullscreen();
	afx_msg void OnUpdateRegionFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnOptionsMinimizeonstart();
	afx_msg void OnUpdateOptionsMinimizeonstart(CCmdUI* pCmdUI);
	afx_msg void OnOptionsHideflashing();
	afx_msg void OnUpdateOptionsHideflashing(CCmdUI* pCmdUI);
	afx_msg void OnOptionsProgramoptionsPlayavi();
	afx_msg void OnUpdateOptionsProgramoptionsPlayavi(CCmdUI* pCmdUI);
	afx_msg void OnHelpWebsite();
	afx_msg void OnHelpHelp();
	afx_msg void OnPause();
	afx_msg void OnUpdatePause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnOptionsRecordaudio();
	afx_msg void OnUpdateOptionsRecordaudio(CCmdUI* pCmdUI);
	afx_msg void OnOptionsAudioformat();
	afx_msg void OnHelpFaq();
	afx_msg void OnOptionsKeyboardshortcuts();
	afx_msg void OnOptionsProgramoptionsSavesettingsonexit();
	afx_msg void OnUpdateOptionsProgramoptionsSavesettingsonexit(CCmdUI* pCmdUI);
	afx_msg void OnOptionsRecordingthreadpriorityNormal();
	afx_msg void OnOptionsRecordingthreadpriorityHighest();
	afx_msg void OnOptionsRecordingthreadpriorityAbovenormal();
	afx_msg void OnOptionsRecordingthreadpriorityTimecritical();
	afx_msg void OnUpdateOptionsRecordingthreadpriorityTimecritical(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsRecordingthreadpriorityNormal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsRecordingthreadpriorityHighest(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsRecordingthreadpriorityAbovenormal(CCmdUI* pCmdUI);
	afx_msg void OnOptionsCapturetrans();
	afx_msg void OnUpdateOptionsCapturetrans(CCmdUI* pCmdUI);
	afx_msg void OnOptionsTempdirWindows();
	afx_msg void OnUpdateOptionsTempdirWindows(CCmdUI* pCmdUI);
	afx_msg void OnOptionsTempdirInstalled();
	afx_msg void OnUpdateOptionsTempdirInstalled(CCmdUI* pCmdUI);
	afx_msg void OnOptionsTempdirUser();
	afx_msg void OnUpdateOptionsTempdirUser(CCmdUI* pCmdUI);
	afx_msg void OnOptionsRecordaudioDonotrecordaudio();
	afx_msg void OnUpdateOptionsRecordaudioDonotrecordaudio(CCmdUI* pCmdUI);
	afx_msg void OnOptionsRecordaudioRecordfromspeakers();
	afx_msg void OnUpdateOptionsRecordaudioRecordfromspeakers(CCmdUI* pCmdUI);
	afx_msg void OnOptionsRecordaudiomicrophone();
	afx_msg void OnUpdateOptionsRecordaudiomicrophone(CCmdUI* pCmdUI);	
	afx_msg void OnOptionsProgramoptionsTroubleshoot();
	afx_msg void OnOptionsProgramoptionsCamstudioplay();
	afx_msg void OnUpdateOptionsProgramoptionsCamstudioplay(CCmdUI* pCmdUI);
	afx_msg void OnOptionsProgramoptionsDefaultplay();
	afx_msg void OnUpdateOptionsProgramoptionsDefaultplay(CCmdUI* pCmdUI);
	afx_msg void OnOptionsProgramoptionsNoplay();
	afx_msg void OnUpdateOptionsProgramoptionsNoplay(CCmdUI* pCmdUI);
	afx_msg void OnHelpDonations();
	afx_msg void OnOptionsUsePlayer20();
	afx_msg void OnUpdateUsePlayer20(CCmdUI* pCmdUI);
	afx_msg void OnViewScreenannotations();
	afx_msg void OnUpdateViewScreenannotations(CCmdUI* pCmdUI);
	afx_msg void OnViewVideoannotations();
	afx_msg void OnOptionsSynchronization();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnToolsSwfproducer();
	afx_msg void OnOptionsSwfLaunchhtml();
	afx_msg void OnOptionsSwfDeleteavifile();
	afx_msg void OnOptionsSwfDisplayparameters();
	afx_msg void OnUpdateOptionsSwfLaunchhtml(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsSwfDisplayparameters(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsSwfDeleteavifile(CCmdUI* pCmdUI);
	afx_msg void OnAviswf();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnOptionsNamingAutodate();
	afx_msg void OnUpdateOptionsNamingAutodate(CCmdUI* pCmdUI);
	afx_msg void OnOptionsNamingAsk();
	afx_msg void OnUpdateOptionsNamingAsk(CCmdUI* pCmdUI);
	afx_msg void OnOptionsProgramoptionsPresettime();
	//}}AFX_MSG
	afx_msg LRESULT OnRecordStart (UINT wParam, LONG lParam);
	afx_msg LRESULT OnRecordInterrupted (UINT wParam, LONG lParam);
	afx_msg LRESULT OnSaveCursor (UINT wParam, LONG lParam);	
	afx_msg LRESULT OnUserGeneric	(UINT wParam, LONG lParam);		
	afx_msg LRESULT OnKeyStart	(UINT wParam, LONG lParam);	
	afx_msg LRESULT OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2);
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in vscapView.cpp
inline CVscapDoc* CVscapView::GetDocument()
   { return (CVscapDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSCAPVIEW_H__DCC4865E_3B37_402E_AC1B_C8ABF4519F51__INCLUDED_)
