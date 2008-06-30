#if !defined(AFX_TEXTDIALOG_H__63374646_A843_482E_998F_8DD0E32477E3__INCLUDED_)
#define AFX_TEXTDIALOG_H__63374646_A843_482E_998F_8DD0E32477E3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TextDialog.h : header file
//

#define WM_USER_INVALIDATEWND_MSG "WM_USER_INVALIDATEWND_MSG"
static UINT WM_USER_INVALIDATEWND = ::RegisterWindowMessage(WM_USER_INVALIDATEWND_MSG);


/////////////////////////////////////////////////////////////////////////////
// TextDialog dialog

class TextDialog : public CDialog
{
// Construction
public:
	TextDialog(CWnd* pParent = NULL);   // standard constructor
	void PreModal(CString* inStr, LOGFONT *inFont, COLORREF* inRgb, CWnd* parent,int* horzalign); 
	void InvalidateTransWnd();
	void ChooseScriptFont(); 
	
	CString* displayStr;
	LOGFONT* logFont; 
	COLORREF* colorrgb;
	CWnd* myparent;
	int* myhorzalign;
	CFont tempFont;
	int mycharset;

// Dialog Data
	//{{AFX_DATA(TextDialog)
	enum { IDD = IDD_TEXTDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TextDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TextDialog)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnFont();
	afx_msg void OnChangeEdit1();
	afx_msg void OnJustleft();
	afx_msg void OnJustcenter();
	afx_msg void OnJustright();
	afx_msg void OnFont2();
	afx_msg void OnScriptWestern();
	afx_msg void OnScriptArabic();
	afx_msg void OnScriptBaltic();
	afx_msg void OnScriptChinesebig5();
	afx_msg void OnScriptChinesegb2312();
	afx_msg void OnScriptDefault();
	afx_msg void OnScriptEasteurope();
	afx_msg void OnScriptGreek();
	afx_msg void OnScriptHangul();
	afx_msg void OnScriptHebrew();
	afx_msg void OnScriptJapaneseshiftjis();
	afx_msg void OnScriptJohabkorean();
	afx_msg void OnScriptMac();
	afx_msg void OnScriptOem();
	afx_msg void OnScriptRussian();
	afx_msg void OnScriptSymbol();
	afx_msg void OnScriptThai();
	afx_msg void OnScriptTurkish();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTDIALOG_H__63374646_A843_482E_998F_8DD0E32477E3__INCLUDED_)
