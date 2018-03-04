#pragma once

// ProgressDlg.h : header file
// CG: This file was added by the Progress Dialog component

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog



class CProgressDlg : public CDialog
{
 public:
    CProgressDlg(UINT nCaptionID = 0);
    ~CProgressDlg() override;

    BOOL Create(CWnd *pParent = nullptr);

    // Checking for Cancel button
    BOOL CheckCancelButton();
    // Progress Dialog manipulation
    void SetRange(int nLower, int nUpper);
    int SetStep(int nStep);
    int SetPos(int nPos);
    int OffsetPos(int nPos);
    int StepIt();

    // Dialog Data
    //{{AFX_DATA(CProgressDlg)
    enum
    {
        IDD = CG_IDD_PROGRESS
    };
    CProgressCtrl m_Progress;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressDlg)
public:
    BOOL DestroyWindow() override;

protected:
    void DoDataExchange(CDataExchange *pDX) override; // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    UINT m_nCaptionID;
    int m_nLower;
    int m_nUpper;
    int m_nStep;

    BOOL m_bCancel;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    void OnCancel() override;
    void OnOK() override{};
    void UpdatePercent(int nCurrent);
    void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CProgressDlg)
    BOOL OnInitDialog() override;
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedProgressCancel();
};
