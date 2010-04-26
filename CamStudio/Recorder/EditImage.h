// EditImage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_EDITIMAGE_H__E8731569_888C_480B_95E7_4CA727CD9207__INCLUDED_)
#define AFX_EDITIMAGE_H__E8731569_888C_480B_95E7_4CA727CD9207__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransparentWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CEditImageDlg dialog

class CEditImageDlg : public CDialog
{
// Construction
public:
	CEditImageDlg(CWnd* pParent = NULL);   // standard constructor

	void UpdateGUI();
	void PreModal(CTransparentWnd* transWnd);

// Dialog Data
	//{{AFX_DATA(CEditImageDlg)
	enum { IDD = IDD_EDITIMAGE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditImageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditImageDlg)
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
private:
	CTransparentWnd* m_transWnd;
	int m_dialogInitialized;
	int pickingColor;

	HCURSOR m_hCursorCross;
	HCURSOR m_hCursorArrow;
	CButton m_ctrlButtonNoBKColor;
	CButton m_ctrlButtonTransparent;
	CButton m_ctrlButtonPreDefined;
	CButton m_ctrlButtonBKColor;
	CButton m_ctrlButtonColor;
	CButton m_ctrlButtonPickScreenColor;
	CStatic m_ctrlStaticColor;
	CStatic m_ctrlStaticChoose;
	CComboBox m_ctrlCBPredefinedShape;
	CButton m_ctrlButtonAddBorder;
	CStatic m_ctrlStaticBorderSize;
	CStatic m_ctrlStaticColor2;
	CButton m_ctrlButtonBorderColor;
	CEdit m_ctrlEditBorderSize;
	CSpinButtonCtrl m_ctrlSpinBorderSize;
	CStatic m_ctrlStaticBorderGroup;
	CButton m_ctrlButtonLoadNewImage;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITIMAGE_H__E8731569_888C_480B_95E7_4CA727CD9207__INCLUDED_)
