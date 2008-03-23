#if !defined(AFX_FLASHCONVERSIONDLG_H__1FE9B0D3_895B_4592_A852_F06969C7CA86__INCLUDED_)
#define AFX_FLASHCONVERSIONDLG_H__1FE9B0D3_895B_4592_A852_F06969C7CA86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlashConversionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FlashConversionDlg dialog

class FlashConversionDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(FlashConversionDlg)

// Construction
public:
	FlashConversionDlg();
	~FlashConversionDlg();
	void UpdateBehavior(int val); 

// Dialog Data
	//{{AFX_DATA(FlashConversionDlg)
	enum { IDD = IDD_FLASHCONVERSION };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(FlashConversionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(FlashConversionDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelectflash();
	afx_msg void OnSelecthtml();
	afx_msg void OnRadio2();
	afx_msg void OnRadio1();
	afx_msg void OnRaw();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLASHCONVERSIONDLG_H__1FE9B0D3_895B_4592_A852_F06969C7CA86__INCLUDED_)
