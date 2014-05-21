#pragma once


// ProgressDlg dialog

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

	BOOL Create(CWnd *pParent=NULL);

	// Checking for Cancel button
	BOOL CheckCancelButton();
	// Progress Dialog manipulation
	void SetRange(short nLower,short nUpper);
	int  SetStep(short nStep);
	int  SetPos(int nPos);
	int  OffsetPos(int nPos);
	int  StepIt();
	short RealMax();
	short FakeMax();
	short MinSecProgress();
	short MaxSecProgress();
	short MinProg();
	short MaxProg();
// Dialog Data
	enum { IDD = IDD_CONV_DIALOG };
	CProgressCtrl	m_Progress;

	virtual BOOL DestroyWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	UINT m_nCaptionID;
	short m_nLower;
	short m_nUpper;
	short m_nStep;
	short m_nMaxSecProgress;
	short m_nMinSecProgress;
	short m_nMinProg;
	short m_nMaxProg;
	short m_nFakeMax;
	BOOL m_bCancel;
	BOOL m_bParentDisabled;

	void ReEnableParent();

	virtual void OnCancel();
	virtual void OnOK() {};
	void UpdatePercent(int nCurrent);
	void PumpMessages();

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedProgressCancel();
	afx_msg void OnStnClickedConversiontext();
};
