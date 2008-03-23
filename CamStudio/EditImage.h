#if !defined(AFX_EDITIMAGE_H__E8731569_888C_480B_95E7_4CA727CD9207__INCLUDED_)
#define AFX_EDITIMAGE_H__E8731569_888C_480B_95E7_4CA727CD9207__INCLUDED_

#include "TransparentWnd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditImage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditImage dialog

class CEditImage : public CDialog
{
// Construction
public:
	void UpdateGUI();
	void PreModal(CTransparentWnd* transWnd);
	CEditImage(CWnd* pParent = NULL);   // standard constructor

	CTransparentWnd* m_transWnd;
	int m_dialogInitialized;
	int pickingColor;

// Dialog Data
	//{{AFX_DATA(CEditImage)
	enum { IDD = IDD_EDITIMAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditImage)
	virtual BOOL OnInitDialog();
	afx_msg void OnNoCutout();
	afx_msg void OnCutoutTrans();
	afx_msg void OnCutoutPredefined();
	afx_msg void OnAddBorder();
	afx_msg void OnBorderColor();
	afx_msg void OnColor();
	afx_msg void OnSelchangePredefinedshape();
	afx_msg void OnChangeBordersize();
	afx_msg void OnColorPick();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBkcolor();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITIMAGE_H__E8731569_888C_480B_95E7_4CA727CD9207__INCLUDED_)
