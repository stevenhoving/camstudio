#pragma once

class CKeyshortcutsDlg : public CDialog
{
public:
    explicit CKeyshortcutsDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CKeyshortcutsDlg)
    enum
    {
        IDD = IDD_KEYSHORTCUTS
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CKeyshortcutsDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
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
    UINT GetCode(int index);

    std::vector<UINT> m_vKeyCode;
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
    CButton m_ctrlButtonAutoSpanCTRL;
    CButton m_ctrlButtonAutoSpanALT;
    CButton m_ctrlButtonAutoSpanSHFT;
    CComboBox m_ctrlCBRecord;
    CComboBox m_ctrlCBStop;
    CComboBox m_ctrlCBCancel;
    CComboBox m_ctrlCBNext;
    CComboBox m_ctrlCBPrev;
    CComboBox m_ctrlCBShow;
    CComboBox m_ctrlCBAutopan;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
