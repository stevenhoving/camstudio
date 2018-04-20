#pragma once

class CTroubleShootDlg : public CDialog
{
public:
    explicit CTroubleShootDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CTroubleShootDlg)
    enum
    {
        IDD = IDD_TROUBLESHOOT
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTroubleShootDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CTroubleShootDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CButton m_ctrlButtonBehavior1;
    CButton m_ctrlButtonBehavior2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
