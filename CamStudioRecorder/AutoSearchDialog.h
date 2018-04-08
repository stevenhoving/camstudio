#pragma once

class CAutoSearchDlg : public CDialog
{
public:
    explicit CAutoSearchDlg(CWnd *pParent = nullptr);
    void SetVarText(CString textstr);
    void SetVarTextLine2(CString textstr);
    void SetButtonEnable(BOOL enable);

    // Dialog Data
    //{{AFX_DATA(CAutoSearchDlg)
    enum
    {
        IDD = IDD_AUTOSEARCH
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAutoSearchDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CAutoSearchDlg)
    afx_msg void OnCloseDialog();
    afx_msg void OnClose();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CStatic m_ctrlStaticText1;
    CStatic m_ctrlStaticText2;
    CButton m_ctrlButtonClose;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


