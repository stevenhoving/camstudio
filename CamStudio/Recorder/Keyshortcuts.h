// CKeyshortcutsDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_KEYSHORTCUTS_H__7D91F35B_8385_4EBB_96CA_AEFB403E2121__INCLUDED_)
#define AFX_KEYSHORTCUTS_H__7D91F35B_8385_4EBB_96CA_AEFB403E2121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKeyshortcutsDlg dialog

class CKeyshortcutsDlg : public CDialog
{
// Construction
public:
	CKeyshortcutsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyshortcutsDlg)
	enum { IDD = IDD_KEYSHORTCUTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyshortcutsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CKeyshortcutsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int GetIndex(UINT keyShortCut);
	UINT GetCode(size_t index);

	std::vector <UINT> m_vKeyCode;
	CButton m_ctrlButtonRecordCTRL;
	CButton m_ctrlButtonRecordALT;
	CButton m_ctrlButtonRecordSHFT;
	CButton m_ctrlButtonStopCTRL;
	CButton m_ctrlButtonStopALT;
	CButton m_ctrlButtonStopSHFT;
	CButton m_ctrlButtonCancelCTRL;
	CButton m_ctrlButtonCancelALT;
	CButton m_ctrlButtonCancelSHFT;
	CButton m_ctrlButtonNextCTRL;
	CButton m_ctrlButtonNextALT;
	CButton m_ctrlButtonNextSHFT;
	CButton m_ctrlButtonPrevCTRL;
	CButton m_ctrlButtonPrevALT;
	CButton m_ctrlButtonPrevSHFT;
	CButton m_ctrlButtonShowCTRL;
	CButton m_ctrlButtonShowALT;
	CButton m_ctrlButtonShowCHFT;
	CComboBox m_ctrlCBRecord;
	CComboBox m_ctrlCBStop;
	CComboBox m_ctrlCBCancel;
	CComboBox m_ctrlCBNext;
	CComboBox m_ctrlCBPrev;
	CComboBox m_ctrlCBShow;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYSHORTCUTS_H__7D91F35B_8385_4EBB_96CA_AEFB403E2121__INCLUDED_)
