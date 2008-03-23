#if !defined(AFX_FLASHINTERFACE_H__907CECCE_3427_42E1_96DD_1258286FF62B__INCLUDED_)
#define AFX_FLASHINTERFACE_H__907CECCE_3427_42E1_96DD_1258286FF62B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlashInterface.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlashInterface dialog

class CFlashInterface : public CPropertyPage
{
	DECLARE_DYNCREATE(CFlashInterface)

// Construction
public:
	CFlashInterface();
	~CFlashInterface();

// Dialog Data
	//{{AFX_DATA(CFlashInterface)
	enum { IDD = IDD_INTERFACE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFlashInterface)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFlashInterface)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButton1();
	afx_msg void OnButtonchoose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLASHINTERFACE_H__907CECCE_3427_42E1_96DD_1258286FF62B__INCLUDED_)
