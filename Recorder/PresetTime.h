#pragma once

class CPresetTimeDlg : public CDialog
{
public:
    explicit CPresetTimeDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CPresetTimeDlg)
    enum
    {
        IDD = IDD_PRESETTIME
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CPresetTimeDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CPresetTimeDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CSpinButtonCtrl m_ctrlSpinTime;
    CButton m_ctrlButtonTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

