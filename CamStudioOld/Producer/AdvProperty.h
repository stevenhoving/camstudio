#if !defined(AFX_ADVPROPERTY_H__06ADD385_A385_4A17_B62F_A0E8AF8063F9__INCLUDED_)
#define AFX_ADVPROPERTY_H__06ADD385_A385_4A17_B62F_A0E8AF8063F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvProperty dialog

class CAdvProperty : public CPropertyPage
{
	DECLARE_DYNCREATE(CAdvProperty)

// Construction
public:
	CAdvProperty();
	~CAdvProperty();

// Dialog Data
	//{{AFX_DATA(CAdvProperty)
	enum { IDD = IDD_ADVPROP };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvProperty)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVPROPERTY_H__06ADD385_A385_4A17_B62F_A0E8AF8063F9__INCLUDED_)
