#pragma once


/////////////////////////////////////////////////////////////////////////////
// CSyncDlg dialog

class CSyncDlg : public CDialog
{

    CSyncDlg(CWnd *pParent = nullptr); //  not implemeted
public:
    CSyncDlg(int iType, int iShift, CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CSyncDlg)
    enum
    {
        IDD = IDD_SYNCHRONIZATION
    };
    int m_iTimeShift;
    int m_iShiftType;
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSyncDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CSyncDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnVideoFirst();
    afx_msg void OnAudioFirst();
    afx_msg void OnNoShift();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CButton m_ctrlButtonNoSync;
    CButton m_ctrlButtonAudioFirst;
    CButton m_ctrlButtonInvalid;
    CSpinButtonCtrl m_ctrlSpinTimeShift;
    CEdit m_ctrlEditTimeShift;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

