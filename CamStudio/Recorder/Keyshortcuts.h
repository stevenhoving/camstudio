// CKeyshortcutsDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#include "afxwin.h"
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
	afx_msg void OnSelchangeStopkey();
	afx_msg void OnSelchangeCancelkey();
	afx_msg void OnSelchangeRecordkey();
	afx_msg void OnSelchangeNextkey();
	afx_msg void OnSelchangePrevkey();
	afx_msg void OnSelchangeShowkey();
	afx_msg void OnCtrl1();
	afx_msg void OnCtrl2();
	afx_msg void OnCtrl3();
	afx_msg void OnCtrl4();
	afx_msg void OnCtrl5();
	afx_msg void OnCtrl6();
	afx_msg void OnShift1();
	afx_msg void OnShift2();
	afx_msg void OnShift3();
	afx_msg void OnShift4();
	afx_msg void OnShift5();
	afx_msg void OnShift6();
	afx_msg void OnAlt1();
	afx_msg void OnAlt2();
	afx_msg void OnAlt3();
	afx_msg void OnAlt4();
	afx_msg void OnAlt5();
	afx_msg void OnAlt6();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int GetIndex(UINT keyShortCut);
	UINT GetCode(size_t index);

	int TestKeysOverlap(int& o1, int& o2);

	std::vector <UINT> m_vKeyCode;

	UINT m_keyNext;
	UINT m_keyPrev;
	UINT m_keyShowLayout;

	UINT m_keyNextCtrl;
	UINT m_keyPrevCtrl;
	UINT m_keyShowLayoutCtrl;

	UINT m_keyNextAlt;
	UINT m_keyPrevAlt;
	UINT m_keyShowLayoutAlt;

	UINT m_keyNextShift;
	UINT m_keyPrevShift;
	UINT m_keyShowLayoutShift;

	UINT m_keyRecordStart;
	UINT m_keyRecordEnd;
	UINT m_keyRecordCancel;

	UINT m_keyRecordStartCtrl;
	UINT m_keyRecordEndCtrl;
	UINT m_keyRecordCancelCtrl;

	UINT m_keyRecordStartAlt;
	UINT m_keyRecordEndAlt;
	UINT m_keyRecordCancelAlt;

	UINT m_keyRecordStartShift;
	UINT m_keyRecordEndShift;
	UINT m_keyRecordCancelShift;

	int m_iNumKeys;
	int m_iNumSpecial;
	int m_iKey[6];
	int m_iKeySpecial[6][3];
	CString keyName[6];
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
