#if !defined(AFX_SWFPROPERTYSHEET_H__8ECF3911_6BE8_48A6_939E_CADE14637205__INCLUDED_)
#define AFX_SWFPROPERTYSHEET_H__8ECF3911_6BE8_48A6_939E_CADE14637205__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SWFPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SWFPropertySheet

class SWFPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SWFPropertySheet)

// Construction
public:
	SWFPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SWFPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SWFPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SWFPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(SWFPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SWFPROPERTYSHEET_H__8ECF3911_6BE8_48A6_939E_CADE14637205__INCLUDED_)
