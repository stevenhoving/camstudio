#pragma once

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum
    {
        IDD = IDD_ABOUTBOX
    };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    afx_msg void OnButtonlink();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    // afx_msg void OnBnClickedButtonlink();
    DECLARE_EVENTSINK_MAP()
    afx_msg void OnBnClickedButtonlink2();

public:
    virtual BOOL OnInitDialog();

private:
    CStatic m_ctrlStaticVersion;
public:
    afx_msg void OnStnClickedStaticVersion();
};