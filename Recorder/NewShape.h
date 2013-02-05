// NewShape.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_NEWSHAPE_H__04B11E2E_DC5E_4AC3_B324_DE8A8B41C9BB__INCLUDED_)
#define AFX_NEWSHAPE_H__04B11E2E_DC5E_4AC3_B324_DE8A8B41C9BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNewShapeDlg dialog

class CNewShapeDlg : public CDialog
{
// Construction
public:
	CNewShapeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewShapeDlg)
	enum { IDD = IDD_NEWSHAPE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewShapeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewShapeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickBlankImage();
	afx_msg void OnClickImageFile();
	virtual void OnOK();
	afx_msg void OnFindImageFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_imageDir;
	CEdit m_ctrlTextFileText;
	CEdit m_ctrlEditName;
	CButton m_ctrlButtoBlankImage;
	CButton m_ctrlButtoImageFile;
	CStatic m_ctrlStaticWidth;
	CStatic m_ctrlStaticHeight;
	CButton m_ctrlButtonImageFile;
	CEdit m_ctrlEditWidth;
	CEdit m_ctrlEditHeight;
	CEdit m_ctrlEditShaepText;
	UINT m_uImageWidth;
	UINT m_uImageHeight;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWSHAPE_H__04B11E2E_DC5E_4AC3_B324_DE8A8B41C9BB__INCLUDED_)
