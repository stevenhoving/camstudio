// ScreenAnnotations.h : header file
//
#if !defined(AFX_SCREENANNOTATIONS_H__BD963A1F_33E6_424C_AEC5_4A3A78700C29__INCLUDED_)
#define AFX_SCREENANNOTATIONS_H__BD963A1F_33E6_424C_AEC5_4A3A78700C29__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TransparentWnd.h"
#include "LayoutList.h"
#include "ListManager.h"

/////////////////////////////////////////////////////////////////////////////
// CScreenAnnotationsDlg dialog

class CScreenAnnotationsDlg : public CDialog
{
// Construction
public:
	CScreenAnnotationsDlg(CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	//{{AFX_DATA(CScreenAnnotationsDlg)
	enum { IDD = IDD_SCREENANNOTATIONS2 };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreenAnnotationsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScreenAnnotationsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBegindragList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditobjNewobject();
	afx_msg void OnEditobjEditimage();
	afx_msg void OnEditobjEdittext();
	afx_msg void OnEditobjEdittransparency();
	afx_msg void OnEditobjCopy();
	afx_msg void OnEditobjPaste();
	afx_msg void OnEditobjRemove();
	afx_msg void OnEditobjTestedit();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnCancel();
	afx_msg void OnEndlabeleditList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditlayoutSavelayout();
	afx_msg void OnEditlayoutOpenlayout();
	afx_msg void OnEditlayoutCloseallobjects();
	afx_msg void OnEditlayoutLayoutshortcuts();
	afx_msg void OnEditobjLibraryCloselibrary();
	afx_msg void OnEditobjLibraryOpenlibrary();
	afx_msg void OnEditobjLibraryNewlibrary();
	afx_msg void OnEditlayoutLibraryNewlibrary();
	afx_msg void OnEditlayoutLibraryOpenlibrary();
	afx_msg void OnEditlayoutLibraryCloselibrary();
	afx_msg void OnHelpHelp();
	afx_msg void OnOptionsClosescreenannotations();
	afx_msg void OnNext();
	afx_msg void OnHelpShapes();
	afx_msg void OnHelpShapetopicsInstantiatingashape();
	afx_msg void OnHelpShapetopicsEditingashape();
	afx_msg void OnHelpShapetopicsEditingtext();
	afx_msg void OnHelpShapetopicsEditingimage();
	afx_msg void OnHelpShapetopicsCreatinganewshape();
	afx_msg void OnHelpShapetopicsEditingtransparency();
	afx_msg void OnHelpShapetopicsManagingshapes();
	afx_msg void OnHelpShapetopicsResizingshapes();
	afx_msg void OnHelpLayouts();
	afx_msg void OnAnnSavelayout();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLibraryOpenshapelibrary();
	afx_msg void OnLibrarySaveshapelibrary();
	afx_msg void OnLibraryNewshapelibrary();
	afx_msg void OnLibraryNewlayoutlibrary();
	afx_msg void OnLibraryOpenlayoutlibrary();
	afx_msg void OnLibrarySavelayoutlibrary();
	afx_msg void OnObjectsCopy();
	afx_msg void OnObjectsPaste();
	afx_msg void OnObjectsRemove();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnObjectsEdittext();
	afx_msg void OnObjectsEditimage();
	afx_msg void OnObjectsEditname();
	afx_msg void OnObjectsEdittransaprency();
	afx_msg void OnObjectsCloseallobjects();
	afx_msg void OnLibraryInsertshapelibaray();
	afx_msg void OnObjectsMoveitemup();
	afx_msg void OnObjectsMoveitemdown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SaveLayoutNew();
	int GetLayoutListSelection();
	void InstantiateLayout(int nItem, int makeselect);
	
	void SaveShapeNew(CTransparentWnd *newWnd);
	void SaveShapeReplace(CTransparentWnd *newWnd);
	void RefreshShapeList();

	void CloseAllWindows(int wantDelete);
private:
	bool m_bEditingLabelOn;
	HCURSOR m_hCursorDrag;
	HCURSOR m_hCursorArrow;
	CListCtrl m_ctrlList;
	CTabCtrl m_ctrlTab;

	CTransparentWnd* LocateWndFromItem(int nItem);
	CTransparentWnd* LocateWndFromShapeList();
	void InstantiateWnd(int x, int y);

	void TabSelectShapeMode(int updateTab);
	void TabSelectLayoutMode(int updateTab);
	void UpdateTabCtrl(int sel);

	CLayoutList* LocateLayoutFromItem(int nItem);
	void RefreshLayoutList();
	void InstantiateLayout();

	BOOL Openlink (CString);
	BOOL OpenUsingShellExecute (CString);
	BOOL OpenUsingRegisteredClass (CString);

	void MoveItem(int direction);
	void AdjustLayoutName(CString& layoutName);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

extern bool AreWindowsEdited();

#endif // !defined(AFX_SCREENANNOTATIONS_H__BD963A1F_33E6_424C_AEC5_4A3A78700C29__INCLUDED_)

