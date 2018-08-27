#pragma once

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    enum
    {
        IDD = IDD_ABOUTBOX
    };

    virtual BOOL OnInitDialog();

protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

protected:
    DECLARE_MESSAGE_MAP()
    DECLARE_EVENTSINK_MAP()

private:
    CStatic version_copyright_label_;
};
