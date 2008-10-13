// Keyshortcuts.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_KEYSHORTCUTS_H__7D91F35B_8385_4EBB_96CA_AEFB403E2121__INCLUDED_)
#define AFX_KEYSHORTCUTS_H__7D91F35B_8385_4EBB_96CA_AEFB403E2121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// Keyshortcuts dialog

class Keyshortcuts : public CDialog
{
// Construction
public:
	Keyshortcuts(CWnd* pParent = NULL);   // standard constructor

	int GetIndex(UINT keyShortCut);
	UINT GetCode(int index);

// Dialog Data
	//{{AFX_DATA(Keyshortcuts)
	enum { IDD = IDD_KEYSHORTCUTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Keyshortcuts)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(Keyshortcuts)
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
	int TestKeysOverlap(int& o1, int& o2);

	UINT keyNextLocal;
	UINT keyPrevLocal;
	UINT keyShowLayoutLocal;

	UINT keyNextLocalCtrl;
	UINT keyPrevLocalCtrl;
	UINT keyShowLayoutLocalCtrl;

	UINT keyNextLocalAlt;
	UINT keyPrevLocalAlt;
	UINT keyShowLayoutLocalAlt;

	UINT keyNextLocalShift;
	UINT keyPrevLocalShift;
	UINT keyShowLayoutLocalShift;

	UINT keyRecordStartLocal;
	UINT keyRecordEndLocal;
	UINT keyRecordCancelLocal;

	UINT keyRecordStartLocalCtrl;
	UINT keyRecordEndLocalCtrl;
	UINT keyRecordCancelLocalCtrl;

	UINT keyRecordStartLocalAlt;
	UINT keyRecordEndLocalAlt;
	UINT keyRecordCancelLocalAlt;

	UINT keyRecordStartLocalShift;
	UINT keyRecordEndLocalShift;
	UINT keyRecordCancelLocalShift;

	int numkeys;
	int numSpecial;
	int key[6];
	int keySpecial[6][3];
	CString keyName[6];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYSHORTCUTS_H__7D91F35B_8385_4EBB_96CA_AEFB403E2121__INCLUDED_)
